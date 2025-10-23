#include "zavparam.h"
extern "C" {
#include <libavformat/avformat.h>
}

ZAVParam* ZAVParam::CreateAVParam() {
    return new ZAVParam();
}

ZAVParam::~ZAVParam() {
    if (pParam) {
        avcodec_parameters_free(&pParam);
    }
    if (pTimebase) {
        delete pTimebase;
        pTimebase = nullptr;
    }
}

ZAVParam::ZAVParam() {
    pParam = avcodec_parameters_alloc();
    pTimebase = new AVRational();
}