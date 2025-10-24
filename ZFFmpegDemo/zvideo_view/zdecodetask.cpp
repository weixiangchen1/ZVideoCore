#include "zdecodetask.h"
#include "utils.h"
#include "zlog.h"

bool ZDecodeTask::OpenDecodec(AVCodecParameters* pCodecParam) {
    if (pCodecParam == nullptr) {
        ZLOGERROR("AVCodecParameters is null");
        return false;
    }
    std::unique_lock<std::mutex> lock(m_mutex);
    AVCodecContext* pCodecCtx = m_decodec.CreateCodecContext(pCodecParam->codec_id, 1);
    if (pCodecCtx == nullptr) {
        ZLOGERROR("Decodec create failed");
        return false;
    }
    // 复制参数到编码上下文
    avcodec_parameters_to_context(pCodecCtx, pCodecParam);
    m_decodec.SetCodecContext(pCodecCtx);
    if (m_decodec.OpenCodec() == false) {
        ZLOGERROR("Open decodec failed");
        return false;
    }

    ZLOGDEBUG("Open decodec success");
    return true;
}

void ZDecodeTask::DoWork(AVPacket* pPacket) {
    std::cout << "#" << std::flush;
    if (pPacket == nullptr || pPacket->stream_index != 1) {
        return;
    }
    m_listPackets.Push(pPacket);
}

void ZDecodeTask::threadFunc() {
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_pFrame == nullptr) {
            m_pFrame = m_decodec.CreateFrame();
        }
    }

    while (!m_bIsExit) {
        AVPacket* pPacket = m_listPackets.Pop();
        if (pPacket == nullptr) {
            Utils::MSleep(1);
            continue;
        }
        
        bool bRet = m_decodec.SendPacket(pPacket);
        av_packet_free(&pPacket);
        if (bRet == false) {
            Utils::MSleep(1);
            continue;
        }

        {
            std::unique_lock<std::mutex> lock(m_mutex);
            int iRet = m_decodec.RecvFrame(m_pFrame);
            if (iRet == 1) {
                std::cout << "@" << std::flush;
                m_bCanRender = true;
            } else {
                continue;
            }
        }

        Utils::MSleep(1);
    }

    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_pFrame == nullptr) {
            av_frame_free(&m_pFrame);
        }
    }
}

AVFrame* ZDecodeTask::GetFrame() {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (!m_bCanRender || !m_pFrame || !m_pFrame->buf[0]) {
        return nullptr;
    }
    AVFrame* pCopyFrame = av_frame_alloc();
    int iRet = av_frame_ref(pCopyFrame, m_pFrame);
    if (iRet != 0) {
        av_frame_free(&pCopyFrame);
        std::cerr << "av_frame_ref error: " << 
            Utils::GetAVErrorMessage(iRet).c_str() << std::endl;
        return nullptr;
    }
    m_bCanRender = false;
    return pCopyFrame;
}
