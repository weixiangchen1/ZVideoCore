#pragma once
#include "zthread.h"
#include "zdemux.h"

class ZDemuxTask : public ZThread {
public:
    /////////////////////////////////////////////////
    // �����򿪽��װ��
    // @para strURL ���װ��ַ ֧��rtsp
    // @para iTimeoutMs ���ӳ�ʱʱ�� ��λ����
    // @return �򿪳ɹ�����true ʧ�ܷ���false
    bool OpenDemux(std::string strURL, int iTimeoutMs = 1000);
    
    // �߳���ں���
    void threadFunc();

    // ������Ƶ����
    std::shared_ptr<ZAVParam> CopyVideoParam();

private:
    int m_iTimeoutMs = 0;
    std::string m_strURL;
    ZDemux m_demux;
};

