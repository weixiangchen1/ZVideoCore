#include "zdecode.h"

bool ZDecode::SendPacket(const AVPacket* pPacket) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_pCodecCtx == nullptr) {
        return false;
    }
    return avcodec_send_packet(m_pCodecCtx, pPacket) == 0;
}

bool ZDecode::RecvFrame(AVFrame* pFrame) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_pCodecCtx == nullptr) {
        return false;
    }
    // 如果有硬件加速 需要进行一次转换
    AVFrame* pShowFrame = pFrame;
    if (m_pCodecCtx->hw_device_ctx) {
        pShowFrame = av_frame_alloc();
    }
    int iRet = avcodec_receive_frame(m_pCodecCtx, pShowFrame);
    if (iRet == 0) {
        if (m_pCodecCtx->hw_device_ctx) {
            iRet = av_hwframe_transfer_data(pFrame, pShowFrame, 0);
            av_frame_free(&pShowFrame);
            if (iRet != 0) {
                std::cerr << "av_hwframe_transfer_data error: " << Utils::GetAVErrorMessage(iRet).c_str() << std::endl;
                return false;
            }
            return true;
        }
    }

    if (m_pCodecCtx->hw_device_ctx) {
        av_frame_free(&pShowFrame);
    }
    return false;
}

std::vector<AVFrame*> ZDecode::GetCacheFrames() {
    std::vector<AVFrame*> vecFrames;
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_pCodecCtx == nullptr) {
        return vecFrames;
    }
    int iRet = avcodec_send_packet(m_pCodecCtx, nullptr);
    while (iRet >= 0) {
        AVFrame* pFrame = av_frame_alloc();
        iRet = avcodec_receive_frame(m_pCodecCtx, pFrame);
        if (iRet < 0) {
            av_frame_free(&pFrame);
            break;
        }
        vecFrames.push_back(pFrame);
    }
    return vecFrames;
}

bool ZDecode::InitHWA(int iType) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_pCodecCtx == nullptr) {
        return false;
    }
    // 硬件加速上下文
    AVBufferRef* pHWCtx = nullptr;
    int iRet = av_hwdevice_ctx_create(&pHWCtx, (AVHWDeviceType)iType, nullptr, nullptr, 0);
    if (iRet != 0) {
        std::cerr << "av_hwdevice_ctx_create error: " << Utils::GetAVErrorMessage(iRet).c_str() << std::endl;
        return false;
    }
    m_pCodecCtx->hw_device_ctx = pHWCtx;
    return true;
}
