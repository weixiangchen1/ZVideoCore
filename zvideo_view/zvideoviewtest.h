#pragma once

#include <QtWidgets/QWidget>
#include "ui_zvideoviewtest.h"
#include <fstream>

class ZVideoView;
struct AVFrame;
class ZVideoViewTest : public QWidget {
    Q_OBJECT

public:
    ZVideoViewTest(QWidget *parent = nullptr);
    ~ZVideoViewTest();

protected:
    void timerEvent(QTimerEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    Ui::ZVideoViewTestClass ui;
    int m_iWidth;
    int m_iHeight;
    std::ifstream m_file;
    ZVideoView* m_pVideoView;
    //unsigned char* m_pData;
    AVFrame* m_pFrame;
};

