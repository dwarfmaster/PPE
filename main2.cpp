
/* This main is only designed to be used as a test */
#include <iostream>
#include <vector>
#include <string>
#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv/highgui.h>
#include "cameras.hpp"

int main(int, char**)
{
    libcv::Cameras cams(1, 2);
    cvNamedWindow("Left",  CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Right", CV_WINDOW_AUTOSIZE);
    std::cout << "Capturing using a " << cams.size().width << "x" << cams.size().height << " webcam." << std::endl;

    while(1)
    {
        cams.queryFrames();
        /* TODO */
    }
    return 0;
}

