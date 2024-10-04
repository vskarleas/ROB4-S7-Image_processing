#include "camera.h"
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

void Camera::afficher_lignes(std::vector<cv::Vec2f> lines, cv::Mat m_frame)
{
	// Dessiner les lignes détectées
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;
		cv::Point pt1(cvRound(x0 + 1000 * (-b)), cvRound(y0 + 1000 * (a)));
		cv::Point pt2(cvRound(x0 - 1000 * (-b)), cvRound(y0 - 1000 * (a)));
		cv::line(m_frame, pt1, pt2, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
	}
}

Camera::Camera()
{
	m_fps = 30;
}

bool Camera::open(std::string filename)
{
	m_fileName = filename;

	// Convert filename to number if you want to
	// open webcam stream
	std::istringstream iss(filename.c_str());
	int devid;
	bool isOpen;
	if (!(iss >> devid))
	{
		isOpen = m_cap.open(filename.c_str());
	}
	else
	{
		isOpen = m_cap.open(devid);
	}

	if (!isOpen)
	{
		std::cerr << "Unable to open video file." << std::endl;
		return false;
	}

	// set framerate, if unable to read framerate, set it to 30
	m_fps = m_cap.get(cv::CAP_PROP_FPS);
	if (m_fps == 0)
		m_fps = 30;

	return true;
}

