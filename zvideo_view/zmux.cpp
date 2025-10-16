#include "zmux.h"
#include "utils.h"

AVFormatContext* ZMux::CreateMuxContext(const char* strURL) {
    AVFormatContext* pFormatCtx = nullptr;
    // ������װ������
    int iRet = avformat_alloc_output_context2(&pFormatCtx, nullptr, nullptr, strURL);
    if (iRet != 0) {
        std::cerr << "avformat_alloc_output_context2 error: " <<
            Utils::GetAVErrorMessage(iRet).c_str() << std::endl;
        return nullptr;
    }

    // �����Ƶ������Ƶ��
    AVStream* pVideoStream = avformat_new_stream(pFormatCtx, nullptr);   // ��Ƶ��
    pVideoStream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    AVStream* pAudioStream = avformat_new_stream(pFormatCtx, nullptr);   // ��Ƶ��
    pAudioStream->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;

    // �����IO
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
    // ��ӡ�����װ��Ϣ
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
    av_interleaved_write_frame(m_pFormatCtx, nullptr);  // д�뻺��
    int iRet = av_write_trailer(m_pFormatCtx);
    if (iRet != 0) {
        std::cerr << "avformat_write_header error: " <<
            Utils::GetAVErrorMessage(iRet).c_str() << std::endl;
        return false;
    }
    return true;
}
