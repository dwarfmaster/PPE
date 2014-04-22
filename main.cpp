
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
        /* Getting and processing video stream. */
        cams.queryFrames();
        cv::Mat fr = cams.get();
        follow.process(fr);
        abod.compute(fr);

        /* Following. */
        switch(follow.pos()) {
            case Follower::Left:
                serial.write(0x50);
                break;
            case Follower::Right:
                serial.write(0x51);
                break;
            case Follower::Center:
                serial.write(0x52);
                break;
            default:
                serial.write(0x53);
                break;
        }

        /* Getting data from android. */
        size = serial.read(buffer, 4096);
        if(size > 0) {
            for(int i = 0; i < size; ++i)
                std::cout << "Received order " << std::hex << (int)buffer[i] << std::endl;
        }
    }

    return 0;
}

