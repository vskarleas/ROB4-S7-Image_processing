#include "camera.h"
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

// Affiche les lignes passés en arguments sur un frame donnée
/* Une ligne a deux paramettres par la tranformation de Hough. Une angle et une distance pour le
point de reference de l'image (m_frame). Ici on utilise les parametres pour trouver les
points qui composent les lignes, ainsi que faire une decison si ils sont tres a gauche ou droit.
On a fait déjà un tri de ligne savant l'appele de cette fonction, sur la partie du code prinicpal de la
lecture des frames (Camera::play) */
void Camera::afficher_lignes(std::vector<cv::Vec2f> lignes, cv::Mat m_frame)
{
	/* On va parcourir tous les lignes */
	for (size_t i = 0; i < lignes.size(); i++)
	{
		/* Obtenir les coordonees polaires de la ligne */
		float rho = lignes[i][0];
		float theta = lignes[i][1];

		double a = cos(theta);
		double b = sin(theta);

		/* Calcul des coordonnees cartesiennes des points qui compose une lignes */
		double x0 = a * rho;
		double y0 = b * rho;

		/* Calcul de deux points qui permetent de designer la ligne à la fin */
		/* x0 et y0 donnent un point sur la ligne qui est le plus proche de l'origine (selon l'équation de Hough).
		Les deux points pt1 et pt2 sont choisis en s'éloignant de ce point le long de la ligne dans les deux sens. */
		cv::Point pt1(cvRound(x0 + 1000 * (-b)), cvRound(y0 + 1000 * (a)));
		cv::Point pt2(cvRound(x0 - 1000 * (-b)), cvRound(y0 - 1000 * (a)));

		// cv::line(m_frame, pt1, pt2, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);

		/* Pour dessiner les lignes de partie gauche et la partie droite, on a besoin
		un Scalaire sur laquelle on declare le couler souhaité. Ici il s'agit d'une initialisation */
		cv::Scalar lineColor;

		/* Decision si la ligne est plus proche à gauche ou plus proche à droit selon theta et rho */
		if (rho < 0 && theta > CV_PI / 2) // est qu'on à gauche ? Si oui, alors theta est superieur que pi/2
		{
			lineColor = cv::Scalar(0, 0, 255); // Red
		}
		else if (rho > 0 && theta < CV_PI / 2) // on à droite? Si oui, alors theta est inferieur que pi/2
		{
			lineColor = cv::Scalar(255, 0, 0); // Blue
		}
		else // Treatment des cas d'erreur
		{
			lineColor = cv::Scalar(0, 255, 0);
		}

		/* Dessiner la ligne sur le frame */
		cv::line(m_frame, pt1, pt2, lineColor, 2, cv::LINE_AA);
	}
}

// Si les lignes de Hough sont tres proches l'un à l'autre, on veut garder que une seule commune pour chaque groupe de lignes
// Remarque 1.2
std::vector<cv::Vec2f> merge_lignes(const std::vector<cv::Vec2f> &lines, float rho_threshold, float theta_threshold)
{
	std::vector<cv::Vec2f> merged_lines;

	std::vector<bool> merged(lines.size(), false);

	for (size_t i = 0; i < lines.size(); ++i)
	{
		if (merged[i])
			continue; // Skip lines that have already been merged

		float rho1 = lines[i][0];
		float theta1 = lines[i][1];

		float rho_sum = rho1;
		float theta_sum = theta1;
		int count = 1;

		// Check for other lines that are close to the current one
		for (size_t j = i + 1; j < lines.size(); ++j)
		{
			float rho2 = lines[j][0];
			float theta2 = lines[j][1];

			// If the lines are close in rho and theta, consider them the same and merge
			if (std::abs(rho1 - rho2) < rho_threshold && std::abs(theta1 - theta2) < theta_threshold)
			{
				rho_sum += rho2;
				theta_sum += theta2;
				count++;
				merged[j] = true; // Mark this line as merged
			}
		}

		// Average the rho and theta values to create the merged line
		float avg_rho = rho_sum / count;
		float avg_theta = theta_sum / count;

		merged_lines.push_back(cv::Vec2f(avg_rho, avg_theta));
	}

	return merged_lines;
}

// Parametrage de la camrea pour la lecture des frames (images)
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

// List pour sauvegarder les voitures detectées avant
std::vector<cv::Rect> voitures_precedentes;
std::vector<cv::Vec2f> fixed_lines;

