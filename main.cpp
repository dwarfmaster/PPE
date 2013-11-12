
/* This main is only designed to be used as a test */
#include <iostream>
#include <vector>
#include <string>
#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv/highgui.h>
#include "calib.hpp"

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
        cc.start(9, 6, 2.5, size);
        std::cout << "CalibCam started" << std::endl;

		printf("PRESS SPACE TO QUIT \n\n");
        int nb = 0;
        const int max = 8;

		while(1)
		{
			f1 = cvQueryFrame(c1);
			cvShowImage("Capture1",f1);
            f2 = cvQueryFrame(c2);
			cvShowImage("Capture2",f2);

            if(cc.compute(f1, f2)) {
                ++nb;
                std::cout << "Found a chessboard : " << nb << "/" << max << "." << std::endl;
                if(nb >= max)
                    break;
            }

            char c = cvWaitKey(1000);
            if(c == ' ')
                break;
		}
        cc.end();
        cc.save("save/");
	}
	else printf("Video stream not found\n\n");

	printf("Appuyez sur une touche ...\n");
	cvWaitKey(0);
    return 0;
}

