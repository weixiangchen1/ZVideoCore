#pragma once
#include "zcodec.h"
#include "utils.h"

class ZDecode: public ZCodec {
public:
    /////////////////////////////////////////////////
    // 向解码线程发送数据 (线程安全)
    // @para pPacket 需要解码的AVPacket
    // @return 失败返回false 成功返回true
    bool SendPacket(const AVPacket* pPacket);

    /////////////////////////////////////////////////
    // 接收解码后的AVFrame (线程安全)
    // @para pFrame 解码后数据
    // @return 1: 成功并填充 pFrame
    // 0: 需要更多数据（AVERROR(EAGAIN)）
    // -1: 错误或 AVERROR_EOF
    int RecvFrame(AVFrame* pFrame);

    /////////////////////////////////////////////////
    // 获取缓存中的AVFrame
    std::vector<AVFrame*> GetCacheFrames();

    /////////////////////////////////////////////////
    // 初始化硬件加速
    // 4 == AV_HWDEVICE_TYPE_DXVA2
    bool InitHWA(int iType = 4);
};

