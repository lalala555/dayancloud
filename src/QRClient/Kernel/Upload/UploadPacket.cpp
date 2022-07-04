#include "stdafx.h"
#include "UploadPacket.h"
#include "TaskCenterManager.h"
#include "MessagePool.h"
#include "Common/SystemUtil.h"

CUploadPacket::CUploadPacket()
    : m_engine(nullptr)
    , m_taskId(0)
    , m_bVip(false)
    , m_storageId(0)
    , m_total(0)
    , m_fileList(nullptr)
{
}

CUploadPacket::~CUploadPacket(void)
{
    stop();
    clearPacket();
}

bool CUploadPacket::init(const UploadInfo* info, UploadFileListByDb* fileList)
{
    if(info == nullptr) {
        LOGFMTE("[UploadPacket] Init failed, input error!");
        return false;
    }
    m_bVip      = info->bVip;
    m_taskId    = info->taskId;
    m_uid       = info->uid;
    m_runStatus = PACKET_FREE;
    m_fileList  = fileList;
    _priority   = info->taskPriority;
    _priorityTime = info->taskPriorityTime;
    _packetIndex  = info->index;
    return true;
}

void CUploadPacket::clearPacket()
{
    m_bVip      = false;
    m_taskId    = 0;
    m_uid       = 0;
    m_runStatus = PACKET_FREE;
    m_engine    = nullptr;
    m_total     = 0;         //需要传输的总文件数目
    m_fileList  = nullptr;
}

bool CUploadPacket::start()
{
    if(m_runStatus == PACKET_RUNNING)
        return false;
    m_runStatus     = PACKET_RUNNING;
    return true;
}

void CUploadPacket::stop()
{
    m_runStatus = PACKET_STOP;
    m_engine    = nullptr;
}

bool CUploadPacket::doWork(AbstractTransEngine* engine)
{
    LOGFMTI("[UploadPacket] %d>Upload task running...", m_taskId);
    if (engine == NULL) {
        return false;
    }

    QList<UncompleteFile> list;
    m_engine = engine;
    if (m_fileList != nullptr) {
        m_total = m_fileList->getTransformListCount(list);
    }
    
    foreach(UncompleteFile file, list) {
        if (!this->isRunning())
            break;
        if (!this->uploadFile(file)) {
            LOGFMTE("[UploadPacket] push upload file [%s] failed", qPrintable(file.local_file));
        }
#ifdef Q_OS_WIN
        Sleep(20);
#else
#endif
    }

    return true;
}

bool CUploadPacket::uploadFile(const UncompleteFile& fileElem)
{
    QJsonObject user_data;
#ifdef Q_OS_WIN
    user_data["os"] = "Windows";
#else Q_OS_LINUX
    user_data["os"] = "Linux";
#endif
    user_data["source"]       = QString("%1 %2").arg(CLIENT_ID_STR).arg(STRFILEVERSION);
    user_data["task_id"]      = (int)getTaskId();
    user_data["user_id"]      = MyConfig.userSet.id;
    user_data["storage_id"]   = fileElem.rootid.toInt();
    user_data["user_name"]    = MyConfig.userSet.userName;
    user_data["machine_id"]   = System::getMacAddress().replace(":", "");
    user_data["machine_user"] = System::getLoginDisplayName();
    QString s_user_data = QJsonDocument(user_data).toJson(QJsonDocument::Compact);

    upload_info_t ui;
    memset(&ui, 0, sizeof(ui));
    ui.task_id        = getTaskId();
    ui.storage_id     = fileElem.rootid.toInt();
    ui.upload_sink    = this;
    ui.user_id        = getUserId();
    ui.encrypt        = LocalSetting::getInstance()->getTransEncrypt();
    ui.job_id         = fileElem.jobid;
    ui.user_data      = (size_t)this;
    ui.ignore_user_id = true/*fileElem.type == TYPE_CFG ? true : false*/;
    ui.vip_channel    = m_bVip;
    ui.check_type     = LocalSetting::getInstance()->getSparseCheckMode();
    strncpy(ui.local_pathname, fileElem.local_file.toUtf8(), 1024);
    strncpy(ui.remote_pathname, fileElem.remote_file.toUtf8(), 1024);
    strncpy(ui.extra_data, s_user_data.toUtf8(), 1024);

    bool ret = false;
    if (m_engine != NULL && this->isRunning()) {
        ret = m_engine->push_upload_task(ui);
        if (!ret && m_fileList != nullptr) {
            m_fileList->setFileUploadFailed(fileElem.local_file, fileElem.remote_file);
            LOGFMTE("[UploadPacket] uploadFile push failed %s", qPrintable(fileElem.local_file));
        }
    } else {
        LOGFMTE("[UploadPacket][taskid=%lld] Trans Engine pointer is NULL or packet not run!, local file is %s",
            fileElem.taskid, qPrintable(fileElem.local_file));
    }

    return ret;
}

