#pragma once
#include <list>
#include <mutex>
#include "utils.h"

// �̰߳�ȫ��AVPacket����
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
        // ���ü��� �������ݸ��� �̰߳�ȫ
        av_packet_ref(pPacketItem, pPacket);
        m_listPackets.push_back(pPacketItem);
        // �������ռ䣬�������ݵ��ؼ�֡λ��
        if (m_listPackets.size() > m_iMaxPacketNum) {
            if (m_listPackets.front()->flags & AV_PKT_FLAG_KEY) {
                av_packet_free(&m_listPackets.front());
                m_listPackets.pop_front();
                return;
            }
            // �������зǹؼ�֮֡ǰ������
            while (!m_listPackets.empty()) {
                if (m_listPackets.front()->flags & AV_PKT_FLAG_KEY) {
                    // �Ѿ����˷ǵ�һ֡�Ĺؼ�֡ ���������
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

