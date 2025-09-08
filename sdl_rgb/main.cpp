#include <iostream>
#include "sdl/SDL.h"

#pragma comment(lib, "SDL2.lib")

#undef main
int main(int argc, char* argv[]) {
    int iWidth = 800, iHeight = 600;
    // 1.��ʼ��SDL video��
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "SDL_Init error: " << SDL_GetError() << std::endl;
        return -1;
    }

    // 2.����SDL����
    SDL_Window* pScreen = SDL_CreateWindow("SDL Window",              // ���ڱ���
                          SDL_WINDOWPOS_CENTERED,                     // ����λ�þ�����ʾ                  
                          SDL_WINDOWPOS_CENTERED,
                          iWidth, iHeight,                            // ���ڿ��
                          SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);  // ���ô������ͺͿɵ�����С
    if (pScreen == nullptr) {
        std::cout << "SDL_CreateWindow error: " << SDL_GetError() << std::endl;
        return -2;
    }

    // 3.������Ⱦ��
    SDL_Renderer* pRenderer = SDL_CreateRenderer(pScreen, 
                                                 -1,                        // Ҫʹ�õ���Ⱦ���������������-1 ��ʾ��ʼ����һ��֧�ֵ���������
                                                 SDL_RENDERER_ACCELERATED); // ʹ��Ӳ������
    if (pRenderer == nullptr) {
        std::cout << "SDL_CreateRenderer error: " << SDL_GetError() << std::endl;
        return -3;
    }

    // 4.���ɲ���
    SDL_Texture* pTexture = SDL_CreateTexture(pRenderer, 
                                              SDL_PIXELFORMAT_ABGR8888,     // ��������ظ�ʽ
                                              SDL_TEXTUREACCESS_STREAMING,  // �仯Ƶ����������
                                              iWidth, iHeight);
    if (pTexture == nullptr) {
        std::cout << "SDL_CreateTexture error: " << SDL_GetError() << std::endl;
        return -4;
    }

    // 5.����ͼ������
    std::shared_ptr<unsigned char> pImage(new unsigned char[iWidth * iHeight * 4]);
    auto pData = pImage.get();
    unsigned char tempPixel = 255;
    while (true) {
        // ����¼�����ͼ��仯�ʹ����˳�
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

        // 6.���ڴ�ͼ������д�����
        SDL_UpdateTexture(pTexture, 
                          NULL,         // Ҫ���µľ�������NULL ��ʾ��������
                          pData,        // �������������ݵĻ�����
                          iWidth * 4);  // һ���������ݵ��ֽ�������� �� ÿ�����ֽ�����

        // 7.������Ļ
        SDL_RenderClear(pRenderer);

        // 8.���Ʋ��ʵ���Ⱦ��
        SDL_Rect destRect;
        destRect.x = 0; destRect.y = 0;
        destRect.w = iWidth; destRect.h = iHeight;
        SDL_RenderCopy(pRenderer, pTexture, 
                       NULL,        // Դ�����еľ�������NULL ��ʾ��������
                       &destRect);  // Ŀ���������NULL ��ʾ������ȾĿ��

        // 9.��Ⱦ
        SDL_RenderPresent(pRenderer);
    }

    return 0;
}