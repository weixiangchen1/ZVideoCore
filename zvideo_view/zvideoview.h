#pragma once
#include <mutex>

// 视频渲染接口类
// 1.隐藏SDL实现 2.渲染方案可替代 3.线程安全
class ZVideoView {
public:
    enum class VideoFormat {
        RGBA = 0,
        ARGB,     
        YUV420P,
        UNSUPPORT_FORMAT = -1
    };

    enum class RenderType {
        SDL = 0
    };
public:
    static ZVideoView* CreateVideoView(RenderType eType = RenderType::SDL);

    /////////////////////////////////////////////////////////
    // 初始化渲染窗口 (线程安全)
    // @para iWidth 窗口宽度
    // @para iHeight 窗口高度
    // @para eFormat 渲染格式
    // @para pWinId 窗口句柄，如果为空创建新窗口
    // @return 是否初始化创建成功
    virtual bool Init(int iWidth, int iHeight, VideoFormat eFormat = VideoFormat::RGBA, void* pWinId = nullptr) = 0;

    /////////////////////////////////////////////////////////
    // 渲染图像 (线程安全)
    // @para pData 渲染的二进制数据
    // @para iLineSize 一行数据的字节数，eg.YUV420P为Y一行字节数
    //       iLineSize <= 0 根据宽度和像素格式自动推导大小
    // @return 是否渲染成功
    virtual bool Draw(const unsigned char* pData, int iLineSize = 0) = 0;

    // 清理申请的资源
    virtual void Close() = 0;

    // 处理窗口退出事件
    virtual bool IsExit() = 0;

    // 设置缩放尺寸
    void Scale(int iScaleWidth, int iScaleHeight);

protected:
    int m_iWidth = 0;                           // 窗口宽度
    int m_iHeight = 0;                          // 窗口高度
    VideoFormat m_eFormat = VideoFormat::RGBA;  // 像素格式
    std::mutex m_mutex;                         // 互斥锁
    int m_iScaleWidth = 0;                      // 缩放宽度
    int m_iScaleHeight = 0;                     // 缩放高度
};