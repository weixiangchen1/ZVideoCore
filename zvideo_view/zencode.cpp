#include "zencode.h"
#include <iostream>
#include "utils.h"

AVCodecContext* ZEncode::CreateContext(int iCodeId) {
    // 1.寻找编码器
    AVCodec* pCodec = avcodec_find_encoder((AVCodecID)iCodeId);
    if (pCodec == nullptr) {
        std::cerr << "avcodec_find_encoder failed: " << iCodeId << std::endl;
        return nullptr;
    }

    // 2.创建编码器上下文
    AVCodecContext* pCodecCtx = avcodec_alloc_context3(pCodec);
    if (pCodecCtx == nullptr) {
        std::cerr << "avcodec_alloc_context3 failed"<< std::endl;
        return nullptr;
    }

    // 3.设置参数默认值
    pCodecCtx->time_base = { 1, 25 };
    pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    pCodecCtx->thread_count = Utils::GetSystemCPUNum();

    return pCodecCtx;
}

void ZEncode::SetCodecContext(AVCodecContext* pCodecCtx) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_pCodecCtx != nullptr) {
        avcodec_free_context(&m_pCodecCtx);
    }
    m_pCodecCtx = pCodecCtx;
}

bool ZEncode::SetAVOpt(const char* pKey, const char* pValue) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_pCodecCtx == nullptr) {
        return false;
    }
    int iRet = av_opt_set(m_pCodecCtx->priv_data, pKey, pValue, 0);
    if (iRet != 0) {
        std::string strMsg = Utils::GetAVErrorMessage(iRet);
        std::cerr << "av_opt_set error: " << strMsg.c_str() << std::endl;
        return false;
    }
    return true;
}

bool ZEncode::SetAVOpt(const char* pKey, const int iValue) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_pCodecCtx == nullptr) {
        return false;
    }
    int iRet = av_opt_set_int(m_pCodecCtx->priv_data, pKey, iValue, 0);
    if (iRet != 0) {
        std::string strMsg = Utils::GetAVErrorMessage(iRet);
        std::cerr << "av_opt_set_int error: " << strMsg.c_str() << std::endl;
        return false;
    }
    return true;
}

bool ZEncode::OpenEncoder() {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_pCodecCtx == nullptr) {
        return false;
    }
    int iRet = avcodec_open2(m_pCodecCtx, nullptr, nullptr);
    if (iRet != 0) {
        std::string strMsg = Utils::GetAVErrorMessage(iRet);
        std::cerr << "avcodec_open2 error: " << strMsg.c_str() << std::endl;
        return false;
    }
    return true;
}

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

AVFrame* ZEncode::CreateFrame() {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_pCodecCtx == nullptr) {
        return nullptr;
    }
    AVFrame* pFrame = av_frame_alloc();
    pFrame->width = m_pCodecCtx->width;
    pFrame->height = m_pCodecCtx->height;
    pFrame->format = m_pCodecCtx->pix_fmt;
    int iRet = av_frame_get_buffer(pFrame, 0);
    if (iRet != 0) {
        av_frame_free(&pFrame);
        std::string strMsg = Utils::GetAVErrorMessage(iRet);
        std::cerr << "av_frame_get_buffer error: " << strMsg.c_str() << std::endl;
        return nullptr;
    }
    return pFrame;
}

std::vector<AVPacket*> ZEncode::GetFlushPacket(){
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
