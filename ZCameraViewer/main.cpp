#include "ZViewerWidget.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    ZViewerWidget w;
    w.show();
    return a.exec();
}
