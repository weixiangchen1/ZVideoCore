#pragma once
#include "zthread.h"
#include "zdemux.h"

class ZDemuxTask : public ZThread {
public:
    /////////////////////////////////////////////////
    // 创建打开解封装器
    // @para strURL 解封装地址 支持rtsp
    // @para iTimeoutMs 连接超时时间 单位毫秒
    // @return 打开成功返回true 失败返回false
    bool OpenDemux(std::string strURL, int iTimeoutMs = 1000);
    
    // 线程入口函数
    void threadFunc();

    // 复制视频参数
    std::shared_ptr<ZAVParam> CopyVideoParam();

private:
    int m_iTimeoutMs = 0;
    std::string m_strURL;
    ZDemux m_demux;
};

