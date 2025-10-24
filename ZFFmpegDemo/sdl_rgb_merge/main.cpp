#include "qsdlmerge.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QSDLMerge window;
    window.show();
    return app.exec();
}
