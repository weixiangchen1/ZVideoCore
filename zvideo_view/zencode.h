#pragma once
#include <mutex>
#include <vector>
struct AVPacket;
struct AVFrame;
struct AVCodecContext;
class ZEncode {
public:
    /////////////////////////////////////////////////
    // 创建编码器上下文
    // @para iCodeId 编码器id与ffmpeg对应
    // @return 编码器上下文 失败返回nullptr
    static AVCodecContext* CreateContext(int iCodeId);

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
    bool OpenEncoder();

    /////////////////////////////////////////////////
    // 编码数据 (线程安全)
    // @para pFrame 资源由用户维护
    // @return 失败返回null 返回的AVPacket由用户通过av_packet_free清理资源
    AVPacket* EncodeData(const AVFrame* pFrame);

    /////////////////////////////////////////////////
    // 根据编码上下文创建AVFrame 资源由用户维护
    AVFrame* CreateFrame();

    /////////////////////////////////////////////////
    // 获取缓存中的AVPacket
    std::vector<AVPacket*> GetFlushPacket();

private:
    AVCodecContext* m_pCodecCtx = nullptr;
    std::mutex m_mutex;
};

