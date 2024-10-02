// #############################################################################
// # File main.cpp
// # Project in C - Polytech Sorbonne - 2023/2024 - S6
// # Authors: Evinia Anastasopoulou, Yanis Sadoun, Vasileios Filippos Skarleas - All rights reserved.
// #############################################################################

/** Bibliothèques C++ intégrées */
#include <iostream>
#include <fstream>
#include <cmath>

/** Bibliothèques OpenCV intégrées */
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"

/** Bibliothèques C intégrées */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <unistd.h>

#define _USE_MATH_DEFINES

#define ROI_SIZE 450        /// Ajustez cela à la taille de votre région d'intérêt (pas plus de 450)

#define MIN_RED_PIXELS 2000 /// Ajustez ce seuil selon vos besoins

#define MOVEMENT_THRESHOLD 10   /// Ajustez en fonction de la sensibilité de mouvement souhaitée

#define TIME_THRESHOLD 0.189
#define SEUIL 30

/** Déclarations explicites */
unsigned int microsecond = 1000000; // utilisé pour la fonction clock
std::pair<int, int> prev_center = {0, 0};
int time_id = 1;

// using namespace std;
// using namespace cv;

/**
 * @brief Fonction de temporisation
 *
 * Cette fonction met en pause l'exécution du programme pendant un certain nombre de secondes.
 *
 * @param seconds Nombre de secondes de pause
 */
void delay(float seconds)
{
    usleep(seconds * microsecond);
}

/**
 * @brief Filtre l'image via la méthode HSV (Hue Saturation Value)
 *
 * Cette fonction convertit une image de l'espace colorimétrique BGR à HSV, puis applique un filtre pour extraire les régions rouges.
 *
 * @param inputImage Image d'entrée en BGR
 * @param binaryImage Image binaire résultante avec les régions rouges extraites
 */
void process_image(const cv::Mat &inputImage, cv::Mat &binaryImage)
{
    // Convertit de l'espace colorimétrique BGR à HSV
    cv::Mat hsv_image;
    cv::cvtColor(inputImage, hsv_image, cv::COLOR_BGR2HSV);

    // Définir la plage des teintes "rouges" en HSV
    // Remarque : Le rouge s'enroule autour de la roue des teintes, donc deux plages sont nécessaires
    cv::Mat redLowerRange, redUpperRange;
    cv::inRange(hsv_image, cv::Scalar(0, 70, 50), cv::Scalar(10, 255, 255), redLowerRange);    // Rouge inférieur
    cv::inRange(hsv_image, cv::Scalar(170, 70, 50), cv::Scalar(180, 255, 255), redUpperRange); // Rouge supérieur

    /** Autres valeurs HSV */
    // cv::inRange(hsv_image, cv::Scalar(160, 50, 50), cv::Scalar(180, 255, 255), redLowerRange);
    // cv::inRange(hsv_image, cv::Scalar(0, 50, 50), cv::Scalar(10, 255, 255), redUpperRange);

    // Combiner les plages rouges
    cv::addWeighted(redLowerRange, 1.0, redUpperRange, 1.0, 0.0, binaryImage);
}

/**
 * @brief Trouver le centre de la plage de couleur
 *
 * Cette fonction trouve le centre des pixels blancs (valeur 255) dans une image binaire.
 *
 * @param binary_image Image binaire contenant des pixels blancs
 * @return std::pair<int, int> Coordonnées (x, y) du centre des pixels blancs
 */
std::pair<int, int> find_image_center(const cv::Mat &binary_image)
{
    int counter = 0;
    int pos_X = 0;
    int pos_Y = 0;

    for (int i = 0; i < binary_image.rows; i++)
    {
        for (int j = 0; j < binary_image.cols; j++)
        {
            if (binary_image.at<uchar>(i, j) == 255) // Cette valeur est blanche
            {
                pos_X += j;
                pos_Y += i;
                counter++;
            }
        }
    }

    if (counter == 0) // Ce cas ne se produit jamais
    {
        return {-99999, -99999};
    }
    else
    {
        pos_X /= counter; // Position moyenne en x
        pos_Y /= counter; // Position moyenne en y
    }

    return {pos_X, pos_Y};
}


/**
 * @brief Calcule le diamètre de l'objet (version vectorisée même si ce n'est pas un cercle).
 *
 * Cette fonction calcule le diamètre approximatif d'un objet en utilisant la formule du diamètre d'un cercle.
 *
 * @param counter Nombre de pixels représentant l'objet.
 * @return Diamètre approximatif de l'objet.
 */
float calculate_diameter(int counter)
{
    return 2 * std::sqrt(counter / M_PI); // Diamètre approximatif. Si M_PI ne fonctionne pas, utilisez 3.14159265358979323846
}

/**
 * @brief Dessine un cercle de suivi sur le flux vidéo en direct.
 *
 * Cette fonction dessine un point de suivi au centre de l'objet détecté et un cercle représentant le diamètre de l'objet.
 *
 * @param image Image sur laquelle dessiner.
 * @param center Coordonnées du centre de l'objet.
 * @param diameter Diamètre de l'objet.
 */
void draw_circle(cv::Mat &image, const std::pair<int, int> &center, int diameter)
{
    if (center.first == -99999 || center.second == -99999)
    {
        return;
    }

    cv::circle(image, cv::Point(center.first, center.second), 5, cv::Scalar(255, 0, 0), 2);            // Cercle central
    cv::circle(image, cv::Point(center.first, center.second), diameter / 2, cv::Scalar(0, 255, 0), 2); // Cercle de diamètre
}

