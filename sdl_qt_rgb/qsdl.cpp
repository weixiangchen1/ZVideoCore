#include "qsdl.h"
#include "sdl/SDL.h"

#pragma comment(lib, "SDL2.lib")

static SDL_Window* g_pWindow = nullptr;
static SDL_Renderer* g_pRenderer = nullptr;
static SDL_Texture* g_pTexture = nullptr;
static int g_iWidth = -1;
static int g_iHeight = -1;
static unsigned char* g_pData = nullptr;

// 使用SDL将RGB图像数据渲染到QWidget
QSDL::QSDL(QWidget *parent) : QWidget(parent) {
    ui.setupUi(this);
    g_iWidth = ui.label->width();
    g_iHeight = ui.label->height();
    SDL_Init(SDL_INIT_VIDEO);
    g_pWindow = SDL_CreateWindowFrom((const void*)ui.label->winId());
    g_pRenderer = SDL_CreateRenderer(g_pWindow, -1, SDL_RENDERER_ACCELERATED);
    g_pTexture = SDL_CreateTexture(g_pRenderer,
                      SDL_PIXELFORMAT_ABGR8888,
                      SDL_TEXTUREACCESS_STREAMING,
                      g_iWidth, g_iHeight);
    g_pData = new unsigned char[g_iWidth * g_iHeight * 4];
    startTimer(10);
}

QSDL::~QSDL() {
    delete[] g_pData;
}

void QSDL::timerEvent(QTimerEvent* event) {
    static unsigned char tempPixel = 255;
    tempPixel--;
    for (int i = 0; i < g_iHeight; ++i) {
        int iLine = i * g_iWidth * 4;
        for (int j = 0; j < g_iWidth * 4; j += 4) {
            g_pData[iLine + j] = 0;
            g_pData[iLine + j + 1] = tempPixel;
            g_pData[iLine + j + 2] = 0;
            g_pData[iLine + j + 3] = 0;
        }
    }
    SDL_UpdateTexture(g_pTexture, NULL, g_pData, g_iWidth * 4);
    SDL_RenderClear(g_pRenderer);

    SDL_Rect destRect;
    destRect.x = 0; destRect.y = 0;
    destRect.w = g_iWidth; destRect.h = g_iHeight;
    SDL_RenderCopy(g_pRenderer, g_pTexture, NULL, &destRect);

    SDL_RenderPresent(g_pRenderer);
}

