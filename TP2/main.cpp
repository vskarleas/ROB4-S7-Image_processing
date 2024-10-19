// #############################################################################
// # file_input main.cpp
// # Traitment d'image TP2 - Polytech Sorbonne - 2024/2025 - S7
// # Auteurs : Vasileios Filippos Skarleas, Rami Aridi - Tous droits réservés.
// #############################################################################

#include "camera.h"


using namespace cv;
using namespace std;

int main(void)
{
	Camera myCam;
		
	myCam.open("cctv.avi");
	
	myCam.play();
	
	myCam.close();
	
	return 1;
	
}
