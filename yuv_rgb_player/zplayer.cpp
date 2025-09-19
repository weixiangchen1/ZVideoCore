#include "zplayer.h"
#include <iostream>
#include <QDebug>
#include <QFileDialog>

#include "utils.h"
#include "zvideoview.h"

ZPlayer::ZPlayer(QWidget *parent)
    : QWidget(parent) {
    ui.setupUi(this);
    SetupUI();
    CreateConnect();
    m_videoViews.push_back(ZVideoView::CreateVideoView());
    m_videoViews.push_back(ZVideoView::CreateVideoView());
    m_videoViews[0]->SetWindow((void*)ui.videoLabel1->winId());
    m_videoViews[1]->SetWindow((void*)ui.videoLabel2->winId());
    m_isRunning = true;
    m_thread = std::thread(&ZPlayer::threadFunc, this);
}

ZPlayer::~ZPlayer() {
    m_thread.join();
    m_isRunning = false;
}

void ZPlayer::OpenFileAndPlay(int index) {
    QFileDialog fileDlg;
    QString strFile = fileDlg.getOpenFileName();
    std::cout << strFile.toLocal8Bit().data() << std::endl;
    // 打开文件
    if (!m_videoViews[index]->OpenFile(strFile.toStdString())) {
        std::cout << "open file failed" << std::endl;
        return;
    }
    // 初始化窗口和材质
    int iWidth = 0, iHeight = 0;
    QString strPixFormat = 0;
    if (index == 0) {
        iWidth = ui.widthSpinBox1->value();
        iHeight = ui.heightSpinBox1->value();
        strPixFormat = ui.formatCombo1->currentText();
    } else if (index == 1) {
        iWidth = ui.widthSpinBox2->value();
        iHeight = ui.heightSpinBox2->value();
        strPixFormat = ui.formatCombo2->currentText();
    }
    int iPixFormat = -1;
    if (strPixFormat == "YUV420P") {
        iPixFormat = (int)ZVideoView::VideoFormat::YUV420P;
    } else if (strPixFormat == "RGBA") {
        iPixFormat = (int)ZVideoView::VideoFormat::RGBA;
    } else if (strPixFormat == "ARGB") {
        iPixFormat = (int)ZVideoView::VideoFormat::ARGB;
    } else if (strPixFormat == "BGRA") {
        iPixFormat = (int)ZVideoView::VideoFormat::BGRA;
    } else if (strPixFormat == "RGB") {
        iPixFormat = (int)ZVideoView::VideoFormat::RGB;
    }
    m_videoViews[index]->Init(iWidth, iHeight, (ZVideoView::VideoFormat)iPixFormat);
}

void ZPlayer::slotView() {
    static int iLastTime[2] = { 0 };
    static int iFPS[2] = { 0 };
    iFPS[0] = ui.fpsSpinBox1->value();
    iFPS[1] = ui.fpsSpinBox2->value();
    for (int i = 0; i < m_videoViews.size(); ++i) {
        if (iFPS[i] <= 0) {
            continue;
        }
        int iSpaceMs = 1000 / iFPS[i];
        if (Utils::GetCurrentTimestamp() - iLastTime[i] < iSpaceMs) {
            continue;
        }
        iLastTime[i] = Utils::GetCurrentTimestamp();
        AVFrame* frame = m_videoViews[i]->ReadFrame();
        if (!frame) {
            continue;
        }
        m_videoViews[i]->DrawFrame(frame);
        QString strFPS = QString("fps: %1").arg(QString::number(m_videoViews[i]->GetRenderFps()));
        if (i == 0) {
            ui.fpsLabel1->setText(strFPS);
        } else if (i == 1) {
            ui.fpsLabel2->setText(strFPS);
        }
    }
}

void ZPlayer::slotOpenAndPlayOne() {
    OpenFileAndPlay(0);

}

void ZPlayer::slotOpenAndPlayTwo() {
    OpenFileAndPlay(1);
}

void ZPlayer::SetupUI() {
}

void ZPlayer::CreateConnect() {
    connect(ui.openFileBtn1, &QPushButton::clicked, this, &ZPlayer::slotOpenAndPlayOne);
    connect(ui.openFileBtn2, &QPushButton::clicked, this, &ZPlayer::slotOpenAndPlayTwo);
    connect(this, &ZPlayer::signalView, this, &ZPlayer::slotView);
}

void ZPlayer::threadFunc() {
    while (m_isRunning) {
        emit signalView();
        Utils::MSleep(10);
    }
}

