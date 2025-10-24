#include "zmux.h"
#include "utils.h"

ZMux::ZMux() {}

ZMux::~ZMux() {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_pSrcAudioTimeBase) {
        delete m_pSrcAudioTimeBase;
        m_pSrcAudioTimeBase = nullptr;
    }
    if (m_pSrcVideoTimeBase) {
        delete m_pSrcVideoTimeBase;
        m_pSrcVideoTimeBase = nullptr;
    }
}

AVFormatContext* ZMux::CreateMuxContext(const char* strURL, AVCodecParameters* pVideoParam, AVCodecParameters* pAudioParam) {
    AVFormatContext* pFormatCtx = nullptr;
    // 创建封装上下文
    int iRet = avformat_alloc_output_context2(&pFormatCtx, nullptr, nullptr, strURL);
    if (iRet != 0) {
        std::cerr << "avformat_alloc_output_context2 error: " <<
            Utils::GetAVErrorMessage(iRet).c_str() << std::endl;
        return nullptr;
    }

    // 添加音频流和视频流
    if (pVideoParam != nullptr) {
        AVStream* pVideoStream = avformat_new_stream(pFormatCtx, nullptr);   // 视频流
        pVideoStream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
        avcodec_parameters_copy(pVideoStream->codecpar, pVideoParam);
    }
    if (pAudioParam != nullptr) {
        AVStream* pAudioStream = avformat_new_stream(pFormatCtx, nullptr);   // 音频流
        pAudioStream->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
        avcodec_parameters_copy(pAudioStream->codecpar, pAudioParam);
    }

    // 打开输出IO
    iRet = avio_open(&pFormatCtx->pb, strURL, AVIO_FLAG_WRITE);
    if (iRet != 0) {
        std::cerr << "avio_open error: " <<
            Utils::GetAVErrorMessage(iRet).c_str() << std::endl;
        return nullptr;
    }
    // 打印输出封装信息
    av_dump_format(pFormatCtx, 0, pFormatCtx->url, 1);

    return pFormatCtx;
}

bool ZMux::WriteHead() {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_pFormatCtx == nullptr) {
        return false;
    }
    int iRet = avformat_write_header(m_pFormatCtx, nullptr);
    if (iRet != 0) {
        std::cerr << "avformat_write_header error: " <<
            Utils::GetAVErrorMessage(iRet).c_str() << std::endl;
        return false;
    }
    // 打印输出封装信息
    av_dump_format(m_pFormatCtx, 0, m_pFormatCtx->url, 1);
    m_lBeginAudioPts = -1;
    m_lBeginVideoPts = -1;

    return true;
}

bool ZMux::WriteFrame(AVPacket* pPacket) {
    if (pPacket == nullptr) {
        return false;
    }
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_pFormatCtx == nullptr) {
        return false;
    }
    // 未读取到pts
    if (pPacket->pts == AV_NOPTS_VALUE) {
        pPacket->pts = 0;
        pPacket->dts = 0;
    }
    if (pPacket->stream_index == m_iVideoIndex) {
        if (m_lBeginVideoPts < 0) {
            m_lBeginVideoPts = pPacket->pts;
        }
        lock.unlock();
        RescaleTimeParam(pPacket, m_lBeginVideoPts, m_pSrcVideoTimeBase);
        lock.lock();
    } else if (pPacket->stream_index == m_iAudioIndex) {
        if (m_lBeginAudioPts < 0) {
            m_lBeginAudioPts = pPacket->pts;
        }
        lock.unlock();
        RescaleTimeParam(pPacket, m_lBeginAudioPts, m_pSrcAudioTimeBase);
        lock.lock();
    }
    std::cout << pPacket->pts << std::flush;
    int iRet = av_interleaved_write_frame(m_pFormatCtx, pPacket);
    if (iRet != 0) {
        std::cerr << "av_interleaved_write_frame error: " <<
            Utils::GetAVErrorMessage(iRet).c_str() << std::endl;
        return false;
    }
    return true;
}

bool ZMux::WriteTail() {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_pFormatCtx == nullptr) {
        return false;
    }
    av_interleaved_write_frame(m_pFormatCtx, nullptr);  // 写入缓冲
    int iRet = av_write_trailer(m_pFormatCtx);
    if (iRet != 0) {
        std::cerr << "avformat_write_header error: " <<
            Utils::GetAVErrorMessage(iRet).c_str() << std::endl;
        return false;
    }
    return true;
}

void ZMux::SetSrcVideoTimeBase(AVRational* pTimeBase) {
    if (pTimeBase == nullptr) {
        return;
    }
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_pSrcVideoTimeBase == nullptr) {
        m_pSrcVideoTimeBase = new AVRational();
    }
    *m_pSrcVideoTimeBase = *pTimeBase;
}

void ZMux::SetSrcAudioTimeBase(AVRational* pTimeBase) {
    if (pTimeBase == nullptr) {
        return;
    }
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_pSrcAudioTimeBase == nullptr) {
        m_pSrcAudioTimeBase = new AVRational();
    }
    *m_pSrcAudioTimeBase = *pTimeBase;
}
