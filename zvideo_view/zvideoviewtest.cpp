#include "zvideoviewtest.h"
#include "zvideoview.h"

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

    m_pData = new unsigned char[m_iWidth * m_iHeight * 2];
    startTimer(10);
}

ZVideoViewTest::~ZVideoViewTest() {}

void ZVideoViewTest::timerEvent(QTimerEvent* event) {
    m_file.read((char*)m_pData, m_iWidth * m_iHeight * 1.5);
    if (m_pVideoView->IsExit()) {
        m_pVideoView->Close();
        exit(0);
    }
    m_pVideoView->Draw(m_pData);
}

void ZVideoViewTest::resizeEvent(QResizeEvent* event) {
    ui.label->resize(size());
    ui.label->move(0, 0);
    m_pVideoView->Scale(width(), height());
}

