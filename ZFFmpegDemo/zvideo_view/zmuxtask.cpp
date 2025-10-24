#include "zmuxtask.h"

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
