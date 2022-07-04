#include "stdafx.h"
#include "DownloadPacket.h"
#include "TaskCenterManager.h"
#include "MessagePool.h"
#include "NewJobs/JobStateMgr.h"
#include "PluginConfig/ProjectMgr.h"
#include "Common/SystemUtil.h"
#include "UI/Views/DownloadPage/DownloadStateMgr.h"

CDownloadPacket::CDownloadPacket()
    : m_isVip(false)
    , m_isIgnoreUserId(false)
    , m_isClone(false)
    , m_priority(PRIORITY1)
    , m_priorityTime(0)
    , m_engine(nullptr)
    , m_bAutoDownload(false)
    , m_taskId(0)
    , m_projectId(0)
    , m_uid(0)
{

}

CDownloadPacket::~CDownloadPacket()
{
    stop();
}

bool CDownloadPacket::init(const DownloadInfo* info, AbstractTransEngine *engine)
{
    if(info == nullptr)
        return false;

    m_localPath      = info->localPath;
    m_remotePath     = info->remotePath;
    m_taskId         = info->taskId;
    m_parentId       = info->parentTaskId;
    m_uid            = info->m_uid;
    m_projectId      = info->storagetId;
    m_runStatus      = PACKET_FREE;
    m_isVip          = info->isVip;
    m_isIgnoreUserId = info->isIgnoreUserId;
    m_isClone        = info->isClone;
    m_priority       = info->priority;
    m_priorityTime   = info->priorityTime;
    m_bAutoDownload  = info->bAutoDownload;
    _priority        = m_priority;
    _priorityTime    = m_priorityTime;
    _packetIndex     = info->index;
    m_engine         = engine;
    m_localPathList  = info->localPathList;
    m_remotePathList = info->remotePathList;

    return true;
}

void CDownloadPacket::clearPacket()
{
    m_localPath.clear();  //下载任务按目录为单位
    m_remotePath.clear();
    m_taskId    = 0;
    m_uid       = 0;
    m_isVip     = false;
    m_isIgnoreUserId = false;
    m_isClone        = false;
    m_priority       = PRIORITY1;
    m_priorityTime   = 0;
    m_bAutoDownload  = false;
    m_projectId      = 0;
    m_runStatus      = PACKET_FREE;
}

void CDownloadPacket::on_download(const download_callback_info_t & callback_info)
{
    int message_type = DOWNLOAD_TRANSFER_STOP;
    msg_callback_info_t* callback = CMessagePool::Inst()->Pop();
    memset(callback, 0, sizeof(msg_callback_info_t));
    callback->trans_info.task_id            = callback_info.task_id;
    callback->trans_info.job_id             = callback_info.job_id;
    callback->trans_info.message_type       = callback_info.message_type;
    callback->trans_info.speed              = callback_info.speed;
    callback->trans_info.progress           = callback_info.progress;
    callback->trans_info.user_data          = callback_info.user_data;
    callback->trans_info.error_type         = callback_info.error_type;
    callback->trans_info.callback_type      = callback_info.callback_type;
    callback->trans_info.connect_type       = callback_info.connect_type;
    callback->trans_info.lost_rate          = callback_info.lost_rate;
    callback->trans_info.delay              = callback_info.delay;
    callback->trans_info.file_transfer_size = callback_info.file_transfer_size;
    callback->trans_info.file_total_size    = callback_info.file_total_size;
    callback->trans_info.storage_id         = this->getStorageId();
    strcpy(callback->trans_info.error_description, callback_info.error_description);
    strcpy(callback->trans_info.local_pathname, callback_info.local_pathname);
    strcpy(callback->trans_info.remote_pathname, callback_info.remote_pathname);

    message_type = translateRayvisionCallback(callback_info);

    callback->trans_info.message_type = message_type;
    callback->msg_type = msg_callback_info_t::TYPE_DOWNLOAD;
    if(isRunning())
        CTaskCenterManager::Inst()->addDownloadTransMsg(callback);
}

int CDownloadPacket::translateRayvisionCallback(const download_callback_info_t & callback_info)
{
    int message_type = UPLOAD_TRANSFER_STOP;

    switch(callback_info.message_type) {
    case transmit_callback_message_type_t::callback_round_begin:
        message_type = DOWNLOAD_SESSION_START;
        break;
    case transmit_callback_message_type_t::callback_send_connect_request:
        message_type = DOWNLOAD_SESSION_CONNECTING;
        break;
    case transmit_callback_message_type_t::callback_recv_connect_response:
        message_type = DOWNLOAD_SESSION_CONNECTED;
        break;
    case transmit_callback_message_type_t::callback_connect_failure:
        message_type = DOWNLOAD_SESSION_ERROR;
        break;
    case transmit_callback_message_type_t::callback_send_disconnect_request:
    case transmit_callback_message_type_t::callback_recv_disconnect_response:
    case transmit_callback_message_type_t::callback_connection_exception:
        message_type = DOWNLOAD_SESSION_CONNECTFAILED;
        break;
    case transmit_callback_message_type_t::callback_file_fail:
        message_type = DOWNLOAD_FILE_ERROR;
        break;
    case transmit_callback_message_type_t::callback_file_begin:
        message_type = DOWNLOAD_FILE_START;
        break;
    case transmit_callback_message_type_t::callback_file_progress:
        message_type = DOWNLOAD_FILE_PROGRESS;
        break;
    case transmit_callback_message_type_t::callback_file_done:
        message_type = DOWNLOAD_FILE_COMPLETED;
        break;
    case transmit_callback_message_type_t::callback_transmit_has_failure:
        message_type = DOWNLOAD_ALLFILE_HASFAILURE;
        break;
    case transmit_callback_message_type_t::callback_transmit_all_success:
        message_type = DOWNLOAD_ALLFILE_COMPLETED;
        break;
    case transmit_callback_message_type_t::callback_round_end:
        message_type = DOWNLOAD_SESSION_STOP;
        break;
    default:
        break;
    }

    if(getStorageId() == MyConfig.storageSet.cloneBid) {
        switch(message_type) {
        case DOWNLOAD_ALLFILE_COMPLETED:
            message_type = DOWNLOAD_CLONE_COMPLETED;
            break;
        case DOWNLOAD_ALLFILE_HASFAILURE:
            message_type = DOWNLOAD_CLONE_SESSION_ERROR;
            break;
        case DOWNLOAD_SESSION_STOP:
            message_type = DOWNLOAD_CLONE_SESSION_STOP;
            break;
        default:
            message_type = DOWNLOAD_DEFAULT;
            break;
        }
    }
    return message_type;
}

