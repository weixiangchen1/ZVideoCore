#pragma once

#include <QtWidgets/QWidget>
#include "ui_qsdlmerge.h"

class QSDLMerge : public QWidget {
    Q_OBJECT

public:
    QSDLMerge(QWidget *parent = nullptr);
    ~QSDLMerge();

protected:
    void timerEvent(QTimerEvent* event) override;

private:
    Ui::QSDLMergeClass ui;
};

