#pragma once

#include <QtWidgets/QWidget>
#include "ui_zvideoviewtest.h"
#include <fstream>
#include <thread>

class ZVideoView;
struct AVFrame;
class ZVideoViewTest : public QWidget {
    Q_OBJECT

public:
    ZVideoViewTest(QWidget *parent = nullptr);
    ~ZVideoViewTest();

signals:
    void signalView();

public slots:
    void slotView();

protected:
    //void timerEvent(QTimerEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void threadFunc();

private:
    Ui::ZVideoViewTestClass ui;
    int m_iWidth;
    int m_iHeight;
    std::ifstream m_file;
    int m_iFileSize;
    ZVideoView* m_pVideoView;
    //unsigned char* m_pData;
    AVFrame* m_pFrame;
    std::thread m_thread;
    bool m_bIsRunning;
    int m_iFps;
};

