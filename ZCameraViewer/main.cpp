#include "ZViewerWidget.h"

#include <QApplication>
#include <QIcon>
#include <QFontDatabase>
#include <QDir>
#include "ZCameraRecord.h"

void LoadFont() {
    QFont font;
    int iFontID = QFontDatabase::addApplicationFont(":/res/wqy-microhei.ttc");
    QStringList listFontFamilies = QFontDatabase::applicationFontFamilies(iFontID);
    if (listFontFamilies.isEmpty() == false) {
        font.setFamily(listFontFamilies.at(0));
    } else {
        font.setFamily(QStringLiteral("Times New Roman"));
    }
    font.setPixelSize(14);
    qApp->setFont(font);
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    ZViewerWidget w;
    LoadFont();
    w.show();
//    QString strPath = "./video/0/";
//    QDir dir;
//    dir.mkpath(strPath);
//    ZCameraRecord record;
//    record.SetCameraUrl("rtsp://127.0.0.1:8554/test");
//    record.SetSavePath(strPath);
//    record.Start();

    return a.exec();
}
