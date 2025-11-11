#ifndef ZVIEWERWIDGET_H
#define ZVIEWERWIDGET_H

#include <QWidget>
#include <QMenu>

QT_BEGIN_NAMESPACE
namespace Ui { class ZViewerWidget; }
QT_END_NAMESPACE

class ZCameraWidget;
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
    void slotMaxWindow();
    void slotRestoreWindow();
    void slotViewOne();
    void slotViewFour();
    void slotViewNine();
    void slotViewSixteen();
    void slotAddCamera();
    void slotEditCamera();
    void slotDeleteCamera();

private:
    Ui::ZViewerWidget *ui;
    bool m_bMousePressed = false;
    QPoint m_pointMoveBefore;
    QMenu m_menu;
    QVector<ZCameraWidget*> m_vecViews;
};
#endif // ZVIEWERWIDGET_H
