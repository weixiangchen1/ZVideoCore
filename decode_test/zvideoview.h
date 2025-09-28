#pragma once
#include <mutex>
#include <fstream>

// ��Ƶ��Ⱦ�ӿ���
// 1.����SDLʵ�� 2.��Ⱦ��������� 3.�̰߳�ȫ
struct AVFrame;
class ZVideoView {
public:
    // VideoFormatö��ֵ��ffmpeg��AVPixelFormatһ��
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
    // ��ʼ����Ⱦ���� (�̰߳�ȫ)
    // @para iWidth ���ڿ��
    // @para iHeight ���ڸ߶�
    // @para eFormat ��Ⱦ��ʽ
    // @return �Ƿ��ʼ�������ɹ�
    virtual bool Init(int iWidth, int iHeight, VideoFormat eFormat = VideoFormat::RGBA) = 0;

    /////////////////////////////////////////////////////////
    // ��Ⱦͼ�� (�̰߳�ȫ)
    // @para pData ��Ⱦ�Ķ���������
    // @para iLineSize һ�����ݵ��ֽ�����eg.YUV420PΪYһ���ֽ���
    //       iLineSize <= 0 ���ݿ�Ⱥ����ظ�ʽ�Զ��Ƶ���С
    // @return �Ƿ���Ⱦ�ɹ�
    virtual bool Draw(const unsigned char* pData, int iLineSize = 0) = 0;

    /////////////////////////////////////////////////////////
    // ��ȾYUV��ʽͼ�� (�̰߳�ȫ)
    // @para pYData ָ��Y�������ݵ�ָ�� iYPitch ��ʾYƽ����ÿ�е��ֽ�����stride��
    // @para pUData ָ��U�������ݵ�ָ�� iUPitch ��ʾUƽ����ÿ�е��ֽ�����stride��
    // @para pVData ָ��V�������ݵ�ָ�� iVPitch ��ʾVƽ����ÿ�е��ֽ�����stride��
    // @return �Ƿ���Ⱦ�ɹ�
    virtual bool Draw(const unsigned char* pYData, int iYPitch,
                      const unsigned char* pUData, int iUPitch,
                      const unsigned char* pVData, int iVPitch) = 0;

    // �����������Դ
    virtual void Close() = 0;

    // �������˳��¼�
    virtual bool IsExit() = 0;

    // �������ųߴ�
    void Scale(int iScaleWidth, int iScaleHeight);

    // ����AVFrame����
    bool DrawFrame(AVFrame* pFrame);

    // ��ȡ֡��
    int GetRenderFps();

    // ���ļ�
    bool OpenFile(std::string strFile);

    // ��ȡһ֡����
    AVFrame* ReadFrame();

    // ���ô��ھ��
    void SetWindow(void* pWindow);

protected:
    void* m_pWindow = nullptr;                  // ���ھ��
    int m_iWidth = 0;                           // ���ڿ��
    int m_iHeight = 0;                          // ���ڸ߶�
    VideoFormat m_eFormat = VideoFormat::RGBA;  // ���ظ�ʽ
    std::mutex m_mutex;                         // ������
    int m_iScaleWidth = 0;                      // ���ſ��
    int m_iScaleHeight = 0;                     // ���Ÿ߶�
    int m_iFps = 0;                             // ֡��
    long long m_lBeginMs = 0;                   // ��ʱ��ʼʱ��
    int m_iCount = 0;

private:
    std::ifstream m_file;
    AVFrame* m_pFrame = nullptr;
    unsigned char* m_pCacheBuf = nullptr;       // ���ڸ���NV12�Ļ�����
};