#pragma once
// 音视频参数类
struct AVCodecParameters;
struct AVRational;
class ZAVParam {
public:
    AVCodecParameters* pParam = nullptr;         // 音视频参数
    AVRational* pTimebase = nullptr;             // 时间基数
    static ZAVParam* CreateAVParam();
    ~ZAVParam();
private:
    ZAVParam();
};

