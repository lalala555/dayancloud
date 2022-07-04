#pragma once
#ifndef DOWNLOAD_LIST_H
#define DOWNLOAD_LIST_H

#include "DownloadPacket.h"
#include "Base/TaskQueue.h"

/*====================================================================
**描述：上传文件列表，提供列表的统一控制（完成、失败等）接口
  带下载优先级
=======================================================================*/
const auto cmp = [](CDownloadPacket* a, CDownloadPacket* b)->bool {
    if(a->getPriority() != b->getPriority())
        return a->getPriority() > b->getPriority();
    else if(a->getPriorityTime() != b->getPriorityTime())
        return a->getPriorityTime() > b->getPriorityTime();

    return false;
};

class DownloadList
{
public:
    DownloadList();
    ~DownloadList();
public:
    CDownloadPacket* findPacket(t_taskid tid)
    {
        CDownloadPacket* packet = findPacket(m_lstPackets, tid);
        if(NULL == packet) {
            packet = findPacket(m_lstPackets2, tid);
        }

        return packet;
    }
    void clearAll()
    {
        CGuard guard(&m_lock);
        clearAll(m_lstPackets);
        clearAll(m_lstPackets2);
    }
    void changeEngine()
    {
        CGuard guard(&m_lock);
        changeEngine(m_lstPackets);
        changeEngine(m_lstPackets2);
    }
    bool addPacket(t_taskid tid, CDownloadPacket* t)
    {
        CGuard guard(&m_lock);
        for(auto it = m_lstPackets.begin(); it != m_lstPackets.end(); ++it) {
            if(tid == (*it)->getTaskId())
                return false;
        }

        m_lstPackets.push_back(t);

        m_lstPackets.sort(cmp);

        // m_taskQueue.push(t);

        return true;
    }
    CDownloadPacket* getPriorityPacket()
    {
        CGuard guard(&m_lock);
        if(m_lstPackets.empty())
            return nullptr;

        CDownloadPacket* packet = m_lstPackets.front();
        m_lstPackets.pop_front();
        m_lstPackets2.push_back(packet);
        if(m_lstPackets.empty()) {
            m_lstPackets.swap(m_lstPackets2);
            m_lstPackets.sort(cmp);
        }

        return packet;
    }
    void  stopAndRemovePacket(t_taskid tid)
    {
        CGuard guard(&m_lock);
        stopAndRemovePacket(m_lstPackets, tid);
        stopAndRemovePacket(m_lstPackets2, tid);
    }
    void changePriority(t_taskid tid, int priority, int priorityTime)
    {
        CGuard guard(&m_lock);
        changePriority(m_lstPackets, tid, priority, priorityTime);
        changePriority(m_lstPackets2, tid, priority, priorityTime);
        m_lstPackets.sort(cmp);

        // updateQueue(m_lstPackets);
    }
private:
    CDownloadPacket* findPacket(std::list<CDownloadPacket*> & lstPackets, t_taskid tid)
    {
        CGuard guard(&m_lock);
        for(auto it = lstPackets.begin(); it != lstPackets.end(); ++it) {
            if(tid == (*it)->getTaskId())
                return *it;
        }

        return nullptr;
    }
    void clearAll(std::list<CDownloadPacket*> & lstPackets)
    {
        for(auto it = lstPackets.begin(); it != lstPackets.end();) {
            auto packet = *it;
            it = lstPackets.erase(it);
            if(packet == nullptr)
                continue;
            packet->stop();
            packet->m_engine->stop_download_task(packet->getTaskId());
        }
    }
    void changeEngine(std::list<CDownloadPacket*> & lstPackets)
    {
        for(auto it = lstPackets.begin(); it != lstPackets.end(); ++it) {
            auto packet = *it;
            if(packet == nullptr)
                continue;
            packet->stop();
        }
    }
    void  stopAndRemovePacket(std::list<CDownloadPacket*> & lstPackets, t_taskid tid)
    {
        auto it = lstPackets.begin();
        for(it; it != lstPackets.end(); ++it) {
            if(tid == (*it)->getTaskId()) {
                auto packet = *it;
                lstPackets.erase(it);
                if(packet == nullptr)
                    return ;
                packet->stop();
                if(!packet->getVip())
                    packet->m_engine->stop_download_task(tid);
                packet->setLocalPath("");
                break;
            }
        }
    }
    void changePriority(std::list<CDownloadPacket*> & lstPackets, t_taskid tid, int priority, int priorityTime)
    {
        for(auto it = lstPackets.begin(); it != lstPackets.end(); ++it) {
            if(tid == (*it)->getTaskId()) {
                (*it)->setPriority(priority);
                (*it)->setPriorityTime(priorityTime);
                return;
            }
        }
    }
    void updateQueue(const std::list<CDownloadPacket*> &lstPackets) {
        // 如果队列非空，则先清理
        if (!m_taskQueue.empty()) {
            m_taskQueue.clear();
        }

        // 重新将数据加入优先队列
        for (auto it = lstPackets.begin(); it != lstPackets.end(); ++it) {
            m_taskQueue.push(*it);
        }
    }

    struct WorkerCmp {
        bool operator()(CDownloadPacket* a, CDownloadPacket* b) {
            if (a->getPriority() < b->getPriority()) {
                return true;
            }
            if (a->getPriority() == b->getPriority() && a->getPriorityTime() > b->getPriorityTime()) {
                return true;
            }
            return false;
        }
    };

private:
    CLock m_lock;
    std::list<CDownloadPacket*>  m_lstPackets;
    std::list<CDownloadPacket*>  m_lstPackets2;

    PQueue<CDownloadPacket*, std::vector<CDownloadPacket*>, WorkerCmp> m_taskQueue;
};


#endif // _DOWNLOAD_LIST_H