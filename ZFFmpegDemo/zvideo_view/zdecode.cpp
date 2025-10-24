#include "zdecode.h"

bool ZDecode::SendPacket(const AVPacket* pPacket) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_pCodecCtx == nullptr) {
        return false;
    }
    return avcodec_send_packet(m_pCodecCtx, pPacket) == 0;
}

int ZDecode::RecvFrame(AVFrame* pFrame) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_pCodecCtx == nullptr) {
        return -1;
    }
    // �����Ӳ������ ��Ҫ����һ��ת��
    AVFrame* pShowFrame = pFrame;
    if (m_pCodecCtx->hw_device_ctx) {
        pShowFrame = av_frame_alloc();
        if (!pShowFrame) {
            return -1;
        }
    }
    int iRet = avcodec_receive_frame(m_pCodecCtx, pShowFrame);
    if (iRet == 0) {
        if (m_pCodecCtx->hw_device_ctx) {
            iRet = av_hwframe_transfer_data(pFrame, pShowFrame, 0);
            av_frame_free(&pShowFrame);
            if (iRet != 0) {
                std::cerr << "av_hwframe_transfer_data error: " << Utils::GetAVErrorMessage(iRet).c_str() << std::endl;
                return -1;
            }
            return 1; // �ɹ�
        }
        return 1; // �ɹ�
    }


    if (m_pCodecCtx->hw_device_ctx) {
        av_frame_free(&pShowFrame);
    }
    if (iRet == AVERROR(EAGAIN)) {
        return 0; // ��Ҫ�����
    }
    if (iRet == AVERROR_EOF) {
        return -1; // �������
    }
    // ��������
    std::cerr << "avcodec_receive_frame error: " << Utils::GetAVErrorMessage(iRet).c_str() << std::endl;
    return -1;
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
    // Ӳ������������
    AVBufferRef* pHWCtx = nullptr;
    int iRet = av_hwdevice_ctx_create(&pHWCtx, (AVHWDeviceType)iType, nullptr, nullptr, 0);
    if (iRet != 0) {
        std::cerr << "av_hwdevice_ctx_create error: " << Utils::GetAVErrorMessage(iRet).c_str() << std::endl;
        return false;
    }
    m_pCodecCtx->hw_device_ctx = pHWCtx;
    return true;
}
