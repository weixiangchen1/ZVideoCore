#pragma once
#include "zthread.h"
#include "zmux.h"

class ZMuxTask : public ZThread {
public:
    /////////////////////////////////////////////////////////
    // �򿪷�װ�� 
    // @para strURL �����ַ
    // @para pVideoParam ��Ƶ����
    // @para pOriginVideoTimeBase ԭ��Ƶʱ�����
    // @para pAudioParam ��Ƶ����
    // @para pOriginVideoTimeBase ԭ��Ƶʱ�����
    // @return �򿪷�װ���ɹ�����true ʧ�ܷ���false
    bool OpenMux(std::string strURL,
        AVCodecParameters* pVideoParam = nullptr, AVRational* pSrcVideoTimeBase = nullptr,
        AVCodecParameters* pAudioParam = nullptr, AVRational* pSrcAudioTimeBase = nullptr);

    // �߳���ں���
    void threadFunc() override;

    // ������������
    void DoWork(AVPacket* pPacket) override;

private:
    ZMux m_mux;
    std::mutex m_mutex;
    ZAVPacketList m_listPackets;
};

