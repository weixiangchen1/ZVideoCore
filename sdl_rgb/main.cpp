#include <iostream>
#include "sdl/SDL.h"

#pragma comment(lib, "SDL2.lib")

#undef main
int main(int argc, char* argv[]) {
    int iWidth = 800, iHeight = 600;
    // 1.初始化SDL video库
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "SDL_Init error: " << SDL_GetError() << std::endl;
        return -1;
    }

    // 2.生成SDL窗口
    SDL_Window* pScreen = SDL_CreateWindow("SDL Window",              // 窗口标题
                          SDL_WINDOWPOS_CENTERED,                     // 窗口位置居中显示                  
                          SDL_WINDOWPOS_CENTERED,
                          iWidth, iHeight,                            // 窗口宽高
                          SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);  // 设置窗口类型和可调整大小
    if (pScreen == nullptr) {
        std::cout << "SDL_CreateWindow error: " << SDL_GetError() << std::endl;
        return -2;
    }

    // 3.生成渲染器
    SDL_Renderer* pRenderer = SDL_CreateRenderer(pScreen, 
                                                 -1,                        // 要使用的渲染驱动程序的索引，-1 表示初始化第一个支持的驱动程序
                                                 SDL_RENDERER_ACCELERATED); // 使用硬件加速
    if (pRenderer == nullptr) {
        std::cout << "SDL_CreateRenderer error: " << SDL_GetError() << std::endl;
        return -3;
    }

    // 4.生成材质
    SDL_Texture* pTexture = SDL_CreateTexture(pRenderer, 
                                              SDL_PIXELFORMAT_ABGR8888,     // 纹理的像素格式
                                              SDL_TEXTUREACCESS_STREAMING,  // 变化频繁，可锁定
                                              iWidth, iHeight);
    if (pTexture == nullptr) {
        std::cout << "SDL_CreateTexture error: " << SDL_GetError() << std::endl;
        return -4;
    }

    // 5.创建图像数据
    std::shared_ptr<unsigned char> pImage(new unsigned char[iWidth * iHeight * 4]);
    auto pData = pImage.get();
    unsigned char tempPixel = 255;
    while (true) {
        // 添加事件控制图像变化和窗口退出
        SDL_Event event;
        SDL_WaitEventTimeout(&event, 10);
        if (event.type == SDL_QUIT) {
            SDL_DestroyWindow(pScreen);
            break;
        }

        tempPixel--;
        for (int i = 0; i < iHeight; ++i) {
            int iLine = i * iWidth * 4;
            for (int j = 0; j < iWidth * 4; j += 4) {
                pData[iLine + j] = 0;
                pData[iLine + j + 1] = tempPixel;
                pData[iLine + j + 2] = 0;
                pData[iLine + j + 3] = 0;
            }
        }

        // 6.将内存图像数据写入材质
        SDL_UpdateTexture(pTexture, 
                          NULL,         // 要更新的矩形区域，NULL 表示整个纹理
                          pData,        // 包含新像素数据的缓冲区
                          iWidth * 4);  // 一行像素数据的字节数（宽度 × 每像素字节数）

        // 7.清理屏幕
        SDL_RenderClear(pRenderer);

        // 8.复制材质到渲染器
        SDL_Rect destRect;
        destRect.x = 0; destRect.y = 0;
        destRect.w = iWidth; destRect.h = iHeight;
        SDL_RenderCopy(pRenderer, pTexture, 
                       NULL,        // 源纹理中的矩形区域，NULL 表示整个纹理
                       &destRect);  // 目标矩形区域，NULL 表示整个渲染目标

        // 9.渲染
        SDL_RenderPresent(pRenderer);
    }

    return 0;
}