QString getListValue(const QStringList& list, const QString& key)
{
    if (list.isEmpty() || key.isEmpty())
        return "";

    foreach(QString path, list) {
        if (path.contains(key)) {
            return path;
        }
    }

    return "";
}

void CDownloadPacket::startTask()
{
    if (!this->start())
        return;

    int taskId = this->getTaskId();

    LOGFMTI("[DownloadQueue] Download task, taskid=%d", taskId);
    CDownloadState *downloadState = CDownloadStateMgr::Inst()->LookupTaskstate(taskId);
    if (downloadState == nullptr)
        return;

    QString projectSavePath = downloadState->GetLocalSavePath();
    QString outputPath = downloadState->getOutputPath(projectSavePath);
    QStringList outputList;
    if (!downloadState->getDownloadFileList().isEmpty()) {
        foreach(QString output, downloadState->getDownloadFileList()) {
            QString path = outputPath;
            path.append("/");
            path.append(output);
            outputList.append(path);
        }
    }
    this->setLocalPathList(outputList);
    // downloadState->resetDownloadedInfo();

    /*QString projectSavePath = ProjectMgr::getInstance()->getProjectOutput(downloadState->getProjectName());
    projectSavePath = downloadState->getDownloadFullPath(projectSavePath);
    if ((this->getLocalPath().isEmpty() || !this->isAutoDownload())) {
        QString strpath = RBHelper::getOutputPath(downloadState->getTaskId(),
            downloadState->getTaskParentId(),
            downloadState->getScene(),
            projectSavePath);
        this->setLocalPath(strpath);
    }*/

    QJsonObject user_data;
#ifdef Q_OS_WIN
    user_data["os"] = "Windows";
#else Q_OS_LINUX
    user_data["os"] = "Linux";
#endif
    user_data["source"]       = QString("%1 %2").arg(CLIENT_ID_STR).arg(STRFILEVERSION);
    user_data["task_id"]      = (int)taskId;
    user_data["user_id"]      = MyConfig.userSet.id;
    user_data["storage_id"]   = this->getStorageId();
    user_data["user_name"]    = MyConfig.userSet.userName;
    user_data["machine_id"]   = System::getMacAddress().replace(":", "");
    user_data["machine_user"] = System::getLoginDisplayName();
    user_data["wan_ip"]       = MyConfig.userSet.ip;
    user_data["lan_ip"]       = System::getLanIP();
    QString s_user_data = QJsonDocument(user_data).toJson(QJsonDocument::Compact);

    QStringList remoteList = this->getRemotePathList();
    QStringList localList = this->getLocalPathList();
    qsrand(QDateTime::currentDateTime().toTime_t());
    for (int i = 0; i < downloadState->getDownloadFileList().size(); i++) {
        QString type = downloadState->getDownloadFileList().at(i);

        QString remote_path = getListValue(remoteList, type);
        QString local_path  = getListValue(localList, type);

        // if (downloadState->isDownloadedFileFinished(type)) {
        //     continue;
        // }
                 
        download_info_t di;
        memset(&di, 0, sizeof(di));
        di.job_id = this->getTaskId() + i + qrand() % 100;
        di.task_id = this->getTaskId();
        di.storage_id = this->getStorageId();
        di.user_id = this->getUserId();
        di.download_sink = this;
        di.user_data = (size_t)this;
        di.vip_channel = this->getVip();
        di.ignore_user_id = this->getIgnoreUserId();
        di.encrypt = LocalSetting::getInstance()->getTransEncrypt();
        di.check_type = LocalSetting::getInstance()->getSparseCheckMode();
        strncpy(di.remote_pathname, remote_path.toUtf8(), 1024);
        strncpy(di.local_pathname, local_path.toUtf8(), 1024);
        strncpy(di.extra_data, s_user_data.toUtf8(), 1024);

        if (JobStateMgr::Inst()->checkDownloadFlag(taskId)) {
            if (m_engine != nullptr && this->isRunning()) {
                bool ret = m_engine->push_download_task2(di);
                if (!ret) LOGFMTE("[DownloadQueue] Download task push failed, taskid=%d", taskId);
            }
        }           
    }

    if (m_engine != nullptr && this->isRunning()) {
        bool ret = m_engine->start_download_task2(taskId);
        if (!ret) LOGFMTE("[DownloadQueue] Download task start failed, taskid=%d", taskId);
    }
}

void CDownloadPacket::stopTask()
{
    stop();
}

void CDownloadPacket::updatePriority(int priority)
{
    _priority = priority;
}

void CDownloadPacket::updateTimePriority(unsigned int timePriority)
{
    _priorityTime = timePriority;
}