#ifndef ZCAMERARECORD_H
#define ZCAMERARECORD_H

#include <QObject>
#include "ZThread.h"

class ZCameraRecord : public ZThread {
public:
    void SetCameraUrl(const QString& strUrl);
    void SetSavePath(const QString& strSavePath);
    void SetFileSec(long long lFileSec);

private:
    void threadFunc();
    QString m_strRtspUrl;
    QString m_strSavePath;
    long long m_lFileSec = 5;
};

#endif // ZCAMERARECORD_H
