#include "basergb.h"
#include <QPainter>
#include <QImage>

static int g_iWidth = 1280;
static int g_iHeight = 720;

BaseRGB::BaseRGB(QWidget *parent)
    : QWidget(parent) {
    ui.setupUi(this);
    resize(g_iWidth, g_iHeight);
}

BaseRGB::~BaseRGB() {}

void BaseRGB::paintEvent(QPaintEvent* event) {
    // 1.����QImage����ʽΪRGB888
    // [R G B] [R G B] [R G B] ...
    // [R G B] [R G B] [R G B]
    // [R G B] [R G B] [R G B]
    // ...
    QImage img(g_iWidth, g_iHeight, QImage::Format_RGB888);
    // 2.��ȡָ��ͼ��ײ�����������ʼλ�õ� uchar*
    auto data = img.bits();
    unsigned char r = 255;
    // 3.����QImage
    for (int i = 0; i < g_iHeight; ++i) {
        r--;
        int b = i * g_iWidth * 3;
        for (int j = 0; j < g_iWidth * 3; j += 3) {
            data[b + j] = r;
            data[b + j + 1] = 0;
            data[b + j + 2] = 0;
        }
    }
    // 4.QPainter����ͼ��QWidget
    QPainter p;
    p.begin(this);
    p.drawImage(0, 0, img);
    p.end();
}

