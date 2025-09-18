#include "zsdlview.h"
#include <iostream>
#include "sdl/SDL.h"
#pragma comment(lib, "SDL2.lib")

bool ZSDLView::InitSDLVideo() {
    static bool bIsFirst = true;
    static std::mutex mutex;
    if (bIsFirst == false) {
        return true;
    }
    bIsFirst = false;
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init error: " << SDL_GetError() << std::endl;
        return false;
    }
    // 采用线性差值算法，解决缩放时锯齿问题
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    return true;
}

bool ZSDLView::Init(int iWidth, int iHeight, VideoFormat eFormat, void* pWinId) {
    if (iWidth <= 0 || iHeight <= 0) {
        std::cerr << "Error param" << std::endl;
        return false;
    }
    // 初始化SDL视频库
    InitSDLVideo();
    // 确保线程安全
    std::unique_lock<std::mutex> lock(m_mutex);
    m_iWidth = iWidth;
    m_iHeight = iHeight;
    m_eFormat = eFormat;

    if (m_pTexture != nullptr) {
        SDL_DestroyTexture(m_pTexture);
    }

    if (m_pRenderer != nullptr) {
        SDL_DestroyRenderer(m_pRenderer);
    }

    // 创建窗口
    if (m_pSrceen == nullptr) {
        if (pWinId == nullptr) {
            m_pSrceen = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                iWidth, iHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
        } else {
            m_pSrceen = SDL_CreateWindowFrom(pWinId);
        }
    }
    if (m_pSrceen == nullptr) {
        std::cerr << "SDL_CreateWindow error: " << SDL_GetError() << std::endl;
        return false;
    }

    // 创建渲染器
    m_pRenderer = SDL_CreateRenderer(m_pSrceen, -1, SDL_RENDERER_ACCELERATED);
    if (m_pRenderer == nullptr) {
        std::cerr << "SDL_CreateRenderer error: " << SDL_GetError() << std::endl;
        return false;
    }

    // 创建材质
    unsigned int iFormat = SDL_PIXELFORMAT_RGBA8888;
    switch (eFormat) {
    case ZVideoView::VideoFormat::ARGB:
        iFormat = SDL_PIXELFORMAT_ARGB32;
        break;
    case ZVideoView::VideoFormat::YUV420P:
        iFormat = SDL_PIXELFORMAT_IYUV;
        break;
    case ZVideoView::VideoFormat::RGBA:
    default:
        break;
    }
    m_pTexture = SDL_CreateTexture(m_pRenderer, iFormat, SDL_TEXTUREACCESS_STREAMING, iWidth, iHeight);
    if (m_pTexture == nullptr) {
        std::cerr << "SDL_CreateTexture error: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

bool ZSDLView::Draw(const unsigned char* pData, int iLineSize) {
    if (pData == nullptr) {
        return false;
    }
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_pSrceen == nullptr || m_pRenderer == nullptr || m_pTexture == nullptr ||
        m_iWidth <= 0 || m_iHeight <= 0) {
        return false;
    }

    if (iLineSize <= 0) {
        switch (m_eFormat) {
        case ZVideoView::VideoFormat::RGBA:
        case ZVideoView::VideoFormat::ARGB:
            iLineSize = m_iWidth * 4;
            break;
        case ZVideoView::VideoFormat::YUV420P:
            iLineSize = m_iWidth;
            break;
        case ZVideoView::VideoFormat::UNSUPPORT_FORMAT:
        default:
            break;
        }
    }
    if (iLineSize <= 0) {
        return false;
    }
    
    // 更新材质 复制内存数据到显存
    if (SDL_UpdateTexture(m_pTexture, nullptr, pData, iLineSize) != 0) {
        std::cerr << "SDL_UpdateTexture error: " << SDL_GetError() << std::endl;
        return false;
    }

    // 清空屏幕
    SDL_RenderClear(m_pRenderer);
    
    // 材质复制到渲染器
    SDL_Rect destRect, *pDestRect = nullptr;
    if (m_iScaleWidth > 0 && m_iScaleHeight > 0) {
        destRect.x = 0; destRect.y = 0;
        destRect.w = m_iScaleWidth;
        destRect.h = m_iScaleHeight;
        pDestRect = &destRect;
    }

    if (SDL_RenderCopy(m_pRenderer, m_pTexture, nullptr, pDestRect) != 0) {
        std::cerr << "SDL_RenderCopy error: " << SDL_GetError() << std::endl;
        return false;
    }
    SDL_RenderPresent(m_pRenderer);

    return true;
}

bool ZSDLView::Draw(const unsigned char* pYData, int iYPitch, 
                    const unsigned char* pUData, int iUPitch, 
                    const unsigned char* pVData, int iVPitch) {
    // 参数检查
    if (pYData == nullptr || pUData == nullptr || pVData == nullptr) {
        return false;
    }
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_pSrceen == nullptr || m_pRenderer == nullptr || m_pTexture == nullptr ||
        m_iWidth <= 0 || m_iHeight <= 0) {
        return false;
    }

    // 更新材质 复制内存数据到显存
    if (SDL_UpdateYUVTexture(m_pTexture, nullptr, pYData, iYPitch, pUData, iUPitch, pVData, iVPitch) != 0) {
        std::cerr << "SDL_UpdateTexture error: " << SDL_GetError() << std::endl;
        return false;
    }

    // 清空屏幕
    SDL_RenderClear(m_pRenderer);

    // 材质复制到渲染器
    SDL_Rect destRect, * pDestRect = nullptr;
    if (m_iScaleWidth > 0 && m_iScaleHeight > 0) {
        destRect.x = 0; destRect.y = 0;
        destRect.w = m_iScaleWidth;
        destRect.h = m_iScaleHeight;
        pDestRect = &destRect;
    }

    if (SDL_RenderCopy(m_pRenderer, m_pTexture, nullptr, pDestRect) != 0) {
        std::cerr << "SDL_RenderCopy error: " << SDL_GetError() << std::endl;
        return false;
    }
    SDL_RenderPresent(m_pRenderer);

    return true;
}

void ZSDLView::Close() {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_pTexture != nullptr) {
        SDL_DestroyTexture(m_pTexture);
        m_pTexture = nullptr;
    }

    if (m_pRenderer != nullptr) {
        SDL_DestroyRenderer(m_pRenderer);
        m_pRenderer = nullptr;
    }

    if (m_pSrceen != nullptr) {
        SDL_DestroyWindow(m_pSrceen);
        m_pSrceen = nullptr;
    }
}

bool ZSDLView::IsExit() {
    SDL_Event event;
    SDL_WaitEventTimeout(&event, 1);
    if (event.type == SDL_QUIT) {
        return true;
    }
    return false;
}
