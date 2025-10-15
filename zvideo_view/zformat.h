#pragma once
#include <iostream>
#include <mutex>
struct AVPacket;
struct AVFormatContext;
struct AVCodecParameters;
struct ZRational {
    int num;    ///< Numerator
    int den;    ///< Denominator
};

// 封装和解封装的基类
class ZFormat {
public:
    /////////////////////////////////////////////////
    // 复制流参数 (线程安全)
    // @para iStreamIndex 需要复制的音频或视频流索引
    // @para dstParameter 输出参数
    // @return 成功返回true 失败返回false
    bool CopyParam(int iStreamIndex, AVCodecParameters* dstParameter);

    /////////////////////////////////////////////////
    // 设置封装或解封装上下文 (线程安全)
    // 设置会自动清理上次设置的上下文 传递null会关闭上下文
    // @para pFormatCtx 封装和解封装上下文
    void SetFormatContext(AVFormatContext* pFormatCtx);

    /////////////////////////////////////////////////
    // 获取音频或视频的索引
    // @return 音频或视频的索引
    int GetVideoIndex();
    int GetAudioIndex();

    /////////////////////////////////////////////////
    // 获取音频或视频的时间基
    // @return 音频或视频的时间基
    ZRational GetVideoTimeBase();
    ZRational GetAudioTimeBase();

protected:
    AVFormatContext* m_pFormatCtx;          // 封装和解封装上下文
    std::mutex m_mutex;
    int m_iVideoIndex = -1;                  // 视频流索引
    int m_iAudioIndex = -1;                  // 音频流索引
    ZRational m_videoTimeBase = { 1,25 };
    ZRational m_audioTimeBase = { 1, 44100 };
};

