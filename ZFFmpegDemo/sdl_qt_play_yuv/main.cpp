#include "qsdlyuv.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QSDLYUV window;
    window.show();
    return app.exec();
}
