#include "stdafx.h"
#include "HistoryDbMgr.h"
#include "UserProfile/ConfigDb.h"
#include "UserProfile/LocalConfig.h"

const std::string sql_table_report_log = "CREATE TABLE IF NOT EXISTS [t_report_log]( "
                                    "[id] INTEGER PRIMARY KEY AUTOINCREMENT, "
                                    "[task_id] INT NOT NULL, "
                                    "[log_file] VARCHAR(1024) UNIQUE ON CONFLICT REPLACE,"
                                    "[create_time] DATETIME DEFAULT (DATETIME ('now', 'localtime')));";

const std::string sql_table_trans_history = "CREATE TABLE IF NOT EXISTS [trans_history]( "
                                     "[index] INTEGER PRIMARY KEY AUTOINCREMENT, "
                                     "[taskid] INT NOT NULL, "
                                     "[tasktype] INT, "
                                     "[transstate] INT DEFAULT 0, "
                                     "[viptask] INT DEFAULT 0, "
                                     "[autodownload] INT DEFAULT 0, "
                                     "[scenename] INT NOT NULL, "
                                     "[scenepath] INT NOT NULL, "
                                     "[downloadpath] VARCHAR(1024), "
                                     "[localsavepath] VARCHAR(1024), "
                                     "[camera] INT NOT NULL, "
                                     "[artist] VARCHAR(100) DEFAULT 'unknown', "
                                     "[starttime] VARCHAR(128), "
                                     "[endtime] VARCHAR(128), "
                                     "[taskalias] VARCHAR(512),"
                                     "[download_mode] INT DEFAULT 1,"
                                     "[unicode_download] INT DEFAULT 0,"
                                     "[downloadtype] VARCHAR(32),"
                                     "[coord_count] INT DEFAULT -1,"
                                     "[picture_count] INT DEFAULT -1);";

const std::string sql_table_download = "CREATE TABLE IF NOT EXISTS [t_download]( "
                                       "[id] INTEGER PRIMARY KEY AUTOINCREMENT, "
                                       "[project] VARCHAR(256) DEFAULT 'default', "
                                       "[taskid] INT NOT NULL, "
                                       "[name] VARCHAR(1024) DEFAULT '-', "
                                       "[artist] VARCHAR(100) DEFAULT 'unknown', "
                                       "[saveas_path] VARCHAR(1024), "
                                       "[save_path] VARCHAR(1024), "
                                       "[status] INT DEFAULT 0, "
                                       "[times] INT DEFAULT 1, "
                                       "[start_time] DATETIME DEFAULT (DATETIME ('now', 'localtime')), "
                                       "[stop_time] DATETIME, "
                                       "[checking] INT DEFAULT 0, "
                                       "[priority] INT DEFAULT 1, "
                                       "[priority_time] INT DEFAULT 0, "
                                       "[layername] CHAR(1024) DEFAULT '-',"
                                       "[parentid] INT DEFAULT 0,"
                                       "[storageid] INT DEFAULT 0,"
                                       "[output_label] VARCHAR(1024),"
                                       "[task_alias] VARCHAR(64),"
                                       "[task_parent_alias] VARCHAR(64),"
                                       "[output_size] VARCHAR(64),"
                                       "[priority_type] INT DEFAULT 0,"
                                       "[operate_type] INT DEFAULT 0,"
                                       "[download_job_type] INT DEFAULT 0,"
                                       "[task_user_id] INT DEFAULT 0,"
                                       "[output_types] VARCHAR(256));";

HistoryDbMgr::HistoryDbMgr(void)
    : m_queue(Q_NULLPTR)
{
}

HistoryDbMgr::~HistoryDbMgr(void)
{
}

bool HistoryDbMgr::beepOn()
{
    m_queue.init(this);
    m_queue.start();

    // 不存在的话创建数据库
    QString userprofile_path = RBHelper::getUserProfilePath();
    QString db_path = userprofile_path.append("/tasklist.dat");
    if(!openDb(db_path)) {
        LOGFMTE("[HistoryDbMgr] initConfig OpenDB error!");
        return false;
    }

    return readTasklist();
}

void HistoryDbMgr::beepOff()
{
    CloseDb();
}

