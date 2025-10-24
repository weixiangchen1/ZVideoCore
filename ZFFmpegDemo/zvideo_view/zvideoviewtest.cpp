#include "zvideoviewtest.h"
#include <iostream>
#include "zvideoview.h"
#include "utils.h"
extern "C" {
#include <libavcodec/avcodec.h>
}
#pragma comment(lib, "avutil.lib")

ZVideoViewTest::ZVideoViewTest(QWidget *parent)
    : QWidget(parent) {
    ui.setupUi(this);
    connect(this, &ZVideoViewTest::signalView, this, &ZVideoViewTest::slotView);
    m_file.open("400_300_25.yuv", std::ios::binary);
    if (!m_file) {
        exit(-1);
    }
    m_file.seekg(0, std::ios::end);
    m_iFileSize = m_file.tellg();
    m_file.seekg(0, std::ios::beg);

    ui.setupUi(this);
    m_iFps = 0;
    m_iWidth = 400;
    m_iHeight = 300;
    ui.label->resize(m_iWidth, m_iHeight);
    m_pVideoView = ZVideoView::CreateVideoView();
    m_pVideoView->Init(m_iWidth, m_iHeight, ZVideoView::VideoFormat::YUV420P);
    m_pVideoView->SetWindow((void*)ui.label->winId());

    //m_pData = new unsigned char[m_iWidth * m_iHeight * 2];
    m_pFrame = av_frame_alloc();
    m_pFrame->width = m_iWidth;
    m_pFrame->height = m_iHeight;
    m_pFrame->format = AV_PIX_FMT_YUV420P;
    m_pFrame->linesize[0] = m_iWidth;
    m_pFrame->linesize[1] = m_iWidth / 2;
    m_pFrame->linesize[2] = m_iWidth / 2;
    // ÉêÇë´æ´¢Í¼Ïñ¿Õ¼ä£¬Ä¬ÈÏ32×Ö½Ú¶ÔÆë
    int iRet = av_frame_get_buffer(m_pFrame, 0);
    if (iRet != 0) {
        char buf[1024] = { 0 };
        av_strerror(iRet, buf, sizeof(buf));
        std::cout << buf << std::endl;
    }
    //startTimer(10);
    m_bIsRunning = true;
    m_thread = std::thread(&ZVideoViewTest::threadFunc, this);
    ui.spinBox->setValue(25);
    ui.spinBox->setRange(1, 300);
    ui.fps_label->setText("fps");
}

ZVideoViewTest::~ZVideoViewTest() {
    m_bIsRunning = false;
    m_thread.join();
}

void ZVideoViewTest::slotView() {
    //m_file.read((char*)m_pData, m_iWidth * m_iHeight * 1.5);
    m_file.read((char*)m_pFrame->data[0], m_iWidth * m_iHeight);        // Y
    m_file.read((char*)m_pFrame->data[1], m_iWidth * m_iHeight / 4);    // U
    m_file.read((char*)m_pFrame->data[2], m_iWidth * m_iHeight / 4);    // V
    if (m_file.tellg() == m_iFileSize) {
        m_file.seekg(0, std::ios::beg);
    }
    if (m_pVideoView->IsExit()) {
        m_pVideoView->Close();
        exit(0);
    }
    //m_pVideoView->Draw(m_pData);
    m_pVideoView->DrawFrame(m_pFrame);
    ui.fps_label->setText(QString("fps: %1").arg(QString::number(m_pVideoView->GetRenderFps())));
    m_iFps = ui.spinBox->value();
}

//void ZVideoViewTest::timerEvent(QTimerEvent* event) {
//    //m_file.read((char*)m_pData, m_iWidth * m_iHeight * 1.5);
//    m_file.read((char*)m_pFrame->data[0], m_iWidth * m_iHeight);        // Y
//    m_file.read((char*)m_pFrame->data[1], m_iWidth * m_iHeight / 4);    // U
//    m_file.read((char*)m_pFrame->data[2], m_iWidth * m_iHeight / 4);    // V
//    if (m_pVideoView->IsExit()) {
//        m_pVideoView->Close();
//        exit(0);
//    }
//    //m_pVideoView->Draw(m_pData);
//    m_pVideoView->DrawFrame(m_pFrame);
//}

void ZVideoViewTest::resizeEvent(QResizeEvent* event) {
    ui.label->resize(size());
    ui.label->move(0, 0);
    m_pVideoView->Scale(width(), height());
}

void ZVideoViewTest::threadFunc() {
    while (m_bIsRunning) {
        emit signalView();
        //std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (m_iFps > 0) {
            Utils::MSleep(1000 / m_iFps);
        } else {
            Utils::MSleep(10);
        }
    }
}

