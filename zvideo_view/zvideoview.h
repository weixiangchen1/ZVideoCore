#pragma once
#include <mutex>

// ��Ƶ��Ⱦ�ӿ���
// 1.����SDLʵ�� 2.��Ⱦ��������� 3.�̰߳�ȫ
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
    // ��ʼ����Ⱦ���� (�̰߳�ȫ)
    // @para iWidth ���ڿ��
    // @para iHeight ���ڸ߶�
    // @para eFormat ��Ⱦ��ʽ
    // @para pWinId ���ھ�������Ϊ�մ����´���
    // @return �Ƿ��ʼ�������ɹ�
    virtual bool Init(int iWidth, int iHeight, VideoFormat eFormat = VideoFormat::RGBA, void* pWinId = nullptr) = 0;

    /////////////////////////////////////////////////////////
    // ��Ⱦͼ�� (�̰߳�ȫ)
    // @para pData ��Ⱦ�Ķ���������
    // @para iLineSize һ�����ݵ��ֽ�����eg.YUV420PΪYһ���ֽ���
    //       iLineSize <= 0 ���ݿ�Ⱥ����ظ�ʽ�Զ��Ƶ���С
    // @return �Ƿ���Ⱦ�ɹ�
    virtual bool Draw(const unsigned char* pData, int iLineSize = 0) = 0;

    // �����������Դ
    virtual void Close() = 0;

    // �������˳��¼�
    virtual bool IsExit() = 0;

    // �������ųߴ�
    void Scale(int iScaleWidth, int iScaleHeight);

protected:
    int m_iWidth = 0;                           // ���ڿ��
    int m_iHeight = 0;                          // ���ڸ߶�
    VideoFormat m_eFormat = VideoFormat::RGBA;  // ���ظ�ʽ
    std::mutex m_mutex;                         // ������
    int m_iScaleWidth = 0;                      // ���ſ��
    int m_iScaleHeight = 0;                     // ���Ÿ߶�
};