#include "ZThread.h"
#include <sstream>
#include "ZLog.h"

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

void ZThread::DoWork(AVPacket* pPacket) {}

void ZThread::Next(AVPacket* pPacket) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_pNext != nullptr) {
        m_pNext->DoWork(pPacket);
    }
}

void ZThread::SetNextNode(ZThread* pNext) {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_pNext = pNext;
}
