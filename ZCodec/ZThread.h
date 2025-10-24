#pragma once
#include <thread>
#include <mutex>
#include "ZAVPacketlist.h"

class ZThread {
public:
    // 启动线程
    virtual void Start();
    
    // 停止线程 (设置退出标志位，等待线程退出)
    virtual void Stop();

    // 执行责任工作
    virtual void DoWork(AVPacket* pPacket);

    // 传递到责任链的下一个结点
    virtual void Next(AVPacket* pPacket);

    // 设置责任链的下一个结点
    void SetNextNode(ZThread* pNext);

protected:
    // 线程入口函数
    virtual void threadFunc() = 0;

    bool m_bIsExit;         // 线程是否退出

private:
    std::mutex m_mutex;
    std::thread m_thread;
    ZThread* m_pNext;       // 责任链的下一个结点
};

