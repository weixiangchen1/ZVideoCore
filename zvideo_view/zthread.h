#pragma once
#include <thread>

class ZThread {
public:
    // 启动线程
    virtual void Start();
    
    // 停止线程 (设置退出标志位，等待线程退出)
    virtual void Stop();

protected:
    // 线程入口函数
    virtual void threadFunc() = 0;

    bool m_bIsExit;         // 线程是否退出
    std::thread m_thread;
};

