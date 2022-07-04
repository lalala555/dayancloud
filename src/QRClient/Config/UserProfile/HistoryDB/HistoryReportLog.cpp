#include "stdafx.h"
#include "HistoryReportLog.h"

bool HistoryReportLog::postAsyncAddReportLog(const LogReportHistoryInfo& info)
{
    SyncAddTask<LogReportHistoryInfo>* syncData = new SyncAddTask<LogReportHistoryInfo>;
    syncData->table = TABLE_REPORT_LOG;
    syncData->type = SYNC_ADD_TASK;
    syncData->info = new LogReportHistoryInfo;
    *(syncData->info) = info;
    return HistoryDbMgr::getInstance()->asyncTask(syncData, sizeof(SyncAddTask<LogReportHistoryInfo>*));
}

bool HistoryReportLog::postAsyncDelReportLog(const std::string& logpath)
{
    SyncDelReportLog* syncData = new SyncDelReportLog;
    syncData->table = TABLE_REPORT_LOG;
    syncData->type = SYNC_DEL_TASK;
    syncData->logpath = logpath;
    return HistoryDbMgr::getInstance()->asyncTask(syncData, sizeof(SyncDelReportLog*));
}

bool HistoryReportLog::postAsyncUpdateReportLog(const LogReportHistoryInfo& info)
{
    SyncAddTask<LogReportHistoryInfo>* syncData = new SyncAddTask<LogReportHistoryInfo>;
    syncData->table = TABLE_REPORT_LOG;
    syncData->type = SYNC_UPDATE_TASK_REPORT_LOG;
    syncData->info = new LogReportHistoryInfo;
    *(syncData->info) = info;
    return HistoryDbMgr::getInstance()->asyncTask(syncData, sizeof(LogReportHistoryInfo*));
}

LogReportHistoryInfo HistoryReportLog::getReportLogInfo(const std::string& logpath)
{
    LogReportHistoryInfo info;
    auto it = m_dhi.find(logpath);
    if (it == m_dhi.end()) {
        return info;
    }

    return it->second;
}

bool HistoryReportLog::addReportLog(const LogReportHistoryInfo& info)
{
    auto it = m_dhi.find(info.logPath);
    if (it != m_dhi.end()) {
        it->second = info;
        return postAsyncUpdateReportLog(info);
    }
    m_dhi[info.logPath] = info;
    return postAsyncAddReportLog(info);
}

bool HistoryReportLog::removeReportLog(const std::string& logpath)
{
    auto it = m_dhi.find(logpath);
    if (it == m_dhi.end()) {
        return false;
    }
    m_dhi.erase(it);

    return postAsyncDelReportLog(logpath);
}

bool HistoryReportLog::updateReportLog(const LogReportHistoryInfo& info)
{
    auto it = m_dhi.find(info.logPath);
    if (it == m_dhi.end()) {
        return false;
    }

    it->second = info;
    return postAsyncUpdateReportLog(info);
}

bool HistoryReportLog::onAddReportLog(const SQLiteConnection& conn, const SyncData *data)
{
    const SyncAddTask<LogReportHistoryInfo>* syncData = dynamic_cast<const SyncAddTask<LogReportHistoryInfo>*>(data);
    LogReportHistoryInfo info = *(syncData->info);

    try {
        SQLiteCommand cmd(conn);

        SQLiteTransaction trans = conn.beginTransaction();
        char sql[1024] = { 0 };
        sprintf(sql, "replace into t_report_log(task_id,log_file)values(%d,'%s')",
            info.taskId,
            info.logPath.c_str());
        cmd.setCommandText(sql);
        cmd.executeNonQuery();
        trans.commit();

    }
    catch (SQLiteException& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
    return true;
}

bool HistoryReportLog::onRemoveReportLog(const SQLiteConnection &conn, const SyncData *data)
{
    const SyncDelReportLog* syncData = dynamic_cast<const SyncDelReportLog* >(data);

    try {
        SQLiteTransaction trans = conn.beginTransaction();
        SQLiteCommand cmd(conn);

        char sql[4096] = { 0 };
        sprintf(sql, "delete from t_report_log where log_file='%s'", syncData->logpath.c_str());

        cmd.setCommandText(sql);
        cmd.executeNonQuery();
        trans.commit();
    }
    catch (SQLiteException& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    return true;
}

bool HistoryReportLog::readReportLoglist(const SQLiteConnection & connection)
{
    try {
        SQLiteCommand cmd(connection);

        char sql[512] = { 0 };
        snprintf(sql, 512, "select * from t_report_log");
        cmd.setCommandText(sql);
        SQLiteDataReader dr = cmd.executeReader();

        while (dr.read()) {
            LogReportHistoryInfo taskInfo;
            for (int i = 0; i < dr.getFieldCount(); i++) {
                if (!dr.isDbNull(i)) {
                    switch (i) {
                    case 0:
                        taskInfo.index = dr.getInt32(i);
                        break;
                    case 1:
                        taskInfo.taskId = dr.getInt32(i);
                        break;
                    case 2:
                        taskInfo.logPath = dr.getString(i);
                        break;
                    }
                }
            }
            m_dhi[taskInfo.logPath] = taskInfo;
        }
    }
    catch (SQLiteException& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
    return true;
}