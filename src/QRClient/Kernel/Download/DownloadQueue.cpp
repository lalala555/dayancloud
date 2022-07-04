#include "stdafx.h"
#include "TaskCenterManager.h"
#include "DownloadQueue.h"
#include "NewJobs/JobStateMgr.h"
#include "PluginConfig/ProjectMgr.h"
#include "Common/SystemUtil.h"
#include "UI/Views/DownloadPage/DownloadStateMgr.h"

DownloadQueue::DownloadQueue()
    : m_queue(nullptr)
    , m_currEngine(nullptr)
    , m_maxDownThreadCount(DOWNLOAD_THREAD_DEFAULT_COUNT)
{
}

DownloadQueue::~DownloadQueue()
{
    stop();
}

bool DownloadQueue::init()
{
    m_maxDownThreadCount = LocalSetting::getInstance()->getDownloadThreadCount();
    m_downloadPacketPool.init(200);

    m_queue.init(this);
    return true;
}

bool DownloadQueue::switchEngine(AbstractTransEngine *transEngine)
{
    if(m_currEngine)
        m_currEngine->pause_download();

    m_packList.changeEngine();

    m_currEngine = transEngine;
    if(transEngine->initialized()) {
        transEngine->resume_download();
        return true;
    }

    return transEngine->init();
}

void DownloadQueue::switchServer(const QString& server)
{
    if(m_currEngine) {
        m_currEngine->switch_server(server.toStdString());
    }
}

bool DownloadQueue::start(AbstractTransEngine *transEngine)
{
    /*
    if(!switchEngine(transEngine))
        return false;
    */
    m_currEngine = transEngine;

#if 0
    m_queue.start();
#else
    m_msgQueue.startQueue();

    m_pTaskQueue.setTaskInterval(50);
    m_pTaskQueue.startQueue();
    m_pTaskQueue.setReserve(LocalSetting::getInstance()->getIsDownloadTaskReserve()); // 任务逆序执行
    m_pTaskQueue.setMaxRunTask(m_maxDownThreadCount);
#endif
    
    return true;
}

void DownloadQueue::stop()
{
    m_currEngine = nullptr;
    m_queue.stop();
    m_pTaskQueue.stopQueue();
    m_msgQueue.stopQueue();
    m_packList.clearAll();
    m_downloadPacketPool.free();
}

bool DownloadQueue::startTask(const DownloadInfo &info)
{
    if(m_packList.findPacket(info.taskId) && m_packList.findPacket(info.taskId)->isRunning()) {
        return false;
    }

    CDownloadPacket* packet = m_downloadPacketPool.popPacket();
    if(packet == nullptr) {
        LOGFMTE("[DownloadQueue] Start download failed, create packet failed, memory error! taskid=%d", info.taskId);
        return false;
    }

    packet->init(&info, m_currEngine);
    m_packList.addPacket(packet->getTaskId(), packet);

    // if(!m_packList.addPacket(packet->getTaskId(), packet)){
    //     m_downloadPacketPool.freePacket(packet);
    // }

#if 0
    return m_queue.post(packet);
#else
    m_pTaskQueue.startTask(packet->getTaskId(), packet);
    return true;
#endif  
}

bool DownloadQueue::stopTask(t_taskid taskid)
{
    m_packList.stopAndRemovePacket(taskid);
    m_pTaskQueue.stopTask(taskid);
    LOGFMTI("[DownloadQueue] Stop task! taskid=%d", taskid);

    return true;
}

bool DownloadQueue::freePacket(CDownloadPacket *packet)
{
    if(packet == nullptr)
        return false;

    m_packList.stopAndRemovePacket(packet->getTaskId());
    m_downloadPacketPool.freePacket(packet);
    m_pTaskQueue.freeTask(packet->getTaskId());

    return true;
}

