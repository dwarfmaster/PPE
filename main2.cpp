
/* This main is only designed to be used as a test */
#include <iostream>
#include <vector>
#include <string>
#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv/highgui.h>
#include "calib.hpp"
#include "compute.hpp"

int main(int, char**)
{
	CvCapture* c1 = cvCaptureFromCAM(0);
	cvNamedWindow("Capture1", CV_WINDOW_AUTOSIZE);
	CvCapture* c2 = cvCaptureFromCAM(1);
    cvNamedWindow("Capture2", CV_WINDOW_AUTOSIZE);

    CvSize size;
    size.width  = cvGetCaptureProperty(c1, CV_CAP_PROP_FRAME_WIDTH);
    size.height = cvGetCaptureProperty(c1, CV_CAP_PROP_FRAME_HEIGHT);
    std::cout << "Capturing using a " << size.width << "x" << size.height << " webcam." << std::endl;

	if(c1 && c2)
	{
		IplImage* f1 = cvQueryFrame(c1);
        IplImage* f2 = cvQueryFrame(c2);
        libcv::CalibCam cc;
        cc.load("save");
        libcv::Compute cpt(&cc);

		printf("PRESS SPACE TO QUIT \n\n");

		while(1)
		{
			f1 = cvQueryFrame(c1);
			cvShowImage("Capture1",f1);
            f2 = cvQueryFrame(c2);
			cvShowImage("Capture2",f2);
            cpt.process(f1, f2);

            char c = cvWaitKey(1000);
            if(c == ' ')
                break;
		}
	}
	else printf("Video stream not found\n\n");

	printf("Appuyez sur une touche ...\n");
	cvWaitKey(0);
    return 0;
}

