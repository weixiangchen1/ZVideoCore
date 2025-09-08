#pragma once

#include <QtWidgets/QWidget>
#include "ui_basergb.h"

class BaseRGB : public QWidget {
    Q_OBJECT

public:
    BaseRGB(QWidget *parent = nullptr);
    ~BaseRGB();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    Ui::BaseRGBClass ui;
};

