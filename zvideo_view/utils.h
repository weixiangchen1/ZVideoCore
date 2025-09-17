#pragma once
#include <iostream>
#include <thread>
#include <chrono>

class Utils {
public:
    static void MSleep(unsigned int iMilliseconds) {
        clock_t begin = clock();
        for (int i = 0; i < iMilliseconds; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            if ((clock() - begin) / (CLOCKS_PER_SEC / 1000) >= iMilliseconds) {
                break;
            }
        }
    }
};