bool DownloadQueue::changePacketPriority(t_taskid taskid, int priority, int priorityTime)
{
    m_packList.changePriority(taskid, priority, priorityTime);
    m_pTaskQueue.updateTaskPriority(taskid, priority, priorityTime);
    return true;
}

// Deprecated
void DownloadQueue::on_queue_data(CDownloadPacket* pData)
{
    if(m_currEngine == nullptr /*|| m_currEngine->get_running_download_task_count() >= m_maxDownThreadCount*/) {
        return;
    }

    CDownloadPacket* packet = pData; // m_packList.getPriorityPacket();
    if(packet == nullptr) return;
    if (!packet->start())
        return;

    int taskId = packet->getTaskId();

    CDownloadState *downloadState = CDownloadStateMgr::Inst()->LookupTaskstate(taskId);
    if(downloadState == nullptr)
        return;

    QString projectSavePath = ProjectMgr::getInstance()->getProjectOutput(downloadState->getProjectName());
    projectSavePath = downloadState->getDownloadFullPath(projectSavePath);
    if((packet->getLocalPath().isEmpty() || !packet->isAutoDownload())) {
        QString strpath = RBHelper::getOutputPath(downloadState->getJobId(),
                          downloadState->GetParentTaskId(),
                          downloadState->GetScene(),
                          projectSavePath);
        packet->setLocalPath(strpath);
    }
    //QString download = QString("%1/%2").arg(projectSavePath).arg(downloadState->getOutputLabel());
    //std::string pathUtf8 = download.toLocal8Bit();
    //packet->setLocalPath(pathUtf8);

    download_info_t di;
    memset(&di, 0, sizeof(di));
    di.job_id         = packet->getTaskId();
    di.task_id        = packet->getTaskId();
    di.storage_id     = packet->getStorageId();
    di.user_id        = packet->getUserId();
    di.download_sink  = packet;
    di.user_data      = (size_t)packet;
    di.vip_channel    = packet->getVip();
    di.ignore_user_id = packet->getIgnoreUserId();
    di.encrypt        = LocalSetting::getInstance()->getTransEncrypt();
    di.check_type     = LocalSetting::getInstance()->getSparseCheckMode();
    strcpy(di.remote_pathname, packet->getRemotePath().toUtf8());
    strcpy(di.local_pathname, packet->getLocalPath().toUtf8());

    if(JobStateMgr::Inst()->checkDownloadFlag(taskId)) {
        if(m_currEngine != nullptr && packet->isRunning()) {
            bool ret = m_currEngine->push_download_task(di);
            if(!ret) LOGFMTE("[DownloadQueue] Download task push failed, taskid=%d", taskId);
        }
    }
}

void DownloadQueue::addUploadRunningMsg(msg_callback_info_t* msg)
{
    DownloadRunningMsg* downmsg = new DownloadRunningMsg();
    downmsg->initMsg(msg);
    m_msgQueue.post_msg(downmsg);
}

void DownloadQueue::updateTaskTable(const QVector<qint64>& table)
{
    m_pTaskQueue.updateTaskTable(table);
}

void DownloadQueue::startAddTask()
{
    m_pTaskQueue.startAddTask();
}

void DownloadQueue::addTaskEnd()
{
    m_pTaskQueue.addTaskEnd();
}
//////////////////////////////////////////////////////////////////////////
// 下载时的消息
DownloadRunningMsg::DownloadRunningMsg()
{
}

DownloadRunningMsg::~DownloadRunningMsg()
{
}

void DownloadRunningMsg::initMsg(msg_callback_info_t* msg)
{
    _msgData = msg;
}

void DownloadRunningMsg::sendMsg()
{
    CTaskCenterManager::Inst()->handleDownloadTransMsg(_msgData);
}

void DownloadRunningMsg::release()
{
    delete this;
}
/////////////////////////////////////////////////////////////////////////////////////////////

CloneQueue::CloneQueue() 
    : m_currEngine(nullptr)
{
}

CloneQueue::~CloneQueue()
{
    stop();
}

