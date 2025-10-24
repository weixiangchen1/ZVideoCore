#ifndef ZVIEWERWIDGET_H
#define ZVIEWERWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class ZViewerWidget; }
QT_END_NAMESPACE

class ZViewerWidget : public QWidget {
    Q_OBJECT
public:
    ZViewerWidget(QWidget *parent = nullptr);
    ~ZViewerWidget();

private:
    Ui::ZViewerWidget *ui;
};
#endif // ZVIEWERWIDGET_H
