#include "zmux.h"
#include "utils.h"

AVFormatContext* ZMux::CreateMuxContext(const char* strURL) {
    AVFormatContext* pFormatCtx = nullptr;
    // 创建封装上下文
    int iRet = avformat_alloc_output_context2(&pFormatCtx, nullptr, nullptr, strURL);
    if (iRet != 0) {
        std::cerr << "avformat_alloc_output_context2 error: " <<
            Utils::GetAVErrorMessage(iRet).c_str() << std::endl;
        return nullptr;
    }

    // 添加音频流和视频流
    AVStream* pVideoStream = avformat_new_stream(pFormatCtx, nullptr);   // 视频流
    pVideoStream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    AVStream* pAudioStream = avformat_new_stream(pFormatCtx, nullptr);   // 音频流
    pAudioStream->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;

    // 打开输出IO
    iRet = avio_open(&pFormatCtx->pb, strURL, AVIO_FLAG_WRITE);
    if (iRet != 0) {
        std::cerr << "avio_open error: " <<
            Utils::GetAVErrorMessage(iRet).c_str() << std::endl;
        return nullptr;
    }

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

    return true;
}

bool ZMux::WriteFrame(AVPacket* pPacket) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_pFormatCtx == nullptr) {
        return false;
    }
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
