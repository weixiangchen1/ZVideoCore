#include "ZCameraConfig.h"
#include <QFile>
#include <QIODevice>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

QSharedPointer<ZCameraConfig> ZCameraConfig::m_pInstance;
QSharedPointer<ZCameraConfig> ZCameraConfig::GetInstance() {
    if (m_pInstance.isNull()) {
        m_pInstance = QSharedPointer<ZCameraConfig>(new ZCameraConfig);
    }
    return m_pInstance;
}

void ZCameraConfig::PushCameraData(const ZCameraData &data) {
    QMutexLocker locker(&m_mutex);
    m_vecCameras.push_back(data);
}

bool ZCameraConfig::DeleteCameraData(int index) {
    QMutexLocker locker(&m_mutex);
    if (index < 0 || index > m_vecCameras.size()) {
        return false;
    }
    m_vecCameras.remove(index);
    return true;
}

ZCameraData ZCameraConfig::GetCameraData(int index) {
    QMutexLocker locker(&m_mutex);
    if (index < 0 || index > m_vecCameras.size()) {
        return ZCameraData();
    }
    return m_vecCameras[index];
}

bool ZCameraConfig::SetCameraData(int index, const ZCameraData &data) {
    QMutexLocker locker(&m_mutex);
    if (index < 0 || index > m_vecCameras.size()) {
        return false;
    }
    m_vecCameras[index] = data;
    return true;
}

int ZCameraConfig::GetCameraCount() {
    QMutexLocker locker(&m_mutex);
    return m_vecCameras.size();
}

bool ZCameraConfig::SaveConfig(const QString &strPath) {
    if (strPath.isEmpty()) {
        return false;
    }
    QFile saveFile(strPath);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        return false;
    }
    QJsonArray jsonCameras;
    QMutexLocker locker(&m_mutex);
    for (const auto& camera : m_vecCameras) {
        QJsonObject cameraObj;
        cameraObj["name"]      = camera.strName;
        cameraObj["url"]       = camera.strURL;
        cameraObj["sub_url"]   = camera.strSubURL;
        cameraObj["save_path"] = camera.strSavePath;
        jsonCameras.append(cameraObj);
    }

    QJsonObject root;
    root["cameras"] = jsonCameras;

    QJsonDocument doc(root);
    saveFile.write(doc.toJson(QJsonDocument::Indented));  // 美化缩进输出
    saveFile.close();
    return true;
}

bool ZCameraConfig::LoadConfig(const QString &strPath) {
    if (strPath.isEmpty()) {
        return false;
    }
    QFile file(strPath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray baCameras = file.readAll();
    file.close();

    QJsonParseError err;
    QJsonDocument docCameras = QJsonDocument::fromJson(baCameras, &err);
    if (err.error != QJsonParseError::NoError || !docCameras.isObject()) {
        return false;
    }

    QJsonObject root = docCameras.object();
    if (!root.contains("cameras") || !root["cameras"].isArray()) {
        return false;
    }

    QJsonArray arrCameras = root["cameras"].toArray();

    QVector<ZCameraData> temp;
    for (const QJsonValue& cameraValue : arrCameras) {
        if (!cameraValue.isObject()) {
            continue;
        }
        QJsonObject cameraObj = cameraValue.toObject();
        ZCameraData camera;
        camera.strName     = cameraObj["name"].toString();
        camera.strURL      = cameraObj["url"].toString();
        camera.strSubURL   = cameraObj["sub_url"].toString();
        camera.strSavePath = cameraObj["save_path"].toString();
        temp.append(camera);
    }

    QMutexLocker locker(&m_mutex);
    m_vecCameras = temp;
    return true;
}

ZCameraConfig::ZCameraConfig() {

}
