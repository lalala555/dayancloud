#include "stdafx.h"
#include "UploadQueue.h"
#include "UploadPacket.h"
#include "TaskCenterManager.h"
#include "MessagePool.h"
#include "TransEngine/TransfomEngineFactory.h"
#include "UserProfile/LocalSetting.h"

UploadQueue::UploadQueue()
    : m_currEngine(nullptr)
{
}

UploadQueue::~UploadQueue()
{
    stop();
}

bool UploadQueue::init()
{
    m_uploadPacketPool.init(200);

    m_queue.init(this);

    return true;
}

bool UploadQueue::switchEngine(AbstractTransEngine *transEngine)
{
    if(m_currEngine)
        m_currEngine->pause_upload();

    m_currEngine = transEngine;
    if(transEngine->initialized()) {
        transEngine->resume_upload();
        return true;
    }

    return transEngine->init();
}

void UploadQueue::switchServer(const QString& server)
{
    if(m_currEngine) {
        m_currEngine->switch_server(server.toStdString());
    }
}

bool UploadQueue::start(AbstractTransEngine *transEngine)
{
    /*
    if(!switchEngine(transEngine))
        return false;
    */
    m_currEngine = transEngine;

    // m_queue.start();

    m_msgQueue.startQueue();

    m_pTaskQueue.startQueue();
    m_pTaskQueue.setMaxRunTask(LocalSetting::getInstance()->getMaxUploadJobCount());
    m_pTaskQueue.setReserve(LocalSetting::getInstance()->getIsUploadTaskReserve()); // 任务逆序执行

    return true;
}

void UploadQueue::stop()
{
    m_currEngine = nullptr;
    m_pTaskQueue.stopQueue();
    m_msgQueue.stopQueue();

    m_queue.stop();
    m_packList.clearAll();
    m_uploadPacketPool.free();
}

bool UploadQueue::startTask(const UploadInfo&info, UploadFileListByDb* fileList)
{
    // 检查重试的次数是否到达上限
    if(info.retryTimes > LocalSetting::getInstance()->getMaxUploadRetryTimes()) {
        LOGFMTE("[UploadQueue] Start task failed! packet exhausted all retry times!taskid=%d", info.taskId);
        return false;
    }

    if(m_packList.findPacket(info.taskId)) {
        LOGFMTE("[UploadQueue] Start task failed! packet is queued!taskid=%d", info.taskId);
        return false;
    }

    /*if(m_packList.isPacketFull(LocalSetting::getInstance()->getMaxUploadJobCount())) {
        LOGFMTE("[UploadQueue] Start task failed! packetlist is full!taskid=%d", info.taskId);
        return false;
    }*/

    CUploadPacket* packet = m_uploadPacketPool.popPacket();
    packet->init(&info, fileList);
    packet->setTransEngine(m_currEngine);

    LOGFMTI("[UploadTask] create task id[%d]", packet->getTaskId());

    m_packList.addPacket(packet->getTaskId(), packet);

    m_pTaskQueue.startTask(packet->getTaskId(), packet);

    // return m_queue.post(packet);
    return true;
}

//被动stop不能释放packet，内部可能还没走完,packet任务结束后自动释放
bool UploadQueue::stopTask(t_taskid taskid)
{
    m_packList.stopPacket(taskid);

    m_pTaskQueue.stopTask(taskid);

    LOGFMTI("[UploadQueue] Stop task! taskid=%d", taskid);

    return m_currEngine->stop_upload_task(taskid);;
}

bool UploadQueue::freePacket(t_taskid taskId)
{
    CUploadPacket *packet = m_packList.removePacket(taskId);
    if(packet == nullptr)
        return false;
    m_uploadPacketPool.freePacket(packet);
    m_pTaskQueue.freeTask(taskId);

    return true;
}

bool UploadQueue::isQueueTask(t_taskid taskid)
{
    return m_packList.findPacket(taskid);
}

void UploadQueue::on_queue_data(CUploadPacket* pData)
{
    if(m_currEngine == nullptr || pData == nullptr) {
        return;
    }
    CUploadPacket* packet = pData;
    if(packet == nullptr) return;

#if 0
    if (packet->isIdle()) {
        int taskId = packet->getTaskId();
        if (!packet->start()) {
            return;
        }
        LOGFMTI("[UploadQueue] Upload task, taskid=%d", taskId);

        bool ret = packet->doWork(m_currEngine);
    }
#else
    m_pTaskQueue.startTask(packet->getTaskId(), packet);
#endif
}

void UploadQueue::updateTaskPriority(t_taskid taskid, int priority, uint updateTime)
{
    m_pTaskQueue.updateTaskPriority(taskid, priority, updateTime);
}

void UploadQueue::addUploadRunningMsg(msg_callback_info_t* msg)
{
    UploadRunningMsg* batchmsg = new UploadRunningMsg();
    batchmsg->initMsg(msg);
    m_msgQueue.post_msg(batchmsg);
}

void UploadQueue::updateTaskTable(const QVector<qint64>& table)
{
    m_pTaskQueue.updateTaskTable(table);
}

void UploadQueue::startAddTask()
{
    m_pTaskQueue.startAddTask();
}

void UploadQueue::addTaskEnd()
{
    m_pTaskQueue.addTaskEnd();
}
//////////////////////////////////////////////////////////////////////////
// 上传时的消息
UploadRunningMsg::UploadRunningMsg()
{
}

UploadRunningMsg::~UploadRunningMsg()
{
}

void UploadRunningMsg::initMsg(msg_callback_info_t* msg)
{
    _msgData = msg;
}

void UploadRunningMsg::sendMsg()
{
    CTaskCenterManager::Inst()->handleUploadTransMsg(_msgData);
}

void UploadRunningMsg::release()
{
    delete this;
}
