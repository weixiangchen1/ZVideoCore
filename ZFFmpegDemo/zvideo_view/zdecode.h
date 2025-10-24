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
    // @return 1: �ɹ������ pFrame
    // 0: ��Ҫ�������ݣ�AVERROR(EAGAIN)��
    // -1: ����� AVERROR_EOF
    int RecvFrame(AVFrame* pFrame);

    /////////////////////////////////////////////////
    // ��ȡ�����е�AVFrame
    std::vector<AVFrame*> GetCacheFrames();

    /////////////////////////////////////////////////
    // ��ʼ��Ӳ������
    // 4 == AV_HWDEVICE_TYPE_DXVA2
    bool InitHWA(int iType = 4);
};

