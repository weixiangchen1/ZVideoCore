#include "zformat.h"
#include "utils.h"

bool ZFormat::CopyParam(int iStreamIndex, AVCodecParameters* dstParameter) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_pFormatCtx == nullptr) {
        return false;
    }
    if (iStreamIndex < 0 || iStreamIndex > m_pFormatCtx->nb_streams) {
        return false;
    }
    return avcodec_parameters_copy(dstParameter, m_pFormatCtx->streams[iStreamIndex]->codecpar) >= 0;
}

void ZFormat::SetFormatContext(AVFormatContext* pFormatCtx) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_pFormatCtx) {
        if (m_pFormatCtx->oformat) {
            // 输出上下文
            if (m_pFormatCtx->pb) {
                avio_close(m_pFormatCtx->pb);
            }
            avformat_free_context(m_pFormatCtx);
        } else if (m_pFormatCtx->iformat) {
            // 输入上下文
            avformat_close_input(&m_pFormatCtx);
        } else {
            avformat_free_context(m_pFormatCtx);
        }
    }
    m_pFormatCtx = pFormatCtx;

    if (m_pFormatCtx == nullptr) {
        return;
    }

    for (int i = 0; i < pFormatCtx->nb_streams; ++i) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            m_iAudioIndex = i;
        } else if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            m_iVideoIndex = i;
        }
    }
}

int ZFormat::GetVideoIndex() {
    return m_iVideoIndex;
}

int ZFormat::GetAudioIndex() {
    return m_iAudioIndex;
}

ZRational ZFormat::GetVideoTimeBase() {
    return m_videoTimeBase;
}

ZRational ZFormat::GetAudioTimeBase() {
    return m_audioTimeBase;
}
