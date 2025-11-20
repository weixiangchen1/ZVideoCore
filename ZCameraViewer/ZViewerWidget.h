#ifndef ZVIEWERWIDGET_H
#define ZVIEWERWIDGET_H

#include <QWidget>
#include <QMenu>

QT_BEGIN_NAMESPACE
namespace Ui { class ZViewerWidget; }
QT_END_NAMESPACE

class ZCameraWidget;
class ZCameraRecord;
class ZViewerWidget : public QWidget {
    Q_OBJECT
public:
    ZViewerWidget(QWidget *parent = nullptr);
    ~ZViewerWidget();
    void SetupUI();
    void CreateViewWindow(int iWindowNum);
    void RefreshCamera();
    void EditCamera(int index);

protected:
    // 鼠标事件 拖动窗口
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    void resizeEvent(QResizeEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

    void timerEvent(QTimerEvent* event) override;

public slots:
    void slotMaxWindow();       // 最大化窗口
    void slotRestoreWindow();   // 还原窗口
    void slotViewOne();         // 切换为1窗口视图
    void slotViewFour();        // 切换为4窗口视图
    void slotViewNine();        // 切换为9窗口视图
    void slotViewSixteen();     // 切换为16窗口视图
    void slotAddCamera();       // 添加相机配置
    void slotEditCamera();      // 编辑相机配置
    void slotDeleteCamera();    // 删除相机配置
    void slotStartRecord();     // 启动所有摄像头录制
    void slotStopRecord();      // 停止所以摄像头录制

private:
    Ui::ZViewerWidget *ui;
    bool m_bMousePressed = false;
    QPoint m_pointMoveBefore;
    QMenu m_menu;
    QVector<ZCameraWidget*> m_vecViews;
    QVector<ZCameraRecord*> m_vecRecorders;
};
#endif // ZVIEWERWIDGET_H
