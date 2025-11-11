#include "ZViewerWidget.h"
#include "ui_ZViewerWidget.h"
#include <QMouseEvent>
#include <QResizeEvent>
#include <QContextMenuEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QListWidget>
#include <QLineEdit>
#include <QDialog>
#include <QMessageBox>
#include <QDebug>

#include "ZCameraConfig.h"
#include "ZCameraWidget.h"

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

    ZCameraConfig::GetInstance()->LoadConfig("config.json");
    RefreshCamera();

    // 启动定时器渲染视频
    startTimer(1);
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
            m_vecViews[i] = new ZCameraWidget();
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

void ZViewerWidget::RefreshCamera() {
    auto pConfig = ZCameraConfig::GetInstance();
    ui->cameraConfigWidget->clear();
    int iCount = pConfig->GetCameraCount();
    for (int i = 0; i < iCount; ++i) {
        ZCameraData data = pConfig->GetCameraData(i);
        QListWidgetItem* pItem = new QListWidgetItem(QIcon(":/res/cam.png"), data.strName);
        ui->cameraConfigWidget->addItem(pItem);
    }
}

void ZViewerWidget::EditCamera(int index) {
    auto pConfig = ZCameraConfig::GetInstance();
    QDialog dialog(this);
    dialog.resize(500, 150);
    QFormLayout mainLayout;
    dialog.setLayout(&mainLayout);
    QLineEdit lineEditName, lineEditURL, lineEditSubURL, lineEditPath;
    mainLayout.addRow(TOUTF8("名称"), &lineEditName);
    mainLayout.addRow(TOUTF8("主码流"), &lineEditURL);
    mainLayout.addRow(TOUTF8("辅码流"), &lineEditSubURL);
    mainLayout.addRow(TOUTF8("保存目录"), &lineEditPath);
    QPushButton saveButton(TOUTF8("保存"));
    saveButton.setFixedHeight(30);
    connect(&saveButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    mainLayout.addRow("", &saveButton);

    if (index >= 0) {
        ZCameraData data = pConfig->GetCameraData(index);
        lineEditName.setText(data.strName);
        lineEditURL.setText(data.strURL);
        lineEditSubURL.setText(data.strSubURL);
        lineEditPath.setText(data.strSavePath);
    }

    while (true) {
        if (dialog.exec() == QDialog::Accepted) {
            if (lineEditName.text().isEmpty()) {
                QMessageBox::information(nullptr, TOUTF8("错误"), TOUTF8("请输入名称"));
                continue;
            }
            if (lineEditURL.text().isEmpty()) {
                QMessageBox::information(nullptr, TOUTF8("错误"), TOUTF8("请输入主码流"));
                continue;
            }
            if (lineEditSubURL.text().isEmpty()) {
                QMessageBox::information(nullptr, TOUTF8("错误"), TOUTF8("请输入辅码流"));
                continue;
            }
            if (lineEditPath.text().isEmpty()) {
                QMessageBox::information(nullptr, TOUTF8("错误"), TOUTF8("请输入保存目录"));
                continue;
            }
            break;
        }
        return;
    }
    ZCameraData cameraData;
    cameraData.strName = lineEditName.text();
    cameraData.strURL = lineEditURL.text();
    cameraData.strSubURL = lineEditSubURL.text();
    cameraData.strSavePath = lineEditPath.text();
    if (index >= 0) {
        pConfig->SetCameraData(index, cameraData);
    } else {
        pConfig->PushCameraData(cameraData);
    }
    pConfig->SaveConfig("config.json");
    RefreshCamera();
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
    Q_UNUSED(event)
    m_bMousePressed = false;
}

void ZViewerWidget::resizeEvent(QResizeEvent *event) {
    Q_UNUSED(event)
    int iX = width() - ui->buttonsWidget->width();
    int iY = ui->buttonsWidget->y();
    ui->buttonsWidget->move(iX, iY);
}

void ZViewerWidget::contextMenuEvent(QContextMenuEvent *event) {
    m_menu.exec(QCursor::pos());
    event->accept();
}

void ZViewerWidget::timerEvent(QTimerEvent *event) {
    for (int i = 0; i < m_vecViews.size(); ++i) {
        if (m_vecViews[i] != nullptr) {
            m_vecViews[i]->DrawVideo();
        }
    }
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

void ZViewerWidget::slotAddCamera() {
    EditCamera(-1);
}

void ZViewerWidget::slotEditCamera() {
    int index = ui->cameraConfigWidget->currentIndex().row();
    if (index < 0) {
        QMessageBox::information(this, TOUTF8("错误"), TOUTF8("请选择摄像机结点"));
        return;
    }
    EditCamera(index);
}

void ZViewerWidget::slotDeleteCamera() {
    int index = ui->cameraConfigWidget->currentIndex().row();
    if (index < 0) {
        QMessageBox::information(this, TOUTF8("错误"), TOUTF8("请选择摄像机结点"));
        return;
    }
    QString strName = ui->cameraConfigWidget->currentItem()->text();
    QString strMessage = TOUTF8("请确认是否删除摄像机:");
    strMessage += strName;
    strMessage += TOUTF8("吗?");
    if (QMessageBox::information(this, TOUTF8("提示"), strMessage, QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes) {
        return;
    }
    ZCameraConfig::GetInstance()->DeleteCameraData(index);
    ZCameraConfig::GetInstance()->SaveConfig("config.json");
    RefreshCamera();
}

