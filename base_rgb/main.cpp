#include "basergb.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    BaseRGB window;
    window.show();
    return app.exec();
}
