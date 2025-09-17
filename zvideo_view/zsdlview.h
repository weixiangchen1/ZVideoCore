#pragma once
#include "zvideoview.h"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
class ZSDLView :public ZVideoView {
public:
    static bool InitSDLVideo();
    /////////////////////////////////////////////////////////
    // 初始化渲染窗口 (线程安全)
    // @para iWidth 窗口宽度
    // @para iHeight 窗口高度
    // @para eFormat 渲染格式
    // @para pWinId 窗口句柄，如果为空创建新窗口
    // @return 是否初始化创建成功
    bool Init(int iWidth, int iHeight, VideoFormat eFormat = VideoFormat::RGBA, void* pWinId = nullptr) override;

    /////////////////////////////////////////////////////////
    // 渲染图像 (线程安全)
    // @para pData 渲染的二进制数据
    // @para iLineSize 一行数据的字节数，eg.YUV420P为Y一行字节数
    // iLineSize <= 0 根据宽度和像素格式自动推导大小
    // @return 是否渲染成功
    bool Draw(const unsigned char* pData, int iLineSize = 0) override;

    // 清理申请的资源
    void Close() override;

    // 处理窗口退出事件
    bool IsExit() override;

private:
    SDL_Window* m_pSrceen;
    SDL_Renderer* m_pRenderer;
    SDL_Texture* m_pTexture;
};

