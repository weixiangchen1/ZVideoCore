#pragma once

#include <QtWidgets/QWidget>
#include <thread>
#include "ui_zplayer.h"

class ZVideoView;
class ZPlayer : public QWidget {
    Q_OBJECT

public:
    ZPlayer(QWidget *parent = nullptr);
    ~ZPlayer();
    void OpenFileAndPlay(int index);

signals:
    void signalView();

public slots:
    void slotView();
    void slotOpenAndPlayOne();
    void slotOpenAndPlayTwo();

private:
    void SetupUI();
    void CreateConnect();
    void threadFunc();
    Ui::ZPlayerClass ui;
    std::vector<ZVideoView *> m_videoViews;
    std::thread m_thread;
    bool m_isRunning;
};

