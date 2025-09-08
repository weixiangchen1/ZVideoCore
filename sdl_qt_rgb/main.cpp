#include "qsdl.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QSDL window;
    window.show();
    return app.exec();
}