void Camera::play()
{
	// Create main window
	namedWindow("Video", cv::WINDOW_AUTOSIZE);
	bool isReading = true;
	// Compute time to wait to obtain wanted framerate
	int timeToWait = 1000 / m_fps;

	Mat gray, edges, hough, hough_final, frame_prec, Movement;

	int threshold_value_1_inf = 200;
	int threshold_value_1_sup = 50;
	int thres_hough = 75;
	int thres_hough_theta = 70;
	int threshold_value = 25;
	// //cv::threshold(gray, bin_d, threshold_value_1, 255, cv::THRESH_BINARY);

	// // Créer une fenêtre pour afficher l'image
	cv::namedWindow("Thresholded Image", cv::WINDOW_AUTOSIZE);
	// Créer les trackbars pour ajuster les seuils
	cv::createTrackbar("Seuil Inferieur", "Thresholded Image", &threshold_value_1_inf, 255);
	cv::createTrackbar("Seuil Superieur", "Thresholded Image", &threshold_value_1_sup, 255);
	cv::createTrackbar("Seuil Hough", "Thresholded Image", &thres_hough, 200);
	cv::createTrackbar("Seuil Hough Theta", "Thresholded Image", &thres_hough_theta, 180);
	cv::createTrackbar("Seuil", "Thresholded Image", &threshold_value, 255);

	m_cap.read(m_frame);
	frame_prec = m_frame.clone();

	while (isReading)
	{
		// Get frame from stream
		isReading = m_cap.read(m_frame);

		if (isReading)
		{
			// Show frame in main window
			imshow("Video", m_frame);
			hough = m_frame.clone(); // init hough
			hough_final = m_frame.clone();

			// ================ Code ==================== //
			cv::cvtColor(m_frame, gray, cv::COLOR_BGR2GRAY);
			imshow("Gray Scale", gray);

			// Détection des bords avec l'algorithme de Canny
			cv::Canny(gray, edges, 255, 255);
			imshow("Canny", edges);

			// Transformation de Hough pour détecter les lignes
			std::vector<cv::Vec2f> lines;
			cv::HoughLines(edges, lines, 1, CV_PI / 180, thres_hough);

			afficher_lignes(lines, hough);
			cv::imshow("Voies detectees", hough);

			// Enlever les lignes de Hough qui ont un angle inferieur a thres_hough_theta
			for (size_t j = 0; j < lines.size();)
			{
				int angle = lines[j][1] * 180 / CV_PI;
				if ((angle > thres_hough_theta) && (angle < 180 - thres_hough_theta))
				{
					lines.erase(lines.begin() + j); // Enlève l'élément à l'indice i
				}
				else
					j++;
			}

			// Afficher les lignes restantes
			for (size_t i = 0; i < lines.size()-1; i++) //ATTENTION: Withouth the -1, there was a segmentation Fault in Macos
			{ // Note : on ne fait pas d'incrémentation ici
				float rho = lines[i][0];
				float theta = lines[i][1];

				// Calcul des points d'intersection de la ligne avec le bord inférieur de l'image
				double a = cos(theta);
				double b = sin(theta);
				double x0 = a * rho;
				double y0 = b * rho;

				// Calcul du point le plus bas sur la ligne
				cv::Point pt_low(cvRound(x0 + 1000 * (-b)), cvRound(y0 + 1000 * (a)));

				// Vérification des conditions
				if (pt_low.x > hough_final.cols / 2 && (theta * 180 / CV_PI) < 90)
				{ // À droite
					lines.erase(lines.begin() + i); // On enlève l'élément à l'indice i
				}
				else
				{
					i++; // On incrémente seulement si on n'a pas effacé l'élément
				}
				if (pt_low.x < hough_final.cols / 2 && (theta * 180 / CV_PI) > 90)
				{ // À gauche
					lines.erase(lines.begin() + i); // On enlève l'élément à l'indice i
				}
				else
				{
					i++; // On incrémente seulement si on n'a pas effacé l'élément
				}
			}

			afficher_lignes(lines, hough_final);
			cv::imshow("Voies detectees Final", hough_final);

			// On detecte les votures
			// Détection des contours avec Canny
			cv::subtract(m_frame, frame_prec, Movement); // init edges
			frame_prec = m_frame.clone();
			imshow("Movement", Movement);

			cv::cvtColor(Movement, gray, cv::COLOR_BGR2GRAY);
			cv::threshold(gray, edges, threshold_value, 255, cv::THRESH_BINARY);
			//cv::Canny(m_frame, edges, threshold_value_1_inf, threshold_value_1_sup);

			// fermeture (dilatation puis erosion)
			//  Création d'un élément structurant pour la dilatation et l'érosion
			cv::Mat masque = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(55, 55));
			cv::Mat dilated, closed;

			// Appliquer la dilatation
			cv::dilate(edges, dilated, masque);

			// Appliquer l'érosion
			cv::erode(dilated, closed, masque);

			// Trouver les contours
			std::vector<std::vector<cv::Point>> contours;
			cv::findContours(closed, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		    // Filtrer et dessiner les contours
			// for (size_t i = 0; i < contours.size(); i++) {
            //     double area = cv::contourArea(contours[i]);
            //     if ((area<10000) && (area>135)) { // Ajustez ce seuil selon la taille des voitures dans la vidéo
            //         cv::drawContours(hough_final, contours, static_cast<int>(i), cv::Scalar(0, 255, 0), 2);
            //         carCount++; // Compter le contour comme une voiture
            //     }
            // }


			int carCount = 0; // Compteur de voitures

			std::vector<cv::Rect> car_rects;

			for (size_t i = 0; i < contours.size(); i++)
			{
				double area = cv::contourArea(contours[i]);
				if ((area < 10000) && (area > 135))
				{
					cv::Rect rect = cv::boundingRect(contours[i]);
					car_rects.push_back(rect);
				}
			}

			// Trying to merge close rectangles. Otherwise we need to merge the contours sohehow
			for (size_t i = 0; i < car_rects.size(); i++)
			{
				for (size_t j = i + 1; j < car_rects.size(); j++)
				{
					if ((car_rects[i] & car_rects[j]).area() > 0 || (cv::norm(car_rects[i].tl() - car_rects[j].tl()) < 50))
					{
						car_rects[i] = car_rects[i] | car_rects[j];
						car_rects.erase(car_rects.begin() + j);
						j--;
					}
				}
			}

			int carCount = car_rects.size();
			for (const auto &rect : car_rects)
			{
				cv::rectangle(hough_final, rect, cv::Scalar(0, 255, 0), 2);
			}
		
			// Afficher le nombre de voitures détectées
			cv::putText(hough_final, "Nombre de voitures: " + std::to_string(carCount), cv::Point(10, 30),
						cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);

			// Afficher les contours détectés
			cv::imshow("Contours des voitures", hough_final);
			cv::imshow("Only cars dilated image", closed);

			/// --------------------------------
		}
		else
		{
			std::cerr << "Unable to read device" << std::endl;
		}

		// If escape key is pressed, quit the program
		if (waitKey(timeToWait) % 256 == 27)
		{
			std::cerr << "Stopped by user" << std::endl;
			isReading = false;
		}
	}
}

bool Camera::close()
{
	// Close the stream
	m_cap.release();

	// Close all the windows
	destroyAllWindows();
	usleep(100000);
	return true;
}
