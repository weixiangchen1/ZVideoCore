#pragma once
#include <thread>

class ZThread {
public:
    // �����߳�
    virtual void Start();
    
    // ֹͣ�߳� (�����˳���־λ���ȴ��߳��˳�)
    virtual void Stop();

protected:
    // �߳���ں���
    virtual void threadFunc() = 0;

    bool m_bIsExit;         // �߳��Ƿ��˳�
    std::thread m_thread;
};

