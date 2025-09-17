#pragma once
#include "zvideoview.h"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
class ZSDLView :public ZVideoView {
public:
    static bool InitSDLVideo();
    /////////////////////////////////////////////////////////
    // ��ʼ����Ⱦ���� (�̰߳�ȫ)
    // @para iWidth ���ڿ��
    // @para iHeight ���ڸ߶�
    // @para eFormat ��Ⱦ��ʽ
    // @para pWinId ���ھ�������Ϊ�մ����´���
    // @return �Ƿ��ʼ�������ɹ�
    bool Init(int iWidth, int iHeight, VideoFormat eFormat = VideoFormat::RGBA, void* pWinId = nullptr) override;

    /////////////////////////////////////////////////////////
    // ��Ⱦͼ�� (�̰߳�ȫ)
    // @para pData ��Ⱦ�Ķ���������
    // @para iLineSize һ�����ݵ��ֽ�����eg.YUV420PΪYһ���ֽ���
    // iLineSize <= 0 ���ݿ�Ⱥ����ظ�ʽ�Զ��Ƶ���С
    // @return �Ƿ���Ⱦ�ɹ�
    bool Draw(const unsigned char* pData, int iLineSize = 0) override;

    // �����������Դ
    void Close() override;

    // �������˳��¼�
    bool IsExit() override;

private:
    SDL_Window* m_pSrceen;
    SDL_Renderer* m_pRenderer;
    SDL_Texture* m_pTexture;
};

