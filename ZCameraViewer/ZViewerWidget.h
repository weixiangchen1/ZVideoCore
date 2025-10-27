#ifndef ZVIEWERWIDGET_H
#define ZVIEWERWIDGET_H

#include <QWidget>
#include <QMenu>

QT_BEGIN_NAMESPACE
namespace Ui { class ZViewerWidget; }
QT_END_NAMESPACE

class ZViewerWidget : public QWidget {
    Q_OBJECT
public:
    ZViewerWidget(QWidget *parent = nullptr);
    ~ZViewerWidget();
    void SetupUI();
    void CreateViewWindow(int iWindowNum);

    // 鼠标事件 拖动窗口
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    void resizeEvent(QResizeEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

public slots:
    void slotMaxWindow();
    void slotRestoreWindow();
    void slotViewOne();
    void slotViewFour();
    void slotViewNine();
    void slotViewSixteen();

private:
    Ui::ZViewerWidget *ui;
    bool m_bMousePressed = false;
    QPoint m_pointMoveBefore;
    QMenu m_menu;
    QVector<QWidget*> m_vecViews;
};
#endif // ZVIEWERWIDGET_H
