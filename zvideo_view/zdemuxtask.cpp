#include "zdemuxtask.h"
#include "zthread.h"
#include "zlog.h"

extern "C" {
#include <libavformat/avformat.h>
}

void ZDemuxTask::threadFunc(){
    AVPacket packet;
    while (!m_bIsExit) {
        if (!m_demux.ReadFrame(&packet)) {
            std::cout << "-" << std::flush;
            if (!m_demux.IsConnected()) {
                OpenDemux(m_strURL, m_iTimeoutMs);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        } 
        Next(&packet);
        av_packet_unref(&packet);
        std::cout << "." << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

std::shared_ptr<ZAVParam> ZDemuxTask::CopyVideoParam() {
    return m_demux.CopyVideoParam();
}

std::shared_ptr<ZAVParam> ZDemuxTask::CopyAudioParam() {
    return m_demux.CopyAudioParam();
}

bool ZDemuxTask::OpenDemux(std::string strURL, int iTimeoutMs) {
    ZLOGDEBUG("OpenDemux begin");
    m_demux.SetFormatContext(nullptr);      // 断开之前的连接
    m_strURL = strURL;
    m_iTimeoutMs = iTimeoutMs;
    AVFormatContext* pFormatCtx = m_demux.CreateDemuxContext(strURL.c_str());
    if (pFormatCtx == nullptr) {
        return false;
    }
    m_demux.SetFormatContext(pFormatCtx);
    m_demux.SetTimeoutMs(iTimeoutMs);
    ZLOGDEBUG("OpenDemux end");
    return true;
}
