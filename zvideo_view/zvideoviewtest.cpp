#include "zvideoviewtest.h"
#include <iostream>
#include "zvideoview.h"
extern "C" {
#include <libavcodec/avcodec.h>
}
#pragma comment(lib, "avutil.lib")

ZVideoViewTest::ZVideoViewTest(QWidget *parent)
    : QWidget(parent) {
    ui.setupUi(this);
    m_file.open("400_300_25.yuv", std::ios::binary);
    if (!m_file) {
        return;
    }
    ui.setupUi(this);
    m_iWidth = 400;
    m_iHeight = 300;
    ui.label->resize(m_iWidth, m_iHeight);
    m_pVideoView = ZVideoView::CreateVideoView();
    m_pVideoView->Init(m_iWidth, m_iHeight, ZVideoView::VideoFormat::YUV420P, (void*)ui.label->winId());

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
    startTimer(10);
}

ZVideoViewTest::~ZVideoViewTest() {}

void ZVideoViewTest::timerEvent(QTimerEvent* event) {
    //m_file.read((char*)m_pData, m_iWidth * m_iHeight * 1.5);
    m_file.read((char*)m_pFrame->data[0], m_iWidth * m_iHeight);        // Y
    m_file.read((char*)m_pFrame->data[1], m_iWidth * m_iHeight / 4);    // U
    m_file.read((char*)m_pFrame->data[2], m_iWidth * m_iHeight / 4);    // V
    if (m_pVideoView->IsExit()) {
        m_pVideoView->Close();
        exit(0);
    }
    //m_pVideoView->Draw(m_pData);
    m_pVideoView->DrawFrame(m_pFrame);
}

void ZVideoViewTest::resizeEvent(QResizeEvent* event) {
    ui.label->resize(size());
    ui.label->move(0, 0);
    m_pVideoView->Scale(width(), height());
}