/**
 * @brief Vérifie si un point est dans la zone morte.
 *
 * Cette fonction vérifie si le centre de l'objet est dans la zone morte définie par les valeurs min et max.
 *
 * @param min Valeur minimale de la zone morte.
 * @param max Valeur maximale de la zone morte.
 * @param center Coordonnées du centre de l'objet.
 * @return Vrai si le centre est dans la zone morte, faux sinon.
 */
bool in_dead_zone(int min, int max, const std::pair<int, int> &center)
{
    if ((center.first <= max && center.first >= min) && (center.second <= max && center.second >= min))
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief Envoie les coordonnées au port série.
 *
 * Cette fonction envoie les coordonnées X et Y au port série pour contrôler les servomoteurs.
 *
 * @param x Coordonnée X à envoyer.
 * @param y Coordonnée Y à envoyer.
 */
void send_coordinates(int x, int y)
{
    FILE *f;

    /* Besoin de changer selon les besoins */
    f = fopen("/dev/ttyACM0", "w");
    if (f == NULL)
    {
        printf("Échec de l'ouverture du fichier\n");
    }
    else
    {
        fprintf(f, "%d %d\n", x, y); // Écrit les données dans le fichier du port série [fonctionne pour Linux] !!! Doit faire attention au changement de ligne
        printf("[%d] Coordonnées X = %d,Y = %d sont écrites.\n", time_id, x, y);
        time_id++;
    }

    fclose(f); 
}

/**
 * @brief Fonction principale qui ouvre la caméra, positionne les servomoteurs par défaut et traite chaque image capturée.
 *
 * Cette fonction utilise le flux vidéo en direct pour capturer des images, détecter les objets rouges,
 * et envoyer les coordonnées pour contrôler les servomoteurs en fonction de la position de l'objet détecté.
 *
 * @param argc Nombre d'arguments.
 * @param argv Tableau d'arguments.
 * @return Code de retour.
 */
int main(int argc, char **argv)
{
    /** Besoin d'utiliser le flux vidéo en direct et d'effectuer des captures d'images pour le traitement (remplacez 0 par l'index de votre caméra si nécessaire) */
    cv::VideoCapture capture(0); // 2 -> emplacement le plus probable de la caméra

    if (!capture.isOpened())
    {
        std::cerr << "Erreur d'ouverture du périphérique de capture vidéo!" << std::endl;
        return -1;
    }

    int dead_min = ROI_SIZE / 2 - SEUIL;
    int dead_max = ROI_SIZE / 2 + SEUIL;

    /** Création des images pour le flux de traitement et initialisations */
    cv::Mat color_image, binary_image, roi_image;
    int servo1_position = 90;
    int servo2_position = 90;

    int position_first, position_second;

    while (true)
    {
        /** Capture d'une image depuis la caméra */
        if (!capture.read(color_image))
        {
            std::cerr << "Erreur: Image colorée capturée vide" << std::endl;
            break;
        }

        /** Déclaration de la région d'intérêt (ROI) */
        roi_image = color_image(cv::Rect(0, 0, ROI_SIZE, ROI_SIZE));

        /** Traitement de l'image */
        process_image(roi_image, binary_image);

        /** Seuil */
        int num_red_pixels = cv::countNonZero(binary_image);
        if (num_red_pixels > MIN_RED_PIXELS)
        {
            auto center = find_image_center(binary_image);
            if (!in_dead_zone(dead_min, dead_max, center))
            {
                float delay_multiplier = 1.0;

                ///
                /// Mise à jour du délai
                ///
                if (abs(prev_center.first - center.first) + abs(prev_center.second - center.second) > MOVEMENT_THRESHOLD)
                {
                    delay_multiplier = 0.5;
                }

                int diameter = calculate_diameter(center.first != -1 ? center.second : 0);
                draw_circle(roi_image, center, diameter);

                delay(delay_multiplier * TIME_THRESHOLD);

                int delta_x, delta_y;
                delta_x = center.first - ROI_SIZE / 2;
                delta_y = center.second - ROI_SIZE / 2;

                position_first = servo1_position - delta_x * 0.01;
                position_second = servo2_position + delta_y * 0.01;

                servo1_position = std::min(std::max(position_first, 20), 220);
                servo2_position = std::min(std::max(position_second, 20), 220);

                /** Envoi des coordonnées à Arduino */
                send_coordinates(servo1_position, servo2_position);
                //printf("Nb pixels: %d\n\n", num_red_pixels);
            }
            else
            {
                printf("Zone morte: X[%d, %d] Y[%d, %d]\n", dead_min, dead_max, dead_min, dead_max);
            }
            prev_center = center;
        }
        else
        {
            printf("[%d] Pas assez de rouge détecté. Aucune coordonnée envoyée\n\n", time_id);
            time_id++;
        }

        /** Flux vidéo en temps réel utilisé pour les tests */
        cv::imshow("Image binaire", binary_image);
        cv::imshow("Image suivie", roi_image); // Affiche l'image ROI avec le suivi en direct
        cv::imshow("Image binaire", binary_image);

        /** Sur n'importe quel flux vidéo, appuyez sur n'importe quelle touche pour quitter et fermer le programme */
        if (cv::waitKey(30) >= 0)
        {
            break;
        }
    }

    return 0;
}
