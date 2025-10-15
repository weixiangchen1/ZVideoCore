#pragma once
#include <iostream>
#include <mutex>
struct AVPacket;
struct AVFormatContext;
struct AVCodecParameters;
struct ZRational {
    int num;    ///< Numerator
    int den;    ///< Denominator
};

// ��װ�ͽ��װ�Ļ���
class ZFormat {
public:
    /////////////////////////////////////////////////
    // ���������� (�̰߳�ȫ)
    // @para iStreamIndex ��Ҫ���Ƶ���Ƶ����Ƶ������
    // @para dstParameter �������
    // @return �ɹ�����true ʧ�ܷ���false
    bool CopyParam(int iStreamIndex, AVCodecParameters* dstParameter);

    /////////////////////////////////////////////////
    // ���÷�װ����װ������ (�̰߳�ȫ)
    // ���û��Զ������ϴ����õ������� ����null��ر�������
    // @para pFormatCtx ��װ�ͽ��װ������
    void SetFormatContext(AVFormatContext* pFormatCtx);

    /////////////////////////////////////////////////
    // ��ȡ��Ƶ����Ƶ������
    // @return ��Ƶ����Ƶ������
    int GetVideoIndex();
    int GetAudioIndex();

    /////////////////////////////////////////////////
    // ��ȡ��Ƶ����Ƶ��ʱ���
    // @return ��Ƶ����Ƶ��ʱ���
    ZRational GetVideoTimeBase();
    ZRational GetAudioTimeBase();

protected:
    AVFormatContext* m_pFormatCtx;          // ��װ�ͽ��װ������
    std::mutex m_mutex;
    int m_iVideoIndex = -1;                  // ��Ƶ������
    int m_iAudioIndex = -1;                  // ��Ƶ������
    ZRational m_videoTimeBase = { 1,25 };
    ZRational m_audioTimeBase = { 1, 44100 };
};

