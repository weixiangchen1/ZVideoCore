#pragma once
#include <mutex>
#include <fstream>

// 视频渲染接口类
// 1.隐藏SDL实现 2.渲染方案可替代 3.线程安全
struct AVFrame;
class ZVideoView {
public:
    // VideoFormat枚举值与ffmpeg的AVPixelFormat一致
    enum class VideoFormat {
        YUV420P = 0,
        RGB = 2,
        NV12 = 23,
        ARGB = 25,
        RGBA = 26,
        BGRA = 28,
        UNSUPPORT_FORMAT = -1
    };

    enum class RenderType {
        SDL = 0
    };
public:
    virtual ~ZVideoView();

    static ZVideoView* CreateVideoView(RenderType eType = RenderType::SDL);

    /////////////////////////////////////////////////////////
    // 初始化渲染窗口 (线程安全)
    // @para iWidth 窗口宽度
    // @para iHeight 窗口高度
    // @para eFormat 渲染格式
    // @return 是否初始化创建成功
    virtual bool Init(int iWidth, int iHeight, VideoFormat eFormat = VideoFormat::RGBA) = 0;

    /////////////////////////////////////////////////////////
    // 渲染图像 (线程安全)
    // @para pData 渲染的二进制数据
    // @para iLineSize 一行数据的字节数，eg.YUV420P为Y一行字节数
    //       iLineSize <= 0 根据宽度和像素格式自动推导大小
    // @return 是否渲染成功
    virtual bool Draw(const unsigned char* pData, int iLineSize = 0) = 0;

    /////////////////////////////////////////////////////////
    // 渲染YUV格式图像 (线程安全)
    // @para pYData 指向Y分量数据的指针 iYPitch 表示Y平面中每行的字节数（stride）
    // @para pUData 指向U分量数据的指针 iUPitch 表示U平面中每行的字节数（stride）
    // @para pVData 指向V分量数据的指针 iVPitch 表示V平面中每行的字节数（stride）
    // @return 是否渲染成功
    virtual bool Draw(const unsigned char* pYData, int iYPitch,
                      const unsigned char* pUData, int iUPitch,
                      const unsigned char* pVData, int iVPitch) = 0;

    // 清理申请的资源
    virtual void Close() = 0;

    // 处理窗口退出事件
    virtual bool IsExit() = 0;

    // 设置缩放尺寸
    void Scale(int iScaleWidth, int iScaleHeight);

    // 绘制AVFrame数据
    bool DrawFrame(AVFrame* pFrame);

    // 获取帧率
    int GetRenderFps();

    // 打开文件
    bool OpenFile(std::string strFile);

    // 读取一帧数据
    AVFrame* ReadFrame();

    // 设置窗口句柄
    void SetWindow(void* pWindow);

protected:
    void* m_pWindow = nullptr;                  // 窗口句柄
    int m_iWidth = 0;                           // 窗口宽度
    int m_iHeight = 0;                          // 窗口高度
    VideoFormat m_eFormat = VideoFormat::RGBA;  // 像素格式
    std::mutex m_mutex;                         // 互斥锁
    int m_iScaleWidth = 0;                      // 缩放宽度
    int m_iScaleHeight = 0;                     // 缩放高度
    int m_iFps = 0;                             // 帧率
    long long m_lBeginMs = 0;                   // 计时开始时间
    int m_iCount = 0;

private:
    std::ifstream m_file;
    AVFrame* m_pFrame = nullptr;
    unsigned char* m_pCacheBuf = nullptr;       // 用于复制NV12的缓冲区
};