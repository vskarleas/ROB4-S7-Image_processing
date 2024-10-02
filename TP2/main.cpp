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
