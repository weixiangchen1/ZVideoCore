#pragma once
#include "zformat.h"
// ��װ��
class ZMux : public ZFormat {
public:
    /////////////////////////////////////////////////
    // ������װ��������
    // @para strURL ��װ��ַ
    // @return ��װ�������� ʧ�ܷ���null
    static AVFormatContext* CreateMuxContext(const char* strURL);

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
};

