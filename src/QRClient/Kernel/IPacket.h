#ifndef IPACKET_H
#define IPACKET_H

#include <QString>
#include "kernel/macro.h"

enum EN_FILE_STATUS {
    FILE_UPLOADED      = 0,    // 传输完成
    FILE_NOT_START     = 1,    // 还没开始传输
    FILE_NOT_FOUND     = 2,    // 文件没有找到
    FILE_UPLOAD_FAILED = 3,    // 传输失败
};

enum EN_FILE_CHECK_STATUS {
    FILE_CHECK_SUCCESS  = 0,     //校验成功
    FILE_CHECK_FAILED   = 0x10,  //校验失败
    FILE_NOT_CHECK      = 0x80,  //未校验
};

//packet传输状态
enum EN_PACKE_STATUS {
    PACKET_RUNNING = 0,
    PACKET_FREE = 1,
    PACKET_STOP = 2
};

class IPacket
{
public:
    IPacket() : m_runStatus(PACKET_FREE) {}
    virtual ~IPacket() {}

public:
    virtual bool callback(void*, int, void*, int)
    {
        return false;
    }
    virtual t_taskid getTaskId() = 0;
    virtual t_uid getUserId() = 0;
    virtual bool isRunning() = 0;
    virtual int getStorageId() = 0;
    virtual int getRunStatus()
    {
        return m_runStatus;
    }
    virtual void startTask() = 0;
    virtual void stopTask() = 0;
    virtual void updatePriority(int priority) = 0;
    virtual void updateTimePriority(unsigned int timePriority) = 0;
    virtual void setPacketIndex(int index) = 0;
public:
    int _priority;
    unsigned int _priorityTime;
    int _packetIndex;

protected:
    int m_runStatus;
};

#endif
