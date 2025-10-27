#ifndef ZCAMERACONFIG_H
#define ZCAMERACONFIG_H
#include <QObject>
#include <QMutex>
#include <QVector>
#include <QSharedPointer>

struct ZCameraData {
    QString strName;            // 摄像机名称
    QString strURL;             // 摄像机主码流
    QString strSubURL;          // 摄像机辅码流
    QString strSavePath;        // 视频录制存储路径
};

class ZCameraConfig : public QObject {
    Q_OBJECT
public:
    static QSharedPointer<ZCameraConfig> GetInstance();

    // 增删改查摄像机 线程安全
    void PushCameraData(const ZCameraData& data);
    bool DeleteCameraData(int index);
    ZCameraData GetCameraData(int index);
    bool SetCameraData(int index, const ZCameraData& data);
    int GetCameraCount();

    bool SaveConfig(const QString& strPath);
    bool LoadConfig(const QString& strPath);
private:
    ZCameraConfig();
    static QSharedPointer<ZCameraConfig> m_pInstance;
    QVector<ZCameraData> m_vecCameras;
    QMutex m_mutex;
};

#endif // ZCAMERACONFIG_H
