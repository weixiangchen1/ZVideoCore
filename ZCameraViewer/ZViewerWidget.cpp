#include "ZViewerWidget.h"
#include "ui_ZViewerWidget.h"
#include <QMouseEvent>
#include <QResizeEvent>
#include <QContextMenuEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QDebug>

#define TOUTF8(str) QString::fromLocal8Bit(str)

ZViewerWidget::ZViewerWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ZViewerWidget) {
    ui->setupUi(this);
    SetupUI();
}

ZViewerWidget::~ZViewerWidget() {
    delete ui;
}

void ZViewerWidget::SetupUI() {
    // 去除原窗口边框
    setWindowFlags(Qt::FramelessWindowHint);
    QVBoxLayout* pMainLayout = new QVBoxLayout;
    pMainLayout->setContentsMargins(0, 0, 0, 0);
    pMainLayout->setSpacing(0);
    pMainLayout->addWidget(ui->titleWidget);
    pMainLayout->addWidget(ui->bodyWidget);

    QHBoxLayout* pCameraLayout = new QHBoxLayout;
    pCameraLayout->setContentsMargins(0, 0, 0, 0);
    pCameraLayout->setSpacing(0);
    pCameraLayout->addWidget(ui->cameraListWidget);
    pCameraLayout->addWidget(ui->cameraViewWidget);
    ui->bodyWidget->setLayout(pCameraLayout);

    setLayout(pMainLayout);

    QMenu* pSubMenu = m_menu.addMenu(TOUTF8("视图"));
    QAction* pOneViewAction = pSubMenu->addAction(TOUTF8("1窗口"));
    connect(pOneViewAction, &QAction::triggered, this, &ZViewerWidget::slotViewOne);
    QAction* pFourViewAction = pSubMenu->addAction(TOUTF8("4窗口"));
    connect(pFourViewAction, &QAction::triggered, this, &ZViewerWidget::slotViewFour);
    QAction* pNineViewAction = pSubMenu->addAction(TOUTF8("9窗口"));
    connect(pNineViewAction, &QAction::triggered, this, &ZViewerWidget::slotViewNine);
    QAction* pSixteenViewAction = pSubMenu->addAction(TOUTF8("16窗口"));
    connect(pSixteenViewAction, &QAction::triggered, this, &ZViewerWidget::slotViewSixteen);

    m_vecViews.fill(nullptr, 16);
    CreateViewWindow(9);
}

void ZViewerWidget::CreateViewWindow(int iWindowNum) {
    int iCols = sqrt(iWindowNum);
    int iTotalWindowNum = m_vecViews.size();
    QGridLayout* pViewLayout = (QGridLayout*)ui->cameraViewWidget->layout();
    if (pViewLayout == nullptr) {
        pViewLayout = new QGridLayout;
        pViewLayout->setContentsMargins(0, 0, 0, 0);
        pViewLayout->setSpacing(2);
        ui->cameraViewWidget->setLayout(pViewLayout);
    }
    for (int i = 0; i < iWindowNum; ++i) {
        if (m_vecViews[i] == nullptr) {
            m_vecViews[i] = new QWidget();
            m_vecViews[i]->setStyleSheet("background-color:rgb(51,51,51);");
        }
        pViewLayout->addWidget(m_vecViews[i], i / iCols, i % iCols);
    }
    // 清理多余窗体
    for (int i = iWindowNum; i < iTotalWindowNum; ++i) {
        if (m_vecViews[i] != nullptr) {
            delete m_vecViews[i];
            m_vecViews[i] = nullptr;
        }
    }
}

void ZViewerWidget::mouseMoveEvent(QMouseEvent *event) {
    if (m_bMousePressed == false) {
        QWidget::mouseMoveEvent(event);
        return;
    }
    this->move(event->globalPos() - m_pointMoveBefore);
}

void ZViewerWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_bMousePressed = true;
        m_pointMoveBefore = event->pos();
    }
}

void ZViewerWidget::mouseReleaseEvent(QMouseEvent *event) {
    m_bMousePressed = false;
}

void ZViewerWidget::resizeEvent(QResizeEvent *event) {
    int iX = width() - ui->buttonsWidget->width();
    int iY = ui->buttonsWidget->y();
    ui->buttonsWidget->move(iX, iY);
}

void ZViewerWidget::contextMenuEvent(QContextMenuEvent *event) {
    m_menu.exec(QCursor::pos());
    event->accept();
}

void ZViewerWidget::slotMaxWindow() {
    ui->maxButton->setVisible(false);
    ui->restoreButton->setVisible(true);
    showMaximized();
}

void ZViewerWidget::slotRestoreWindow() {
    ui->restoreButton->setVisible(false);
    ui->maxButton->setVisible(true);
    showNormal();
}

void ZViewerWidget::slotViewOne() {
    CreateViewWindow(1);
}

void ZViewerWidget::slotViewFour() {
    CreateViewWindow(4);
}

void ZViewerWidget::slotViewNine() {
    CreateViewWindow(9);
}

void ZViewerWidget::slotViewSixteen() {
    CreateViewWindow(16);
}

