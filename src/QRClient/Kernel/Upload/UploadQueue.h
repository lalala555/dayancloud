#ifndef UPLOAD_QUEUE_H
#define UPLOAD_QUEUE_H

#include "PacketController.h"
#include "TaskPacketPool.h"
#include "TransEngine/TransfomEngineFactory.h"
#include "Kernel/Base/TaskQueue.h"
#include "Kernel/Base/MsgQueue.h"

//////////////////////////////////////////////////////////////////////////
class UploadRunningMsg : public TaskRunningMsg<msg_callback_info_t*>
{
public:
    UploadRunningMsg();
    ~UploadRunningMsg();
    virtual void initMsg(msg_callback_info_t* msg);
    virtual void sendMsg();
    virtual void release();
};
//////////////////////////////////////////////////////////////////////////

class UploadQueue: public IQueueSink<CUploadPacket>
{
public:
    UploadQueue();
    ~UploadQueue();

    virtual void on_queue_data(CUploadPacket* pData);

    bool init();
    bool start(AbstractTransEngine *transEngine);
    void stop();

    bool startTask(const UploadInfo&info, UploadFileListByDb* fileList);
    bool stopTask(t_taskid taskid);
    bool freePacket(t_taskid taskid);
    bool switchEngine(AbstractTransEngine *transEngine);
    void switchServer(const QString& server);
    bool isQueueTask(t_taskid taskid);
    void updateTaskPriority(t_taskid taskid, int priority, uint updateTime);

    void addUploadRunningMsg(msg_callback_info_t* msg);
    void updateTaskTable(const QVector<qint64>& table);
    void startAddTask();
    void addTaskEnd();
private:
    IQueue<CUploadPacket> m_queue;
    AbstractTransEngine* m_currEngine;
    PacketList<CUploadPacket> m_packList;
    UploadPacketPool m_uploadPacketPool;
    PriorityTaskQueue<CUploadPacket*> m_pTaskQueue;
    MsgQueue<UploadRunningMsg*> m_msgQueue;
    QMutex m_mutex;
};

// 校验队列
class CheckQueue: public IQueueSink<CUploadPacket>
{
public:
    CheckQueue();
    ~CheckQueue();

    void on_queue_data(CUploadPacket* pData);

    bool init();
    bool start();
    void stop();

    void switchServer(const QString& server);

private:
    IQueue<CUploadPacket> m_queue;
};

#endif