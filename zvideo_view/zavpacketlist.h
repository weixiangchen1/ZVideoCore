#pragma once
#include <list>
#include <mutex>
#include "utils.h"

// 线程安全的AVPacket队列
struct AVPacket;
class ZAVPacketList {
public:
    AVPacket* Pop() {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_listPackets.empty()) {
            return nullptr;
        }
        AVPacket* pPacket = m_listPackets.front();
        m_listPackets.pop_front();
        return pPacket;
    }
    
    void Push(AVPacket* pPacket) {
        std::unique_lock<std::mutex> lock(m_mutex);
        AVPacket* pPacketItem = av_packet_alloc();
        // 引用计数 减少数据复制 线程安全
        av_packet_ref(pPacketItem, pPacket);
        m_listPackets.push_back(pPacketItem);
        // 超出最大空间，清理数据到关键帧位置
        if (m_listPackets.size() > m_iMaxPacketNum) {
            if (m_listPackets.front()->flags & AV_PKT_FLAG_KEY) {
                av_packet_free(&m_listPackets.front());
                m_listPackets.pop_front();
                return;
            }
            // 清理所有非关键帧之前的数据
            while (!m_listPackets.empty()) {
                if (m_listPackets.front()->flags & AV_PKT_FLAG_KEY) {
                    // 已经到了非第一帧的关键帧 完成清理工作
                    return;
                }
                av_packet_free(&m_listPackets.front());
                m_listPackets.pop_front();
            }
        }
    }

private:
    std::mutex m_mutex;
    std::list<AVPacket*> m_listPackets;
    int m_iMaxPacketNum = 100;
};

