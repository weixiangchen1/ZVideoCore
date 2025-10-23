#pragma once
#include <thread>
#include <mutex>
#include "zavpacketlist.h"

struct AVPacket;
class ZThread {
public:
    // �����߳�
    virtual void Start();
    
    // ֹͣ�߳� (�����˳���־λ���ȴ��߳��˳�)
    virtual void Stop();

    // ִ�����ι���
    virtual void DoWork(AVPacket* pPacket);

    // ���ݵ�����������һ�����
    virtual void Next(AVPacket* pPacket);

    // ��������������һ�����
    void SetNextNode(ZThread* pNext);

protected:
    // �߳���ں���
    virtual void threadFunc() = 0;

    bool m_bIsExit;         // �߳��Ƿ��˳�

private:
    std::mutex m_mutex;
    std::thread m_thread;
    ZThread* m_pNext;       // ����������һ�����
};

