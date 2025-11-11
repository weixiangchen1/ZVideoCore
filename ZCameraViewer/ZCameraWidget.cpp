#include "ZCameraWidget.h"
#include <QStyleOption>
#include <QPainter>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QListWidget>
#include <QMimeData>
#include <QDebug>

#include "ZDecodeTask.h"
#include "ZDemuxTask.h"
#include "ZVideoView.h"
#include "ZAVParam.h"
#include "ZCameraConfig.h"

ZCameraWidget::ZCameraWidget(QWidget *parent)
    : QWidget(parent) {
    // 接收拖拽事件
    setAcceptDrops(true);
}

ZCameraWidget::~ZCameraWidget() {
    if (m_pDemuxTask) {
        m_pDemuxTask->Stop();
        delete m_pDemuxTask;
        m_pDemuxTask = nullptr;
    }
    if (m_pDecodeTask) {
        m_pDecodeTask->Stop();
        delete m_pDecodeTask;
        m_pDecodeTask = nullptr;
    }
    if (m_pVideoView) {
        m_pVideoView->Close();
        delete m_pVideoView;
        m_pVideoView = nullptr;
    }
}

bool ZCameraWidget::OpenCamera(const char *pUrl) {
    if (m_pDemuxTask) {
        m_pDemuxTask->Stop();
    }
    if (m_pDecodeTask) {
        m_pDecodeTask->Stop();
    }
    // 打开解封装器
    m_pDemuxTask = new ZDemuxTask();
    if (!m_pDemuxTask->OpenDemux(pUrl)) {
        return false;
    }
    // 拷贝视频参数并打开视频解码器
    m_pDecodeTask = new ZDecodeTask();
    auto pVideoParam = m_pDemuxTask->CopyVideoParam();
    if (!m_pDecodeTask->OpenDecodec(pVideoParam->pParam)) {
        return false;
    }
    // 设置责任链
    m_pDemuxTask->SetNextNode(m_pDecodeTask);

    // 初始化渲染器
    m_pVideoView = ZVideoView::CreateVideoView();
    m_pVideoView->SetWindow((void*)winId());
    m_pVideoView->Init(pVideoParam->pParam);

    // 启动解封装和解码线程
    m_pDemuxTask->Start();
    m_pDecodeTask->Start();
    return true;
}

bool ZCameraWidget::DrawVideo() {
    if (!m_pDecodeTask || !m_pDemuxTask || !m_pVideoView) {
        return false;
    }
    AVFrame* pFrame = m_pDecodeTask->GetFrame();
    if (!pFrame) {
        return false;
    }
    m_pVideoView->DrawFrame(pFrame);
    Utils::ReleaseFrame(&pFrame);
    return true;
}

void ZCameraWidget::dropEvent(QDropEvent *event) {
    QListWidget* pCameraList = (QListWidget*)event->source();
    ZCameraData data = ZCameraConfig::GetInstance()->GetCameraData(pCameraList->currentRow());
    OpenCamera(data.strSubURL.toUtf8());
}

void ZCameraWidget::dragEnterEvent(QDragEnterEvent *event) {
    QListWidget* sourceList = (QListWidget*)(event->source());
    if (!sourceList) {
        event->ignore();
        return;
    }

    // 检查是否包含实际的MIME类型：application/x-qabstractitemmodeldatalist
    if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
        event->acceptProposedAction(); // 接受拖拽
    } else {
        event->ignore();
    }
}

void ZCameraWidget::dragMoveEvent(QDragMoveEvent *event) {
    QListWidget* sourceList = (QListWidget*)(event->source());
    if (sourceList && event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void ZCameraWidget::paintEvent(QPaintEvent *event) {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
