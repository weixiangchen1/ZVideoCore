#pragma once
#include "zformat.h"
class ZDemux : public ZFormat {
public:
    /////////////////////////////////////////////////
    // �������װ��������
    // @para strURL ���װ��ַ ֧��rtsp
    // @return ���װ�������� ʧ�ܷ���null
    static AVFormatContext* CreateDemuxContext(const char* strURL);

    /////////////////////////////////////////////////
    // ��ȡһ֡����
    // @para pPacket �������
    // @return �ɹ�����true ʧ�ܷ���false
    bool ReadFrame(AVPacket* pPacket);
};

