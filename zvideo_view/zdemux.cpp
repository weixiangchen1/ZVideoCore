#include "zdemux.h"
#include "utils.h"

AVFormatContext* ZDemux::CreateDemuxContext(const char* strURL) {
    AVFormatContext* pFormatCtx = nullptr;
    // �򿪽��װ������
    int iRet = avformat_open_input(&pFormatCtx, strURL, nullptr, nullptr);
    if (iRet != 0) {
        std::cerr << "avformat_open_input error: " <<
            Utils::GetAVErrorMessage(iRet).c_str() << std::endl;
        return nullptr;
    }

    // ��ȡý����Ϣ
    iRet = avformat_find_stream_info(pFormatCtx, nullptr);
    if (iRet != 0) {
        std::cerr << "avformat_find_stream_info error: " <<
            Utils::GetAVErrorMessage(iRet).c_str() << std::endl;
        return nullptr;
    }

    // ��ӡ�����װ��Ϣ
    av_dump_format(pFormatCtx, 0, strURL, 0);

    return pFormatCtx;
}

bool ZDemux::ReadFrame(AVPacket* pPacket) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_pFormatCtx == nullptr) {
        return false;
    }
    int iRet = av_read_frame(m_pFormatCtx, pPacket);
    if (iRet != 0) {
        std::cerr << "av_read_frame error: " <<
            Utils::GetAVErrorMessage(iRet).c_str() << std::endl;
        return false;
    }
    return true;
}
