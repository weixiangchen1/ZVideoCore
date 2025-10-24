#pragma once
#include <mutex>
#include <vector>
#include "zcodec.h"
#include "utils.h"

class ZEncode: public ZCodec {
public:
    /////////////////////////////////////////////////
    // �������� (�̰߳�ȫ)
    // @para pFrame ��Դ���û�ά��
    // @return ʧ�ܷ���null ���ص�AVPacket���û�ͨ��av_packet_free������Դ
    AVPacket* EncodeData(const AVFrame* pFrame);

    /////////////////////////////////////////////////
    // ��ȡ�����е�AVPacket
    std::vector<AVPacket*> GetCachePackets();
};

