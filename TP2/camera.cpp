#include "camera.h"
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>


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
	if(!(iss >> devid))
	{
		isOpen = m_cap.open(filename.c_str());
	}
	else 
	{
		isOpen = m_cap.open(devid);
	}
	
	if(!isOpen)
	{
		std::cerr << "Unable to open video file." << std::endl;
		return false;
	}
	
	// set framerate, if unable to read framerate, set it to 30
	m_fps = m_cap.get(CV_CAP_PROP_FPS);
	if(m_fps == 0)
		m_fps = 30;
}

void Camera::play()
{
	// Create main window
	namedWindow("Video", CV_WINDOW_AUTOSIZE);
	bool isReading = true;
	// Compute time to wait to obtain wanted framerate
	int timeToWait = 1000/m_fps;
		
	while(isReading)
	{
		// Get frame from stream
		isReading = m_cap.read(m_frame);
		
		if(isReading)
		{
			// Show frame in main window
			imshow("Video",m_frame);		
		
			/// A completer
			
			
			
			/// --------------------------------			
		}
		else
		{
			std::cerr << "Unable to read device" << std::endl;
		}
		
		// If escape key is pressed, quit the program
		if(waitKey(timeToWait)%256 == 27)
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
}


























