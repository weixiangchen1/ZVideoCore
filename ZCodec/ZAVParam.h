#pragma once
#include "Utils.h"
// 音视频参数类
class ZAVParam {
public:
    AVCodecParameters* pParam = nullptr;         // 音视频参数
    AVRational* pTimebase = nullptr;             // 时间基数

    static ZAVParam* CreateAVParam() {
        return new ZAVParam();
    }

    ~ZAVParam() {
        if (pParam) {
            avcodec_parameters_free(&pParam);
        }
        if (pTimebase) {
            delete pTimebase;
            pTimebase = nullptr;
        }
    }
private:
    ZAVParam() {
        pParam = avcodec_parameters_alloc();
        pTimebase = new AVRational();
    }
};
