#include "zencode.h"

AVPacket* ZEncode::EncodeData(const AVFrame* pFrame) {
    if (pFrame == nullptr) {
        return nullptr;
    }
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_pCodecCtx == nullptr) {
        return nullptr;
    }
    // 发送AVFrame到编码线程中
    int iRet = avcodec_send_frame(m_pCodecCtx, pFrame);
    if (iRet != 0) {
        return nullptr;
    }
    AVPacket* pPacket = av_packet_alloc();
    // 接收编码数据
    iRet = avcodec_receive_packet(m_pCodecCtx, pPacket);
    if (iRet == 0) {
        return pPacket;
    }
    av_packet_free(&pPacket);
    if (iRet == AVERROR(EAGAIN) || iRet == AVERROR_EOF) {
        return nullptr;
    } 
    if (iRet < 0) {
        std::string strMsg = Utils::GetAVErrorMessage(iRet);
        std::cerr << "avcodec_receive_packet error: " << strMsg.c_str() << std::endl;
    }
    return nullptr;
}

std::vector<AVPacket*> ZEncode::GetCachePackets(){
    std::vector<AVPacket*> vecPackets;
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_pCodecCtx == nullptr) {
        return vecPackets;
    }
    // 传入null获取缓冲区中的AVPacket
    int iRet = avcodec_send_frame(m_pCodecCtx, nullptr);
    if (iRet != 0) {
        return vecPackets;
    }
    while (iRet >= 0) {
        AVPacket* pPacket = av_packet_alloc();
        iRet = avcodec_receive_packet(m_pCodecCtx, pPacket);
        if (iRet != 0) {
            av_packet_free(&pPacket);
            break;
        }
        vecPackets.push_back(pPacket);
    }
    return vecPackets;
}
