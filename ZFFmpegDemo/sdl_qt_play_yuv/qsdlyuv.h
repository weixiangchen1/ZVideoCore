#pragma once

#include <QtWidgets/QWidget>
#include "ui_qsdlyuv.h"

class QSDLYUV : public QWidget {
    Q_OBJECT

public:
    QSDLYUV(QWidget *parent = nullptr);
    ~QSDLYUV();

protected:
    void timerEvent(QTimerEvent* event) override;

private:
    Ui::QSDLYUVClass ui;
};

