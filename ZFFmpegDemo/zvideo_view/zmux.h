#pragma once
#include "zformat.h"
// ��װ��
struct AVRational;
struct AVCodecParameters;
class ZMux : public ZFormat {
public:
    ZMux();
    ~ZMux();
    /////////////////////////////////////////////////
    // ������װ��������
    // @para strURL ��װ��ַ
    // @para pVideoParam ��Ƶ����
    // @para pAudioParam ��Ƶ����
    // @return ��װ�������� ʧ�ܷ���null
    static AVFormatContext* CreateMuxContext(const char* strURL,
        AVCodecParameters* pVideoParam = nullptr,
        AVCodecParameters* pAudioParam = nullptr);
    /////////////////////////////////////////////////
    // д���ļ�ͷ������
    // @return �ɹ�����true ʧ�ܷ���false
    bool WriteHead();

    /////////////////////////////////////////////////
    // д��һ֡����
    // @para pPacket ��������
    // @return �ɹ�����true ʧ�ܷ���false
    bool WriteFrame(AVPacket* pPacket);

    /////////////////////////////////////////////////
    // д���ļ�β������
    // @return �ɹ�����true ʧ�ܷ���false
    bool WriteTail();

    /////////////////////////////////////////////////
    // ����ʱ�����
    // @para pTimeBase ʱ�����
    void SetSrcVideoTimeBase(AVRational* pTimeBase);
    void SetSrcAudioTimeBase(AVRational* pTimeBase);

private:
    AVRational* m_pSrcVideoTimeBase = nullptr;
    AVRational* m_pSrcAudioTimeBase = nullptr;

    long long m_lBeginVideoPts = -1;            // ԭ��Ƶ��ʼʱ��
    long long m_lBeginAudioPts = -1;            // ԭ��Ƶ��ʼʱ��
};

