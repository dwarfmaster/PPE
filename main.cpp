
#include <iostream>
#include <vector>
#include <string>
#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv/highgui.h>
#include "cameras.hpp"
#include "abod.hpp"
#include "follower.hpp"
#include "serial.hpp"

int main(int, char**)
{
    libcv::Cameras cams(1);
    if(!cams.loaded())
        return 1;
    Abod abod;
    if(!abod.load("ground"))
        return 1;
    Follower follow;
    Serial serial;
    if(!serial.open())
        return 1;

    char buffer[4096];
    int size;

    cv::Mat img;
    while(1)
    {
        /* TODO */
    }

    return 0;
}

