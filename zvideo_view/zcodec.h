#pragma once
#include <mutex>
#include <vector>
struct AVPacket;
struct AVFrame;
struct AVCodecContext;

// ��������Ļ���
class ZCodec {
public:
    /////////////////////////////////////////////////
    // �����������������
    // @para iCodecId �������id��ffmpeg��Ӧ
    // @para iCodecType ��ʶ������������� 0-������ 1-������
    // @return ������������� ʧ�ܷ���nullptr
    static AVCodecContext* CreateCodecContext(int iCodecId, int iCodecType);

    /////////////////////////////////////////////////
    // ���ñ����������� ��Դ�ɶ���ά�� (�̰߳�ȫ)
    // m_pCodecCtx��Ϊnull ����������Դ
    // @para pCodecCtx ������������
    void SetCodecContext(AVCodecContext* pCodecCtx);

    /////////////////////////////////////////////////
    // ���ñ��������� (�̰߳�ȫ)
    bool SetAVOpt(const char* pKey, const char* pValue);
    bool SetAVOpt(const char* pKey, const int iValue);

    /////////////////////////////////////////////////
    // �򿪱����� (�̰߳�ȫ)
    bool OpenCodec();

    /////////////////////////////////////////////////
    // ���ݱ��������Ĵ���AVFrame ��Դ���û�ά��
    AVFrame* CreateFrame();

protected:
    AVCodecContext* m_pCodecCtx = nullptr;
    std::mutex m_mutex;
};

