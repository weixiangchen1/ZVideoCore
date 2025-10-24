#include "ZViewerWidget.h"
#include "ui_ZViewerWidget.h"

ZViewerWidget::ZViewerWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ZViewerWidget) {
    ui->setupUi(this);
}

ZViewerWidget::~ZViewerWidget() {
    delete ui;
}

