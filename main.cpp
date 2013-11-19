
/* This main is only designed to be used as a test */
#include <iostream>
#include <vector>
#include <string>
#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv/highgui.h>
#include "calib.hpp"
#include "cameras.hpp"

int main(int, char**)
{
    libcv::Cameras cams(1, 2);
    cvNamedWindow("Left",  CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Right", CV_WINDOW_AUTOSIZE);
    std::cout << "Capturing using a " << cams.size().width << "x" << cams.size().height << " webcam." << std::endl;

    libcv::CalibCam cc;
    cc.start(9, 6, 2.5, cams.size());
    std::cout << "CalibCam started" << std::endl;

    printf("PRESS SPACE TO QUIT \n\n");
    int nb = 0;
    const int max = 16;

    while(1)
    {
        if(!cams.queryFrames())
            throw libcv::CamException("Couldn't query frames.");
        std::cout << "New frame." << std::endl;

        if(cc.compute(cams.left(), cams.right())) {
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

    printf("Appuyez sur une touche ...\n");
    cvWaitKey(0);
    return 0;
}