bool CloneQueue::init()
{
    m_downloadPacketPool.init(200);

    m_queue.init(this);
    return true;
}

bool CloneQueue::switchEngine(AbstractTransEngine *transEngine)
{
    if(m_currEngine)
        m_currEngine->pause_download();

    m_currEngine = transEngine;
    if(transEngine->initialized()) {
        transEngine->resume_download();
        return true;
    }

    return transEngine->init();
}

void CloneQueue::switchServer(const QString& server)
{
    if(m_currEngine) {
        m_currEngine->switch_server(server.toStdString());
    }
}

bool CloneQueue::start(AbstractTransEngine *transEngine)
{
    /*
    if(!switchEngine(transEngine))
    return false;
    */
    m_currEngine = transEngine;

    m_queue.start();
    return true;
}

void CloneQueue::stop()
{
    m_queue.stop();
    m_packList.clearAll();
    m_downloadPacketPool.free();
}

bool CloneQueue::startTask(const DownloadInfo &info)
{
    if(m_packList.findPacket(info.taskId)) {
        return false;
    }

    CDownloadPacket* packet = m_downloadPacketPool.popPacket();
    if(packet == nullptr) {
        LOGFMTE("[CloneQueue] Start download failed, create packet failed, memory error! taskid=%d", info.taskId);
        return false;
    }

    packet->init(&info, m_currEngine);

    m_packList.addPacket(packet->getTaskId(), packet);

    return m_queue.post(packet);
}

bool CloneQueue::freePacket(CDownloadPacket *packet)
{
    if(packet == nullptr)
        return false;

    m_packList.stopAndRemovePacket(packet->getTaskId());
    m_downloadPacketPool.freePacket(packet);

    return true;
}

void CloneQueue::on_queue_data(CDownloadPacket* pData)
{
    if (pData == nullptr) {
        LOGFMTW("[CloneQueue] Download failed, packet invalid!");
        return;
    }

    CDownloadPacket* packet = pData;
    int taskId = packet->getTaskId();
    packet->start();

    LOGFMTI("[CloneQueue] Clone task, taskid=%d", taskId);

    QJsonObject user_data;
#ifdef Q_OS_WIN
    user_data["os"] = "Windows";
#else Q_OS_LINUX
    user_data["os"] = "Linux";
#endif
    user_data["source"]       = QString("%1 %2").arg(CLIENT_ID_STR).arg(STRFILEVERSION);
    user_data["task_id"]      = (int)taskId;
    user_data["user_id"]      = MyConfig.userSet.id;
    user_data["storage_id"]   = packet->getStorageId();
    user_data["user_name"]    = MyConfig.userSet.userName;
    user_data["machine_id"]   = System::getMacAddress().replace(":", "");
    user_data["machine_user"] = System::getLoginDisplayName();
    user_data["wan_ip"]       = MyConfig.userSet.ip;
    user_data["lan_ip"]       = System::getLanIP();
    QString s_user_data = QJsonDocument(user_data).toJson(QJsonDocument::Compact);

    download_info_t di;
    memset(&di, 0, sizeof(di));
    di.job_id         = packet->getTaskId();
    di.task_id        = packet->getTaskId();
    di.storage_id     = packet->getStorageId();
    di.user_id        = packet->getUserId();
    di.download_sink  = packet;
    di.user_data      = (size_t)packet;
    di.vip_channel    = packet->getVip();
    di.ignore_user_id = packet->getIgnoreUserId();
    di.encrypt        = false;
    strncpy(di.remote_pathname, packet->getRemotePath().toLocal8Bit(), 1024);
    strncpy(di.local_pathname, packet->getLocalPath().toLocal8Bit(), 1024);
    strncpy(di.extra_data, s_user_data.toUtf8(), 1024);
    bool ret = packet->m_engine->push_download_task(di);

    if(!ret)
        LOGFMTE("[CloneQueue] Clone task push failed, taskid=%d", taskId);
}

