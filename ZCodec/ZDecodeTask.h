#pragma once
#include "ZThread.h"
#include "ZDecode.h"

class ZDecodeTask : public ZThread {
public:
    // 打开解码器
    bool OpenDecodec(AVCodecParameters* pCodecParam);

    // 责任链处理函数
    void DoWork(AVPacket* pPacket) override;
    
    // 线程入口函数
    void threadFunc();
    
    // 返回需要渲染的AVFrame 没有数据返回null
    // 返回的AVFrame资源需要用户释放
    AVFrame* GetFrame();

private:
    std::mutex m_mutex;
    ZDecode m_decodec;
    ZAVPacketList m_listPackets;
    AVFrame* m_pFrame = nullptr;
    bool m_bCanRender = false;
};

