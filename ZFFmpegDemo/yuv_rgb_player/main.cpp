#include "zplayer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    ZPlayer window;
    window.show();
    return app.exec();
}