bool HistoryDbMgr::openDb(const QString& db_path)
{
    if(!OpenDb(db_path.toStdString()))
        return false;

    if(!m_conn)
        return false;

    try {
        SQLiteCommand cmd(*m_conn);
        cmd.setCommandText(sql_table_report_log);
        cmd.executeNonQuery();
        cmd.setCommandText(sql_table_trans_history);
        cmd.executeNonQuery();
        cmd.setCommandText(sql_table_download);
        cmd.executeNonQuery();
    } catch(SQLiteException& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    return true;
}

void HistoryDbMgr::on_queue_data(SyncData* pData)
{
    if(pData == nullptr)
        return;

    const SyncData* data = pData;
    if(data == nullptr)  return;

    switch(data->type) {
    case SYNC_ADD_TASK:
        this->onAddTask(data->taskId, data);
        break;
    case SYNC_DEL_TASK:
        this->onRemoveTask(data->taskId, data);
        break;
    case SYNC_UPDATE_TASK_TRANS_STATE:
        this->onUpdateTransTaskState(data->taskId, data);
        break;
    case SYNC_UPDATE_TASK_SCENE_PATH:
        this->onUpdateTransTaskScenePath(data->taskId, data);
        break;
    case SYNC_UPDATE_TASK_DOWNLOAD_PATH:
        this->onUpdateTransTaskDownloadPath(data->taskId, data);
        break;
    case SYNC_UPDATE_TASK:
        this->onUpdateTransTask(data->taskId, data);
        break;
    case SYNC_UPDATE_TASK_LOCALSAVE_PATH:
        this->onUpdateTransTaskLocalSavePath(data->taskId, data);
        break;
    case SYNC_UPDATE_TASK_STATUS:
        this->onUpdateDownloadStatus(data->taskId, data);
        break;
    default:
        break;
    }
    delete data;
    data = nullptr;
}

bool HistoryDbMgr::readTasklist()
{
    return m_transHistory.updateTableStruct(*m_conn)
           && m_transHistory.readTasklist(*m_conn)
           && m_downloadHistory.updateTableStruct(*m_conn)
           && m_downloadHistory.readTasklist(*m_conn);
}

bool HistoryDbMgr::asyncTask(void *data, int size)
{
    return m_queue.post((SyncData*)data);
}

bool HistoryDbMgr::onAddTask(int taskId, const SyncData* data)
{
    switch(data->table) {
    case TABLE_TRANS:
        m_transHistory.onAddTask(*m_conn, taskId, data);
        break;
    case TABLE_DOWNLOAD:
        m_downloadHistory.onAddTask(*m_conn, taskId, data);
        break;
    }

    return true;
}

bool HistoryDbMgr::onRemoveTask(int taskId, const SyncData* data)
{
    switch(data->table) {
    case TABLE_TRANS:
        m_transHistory.onRemoveTask(*m_conn, taskId, data);
        break;
    case TABLE_DOWNLOAD:
        m_downloadHistory.onRemoveTask(*m_conn, taskId, data);
        break;
    }

    return true;
}

bool HistoryDbMgr::addTransTask(int taskId, const TransHistoryInfo& info)
{
    return m_transHistory.addTask(taskId, info);
}

bool HistoryDbMgr::removeTransTask(int taskId)
{
    return m_transHistory.removeTask(taskId);
}

bool HistoryDbMgr::updateTransTask(int taskId, const TransHistoryInfo& info)
{
    return m_transHistory.updateTask(taskId, info);
}

bool HistoryDbMgr::updateTaskTransState(int taskId, int transState)
{
    return m_transHistory.updateTaskTransState(taskId, transState);
}

bool HistoryDbMgr::updateTransTaskScenePath(int taskId, std::string& scenePath)
{
    return m_transHistory.updateTaskScenePath(taskId, scenePath);
}

bool HistoryDbMgr::updateTransTaskDownloadPath(int taskId, std::string& downloadPath)
{
    return m_transHistory.updateTaskDownloadPath(taskId, downloadPath);
}

int  HistoryDbMgr::getUnfinishedUploadTransTaskCount()
{
    std::map<int, TransHistoryInfo> list;
    return m_transHistory.getUploadUnfinishedCount(list);
}

void HistoryDbMgr::getUnfinishedUploadTransTaskList(std::map<int, TransHistoryInfo>& list)
{
    m_transHistory.getUploadUnfinishedCount(list);
}

int  HistoryDbMgr::getUnfinishedDownloadTransTaskCount()
{
    std::map<int, TransHistoryInfo> list;
    return m_transHistory.getDownloadUnfinishedCount(list);
}

void HistoryDbMgr::getUnfinishedDownloadTransTaskList(std::map<int, TransHistoryInfo>& list)
{
    m_transHistory.getDownloadUnfinishedCount(list);
}

void HistoryDbMgr::getUnstoppedDownloadTransTaskList(std::map<int, TransHistoryInfo>& list)
{
    m_transHistory.getDownloadUnstoppedTaskList(list);
}

int  HistoryDbMgr::getDownloadTransTaskStatus(int taskId)
{
    return m_transHistory.getDownloadTaskStatus(taskId);
}

bool HistoryDbMgr::onUpdateTransTaskScenePath(int taskId, const SyncData* data)
{
    return m_transHistory.onUpdateTaskScenePath(*m_conn, taskId, data);
}

bool HistoryDbMgr::onUpdateTransTaskDownloadPath(int taskId, const SyncData* data)
{
    return m_transHistory.onUpdateTaskDownloadPath(*m_conn, taskId, data);
}

bool HistoryDbMgr::onUpdateTransTaskLocalSavePath(int taskId, const SyncData* data)
{
    return m_transHistory.onUpdateTaskLoaclSavePath(*m_conn, taskId, data);
}

bool HistoryDbMgr::onUpdateTransTask(int taskId, const SyncData* data)
{
    return m_transHistory.onUpdateTask(*m_conn, taskId, data);
}

bool HistoryDbMgr::onUpdateTransTaskState(int taskId, const SyncData* data)
{
    return m_transHistory.onUpdateTaskTransState(*m_conn, taskId, data);
}

void HistoryDbMgr::getAllTransTaskHistory(std::map<int, TransHistoryInfo>& list)
{
    m_transHistory.getAllTaskList(list);
}

void HistoryDbMgr::getTransDownloadHistroy(std::map<int, TransHistoryInfo>& list)
{
    m_transHistory.getDownloadTaskList(list);
}

bool HistoryDbMgr::addDownloadTask(int taskId, const DownloadHistoryInfo& info)
{
    return m_downloadHistory.addTask(taskId, info);
}

bool HistoryDbMgr::removeDownloadTask(int taskId)
{
    return m_downloadHistory.removeTask(taskId);
}

bool HistoryDbMgr::updateDownloadTaskStatus(int taskId, int status, int nchecking, const std::string& completeTime, const std::string& downloadList)
{
    return m_downloadHistory.updateTaskStatus(taskId, status, nchecking, completeTime, downloadList);
}

bool HistoryDbMgr::updateDownloadTaskPriority(int taskId, int priority, int priorityTime, int prorityType)
{
    return m_downloadHistory.updateTaskPriority(taskId, priority, priorityTime, prorityType);
}

bool HistoryDbMgr::updateDownLoadTaskScene(int taskId, const std::string &strScene)
{
    return m_downloadHistory.updateTaskScene(taskId, strScene);
}

int HistoryDbMgr::getUnfinishDownloadCount(int filterDate)
{
    return m_downloadHistory.getUnfinishCount(filterDate);
}

void HistoryDbMgr::getDownloadTaskList(std::map<int, DownloadHistoryInfo>& list, int filterDate)
{
    m_downloadHistory.getTaskList(list, filterDate);
}

void HistoryDbMgr::getUnstoppedDownloadTaskList(std::map<int, DownloadHistoryInfo>& list, int filterDate)
{
    m_downloadHistory.getUnstoppedTaskList(list, filterDate);
}

int HistoryDbMgr::getDownloadTaskStatus(int taskId)
{
    return m_downloadHistory.getTaskStatus(taskId);
}

bool HistoryDbMgr::isTaskHistoryExist(int taskId)
{
    return m_downloadHistory.findTask(taskId);
}

void HistoryDbMgr::getDownloadUnfinishedTaskIds(std::list<int>& list, int filterDate)
{
    m_downloadHistory.getUnfinishedTaskList(list, filterDate);
}

bool HistoryDbMgr::updateTaskParentInfo(int taskId, int parentId, const std::string & parentAlias)
{
    return m_downloadHistory.updateTaskParentInfo(taskId, parentId, parentAlias);
}

void HistoryDbMgr::getDownloadTaskIds(std::list<int>& list, int filterDate)
{
    m_downloadHistory.getTaskListIds(list, filterDate);
}

bool HistoryDbMgr::onUpdateDownloadStatus(int taskId, const SyncData* data)
{
    return m_downloadHistory.onUpdateTaskStatus(*m_conn, taskId, data);
}