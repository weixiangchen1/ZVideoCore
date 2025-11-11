#ifndef ZCAMERAWIDGET_H
#define ZCAMERAWIDGET_H

#include <QObject>
#include <QWidget>

class ZDecodeTask;
class ZDemuxTask;
class ZVideoView;
class ZCameraWidget : public QWidget {
    Q_OBJECT
public:
    explicit ZCameraWidget(QWidget *parent = nullptr);
    ~ZCameraWidget();
    // 打开Rtsp流进行解封装解码
    bool OpenCamera(const char* pUrl);
    // 渲染
    bool DrawVideo();

protected:
    void dropEvent(QDropEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    ZDecodeTask* m_pDecodeTask = nullptr;
    ZDemuxTask* m_pDemuxTask = nullptr;
    ZVideoView* m_pVideoView = nullptr;
};

#endif // ZCAMERAWIDGET_H
