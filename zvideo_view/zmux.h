#pragma once
#include "zformat.h"
// 封装类
class ZMux : public ZFormat {
public:
    /////////////////////////////////////////////////
    // 创建封装的上下文
    // @para strURL 封装地址
    // @return 封装的上下文 失败返回null
    static AVFormatContext* CreateMuxContext(const char* strURL);

    /////////////////////////////////////////////////
    // 写入文件头部数据
    // @return 成功返回true 失败返回false
    bool WriteHead();

    /////////////////////////////////////////////////
    // 写入一帧数据
    // @para pPacket 输入数据
    // @return 成功返回true 失败返回false
    bool WriteFrame(AVPacket* pPacket);

    /////////////////////////////////////////////////
    // 写入文件尾部数据
    // @return 成功返回true 失败返回false
    bool WriteTail();
};