// Compter les voitures selon les rectangles approximatifs donnes
void compter_voitures(std::vector<cv::Rect> &voitures_maintenant, int &voitures_gauche, int &voitures_droite, cv::Mat &frame)
{
	const int distance_threshold = 100; // pour la distance entre les centres des rectangles
	const int erreur = 550;				// pour la tolérance de l'erreur sur la voie de gauche ou les voitures viens vers nous et on a un effet de mal comptage quand les rectangles sont places de le debut

	for (const auto &voiture_maintenant : voitures_maintenant)
	{
		bool nouveau = true; // est que c'est nouveau le voiture ?

		for (const auto &voiture_prec : voitures_precedentes)
		{

			// Calcul de la distance euclidiene entre les centres des deux rectangles
			cv::Point centre(voiture_maintenant.x + voiture_maintenant.width / 2, voiture_maintenant.y + voiture_maintenant.height / 2);
			cv::Point centre_prec(voiture_prec.x + voiture_prec.width / 2, voiture_prec.y + voiture_prec.height / 2);

			double distance = cv::norm(centre - centre_prec);

			// If the distance is below the threshold, it's the same car
			if (distance < distance_threshold)
			{
				nouveau = false;
				break; // REALLY IMPORTANT: othwerwise it doesn't stop at the moment that we detected the new car
			}
		}

		// Si c'est vraiment un nouveau voiture, on incremente le compteur global (passe en methode pointeur)
		if (nouveau)
		{
			if ((voiture_maintenant.x) < (frame.cols / 2))
			{
				voitures_gauche++;
			}
			else
			{
				voitures_droite++;
			}
		}
	}

	// Mis a jour de la liste voitures_precedentes avec la liste voitures_maintenant
	voitures_precedentes = voitures_maintenant;
}

// Traitment d'image edges (par Movement) pour compter les voitures
void process_frame(cv::Mat &frame, cv::Mat &edges, int &voitures_gauche, int &voitures_droite)
{
	/* Images qu'on a besoin pour faire la fermeture */
	cv::Mat gray, closed, dilated;

	/* Creation de la masque pour fermeture */
	cv::Mat masque = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15));

	/* On applique la fermeture (Dilation -> Erosion) pour fermer des gaps à l'image edges (binarisé) */
	// Appliquer la dilatation
	cv::dilate(edges, dilated, masque);

	// Appliquer l'érosion
	cv::erode(dilated, closed, masque);

	/* Une autre maniere de faire la fermeture en OpenCV */
	// cv::morphologyEx(edges, closed, cv::MORPH_CLOSE, masque);

	/* Trouver les contours de l'image binarisé & fermé */
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(closed, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	/* Utilisé pour stocker les rectangles de l'image actuelle */
	std::vector<cv::Rect> voitures_maintenant;

	/* Accepter seulment les contours avec une surface normalisé choisi avant */
	for (const auto &contour : contours)
	{
		double area = cv::contourArea(contour); // calcul de la surface du contour

		if (area > 135 && area < 10000)
		{
			// Donner un rectangle approximatif du contour
			cv::Rect boundingBox = cv::boundingRect(contour); // boundingbox: technique par matlab
			voitures_maintenant.push_back(boundingBox);		  // Ajouter le rectangle approximatif à la liste des voitures

			// Afficher le rectangle sur l'image
			cv::rectangle(frame, boundingBox, cv::Scalar(0, 255, 0), 2);
		}
	}

	/* Compter les voitures selon les rectangles approximatifs donnes */
	compter_voitures(voitures_maintenant, voitures_gauche, voitures_droite, frame); // voitures est un pointer int qui point vers voiture_total du programme appelant

	/* Afficher le resultat sur l'image */
	cv::putText(frame, "G: " + std::to_string(voitures_gauche) + " | D: " + std::to_string(voitures_droite), cv::Point(10, 30),
				cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);
}

