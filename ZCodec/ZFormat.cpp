#include "ZFormat.h"
#include "Utils.h"
#include "ZAVParam.h"

static int TimeoutCallback(void* pParam) {
    ZFormat* pFormat = (ZFormat*)pParam;
    if (pFormat->IsTimeout()) {
        return 1;
    }
    return 0;
}

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

std::shared_ptr<ZAVParam> ZFormat::CopyVideoParam() {
    int iVideoStreamIdx = GetVideoIndex();
    std::shared_ptr<ZAVParam> pAVParam;
    std::unique_lock<std::mutex> lock(m_mutex);
    if (iVideoStreamIdx < 0 || m_pFormatCtx == nullptr) {
        return pAVParam;
    }
    pAVParam.reset(ZAVParam::CreateAVParam());
    *pAVParam->pTimebase = m_pFormatCtx->streams[iVideoStreamIdx]->time_base;
    avcodec_parameters_copy(pAVParam->pParam, m_pFormatCtx->streams[iVideoStreamIdx]->codecpar);

    return pAVParam;
}

std::shared_ptr<ZAVParam> ZFormat::CopyAudioParam() {
    int iAudioStreamIdx = GetAudioIndex();
    std::shared_ptr<ZAVParam> pAVParam;
    std::unique_lock<std::mutex> lock(m_mutex);
    if (iAudioStreamIdx < 0 || m_pFormatCtx == nullptr) {
        return pAVParam;
    }
    pAVParam.reset(ZAVParam::CreateAVParam());
    *pAVParam->pTimebase = m_pFormatCtx->streams[iAudioStreamIdx]->time_base;
    avcodec_parameters_copy(pAVParam->pParam, m_pFormatCtx->streams[iAudioStreamIdx]->codecpar);

    return pAVParam;
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
        m_bIsConnected = false;
        return;
    }
    m_bIsConnected = true;

    m_lLastTime = Utils::GetCurrentTimestamp();
    // 设置超时处理回调函数
    if (m_iTimeoutMs > 0) {
        AVIOInterruptCB cb = { TimeoutCallback, this };
        m_pFormatCtx->interrupt_callback = cb;
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
    AVRational inTimeBase;
    inTimeBase.num = timeBase.num;
    inTimeBase.den = timeBase.den;

    return RescaleTimeParam(pPacket, lOffsetPts, &inTimeBase);
}

bool ZFormat::RescaleTimeParam(AVPacket* pPacket, long long lOffsetPts, AVRational* pTimeBase) {
    if (pPacket == nullptr || pTimeBase == nullptr) {
        return false;
    }
    AVStream* pOutStream = m_pFormatCtx->streams[pPacket->stream_index];
    pPacket->pts = av_rescale_q_rnd(pPacket->pts - lOffsetPts, *pTimeBase,
        pOutStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
    pPacket->dts = av_rescale_q_rnd(pPacket->dts - lOffsetPts, *pTimeBase,
        pOutStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
    pPacket->duration = av_rescale_q(pPacket->duration, *pTimeBase, pOutStream->time_base);
    pPacket->pos = -1;

    return true;
}

int ZFormat::GetVideoCodecId() {

    return m_iVideoCodecId;
}

void ZFormat::SetTimeoutMs(int iTime) {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_iTimeoutMs = iTime;
    // 设置回调函数 处理超时退出
    if (m_pFormatCtx != nullptr) {
        AVIOInterruptCB cb = { TimeoutCallback, this };
        m_pFormatCtx->interrupt_callback = cb;
    }
}

bool ZFormat::IsTimeout() {
    if (Utils::GetCurrentTimestamp() - m_lLastTime > m_iTimeoutMs) {
        m_lLastTime = Utils::GetCurrentTimestamp();
        m_bIsConnected = false;
        return true;
    }
    return false;
}

bool ZFormat::IsConnected() {
    return m_bIsConnected;
}
