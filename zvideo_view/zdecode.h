#pragma once
#include "zcodec.h"
#include "utils.h"

class ZDecode: public ZCodec {
public:
    /////////////////////////////////////////////////
    // ������̷߳������� (�̰߳�ȫ)
    // @para pPacket ��Ҫ�����AVPacket
    // @return ʧ�ܷ���false �ɹ�����true
    bool SendPacket(const AVPacket* pPacket);

    /////////////////////////////////////////////////
    // ���ս�����AVFrame (�̰߳�ȫ)
    // @para pFrame ���������
    // @return ʧ�ܷ���false �ɹ�����true
    bool RecvFrame(AVFrame* pFrame);

    /////////////////////////////////////////////////
    // ��ȡ�����е�AVFrame
    std::vector<AVFrame*> GetCacheFrames();

    /////////////////////////////////////////////////
    // ��ʼ��Ӳ������
    // 4 == AV_HWDEVICE_TYPE_DXVA2
    bool InitHWA(int iType = 4);
};

