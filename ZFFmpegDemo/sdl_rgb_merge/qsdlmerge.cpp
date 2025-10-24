#include "qsdlmerge.h"
#include "sdl/SDL.h"

#pragma comment(lib, "SDL2.lib")

static SDL_Window* g_pWindow = nullptr;
static SDL_Renderer* g_pRenderer = nullptr;
static SDL_Texture* g_pTexture = nullptr;
static int g_iWidth = -1;
static int g_iHeight = -1;
static unsigned char* g_pData = nullptr;
static int g_iPixSize = 4;

QSDLMerge::QSDLMerge(QWidget *parent)
    : QWidget(parent) {
    ui.setupUi(this);
    SDL_Init(SDL_INIT_VIDEO);
    g_pWindow = SDL_CreateWindowFrom((const void*)ui.label->winId());
    g_pRenderer = SDL_CreateRenderer(g_pWindow, -1, SDL_RENDERER_ACCELERATED);

    QImage img1("1.jpg");
    QImage img2("2.jpg");
    if (img1.isNull() || img2.isNull()) {
        return;
    }
    g_iWidth = img1.width() + img2.width();
    g_iHeight = qMax(img1.height(), img2.height());
    resize(g_iWidth, g_iHeight);
    ui.label->move(0, 0);
    ui.label->resize(g_iWidth, g_iHeight);

    g_pTexture = SDL_CreateTexture(g_pRenderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        g_iWidth, g_iHeight);
    g_pData = new unsigned char[g_iWidth * g_iHeight * g_iPixSize];

    // 设置图像数据为透明
    memset(g_pData, 0, g_iWidth * g_iHeight * g_iPixSize);

    // 合并两张图像
    for (int i = 0; i < g_iHeight; ++i) {
        int iLine = i * g_iWidth * g_iPixSize;
        if (i < img1.height()) {
            memcpy(g_pData + iLine, img1.scanLine(i), img1.width() * g_iPixSize);
        }
        iLine += img1.width() * g_iPixSize;
        if (i < img2.height()) {
            memcpy(g_pData + iLine, img2.scanLine(i), img2.width() * g_iPixSize);
        }
        iLine += img2.width() * g_iPixSize;
    }
    QImage out(g_pData, g_iWidth, g_iHeight, QImage::Format_ARGB32);
    out.save("out.png");
    startTimer(10);
}

QSDLMerge::~QSDLMerge() {}

void QSDLMerge::timerEvent(QTimerEvent* event) {
    static unsigned char tempPixel = 255;
    tempPixel--;
    for (int i = 0; i < g_iHeight; ++i) {
        int iLine = i * g_iWidth * g_iPixSize;
        for (int j = 0; j < g_iWidth * g_iPixSize; j += g_iPixSize) {
            //g_pData[iLine + j] = 0;
            //g_pData[iLine + j + 1] = tempPixel;
            //g_pData[iLine + j + 2] = 0;
            //g_pData[iLine + j + 3] = 0;
        }
    }
    SDL_UpdateTexture(g_pTexture, NULL, g_pData, g_iWidth * g_iPixSize);
    SDL_RenderClear(g_pRenderer);

    SDL_Rect destRect;
    destRect.x = 0; destRect.y = 0;
    destRect.w = g_iWidth; destRect.h = g_iHeight;
    SDL_RenderCopy(g_pRenderer, g_pTexture, NULL, &destRect);

    SDL_RenderPresent(g_pRenderer);
}

