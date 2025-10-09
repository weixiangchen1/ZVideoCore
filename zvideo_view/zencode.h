#pragma once
#include <mutex>
#include <vector>
#include "zcodec.h"
#include "utils.h"

class ZEncode: public ZCodec {
public:
    /////////////////////////////////////////////////
    // 编码数据 (线程安全)
    // @para pFrame 资源由用户维护
    // @return 失败返回null 返回的AVPacket由用户通过av_packet_free清理资源
    AVPacket* EncodeData(const AVFrame* pFrame);

    /////////////////////////////////////////////////
    // 获取缓存中的AVPacket
    std::vector<AVPacket*> GetCachePackets();
};

