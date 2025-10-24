#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#ifndef LINUX_VERSION
#include "Windows.h"
#else
#include "unistd.h"
#include "sys/sysinfo.h"
#endif
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavutil/opt.h"
#include "libavformat/avformat.h"
}

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

    static long long GetCurrentTimestamp() {
        return clock() / (CLOCKS_PER_SEC / 1000);
    }

    static int GetSystemCPUNum() {
#ifndef LINUX_VERSION
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        return sysInfo.dwNumberOfProcessors;
#else
        return sysconf(_SC_NPROCS_ONLN);
#endif
    }

    static std::string GetAVErrorMessage(int iErrorId) {
        char strMsg[1024] = { 0 };
        av_strerror(iErrorId, strMsg, sizeof(strMsg) - 1);
        return std::string(strMsg);
    }

    static void ReleaseFrame(AVFrame** pFrame) {
        if (!pFrame || !(*pFrame)) {
            return;
        }
        av_frame_free(pFrame);
    }
};

