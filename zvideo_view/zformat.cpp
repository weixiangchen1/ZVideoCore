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

bool ZFormat::CopyParam(int iStreamIndex, AVCodecContext* dstCodecCtx) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_pFormatCtx == nullptr) {
        return false;
    }
    if (iStreamIndex < 0 || iStreamIndex > m_pFormatCtx->nb_streams) {
        return false;
    }
    return avcodec_parameters_to_context(dstCodecCtx, m_pFormatCtx->streams[iStreamIndex]->codecpar) >= 0;
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
            m_audioTimeBase.num = pFormatCtx->streams[i]->time_base.num;
            m_audioTimeBase.den = pFormatCtx->streams[i]->time_base.den;
        } else if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            m_iVideoIndex = i;
            m_iVideoCodecId = pFormatCtx->streams[i]->codecpar->codec_id;
            m_videoTimeBase.num = pFormatCtx->streams[i]->time_base.num;
            m_videoTimeBase.den = pFormatCtx->streams[i]->time_base.den;
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

bool ZFormat::RescaleTimeParam(AVPacket* pPacket, long long lOffsetPts, ZRational timeBase) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_pFormatCtx == nullptr) {
        return false;
    }
    AVStream* pOutStream = m_pFormatCtx->streams[pPacket->stream_index];
    AVRational inTimeBase;
    inTimeBase.den = timeBase.den;
    inTimeBase.num = timeBase.num;
    pPacket->pts = av_rescale_q_rnd(pPacket->pts - lOffsetPts, inTimeBase,
        pOutStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
    pPacket->dts = av_rescale_q_rnd(pPacket->dts - lOffsetPts, inTimeBase,
        pOutStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
    pPacket->duration = av_rescale_q(pPacket->duration, inTimeBase, pOutStream->time_base);
    pPacket->pos = -1;

    return true;
}

int ZFormat::GetVideoCodecId() {

    return m_iVideoCodecId;
}
