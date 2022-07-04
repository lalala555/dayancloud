#ifndef REPETITION_CONTROLLER_H
#define REPETITION_CONTROLLER_H

template <class TPacket>
class PacketList
{
public:
    bool findPacket(t_taskid tid)
    {
        CGuard guard(&m_lock);
        if(m_packList.find(tid) == m_packList.end()) {
            return false;
        }
        return true;
    }
    void clearAll()
    {
        CGuard guard(&m_lock);

        auto it = m_packList.begin();
        while(it != m_packList.end()) {
            TPacket* packet = it->second;
            if(packet == nullptr)
                continue;
            packet->stop();
#if 0 // TODO
            delete packet;
#endif
            m_packList.erase(it++);
        }
    }
    void addPacket(t_taskid tid, TPacket* t)
    {
        CGuard guard(&m_lock);
        m_packList[tid] = t;
    }
    TPacket* getPacket(t_taskid tid)
    {
        CGuard guard(&m_lock);

        auto it = m_packList.find(tid);
        return it->second;
    }
    TPacket* removePacket(t_taskid tid)
    {
        CGuard guard(&m_lock);

        auto it = m_packList.find(tid);
        if(it == m_packList.end()) {
            return 0;
        }
        TPacket* packet = it->second;
        if(packet == nullptr) {
            return 0;
        }
        packet->stop();

        m_packList.erase(it);

        return packet;
    }
    void stopPacket(t_taskid tid)
    {
        CGuard guard(&m_lock);

        auto it = m_packList.find(tid);
        if(it == m_packList.end()) {
            return ;
        }
        TPacket* packet = it->second;
        if(packet == nullptr) {
            return ;
        }
        packet->stop();
        m_packList.erase(it);
    }
    std::list<TPacket*> allPacket()
    {
        std::list<TPacket*> packets;

        auto it = m_packList.begin();
        for (it; it != m_packList.end(); ++it) {
            packets.push_back(it->second);
        }

        return packets;
    }
    bool isPacketFull(int fullcount)
    {
        return m_packList.size() >= fullcount;
    }
private:
    CLock m_lock;
    std::map<t_taskid, TPacket*> m_packList;
};

#endif