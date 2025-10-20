#include "zthread.h"
#include <sstream>
#include "zlog.h"

void ZThread::Start() {
    m_bIsExit = false;
    m_thread = std::thread(&ZThread::threadFunc, this);
    std::stringstream ss;
    ss << "thread start thread id=" << m_thread.get_id();
    ZLOGINFO(ss.str());
}

void ZThread::Stop() {
    std::stringstream ss;
    ss << "thread stop thread id=" << m_thread.get_id();
    ZLOGINFO(ss.str());
    m_bIsExit = true;
    if (m_thread.joinable()) {
        m_thread.join();
    }
    ZLOGINFO("thread stop already thread");
}
