#pragma once

#include <QtWidgets/QWidget>
#include "ui_zplayer.h"

class ZPlayer : public QWidget
{
    Q_OBJECT

public:
    ZPlayer(QWidget *parent = nullptr);
    ~ZPlayer();

private:
    Ui::ZPlayerClass ui;
};