// Tri des lignes de Hough selon les critères de l'analyse (Objectif: Garder les lignes qui sont plus proches à la route et qui respectent les normes)
void trier_lignes(std::vector<cv::Vec2f> &lignes, cv::Mat &frame, cv::Mat &hough_final, int thres_hough_theta, int rho_threshold, int theta_threshold)
{

	/* Enlever les lignes de Hough qui ont un angle inferieur a thres_hough_theta choisi pendant l'analyse */
	// Tri No 1
	for (size_t j = 0; j < lignes.size();)
	{
		int angle = lignes[j][1] * 180 / CV_PI; // recuperer l'angle en degre car il est donnee en radians par HoughLines()
		if ((angle > thres_hough_theta) && (angle < 180 - thres_hough_theta))
		{
			lignes.erase(lignes.begin() + j); // Enlève l'élément à l'indice j
		}
		else // passer à la prochaine
		{
			j++;
		}
	}

	/* Enlever les lignes restants qui n'ont pas la bonne inclinaison au correct partie de limage */
	// Tri No 2
	for (size_t i = 0; i < lignes.size() - 1; i++) // ATTENTION: Withouth the -1, there was a segmentation Fault in Macos
	{

		float rho = lignes[i][0];
		float theta = lignes[i][1];
		double a = cos(theta), b = sin(theta);

		/* Calcul des coordonnees cartesiennes des points qui compose une ligne.
		C'est la projection d'un vecteur ligne sur le repere de base */
		double x0 = a * rho;
		double y0 = b * rho;

		/* Calcul du point le plus bas sur la ligne */
		// Le point (x0, y0) s'agit d'un point le plus proche de l'origine (selon l'équation de Hough).
		// Le point pt_low est choisi en s'éloignant de ce point le long de la ligne dans une sense (vers le bas dans ce cas là).*/
		cv::Point pt_low(cvRound(x0 + 1000 * (-b)), cvRound(y0 + 1000 * (a)));

		/* Corps du tri No 2 */
		if (pt_low.x > hough_final.cols / 2 && (theta * 180 / CV_PI) < 90) // À droite
		{
			lignes.erase(lignes.begin() + i); // On enlève l'élément à l'indice i
		}
		else
		{
			i++; // On incrémente seulement si on n'a pas effacé l'élément
		}
		if (pt_low.x < hough_final.cols / 2 && (theta * 180 / CV_PI) > 90) // À gauche
		{
			lignes.erase(lignes.begin() + i); // On enlève l'élément à l'indice i
		}
		else
		{
			i++;
		}
	}

	/* Si il y a plusiers lignes qui sont très proche (+- rho_threshold) avec une angle similaire
	(+- theta_threshold), on veut faire un merging et considerer une seule ligne dans ce cas */
	lignes = merge_lignes(lignes, rho_threshold, theta_threshold); // On merge les lignes
}

std::vector<cv::Vec2f> keep_one_line(std::vector<cv::Vec2f> &lignes)
{
	std::vector<cv::Vec2f> one_line;
	float rho_moyenne_gauche = 0;
	float theta_moyen_gauche = 0;
	float rho_moyenne_droite = 0;
	float theta_moyen_droite = 0;

	int nombre_droite = 0; // nombre de droite a droite
	int nombre_gauche = 0; // nombre de droite a gauche

	/* On va parcourir tous les lignes */
	for (size_t i = 0; i < lignes.size(); i++)
	{
		/* Obtenir les coordonees polaires de la ligne */
		float rho = lignes[i][0];
		float theta = lignes[i][1];

		/* Decision si la ligne est plus proche à gauche ou plus proche à droit selon theta et rho */
		if (rho < 0 && theta > CV_PI / 2) // est qu'on à gauche ? Si oui, alors theta est superieur que pi/2
		{
			rho_moyenne_gauche += rho;
			theta_moyen_gauche += theta;
			nombre_gauche++;
		}
		else if (rho > 0 && theta < CV_PI / 2) // on à droite? Si oui, alors theta est inferieur que pi/2
		{
			rho_moyenne_droite += rho;
			theta_moyen_droite += theta;
			nombre_droite++;
		}
	}

	rho_moyenne_gauche = rho_moyenne_gauche / nombre_gauche;
	theta_moyen_gauche = theta_moyen_gauche / nombre_gauche;

	rho_moyenne_droite = rho_moyenne_droite / nombre_droite;
	theta_moyen_droite = theta_moyen_droite / nombre_droite;

	// Ajouter deux éléments au vecteur
	one_line.push_back(cv::Vec2f(rho_moyenne_droite, theta_moyen_droite)); // Premier élément
	one_line.push_back(cv::Vec2f(rho_moyenne_gauche, theta_moyen_gauche)); // Deuxième élément

	return one_line;
}

