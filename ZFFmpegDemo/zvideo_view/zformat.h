#pragma once
#include <iostream>
#include <mutex>

class ZAVParam;
struct AVPacket;
struct AVRational;
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
    // ������Ƶ���� (�̰߳�ȫ)
    // @return ������Ƶ����������ָ��
    std::shared_ptr<ZAVParam> CopyVideoParam();
    std::shared_ptr<ZAVParam> CopyAudioParam();

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
    bool RescaleTimeParam(AVPacket* pPacket, long long lOffsetPts, AVRational* pTimeBase);

    /////////////////////////////////////////////////
    // ��ȡ��Ƶ������ID
    // @return ��Ƶ������ID
    int GetVideoCodecId();

    /////////////////////////////////////////////////
    // ���ó�ʱʱ��
    // @para iTime ��ʱʱ��
    void SetTimeoutMs(int iTime);

    /////////////////////////////////////////////////
    // �ж��Ƿ�ʱ
    // @return ��ʱ����true δ��ʱ����false
    bool IsTimeout();

    /////////////////////////////////////////////////
    // �ж��Ƿ����ӳɹ�
    // @return ���ӳɹ�����true ʧ�ܷ���false
    bool IsConnected();

protected:
    int m_iTimeoutMs = 0;                   // ��ʱʱ��
    long long m_lLastTime = 0;              // ��һ�ν��յ����ݵ�ʱ��
    bool m_bIsConnected = false;            // �Ƿ����ӳɹ�
    AVFormatContext* m_pFormatCtx;          // ��װ�ͽ��װ������
    std::mutex m_mutex;
    int m_iVideoIndex = -1;                  // ��Ƶ������
    int m_iAudioIndex = -1;                  // ��Ƶ������
    ZRational m_videoTimeBase = { 1,25 };
    ZRational m_audioTimeBase = { 1, 44100 };
    int m_iVideoCodecId = 0;                 // ������ID
};

