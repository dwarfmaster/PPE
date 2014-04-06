
#include <iostream>
#include <vector>
#include <string>
#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv/highgui.h>
#include "cameras.hpp"
#include "abod.hpp"

int main(int, char**)
{
    libcv::Cameras cams(1);
    if(!cams.loaded())
        return 1;
    Abod abod;
    if(!abod.load("ground"))
        return 1;

    cv::Mat img;
    while(1)
    {
        cams.queryFrames();
        img = cams.get();
        abod.compute(img, true);

        while((char)cvWaitKey(0) != ' ');
    }
    return 0;
}

