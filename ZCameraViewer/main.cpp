#include "ZViewerWidget.h"

#include <QApplication>
#include <QIcon>
#include <QFontDatabase>

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
    ZViewerWidget w;
    LoadFont();
    w.show();
    return a.exec();
}
