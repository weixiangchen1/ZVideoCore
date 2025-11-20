#include "ZCameraRecord.h"
#include <QDate>
#include "ZLog.h"
#include "ZDemuxTask.h"
#include "ZMuxTask.h"
#include "ZAVParam.h"

static QString GetFileName(QString strPath) {
    QString strTime = QDateTime::currentDateTime().toString("yyyy_MM_dd_HH_mm_ss");
    return QString("%1/cam_%2.mp4").arg(strPath).arg(strTime);
}

void ZCameraRecord::SetCameraUrl(const QString &strUrl) {
    m_strRtspUrl = strUrl;
}

void ZCameraRecord::SetSavePath(const QString &strSavePath) {
    m_strSavePath = strSavePath;
}

void ZCameraRecord::SetFileSec(long long lFileSec) {
    m_lFileSec = lFileSec;
}

void ZCameraRecord::threadFunc() {
    ZDemuxTask demuxTask;
    ZMuxTask muxTask;
    if (m_strRtspUrl.isEmpty()) {
        ZLOGERROR("rtsp url is empty");
        return;
    }

    // 设置自动重连
    while (!m_bIsExit) {
        if (demuxTask.OpenDemux(m_strRtspUrl.toStdString())) {
            break;
        }
        Utils::MSleep(3000);
        continue;
    }

    auto pVideoParam = demuxTask.CopyVideoParam();
    if (!pVideoParam) {
        ZLOGERROR("video param is empty");
        demuxTask.Stop();
        return;
    }
    // 提前启动解封装器 防止超时
    demuxTask.Start();
    auto pAudioParam = demuxTask.CopyAudioParam();
    AVCodecParameters* pAudioParameter = nullptr;    // 音频参数
    AVRational* pAudioTimeBase = nullptr;        // 音频时间基数
    if (pAudioParam) {
        pAudioParameter = pAudioParam->pParam;
        pAudioTimeBase = pAudioParam->pTimebase;
    }
    if (!muxTask.OpenMux(GetFileName(m_strSavePath).toStdString(),
                         pVideoParam->pParam, pVideoParam->pTimebase,
                         pAudioParameter, pAudioTimeBase)) {
        ZLOGERROR("open mux failed");
        demuxTask.Stop();
        muxTask.Stop();
        return;
    }
    demuxTask.SetNextNode(&muxTask);
    muxTask.Start();
    long long lBeginTime = Utils::GetCurrentTimestamp();
    while (!m_bIsExit) {
        if (Utils::GetCurrentTimestamp() - lBeginTime > m_lFileSec * 1000) {
            lBeginTime = Utils::GetCurrentTimestamp();
            muxTask.Stop();
            if (!muxTask.OpenMux(GetFileName(m_strSavePath).toStdString(),
                                 pVideoParam->pParam, pVideoParam->pTimebase,
                                 pAudioParameter, pAudioTimeBase)) {
                ZLOGERROR("open mux failed");
                demuxTask.Stop();
                muxTask.Stop();
                return;
            }
            muxTask.Start();
        }
    }
    muxTask.Stop();
    demuxTask.Stop();
}
