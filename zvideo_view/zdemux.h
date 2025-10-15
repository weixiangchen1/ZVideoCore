#pragma once
#include "zformat.h"
class ZDemux : public ZFormat {
public:
    /////////////////////////////////////////////////
    // 创建解封装的上下文
    // @para strURL 解封装地址 支持rtsp
    // @return 解封装的上下文 失败返回null
    static AVFormatContext* CreateDemuxContext(const char* strURL);

    /////////////////////////////////////////////////
    // 读取一帧数据
    // @para pPacket 输出数据
    // @return 成功返回true 失败返回false
    bool ReadFrame(AVPacket* pPacket);
};

