#include "qsdlyuv.h"
#include "sdl/SDL.h"
#include <fstream>

#pragma comment(lib, "SDL2.lib")

static SDL_Window* g_pWindow = nullptr;
static SDL_Renderer* g_pRenderer = nullptr;
static SDL_Texture* g_pTexture = nullptr;
static int g_iWidth = 400;
static int g_iHeight = 300;
static unsigned char* g_pData = nullptr;
static std::ifstream g_yuvFile;

QSDLYUV::QSDLYUV(QWidget *parent)
    : QWidget(parent) {
    ui.setupUi(this);
    resize(g_iWidth, g_iHeight);
    ui.label->move(0, 0);
    ui.label->resize(g_iWidth, g_iHeight);
    g_yuvFile.open("400_300_25.yuv", std::ios::binary);
    if (!g_yuvFile) {
        exit(-1);
    }

    SDL_Init(SDL_INIT_VIDEO);
    g_pWindow = SDL_CreateWindowFrom((const void*)ui.label->winId());
    g_pRenderer = SDL_CreateRenderer(g_pWindow, -1, SDL_RENDERER_ACCELERATED);
    g_pTexture = SDL_CreateTexture(g_pRenderer,
        SDL_PIXELFORMAT_IYUV,
        SDL_TEXTUREACCESS_STREAMING,
        g_iWidth, g_iHeight);
    g_pData = new unsigned char[g_iWidth * g_iHeight * 1.5];
    startTimer(10);
}

QSDLYUV::~QSDLYUV() {}

void QSDLYUV::timerEvent(QTimerEvent* event) {
    g_yuvFile.read((char*)g_pData, g_iWidth * g_iHeight * 1.5);
    // YUV420P为平面存储
    // YYYYYYYY UU VV
    SDL_UpdateTexture(g_pTexture, NULL, g_pData, 
        g_iWidth    // 只需提供一行Y宽度
    );
    SDL_RenderClear(g_pRenderer);

    SDL_Rect destRect;
    destRect.x = 0; destRect.y = 0;
    destRect.w = g_iWidth; destRect.h = g_iHeight;
    SDL_RenderCopy(g_pRenderer, g_pTexture, NULL, &destRect);

    SDL_RenderPresent(g_pRenderer);
}

