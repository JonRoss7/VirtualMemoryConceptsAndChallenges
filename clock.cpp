#include "clock.h"
#include <iostream>

Clock::Clock(int frameCount) {
    this->frameCount = frameCount;
    this->clockHand = 0;
    
    frames.resize(frameCount);
    for (int i = 0; i < frameCount; ++i) {
        frames[i].pid = -1;
        frames[i].pageNumber = -1;
        frames[i].referenced = false;
    }
}

int Clock::evictAndReplace(int pid, int pageNumber, int& evictedPage) {
    while (true) {
        if (frames[clockHand].referenced == true) {
            frames[clockHand].referenced = false;
            clockHand = (clockHand + 1) % frameCount;
        } else {
            
            if (frames[clockHand].pid != -1) {
                evictedPage = frames[clockHand].pageNumber;
            } else {
                evictedPage = -1;
            }

            frames[clockHand].pid = pid;
            frames[clockHand].pageNumber = pageNumber;
            frames[clockHand].referenced = true;

            int victimFrameIndex = clockHand;
            
            clockHand = (clockHand + 1) % frameCount;

            return victimFrameIndex;
        }
    }
}