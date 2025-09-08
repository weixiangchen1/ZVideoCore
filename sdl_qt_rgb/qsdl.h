#pragma once

#include <QtWidgets/QWidget>
#include "ui_qsdl.h"

class QSDL : public QWidget {
    Q_OBJECT

public:
    QSDL(QWidget *parent = nullptr);
    ~QSDL();

protected:
    void timerEvent(QTimerEvent* event) override;

private:
    Ui::QSDLClass ui;
};

