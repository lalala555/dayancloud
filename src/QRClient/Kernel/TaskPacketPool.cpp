#include "stdafx.h"
#include "TaskPacketPool.h"
#define RESIZE 20

//////////////////////////////////////////////////////////////////////////
UploadPacketPool::UploadPacketPool(void)
{
}

UploadPacketPool::~UploadPacketPool(void)
{
}

void UploadPacketPool::init(int size)
{
    allocMemory(size);
}

void UploadPacketPool::allocMemory(int size)
{
    CGuard guard(m_lock);
    for(int i = 0; i < size; ++i) {
        CUploadPacket* packet = new CUploadPacket();
        m_freePacketPool.push_back(packet);
    }
}

void UploadPacketPool::free()
{
    for(auto iter = m_freePacketPool.begin();
            iter != m_freePacketPool.end(); ++iter) {
        CUploadPacket* packet = *iter;
        delete packet, packet = 0;
    }

    m_freePacketPool.clear();
}

CUploadPacket* UploadPacketPool::popPacket()
{
    CGuard guard(m_lock);
    if(m_freePacketPool.empty()) {
        allocMemory(RESIZE);
    }
    CUploadPacket* packet = m_freePacketPool.front();
    m_freePacketPool.pop_front();
    packet->clearPacket();
    return packet;
}

void UploadPacketPool::freePacket(CUploadPacket* packet)
{
    CGuard guard(m_lock);
    m_freePacketPool.push_back(packet);
}

//////////////////////////////////////////////////////////////////////////


DownloadPacketPool::DownloadPacketPool(void)
{
}


DownloadPacketPool::~DownloadPacketPool(void)
{
}

void DownloadPacketPool::init(int size)
{
    allocMemory(size);
}

void DownloadPacketPool::allocMemory(int size)
{
    CGuard guard(m_lock);
    for(int i = 0; i < size; ++i) {
        CDownloadPacket* packet = new CDownloadPacket();
        m_freePacketPool.push_back(packet);
    }
}

void DownloadPacketPool::free()
{
    for(auto iter = m_freePacketPool.begin();
            iter != m_freePacketPool.end(); ++iter) {
        CDownloadPacket* packet = *iter;
        delete packet, packet = 0;
    }

    m_freePacketPool.clear();
}

CDownloadPacket* DownloadPacketPool::popPacket()
{
    CGuard guard(m_lock);
    if(m_freePacketPool.empty()) {
        allocMemory(RESIZE);
    }
    CDownloadPacket* packet = m_freePacketPool.front();
    m_freePacketPool.pop_front();
    packet->clearPacket();
    return packet;
}

void DownloadPacketPool::freePacket(CDownloadPacket* packet)
{
    CGuard guard(m_lock);
    m_freePacketPool.push_back(packet);
}

//////////////////////////////////////////////////////////////////////////


/*ScriptPacketPool::ScriptPacketPool(void)
{
}


ScriptPacketPool::~ScriptPacketPool(void)
{
}

void ScriptPacketPool::init(int size)
{
    allocMemory(size);
}

void ScriptPacketPool::allocMemory(int size)
{
    CGuard guard(m_lock);
    for (int i = 0; i < size; ++i) {
        ScriptPacket* packet = new ScriptPacket();
        m_freePacketPool.push_back(packet);
    }
}

void ScriptPacketPool::free()
{
    for (auto iter = m_freePacketPool.begin();
        iter != m_freePacketPool.end(); ++iter) {
        ScriptPacket* packet = *iter;
        delete packet, packet = 0;
    }

    m_freePacketPool.clear();
}

ScriptPacket* ScriptPacketPool::popPacket()
{
    CGuard guard(m_lock);
    if (m_freePacketPool.empty()) {
        allocMemory(RESIZE);
    }
    ScriptPacket* packet = m_freePacketPool.front();
    m_freePacketPool.pop_front();
    packet->clearPacket();
    return packet;
}

void ScriptPacketPool::freePacket(ScriptPacket* packet)
{
    CGuard guard(m_lock);
    m_freePacketPool.push_back(packet);
}*/