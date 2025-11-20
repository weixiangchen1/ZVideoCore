#include "ZMuxTask.h"

bool ZMuxTask::OpenMux(std::string strURL, 
    AVCodecParameters* pVideoParam, AVRational* pSrcVideoTimeBase,
    AVCodecParameters* pAudioParam, AVRational* pSrcAudioTimeBase) {
    AVFormatContext* pFormatCtx = m_mux.CreateMuxContext(strURL.c_str(), pVideoParam, pAudioParam);
    if (pFormatCtx == nullptr) {
        return false;
    }
    m_mux.SetFormatContext(pFormatCtx);
    m_mux.SetSrcAudioTimeBase(pSrcAudioTimeBase);
    m_mux.SetSrcVideoTimeBase(pSrcVideoTimeBase);

    return true;
}

void ZMuxTask::threadFunc() {
    m_mux.WriteHead();
    // 找到关键帧
    while (!m_bIsExit) {
        std::unique_lock<std::mutex> lock(m_mutex);
        AVPacket* pPacket = m_listPackets.Pop();
        if (pPacket == nullptr) {
            Utils::MSleep(1);
            continue;
        }
        if (pPacket->stream_index == m_mux.GetVideoIndex() &&
            pPacket->flags & AV_PKT_FLAG_KEY) {
            m_mux.WriteFrame(pPacket);
            av_packet_free(&pPacket);
            break;
        }
        av_packet_free(&pPacket);   // 丢弃非关键帧
    }

    while (!m_bIsExit) {
        std::unique_lock<std::mutex> lock(m_mutex);
        AVPacket* pPacket = m_listPackets.Pop();
        if (pPacket == nullptr) {
            Utils::MSleep(1);
            continue;
        }
        m_mux.WriteFrame(pPacket);
        std::cout << "W" << std::flush;
        av_packet_free(&pPacket);
    }
    m_mux.WriteTail();
    m_mux.SetFormatContext(nullptr);
}

void ZMuxTask::DoWork(AVPacket* pPacket) {
    m_listPackets.Push(pPacket);
    Next(pPacket);
}
