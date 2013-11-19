
/* This main is only designed to be used as a test */
#include <iostream>
#include <vector>
#include <string>
#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv/highgui.h>
#include "calib.hpp"
#include "compute.hpp"
#include "cameras.hpp"

int main(int, char**)
{
    libcv::Cameras cams(1, 2);
    cvNamedWindow("Left",  CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Right", CV_WINDOW_AUTOSIZE);
    std::cout << "Capturing using a " << cams.size().width << "x" << cams.size().height << " webcam." << std::endl;

    libcv::CalibCam cc;
    cc.load("save");
    libcv::Compute cpt(&cc);

    printf("PRESS SPACE TO QUIT \n\n");

    while(1)
    {
        cams.queryFrames();
        cvShowImage("Left",  cams.left());
        cvShowImage("Right", cams.right());
        cpt.process(cams.left(), cams.right());

        char c = cvWaitKey(1000);
        if(c == ' ')
            break;
    }

    printf("Appuyez sur une touche ...\n");
    cvWaitKey(0);
    return 0;
}

