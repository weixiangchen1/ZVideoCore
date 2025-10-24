#pragma once
#include <mutex>
#include <vector>
#include "Utils.h"

// 编解码器的基类
class ZCodec {
public:
    /////////////////////////////////////////////////
    // 创建编解码器上下文
    // @para iCodecId 编解码器id与ffmpeg对应
    // @para iCodecType 标识编码器与解码器 0-编码器 1-解码器
    // @return 编解码器上下文 失败返回nullptr
    static AVCodecContext* CreateCodecContext(int iCodecId, int iCodecType);

    /////////////////////////////////////////////////
    // 设置编码器上下文 资源由对象维护 (线程安全)
    // m_pCodecCtx不为null 则先清理资源
    // @para pCodecCtx 编码器上下文
    void SetCodecContext(AVCodecContext* pCodecCtx);

    /////////////////////////////////////////////////
    // 设置编码器参数 (线程安全)
    bool SetAVOpt(const char* pKey, const char* pValue);
    bool SetAVOpt(const char* pKey, const int iValue);

    /////////////////////////////////////////////////
    // 打开编码器 (线程安全)
    bool OpenCodec();

    /////////////////////////////////////////////////
    // 根据编码上下文创建AVFrame 资源由用户维护
    AVFrame* CreateFrame();

protected:
    AVCodecContext* m_pCodecCtx = nullptr;
    std::mutex m_mutex;
};

