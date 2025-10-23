#pragma once
#include "zthread.h"
#include "zdecode.h"

struct AVPacket;
struct AVCodecContext;
struct AVCodecParameters;
class ZDecodeTask : public ZThread {
public:
    // �򿪽�����
    bool OpenDecodec(AVCodecParameters* pCodecParam);

    // ������������
    void DoWork(AVPacket* pPacket) override;
    
    // �߳���ں���
    void threadFunc();
    
    // ������Ҫ��Ⱦ��AVFrame û�����ݷ���null
    // ���ص�AVFrame��Դ��Ҫ�û��ͷ�
    AVFrame* GetFrame();

private:
    std::mutex m_mutex;
    ZDecode m_decodec;
    ZAVPacketList m_listPackets;
    AVFrame* m_pFrame = nullptr;
    bool m_bCanRender = false;
};

