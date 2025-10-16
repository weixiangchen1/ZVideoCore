#pragma once
#include <iostream>
#include <mutex>
struct AVPacket;
struct AVFormatContext;
struct AVCodecContext;
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
    bool CopyParam(int iStreamIndex, AVCodecContext* dstCodecCtx);

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

    /////////////////////////////////////////////////
    // ����time_base����ʱ�����
    // @para pPacket ��Ҫ����ת������Ƶ֡
    // @para lOffsetPts ʱ���ƫ����
    // @para timeBase ��ʾ pPacket ԭʼʱ���
    // @return �ɹ�����true ʧ�ܷ���false
    bool RescaleTimeParam(AVPacket* pPacket, long long lOffsetPts, ZRational timeBase);

    /////////////////////////////////////////////////
    // ��ȡ��Ƶ������ID
    // @return ��Ƶ������ID
    int GetVideoCodecId();

protected:
    AVFormatContext* m_pFormatCtx;          // ��װ�ͽ��װ������
    std::mutex m_mutex;
    int m_iVideoIndex = -1;                  // ��Ƶ������
    int m_iAudioIndex = -1;                  // ��Ƶ������
    ZRational m_videoTimeBase = { 1,25 };
    ZRational m_audioTimeBase = { 1, 44100 };
    int m_iVideoCodecId = 0;                 // ������ID
};

