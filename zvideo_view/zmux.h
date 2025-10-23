#pragma once
#include "zformat.h"
// 封装类
struct AVRational;
struct AVCodecParameters;
class ZMux : public ZFormat {
public:
    ZMux();
    ~ZMux();
    /////////////////////////////////////////////////
    // 创建封装的上下文
    // @para strURL 封装地址
    // @para pVideoParam 视频参数
    // @para pAudioParam 音频参数
    // @return 封装的上下文 失败返回null
    static AVFormatContext* CreateMuxContext(const char* strURL,
        AVCodecParameters* pVideoParam = nullptr,
        AVCodecParameters* pAudioParam = nullptr);
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

    /////////////////////////////////////////////////
    // 设置时间基数
    // @para pTimeBase 时间基数
    void SetSrcVideoTimeBase(AVRational* pTimeBase);
    void SetSrcAudioTimeBase(AVRational* pTimeBase);

private:
    AVRational* m_pSrcVideoTimeBase = nullptr;
    AVRational* m_pSrcAudioTimeBase = nullptr;

    long long m_lBeginVideoPts = -1;            // 原视频开始时间
    long long m_lBeginAudioPts = -1;            // 原音频开始时间
};

