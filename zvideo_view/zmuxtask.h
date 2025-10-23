#pragma once
#include "zthread.h"
#include "zmux.h"

class ZMuxTask : public ZThread {
public:
    /////////////////////////////////////////////////////////
    // 打开封装器 
    // @para strURL 输出地址
    // @para pVideoParam 视频参数
    // @para pOriginVideoTimeBase 原视频时间基数
    // @para pAudioParam 音频参数
    // @para pOriginVideoTimeBase 原音频时间基数
    // @return 打开封装器成功返回true 失败返回false
    bool OpenMux(std::string strURL,
        AVCodecParameters* pVideoParam = nullptr, AVRational* pSrcVideoTimeBase = nullptr,
        AVCodecParameters* pAudioParam = nullptr, AVRational* pSrcAudioTimeBase = nullptr);

    // 线程入口函数
    void threadFunc() override;

    // 责任链处理函数
    void DoWork(AVPacket* pPacket) override;

private:
    ZMux m_mux;
    std::mutex m_mutex;
    ZAVPacketList m_listPackets;
};