// Main loop to capture and process video frames
void Camera::play()
{
	// Create main window
	namedWindow("Video", cv::WINDOW_AUTOSIZE);

	// Compute time to wait to obtain wanted framerate
	int timeToWait = 1000 / m_fps;

	// Declaration des matrices des images utilise dans la logique ci-dessous
	Mat gray, edges, hough, hough_final, frame_prec, Movement, frame_with_fixed_lines;

	int threshold_value_1_inf = 200;
	int threshold_value_1_sup = 50;
	int thres_hough = 75;		// Parametre pour la transformation de Hough
	int thres_hough_theta = 70; // Utilisé pour le premier tri des lignes de Hough, pour si ils sont tres inclinées
	int threshold_value = 25;	// Utilisé pour la binarisation en euxieme partie de la logique

	int voiture_total_gauche = 0; // Compteur toral des voitures gauche
	int voiture_total_droite = 0; // Compteur toral des voitures droite
	// int previousCarCount = 0; // Initialize previous car count

	int frame_id = 0; // Compter les frames du video pour appliquer la logique de tranformation de Hough dans une selection des images

	// Ils sont utilisé pour le merging des lignes qui sont tres proches. regarde remarque 1.2
	float rho_threshold = 23;
	float theta_threshold = CV_PI / 180 * 13;

	/* Créer une fenêtre pour afficher l'image */
	cv::namedWindow("Thresholded Image", cv::WINDOW_AUTOSIZE);

	/* Créer les trackbars pour ajuster les seuils */
	// Dans le cadre de notre analyse pour trouver les meulleirs seuils, on
	// a utilisé la fonction cv::createTrackbar pour créer des trackbars pour changer
	// les valeurs des seuils en temps reel pour observer le comportment de l'image en temps réel.
	cv::createTrackbar("Seuil Inferieur", "Thresholded Image", &threshold_value_1_inf, 255);
	cv::createTrackbar("Seuil Superieur", "Thresholded Image", &threshold_value_1_sup, 255);
	cv::createTrackbar("Seuil Hough", "Thresholded Image", &thres_hough, 200);
	cv::createTrackbar("Seuil Hough Theta", "Thresholded Image", &thres_hough_theta, 180);
	cv::createTrackbar("Seuil", "Thresholded Image", &threshold_value, 255);

	// Capture the first frame
	bool isReading = m_cap.read(m_frame);
	frame_prec = m_frame.clone();
	hough_final = m_frame.clone();

	bool first_frame = true; // pour avoir uniquement la premiere image, (vaut false apres la premiere image)

	/* Obtenir l'image en niveau de grey */
	cv::cvtColor(m_frame, gray, cv::COLOR_BGR2GRAY);

	/* Detection des bords selon l'algorithme de Canny */
	cv::Canny(gray, edges, 255, 255);

	/* Application de la transformation de Hough pour dectecter des lignes */
	std::vector<cv::Vec2f> lignes, ligne_fix; // chaque ligne composé par des coordonees polaires (rho, theta), lignes contient toute les lignes et les lignes unqiuement celles une ligne par route
	cv::HoughLines(edges, lignes, 1, CV_PI / 180, thres_hough);

	/*Sauvegarder le frame en copie pour realiser la substraction avec le prochaine frame.
	  Comme ça on peut obtenir l'image Movement qui aura seulment les objets qui bougent seulment */
	// Remarque 1.1
	frame_prec = m_frame.clone();

	while (isReading)
	{
		// Get frame from stream
		isReading = m_cap.read(m_frame);

		if (isReading)
		{
			// Show frame in main window
			imshow("Video", m_frame);
			hough = m_frame.clone(); // meme que Remarque 1.1
			hough_final = m_frame.clone();
			frame_with_fixed_lines = m_frame.clone();

			// ================ Code ==================== //
			// **************** Partie 1 **************** //
			/* Detection des lignes de la rue */

			/* Obtenir l'image en niveau de grey */
			cv::cvtColor(m_frame, gray, cv::COLOR_BGR2GRAY);
			imshow("Gray Scale", gray);

			/* Detection des bords selon l'algorithme de Canny */
			cv::Canny(gray, edges, 255, 255);
			imshow("Canny", edges);

			/* Application de la transformation de Hough pour dectecter des lignes */
			std::vector<cv::Vec2f> lignes; // chaque ligne composé par des coordonees polaires (rho, theta)
			cv::HoughLines(edges, lignes, 1, CV_PI / 180, thres_hough);

			afficher_lignes(lignes, hough);
			cv::imshow("Voies detectees", hough);

			trier_lignes(lignes, hough, hough_final, thres_hough_theta, rho_threshold, theta_threshold);
			afficher_lignes(lignes, hough_final); // Mettre des lignes mergees sur hough_final

			// cv::imshow("Voies detectees Final", hough_final); // Afficher la nouvelle image (hough_final) avec les lignes mergees

			if (frame_id < 10) // Collect lines in the first 10 frames
			{
				/* Si il y a plusiers lignes qui sont très proche (+- rho_threshold) avec une angle similaire
				(+- theta_threshold), on veut faire un merging et considerer une seule ligne dans ce cas */
				lignes = merge_lignes(lignes, rho_threshold, theta_threshold);

				/* Sauvegarder sur un objet global les lignes qu'on garde */
				fixed_lines.insert(fixed_lines.end(), lignes.begin(), lignes.end());
			}

			// **************** Partie 2 **************** //
			/* Detection des voitures et comptage */

			/* Subtraction du frame actuel avec le frame precedent => image avec objets qui bougent seulment (image Movement)*/
			cv::subtract(m_frame, frame_prec, Movement); // init edges
			frame_prec = m_frame.clone();				 // Voir Remarque 1.1
			cv::imshow("Movement", Movement);

			/* Obtenir l'image Movement en niveau de grey */
			cv::cvtColor(Movement, gray, cv::COLOR_BGR2GRAY);

			/* Segmentation de l'image ndg Movement */
			cv::threshold(gray, edges, threshold_value, 255, cv::THRESH_BINARY);

			/*

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

			// int carCount = 0; // Compteur de voitures

			// Filtrer et dessiner les contours
			// for (size_t i = 0; i < contours.size(); i++) {
			//     double area = cv::contourArea(contours[i]);
			//     if ((area<10000) && (area>135)) { // Ajustez ce seuil selon la taille des voitures dans la vidéo
			//         cv::drawContours(hough_final, contours, static_cast<int>(i), cv::Scalar(0, 255, 0), 2);
			//         carCount++; // Compter le contour comme une voiture
			//     }
			// }

			//====================== Vasilis Proposal =========================
			std::vector<cv::Rect> car_rects;

			for (size_t i = 0; i < contours.size(); i++)
			{
				double area = cv::contourArea(contours[i]);
				if ((area < 10000) && (area > 135))
				{
					cv::Rect rect = cv::boundingRect(contours[i]);
					car_rects.push_back(rect); // Ajoute le rectangle à la liste des rectangles des voitures
				}
			}

			// Trying to merge close rectangles. Otherwise we need to merge the contours sohehow
			for (size_t i = 0; i < car_rects.size(); i++)
			{
				for (size_t j = i + 1; j < car_rects.size(); j++)
				{
					if ((car_rects[i] & car_rects[j]).area() > 0 || (cv::norm(car_rects[i].tl() - car_rects[j].tl()) < 50)) // Merge rectangles if they intersect or are close
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
			//======================== End of Vasilis Proposal =========================

			// Compare carCount with previous frame's car count - COMMENT: We need to use a ROI to improve the results. This is what I believe
			if (carCount > previousCarCount)
			{
				car_total += (carCount - previousCarCount); // Add only new cars detected
			}

			// Update the previous car count
			previousCarCount = carCount;

			// Print total number of cars detected
			std::cout << "Total cars detected: " << car_total << std::endl;

			// Afficher le nombre de voitures détectées
			cv::putText(hough_final, "Nombre de voitures: " + std::to_string(carCount), cv::Point(10, 30),
						cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);

			// Afficher les contours détectés
			cv::imshow("Contours des voitures", hough_final);
			cv::imshow("Only cars dilated image", closed);

			/// --------------------------------

			*/

			// if (first_frame)
			// { // Pour que les ligne de la route s'applique sur la matrice final en faisant reference uniquement a la premiere image
			// 	ligne_fix = keep_one_line(lignes);
			// 	first_frame = false;
			// }
			// afficher_lignes(ligne_fix, frame_with_fixed_lines);

			/* Application des lignes trouvés par la transformation de Hough sur le reste de frames */
			if (frame_id >= 10)
			{
				for (const auto &line : fixed_lines)
				{
					afficher_lignes({line}, frame_with_fixed_lines);
				}
			}

			process_frame(frame_with_fixed_lines, edges, voiture_total_gauche, voiture_total_droite);

			cv::imshow("Car Detection", frame_with_fixed_lines);
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

		frame_id++; // Incrementation du frame counter
	}

	printf("Total cars detected: %d\n", voiture_total_gauche + voiture_total_droite);
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
