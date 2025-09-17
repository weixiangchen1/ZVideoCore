#include "zvideoviewtest.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    ZVideoViewTest window;
    window.show();
    return app.exec();
}
