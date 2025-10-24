#pragma once
#include <iostream>
#include <mutex>

class ZAVParam;
struct AVPacket;
struct AVRational;
struct AVFormatContext;
struct AVCodecContext;
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
    bool CopyParam(int iStreamIndex, AVCodecContext* dstCodecCtx);

    /////////////////////////////////////////////////
    // 复制视频参数 (线程安全)
    // @return 返回视频参数的智能指针
    std::shared_ptr<ZAVParam> CopyVideoParam();
    std::shared_ptr<ZAVParam> CopyAudioParam();

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

    /////////////////////////////////////////////////
    // 根据time_base换算时间参数
    // @para pPacket 需要进行转换的视频帧
    // @para lOffsetPts 时间戳偏移量
    // @para timeBase 表示 pPacket 原始时间戳
    // @return 成功返回true 失败返回false
    bool RescaleTimeParam(AVPacket* pPacket, long long lOffsetPts, ZRational timeBase);
    bool RescaleTimeParam(AVPacket* pPacket, long long lOffsetPts, AVRational* pTimeBase);

    /////////////////////////////////////////////////
    // 获取视频编码器ID
    // @return 视频编码器ID
    int GetVideoCodecId();

    /////////////////////////////////////////////////
    // 设置超时时间
    // @para iTime 超时时间
    void SetTimeoutMs(int iTime);

    /////////////////////////////////////////////////
    // 判断是否超时
    // @return 超时返回true 未超时返回false
    bool IsTimeout();

    /////////////////////////////////////////////////
    // 判断是否连接成功
    // @return 连接成功返回true 失败返回false
    bool IsConnected();

protected:
    int m_iTimeoutMs = 0;                   // 超时时间
    long long m_lLastTime = 0;              // 上一次接收到数据的时间
    bool m_bIsConnected = false;            // 是否连接成功
    AVFormatContext* m_pFormatCtx;          // 封装和解封装上下文
    std::mutex m_mutex;
    int m_iVideoIndex = -1;                  // 视频流索引
    int m_iAudioIndex = -1;                  // 音频流索引
    ZRational m_videoTimeBase = { 1,25 };
    ZRational m_audioTimeBase = { 1, 44100 };
    int m_iVideoCodecId = 0;                 // 编码器ID
};

