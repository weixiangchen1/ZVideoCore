#pragma once
#include "zvideoview.h"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
class ZSDLView :public ZVideoView {
public:
    static bool InitSDLVideo();
    ////////////////////////////////////////////////////////////////////////
    // ��ʼ����Ⱦ���� (�̰߳�ȫ)
    // @para iWidth ���ڿ��
    // @para iHeight ���ڸ߶�
    // @para eFormat ��Ⱦ��ʽ
    // @return �Ƿ��ʼ�������ɹ�
    bool Init(int iWidth, int iHeight, VideoFormat eFormat = VideoFormat::RGBA) override;

    ////////////////////////////////////////////////////////////////////////
    // ��Ⱦͼ�� (�̰߳�ȫ)
    // @para pData ��Ⱦ�Ķ���������
    // @para iLineSize һ�����ݵ��ֽ�����eg.YUV420PΪYһ���ֽ���
    // iLineSize <= 0 ���ݿ�Ⱥ����ظ�ʽ�Զ��Ƶ���С
    // @return �Ƿ���Ⱦ�ɹ�
    bool Draw(const unsigned char* pData, int iLineSize = 0) override;

    ////////////////////////////////////////////////////////////////////////
    // ��ȾYUV��ʽͼ�� (�̰߳�ȫ)
    // @para pYData ָ��Y�������ݵ�ָ�� iYPitch ��ʾYƽ����ÿ�е��ֽ�����stride��
    // @para pUData ָ��U�������ݵ�ָ�� iUPitch ��ʾUƽ����ÿ�е��ֽ�����stride��
    // @para pVData ָ��V�������ݵ�ָ�� iVPitch ��ʾVƽ����ÿ�е��ֽ�����stride��
    // @return �Ƿ���Ⱦ�ɹ�
    bool Draw(const unsigned char* pYData, int iYPitch,
              const unsigned char* pUData, int iUPitch,
              const unsigned char* pVData, int iVPitch);

    // �����������Դ
    void Close() override;

    // �������˳��¼�
    bool IsExit() override;

private:
    SDL_Window* m_pSrceen;
    SDL_Renderer* m_pRenderer;
    SDL_Texture* m_pTexture;
};