void CUploadPacket::on_upload(const upload_callback_info_t & callback_info)
{
    int message_type = UPLOAD_TRANSFER_STOP;
    msg_callback_info_t *callback   = CMessagePool::Inst()->Pop();
    memset(callback, 0, sizeof(msg_callback_info_t));
    callback->trans_info.task_id            = callback_info.task_id;
    callback->trans_info.job_id             = callback_info.job_id;
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
    strcpy(callback->trans_info.error_description, callback_info.error_description);
    strcpy(callback->trans_info.local_pathname, callback_info.local_pathname);
    strcpy(callback->trans_info.remote_pathname, callback_info.remote_pathname);

    message_type = translateRayvisionCallback(callback_info);

    callback->trans_info.message_type = message_type;
    callback->msg_type = msg_callback_info_t::TYPE_UPLOAD;
    if(isRunning()) {
        // CTaskCenterManager::Inst()->addTransferMsg(callback);
        CTaskCenterManager::Inst()->addUploadTransMsg(callback);
    }
}

// 转义
int CUploadPacket::translateRayvisionCallback(const upload_callback_info_t & callback_info)
{
    int message_type = UPLOAD_TRANSFER_STOP;

    switch(callback_info.message_type) {
    case transmit_callback_message_type_t::callback_round_begin:
        message_type = UPLOAD_SESSION_START;
        break;
    case transmit_callback_message_type_t::callback_send_connect_request:
        message_type = UPLOAD_SESSION_CONNECTING;
        break;
    case transmit_callback_message_type_t::callback_recv_connect_response:
        message_type = UPLOAD_SESSION_CONNECTED;
        break;
    case transmit_callback_message_type_t::callback_connect_failure:
        message_type = UPLOAD_SESSION_ERROR;
        break;
    case transmit_callback_message_type_t::callback_send_disconnect_request:
    case transmit_callback_message_type_t::callback_recv_disconnect_response:
    case transmit_callback_message_type_t::callback_connection_exception:
        message_type = UPLOAD_SESSION_CONNECTFAILED;
        break;
    case transmit_callback_message_type_t::callback_file_begin:
        message_type = UPLOAD_FILE_START;
        break;
    case transmit_callback_message_type_t::callback_file_progress:
        message_type = UPLOAD_FILE_PROGRESS;
        break;
    case transmit_callback_message_type_t::callback_file_done:
        message_type = UPLOAD_FILE_COMPELETE;
        break;
    case transmit_callback_message_type_t::callback_file_fail: {
        switch(callback_info.error_type) {
        case transmit_callback_error_type_t::local_upload_same_file:
        case transmit_callback_error_type_t::remote_upload_same_file:
            message_type = UPLOAD_FILE_FAIL_SAME;
            break;
        default:
            message_type = UPLOAD_FILE_FAIL;
            break;
        }
        break;
    }
    case transmit_callback_message_type_t::callback_file_interrupt:
        message_type = UPLOAD_FILE_INTERRUPT;
        break;
    case transmit_callback_message_type_t::callback_transmit_has_failure:
    case transmit_callback_message_type_t::callback_transmit_all_success:
        break;
    case transmit_callback_message_type_t::callback_round_end:
        message_type = UPLOAD_SESSION_STOP;
        break;
    default:
        break;
    }
    return message_type;
}

void CUploadPacket::startTask()
{
    //m_future = QtConcurrent::run([this] {
        if (this->isIdle()) {
            int taskId = this->getTaskId();
            if (!this->start()) {
                return;
            }
            LOGFMTI("[UploadQueue] Upload task, taskid=%d", taskId);

            bool ret = this->doWork(m_engine);
        }
    //});
}

void CUploadPacket::stopTask()
{
    // m_future.cancel();
    stop(); 
    // m_future.waitForFinished();
}

void CUploadPacket::updatePriority(int priority)
{
    _priority = priority;
}

void CUploadPacket::updateTimePriority(unsigned int timePriority)
{
    _priorityTime = timePriority;
}