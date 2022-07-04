#ifndef DOWNLOAD_QUEUE_H
#define DOWNLOAD_QUEUE_H

#include "PacketController.h"
#include "DownloadPacket.h"
#include "TaskPacketPool.h"
#include "TransEngine/TransfomEngineFactory.h"
#include "DownloadList.h"
//////////////////////////////////////////////////////////////////////////
class DownloadRunningMsg : public TaskRunningMsg<msg_callback_info_t*>
{
public:
    DownloadRunningMsg();
    ~DownloadRunningMsg();
    virtual void initMsg(msg_callback_info_t* msg);
    virtual void sendMsg();
    virtual void release();
};
//////////////////////////////////////////////////////////////////////////
class DownloadQueue: public IQueueSink<CDownloadPacket>
{

public:
    DownloadQueue();
    ~DownloadQueue();

    virtual void on_queue_data(CDownloadPacket* pData);

    bool init();
    bool start(AbstractTransEngine *transEngine);
    void stop();

    bool startTask(const DownloadInfo &info);
    bool stopTask(t_taskid taskid);
    bool freePacket(CDownloadPacket *packet);
    bool changePacketPriority(t_taskid taskid, int priority, int priorityTime);
    bool switchEngine(AbstractTransEngine *transEngine);
    void switchServer(const QString& server);

    void addUploadRunningMsg(msg_callback_info_t* msg);
    void updateTaskTable(const QVector<qint64>& table);
    void startAddTask();
    void addTaskEnd();
private:
    IQueue<CDownloadPacket> m_queue;
    AbstractTransEngine* m_currEngine;
    DownloadList m_packList;
    DownloadPacketPool m_downloadPacketPool;
    int m_maxDownThreadCount;
    PriorityTaskQueue<CDownloadPacket*> m_pTaskQueue;
    MsgQueue<DownloadRunningMsg*> m_msgQueue;
};

// ¿ËÂ¡¶ÓÁÐ
class CloneQueue: public IQueueSink<CDownloadPacket>
{
public:
    CloneQueue();
    ~CloneQueue();

    void on_queue_data(CDownloadPacket* pData);

    bool init();
    bool start(AbstractTransEngine *transEngine);
    void stop();

    bool startTask(const DownloadInfo &info);
    bool freePacket(CDownloadPacket *packet);
    bool switchEngine(AbstractTransEngine *transEngine);
    void switchServer(const QString& server);
private:

    IQueue<CDownloadPacket> m_queue;
    AbstractTransEngine* m_currEngine;
    DownloadList m_packList;            /* DEPRECATED */
    DownloadPacketPool m_downloadPacketPool;
};

#endif