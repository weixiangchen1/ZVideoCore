#pragma once
#include "ZVideoView.h"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
class ZSDLView :public ZVideoView {
public:
    static bool InitSDLVideo();
    ////////////////////////////////////////////////////////////////////////
    // 初始化渲染窗口 (线程安全)
    // @para iWidth 窗口宽度
    // @para iHeight 窗口高度
    // @para eFormat 渲染格式
    // @return 是否初始化创建成功
    bool Init(int iWidth, int iHeight, VideoFormat eFormat = VideoFormat::RGBA) override;

    ////////////////////////////////////////////////////////////////////////
    // 渲染图像 (线程安全)
    // @para pData 渲染的二进制数据
    // @para iLineSize 一行数据的字节数，eg.YUV420P为Y一行字节数
    // iLineSize <= 0 根据宽度和像素格式自动推导大小
    // @return 是否渲染成功
    bool Draw(const unsigned char* pData, int iLineSize = 0) override;

    ////////////////////////////////////////////////////////////////////////
    // 渲染YUV格式图像 (线程安全)
    // @para pYData 指向Y分量数据的指针 iYPitch 表示Y平面中每行的字节数（stride）
    // @para pUData 指向U分量数据的指针 iUPitch 表示U平面中每行的字节数（stride）
    // @para pVData 指向V分量数据的指针 iVPitch 表示V平面中每行的字节数（stride）
    // @return 是否渲染成功
    bool Draw(const unsigned char* pYData, int iYPitch,
              const unsigned char* pUData, int iUPitch,
              const unsigned char* pVData, int iVPitch);

    // 清理申请的资源
    void Close() override;

    // 处理窗口退出事件
    bool IsExit() override;

private:
    SDL_Window* m_pSrceen;
    SDL_Renderer* m_pRenderer;
    SDL_Texture* m_pTexture;
};

