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

    /////////////////////////////////////////////////
    // ����ָ����ʱ��������������ж�λ
    // @para lPts Ŀ�궨λ��ʱ���
    // @para iStreamIndex ָ��Ҫ��λ��������
    // @return �ɹ�����true ʧ�ܷ���false
    bool SeekFrame(long long lPts, int iStreamIndex);
};

