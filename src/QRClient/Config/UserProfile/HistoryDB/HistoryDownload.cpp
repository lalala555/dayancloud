#include "stdafx.h"
#include "HistoryDownload.h"


HistoryDownload::HistoryDownload()
{
}


HistoryDownload::~HistoryDownload()
{
}

bool HistoryDownload::findTask(int taskId)
{
    std::map<int, DownloadHistoryInfo>::const_iterator it = m_dhi.find(taskId);
    if (it != m_dhi.end()) {
        return true;
    }
    return false;
}

int HistoryDownload::getTaskStatus(int taskId)
{
    std::map<int, DownloadHistoryInfo>::const_iterator it = m_dhi.find(taskId);
    if (it != m_dhi.end()) {
        return it->second.status;
    }

    return INVALID_STATUS;
}

//获取所有历史记录
void HistoryDownload::getTaskList(std::map<int, DownloadHistoryInfo>& list, int filterDate)
{
    auto it = m_dhi.begin();
    while (it != m_dhi.end()) {
        std::string startTime = it->second.startTime;
        if (this->isDateContains(startTime, filterDate)) {
            list.insert(std::make_pair(it->first, it->second));
        }
        it++;
    }
}

bool HistoryDownload::addTask(int taskId, const DownloadHistoryInfo& info)
{
    std::map<int, DownloadHistoryInfo>::iterator it = m_dhi.find(taskId);

    if (it != m_dhi.end()) {
        m_dhi[taskId] = info;
        return postAsyncUpdateTaskStatus(taskId, info.status, info.ncheckingstatus, info.stopTime, info.downloadType);
    }
    m_dhi[taskId] = info;
    return postAsyncAddTask(taskId, info);
}

bool HistoryDownload::removeTask(int taskId)
{
    std::map<int, DownloadHistoryInfo>::iterator it = m_dhi.find(taskId);
    if (it == m_dhi.end()) {
        return false;
    }

    m_dhi.erase(it);

    return postAsyncDelTask(taskId);
}

bool HistoryDownload::updateTaskStatus(int taskId, int status, int checking, const std::string& completeTime, const std::string& downloadList)
{
    std::map<int, DownloadHistoryInfo>::iterator it = m_dhi.find(taskId);
    if (it == m_dhi.end()) {
        return false;
    }

    it->second.status = status;
    it->second.stopTime = completeTime;
    it->second.downloadType = downloadList;

    return postAsyncUpdateTaskStatus(taskId, status, checking, completeTime, downloadList);
}

bool HistoryDownload::updateTaskPriority(int taskId, int priority, int priorityTime, int prorityType)
{
    std::map<int, DownloadHistoryInfo>::iterator it = m_dhi.find(taskId);
    if (it == m_dhi.end()) {
        return false;
    }

    it->second.priority = priority;
    it->second.priorityTime = priorityTime;
    it->second.priorityType = prorityType;

    return postAsyncUpdateTaskPriority(taskId, priority, priorityTime, prorityType);
}
bool HistoryDownload::updateTaskScene(int taskId, const std::string &strScene)
{
    std::map<int, DownloadHistoryInfo>::iterator it = m_dhi.find(taskId);
    if (it == m_dhi.end()) {
        return false;
    }

    it->second.sceneName = strScene;
    return postAsyncUpdateTaskScene(taskId, strScene);
}
bool HistoryDownload::updateTaskParentInfo(int taskId, int parentId, const std::string &taskAlias)
{
    auto it = m_dhi.find(taskId);
    if (it == m_dhi.end()) {
        return false;
    }
    it->second.parentId = parentId;
    it->second.parentTaskAlias = taskAlias;
    return postAsyncUpdateParentInfo(taskId, parentId, taskAlias);
}
bool HistoryDownload::updateTaskOutputSize(int taskId, const std::string &outputsize)
{
    auto it = m_dhi.find(taskId);
    if (it == m_dhi.end()) {
        return false;
    }
    it->second.outputSize = outputsize;
    return postAsyncTaskOutputSize(taskId, outputsize);
}

int HistoryDownload::getUnfinishCount(int filterDate)
{
    int count = 0;
    std::map<int, DownloadHistoryInfo>::iterator it = m_dhi.begin();
    while (it != m_dhi.end()) {
        int status = it->second.status;
        std::string startTime = it->second.startTime;
        if (isDateContains(startTime, filterDate) &&
            (status == DOWNLOAD_LASTTIME || status == DOWNLOAD_UNFINISH)) {
            count++;
        }
        ++it;
    }

    return count;
}

//添加任务同步到数据库
bool HistoryDownload::postAsyncAddTask(int taskId, const DownloadHistoryInfo& info)
{
    SyncAddTask<DownloadHistoryInfo>* syncData = new SyncAddTask<DownloadHistoryInfo>;
    syncData->table = TABLE_DOWNLOAD;
    syncData->type = SYNC_ADD_TASK;
    syncData->taskId = taskId;
    syncData->info = new DownloadHistoryInfo;
    *(syncData->info) = info;
    return HistoryDbMgr::getInstance()->asyncTask(syncData, sizeof(SyncAddTask<DownloadHistoryInfo>*));
}
//修改任务状态同步到数据库
bool HistoryDownload::postAsyncUpdateTaskStatus(int taskId, int status, int checking, const std::string& completeTime, const std::string& downloadList)
{
    SyncUpdateTaskStatus* syncData = new SyncUpdateTaskStatus;
    syncData->table = TABLE_DOWNLOAD;
    syncData->type = SYNC_UPDATE_TASK_STATUS;
    syncData->taskId = taskId;
    syncData->status = status;
    syncData->nChecking = checking;
    syncData->completeTime = completeTime;
    syncData->downloadList = downloadList;
    return HistoryDbMgr::getInstance()->asyncTask(syncData, sizeof(SyncUpdateTaskStatus*));
}
//修改任务优先级同步到数据库
bool HistoryDownload::postAsyncUpdateTaskPriority(int taskId, int priority, int priorityTime, int prorityType)
{
    SyncUpdateTaskPriority* syncData = new SyncUpdateTaskPriority;
    syncData->table = TABLE_DOWNLOAD;
    syncData->type = SYNC_UPDATE_TASK_PRIORITY;
    syncData->taskId = taskId;
    syncData->priority = priority;
    syncData->priorityTime = priorityTime;
    syncData->customPriority = prorityType;
    return HistoryDbMgr::getInstance()->asyncTask(syncData, sizeof(SyncUpdateTaskPriority*));
}
//修改任务场景名同步到数据库
bool HistoryDownload::postAsyncUpdateTaskScene(int taskId, const std::string &strScene)
{
    SyncUpdateTaskScene* syncData = new SyncUpdateTaskScene;
    syncData->table = TABLE_DOWNLOAD;
    syncData->type = SYNC_UPDATE_TASK_SCENE;
    syncData->taskId = taskId;
    syncData->strScene = strScene;
    return HistoryDbMgr::getInstance()->asyncTask(syncData, sizeof(SyncUpdateTaskPriority*));
}
//删除任务同步到数据库
bool HistoryDownload::postAsyncDelTask(int taskId)
{
    SyncDelTask* syncData = new SyncDelTask;
    syncData->table = TABLE_DOWNLOAD;
    syncData->type = SYNC_DEL_TASK;
    syncData->taskId = taskId;
    return HistoryDbMgr::getInstance()->asyncTask(syncData, sizeof(SyncDelTask*));
}
// 更新父任务ID和别名
bool HistoryDownload::postAsyncUpdateParentInfo(int taskId, int parentId, const std::string &parentAlias)
{
    SyncUpdateTaskParentInfo* syncData = new SyncUpdateTaskParentInfo;
    syncData->table = TABLE_DOWNLOAD;
    syncData->type = SYNC_UPDATE_TASK_PARENT_INFO;
    syncData->taskId = taskId;
    syncData->tParentId = parentId;
    syncData->strParentAlias = parentAlias;
    return HistoryDbMgr::getInstance()->asyncTask(syncData, sizeof(SyncDelTask*));
}
// 更新下载路径（修复旧版本问题）
bool HistoryDownload::postAsyncUpdateTaskSavePath(int taskId, const std::string &path)
{
    SyncUpdateTaskSavePath* syncData = new SyncUpdateTaskSavePath;
    syncData->table = TABLE_DOWNLOAD;
    syncData->type = SYNC_UPDATE_TASK_SAVE_PATH;
    syncData->taskId = taskId;
    syncData->savePath = path;
    return HistoryDbMgr::getInstance()->asyncTask(syncData, sizeof(SyncDelTask*));
}
bool HistoryDownload::postAsyncTaskOutputSize(int taskId, const std::string &outputsize)
{
    SyncUpdateTaskOutputSize* syncData = new SyncUpdateTaskOutputSize;
    syncData->table = TABLE_DOWNLOAD;
    syncData->type = SYNC_UPDATE_TASK_OUTPUT_SIZE;
    syncData->taskId = taskId;
    syncData->outputSize = outputsize;
    return HistoryDbMgr::getInstance()->asyncTask(syncData, sizeof(SyncDelTask*));
}

bool HistoryDownload::onAddTask(const SQLiteConnection& conn, int taskId, const SyncData* data)
{
    const SyncAddTask<DownloadHistoryInfo>* syncData = dynamic_cast<const SyncAddTask<DownloadHistoryInfo>*>(data);
    DownloadHistoryInfo info = *(syncData->info);

    try {
        SQLiteCommand cmd(conn);

        SQLiteTransaction trans = conn.beginTransaction();
        char sql[1024] = { 0 };
        sprintf(sql, "insert into t_download(project,taskid,name,artist,saveas_path,save_path,status,times,start_time,stop_time,checking,priority,priority_time,\
                layername,parentid,storageid,output_label,task_alias,task_parent_alias,output_size,priority_type,operate_type,download_job_type,\
                task_user_id,output_types) values('%s',%d,'%s','%s','%s','%s',%d,%d,'%s','%s',%d,%d,%d,'%s',%d,%d,'%s','%s','%s','%s',%d,%d,%d,%d,'%s')",
            info.project.c_str(),
            info.taskId,
            info.sceneName.c_str(),
            info.artist.c_str(),
            info.saveAsPath.c_str(),
            info.savePath.c_str(),
            info.status,
            0,
            info.startTime.c_str(),
            info.stopTime.c_str(),
            info.ncheckingstatus,
            info.priority,
            info.priorityTime,
            info.layerName.c_str(),
            info.parentId,
            info.storageId,
            info.outputLabel.c_str(),
            info.taskAlias.c_str(),
            info.parentTaskAlias.c_str(),
            info.outputSize.c_str(),
            info.priorityType,
            info.operateType,
            info.downloadJobType,
            info.taskUserId,
            info.downloadType.c_str());
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

bool HistoryDownload::onRemoveTask(const SQLiteConnection& conn, int taskId, const SyncData* data)
{
    try {
        SQLiteTransaction trans = conn.beginTransaction();
        SQLiteCommand cmd(conn);

        char sql[4096] = { 0 };
        sprintf(sql, "delete from t_download where taskid=%d", taskId);

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

bool HistoryDownload::onUpdateTaskStatus(const SQLiteConnection& conn, int taskId, const SyncData* data)
{
    const SyncUpdateTaskStatus* syncData = dynamic_cast<const SyncUpdateTaskStatus*>(data);
    int status = syncData->status;
    int checking = syncData->nChecking;
    std::string completeTime = syncData->completeTime;
    std::string downloadList = syncData->downloadList;
    try {
        SQLiteTransaction trans = conn.beginTransaction();
        SQLiteCommand cmd(conn);

        char sql[4096] = { 0 };
        // sprintf(sql, "update t_download set status=?, checking=?, stop_time=datetime('now', 'localtime') where taskid=?");
        sprintf(sql, "update t_download set status=?, checking=?, stop_time=?,  output_types=? where taskid=?");

        cmd.setCommandText(sql);

        SQLiteParameter param1 = cmd.createParameter();
        param1.set(1, status);
        SQLiteParameter param2 = cmd.createParameter();
        param2.set(2, checking);
        SQLiteParameter param3 = cmd.createParameter();
        param3.set(3, completeTime);
        SQLiteParameter param4 = cmd.createParameter();
        param4.set(4, downloadList);
        SQLiteParameter param5 = cmd.createParameter();
        param5.set(5, taskId);

        cmd.executeNonQuery();

        trans.commit();

    }
    catch (SQLiteException& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    return true;
}

bool HistoryDownload::onUpdateTaskPriority(const SQLiteConnection& conn, int taskId, const SyncData* data)
{
    const SyncUpdateTaskPriority* syncData = dynamic_cast<const SyncUpdateTaskPriority*>(data);
    int priority = syncData->priority;
    int priorityTime = syncData->priorityTime;
    int customPriority = syncData->customPriority;

    try {
        SQLiteTransaction trans = conn.beginTransaction();
        SQLiteCommand cmd(conn);

        char sql[4096] = { 0 };
        sprintf(sql, "update t_download set priority=?,priority_time=?,priority_type=? where taskid=?");

        cmd.setCommandText(sql);

        SQLiteParameter param1 = cmd.createParameter();
        param1.set(1, priority);
        SQLiteParameter param2 = cmd.createParameter();
        param2.set(2, priorityTime);
        SQLiteParameter param3 = cmd.createParameter();
        param3.set(3, customPriority);
        SQLiteParameter param4 = cmd.createParameter();
        param4.set(4, taskId);

        cmd.executeNonQuery();

        trans.commit();

    }
    catch (SQLiteException& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    return true;
}
bool HistoryDownload::onUpdateTaskScene(const SQLiteConnection& conn, int taskId, const SyncData* data)
{
    const SyncUpdateTaskScene* syncData = dynamic_cast<const SyncUpdateTaskScene*>(data);
    std::string strScene = syncData->strScene;

    try {
        SQLiteTransaction trans = conn.beginTransaction();
        SQLiteCommand cmd(conn);

        char sql[4096] = { 0 };
        sprintf(sql, "update t_download set name=? where taskid=?");

        cmd.setCommandText(sql);

        SQLiteParameter param1 = cmd.createParameter();
        param1.set(1, strScene);
        SQLiteParameter param2 = cmd.createParameter();
        param2.set(2, taskId);

        cmd.executeNonQuery();

        trans.commit();

    }
    catch (SQLiteException& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    return true;
}

bool HistoryDownload::onUpdateTaskParentInfo(const SQLiteConnection& conn, int taskId, const SyncData* data)
{
    const SyncUpdateTaskParentInfo* syncData = dynamic_cast<const SyncUpdateTaskParentInfo*>(data);

    try {
        SQLiteTransaction trans = conn.beginTransaction();
        SQLiteCommand cmd(conn);

        char sql[4096] = { 0 };
        sprintf(sql, "update t_download set parentid=?, task_parent_alias=? where taskid=?");

        cmd.setCommandText(sql);

        SQLiteParameter param1 = cmd.createParameter();
        param1.set(1, syncData->tParentId);
        SQLiteParameter param2 = cmd.createParameter();
        param2.set(2, syncData->strParentAlias);
        SQLiteParameter param3 = cmd.createParameter();
        param3.set(3, taskId);

        cmd.executeNonQuery();

        trans.commit();

    }
    catch (SQLiteException& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    return true;
}

bool HistoryDownload::readTasklist(const SQLiteConnection& connection)
{
    try {
        SQLiteCommand cmd(connection);

        char sql[512] = { 0 };
#if 1
        int days = LocalSetting::getInstance()->getDownloadShowInDay();
        if (days == 0)
            snprintf(sql, 512, "select * from t_download");
        else
            snprintf(sql, 512, "select * from t_download where start_time >= date('now','-%d day')", days);
#else
        snprintf(sql, 512, "select * from t_download");
#endif

        cmd.setCommandText(sql);
        SQLiteDataReader dr = cmd.executeReader();

        while (dr.read()) {
            DownloadHistoryInfo taskInfo;
            for (int i = 0; i < dr.getFieldCount(); i++) {
                if (!dr.isDbNull(i)) {
                    switch (i) {
                    case 0:
                        taskInfo.index = dr.getInt32(i);
                        break;
                    case 1:
                        taskInfo.project = dr.getString(i);
                        break;
                    case 2:
                        taskInfo.taskId = dr.getInt64(i);
                        break;
                    case 3:
                        taskInfo.sceneName = dr.getString(i);
                        break;
                    case 4:
                        taskInfo.artist = dr.getString(i);
                        break;
                    case 5:
                        taskInfo.saveAsPath = dr.getString(i);
                        break;
                    case 6:
                        taskInfo.savePath = dr.getString(i);
                        break;
                    case 7:
                        taskInfo.status = dr.getInt32(i);
                        break;
                    case 8:
                        taskInfo.times = dr.getInt32(i);
                        break;
                    case 9:
                        taskInfo.startTime = dr.getString(i);
                        break;
                    case 10:
                        taskInfo.stopTime = dr.getString(i);
                        break;
                    case 11:
                        taskInfo.ncheckingstatus = dr.getInt32(i);
                        break;
                    case 12:
                        taskInfo.priority = dr.getInt32(i);
                        break;
                    case 13:
                        taskInfo.priorityTime = dr.getInt32(i);
                        break;
                    case 14:
                        taskInfo.layerName = dr.getString(i);
                        break;
                    case 15:
                        taskInfo.parentId = dr.getInt64(i);
                        break;
                    case 16:
                        taskInfo.storageId = dr.getInt32(i);
                        break;
                    case 17:
                        taskInfo.outputLabel = dr.getString(i);
                        break;
                    case 18:
                        taskInfo.taskAlias = dr.getString(i);
                        break;
                    case 19:
                        taskInfo.parentTaskAlias = dr.getString(i);
                        break;
                    case 20:
                        taskInfo.outputSize = dr.getString(i);
                        break;
                    case 21:
                        taskInfo.priorityType = dr.getInt32(i);
                        break;
                    case 22:
                        taskInfo.operateType = dr.getInt32(i);
                        break;
                    case 23:
                        taskInfo.downloadJobType = dr.getInt32(i);
                        break;
                    case 24:
                        taskInfo.taskUserId = dr.getInt64(i);
                        break;
                    case 25:
                        taskInfo.downloadType = dr.getString(i);
                        break;
                    }
                }
            }
            m_dhi[taskInfo.taskId] = taskInfo;
        }
    }
    catch (SQLiteException& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
    return true;
}

bool HistoryDownload::onUpdateTaskSavePath(const SQLiteConnection& conn, int taskId, const SyncData* data)
{
    const SyncUpdateTaskSavePath* syncData = dynamic_cast<const SyncUpdateTaskSavePath*>(data);
    std::string path = syncData->savePath;

    try {
        SQLiteTransaction trans = conn.beginTransaction();
        SQLiteCommand cmd(conn);

        char sql[4096] = { 0 };
        sprintf(sql, "update t_download set save_path=? where taskid=?");

        cmd.setCommandText(sql);

        SQLiteParameter param1 = cmd.createParameter();
        param1.set(1, path);
        SQLiteParameter param2 = cmd.createParameter();
        param2.set(2, taskId);

        cmd.executeNonQuery();

        trans.commit();

    }
    catch (SQLiteException& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    return true;
}

bool HistoryDownload::onUpdateTaskOutputSize(const SQLiteConnection& conn, int taskId, const SyncData* data)
{
    const SyncUpdateTaskOutputSize* syncData = dynamic_cast<const SyncUpdateTaskOutputSize*>(data);
    std::string outputSize = syncData->outputSize;

    try {
        SQLiteTransaction trans = conn.beginTransaction();
        SQLiteCommand cmd(conn);

        char sql[4096] = { 0 };
        sprintf(sql, "update t_download set output_size=? where taskid=?");

        cmd.setCommandText(sql);

        SQLiteParameter param1 = cmd.createParameter();
        param1.set(1, outputSize);
        SQLiteParameter param2 = cmd.createParameter();
        param2.set(2, taskId);

        cmd.executeNonQuery();

        trans.commit();

    }
    catch (SQLiteException& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    return true;
}

void HistoryDownload::getUnstoppedTaskList(std::map<int, DownloadHistoryInfo>& list, int filterDate)
{
    std::map<int, DownloadHistoryInfo>::iterator it = m_dhi.begin();
    while (it != m_dhi.end()) {
        int status = it->second.status;
        std::string startTime = it->second.startTime;
        if (status != DOWNLOAD_STOP && isDateContains(startTime, filterDate)) {
            list[it->first] = it->second;
        }
        ++it;
    }
}

void HistoryDownload::getUnfinishedTaskList(std::list<int>& list, int filterDate)
{
    std::map<int, DownloadHistoryInfo>::iterator it = m_dhi.begin();
    while (it != m_dhi.end()) {
        int status = it->second.status;
        std::string startTime = it->second.startTime;
        if (status != DOWNLOAD_FINISHED && isDateContains(startTime, filterDate)) {
            list.push_back(it->first);
        }
        ++it;
    }
}

void HistoryDownload::getTaskListIds(std::list<int>& list, int filterDate)
{
    std::map<int, DownloadHistoryInfo>::iterator it = m_dhi.begin();
    while (it != m_dhi.end()) {
        std::string startTime = it->second.startTime;
        if (isDateContains(startTime, filterDate)) {
            list.push_back(it->first);
        }
        ++it;
    }
}

bool HistoryDownload::updateDownloadTaskSavePath(int taskId, const std::string &path)
{
    std::map<int, DownloadHistoryInfo>::iterator it = m_dhi.find(taskId);
    if (it == m_dhi.end()) {
        return false;
    }

    it->second.savePath = path;
    return postAsyncUpdateTaskSavePath(taskId, path);
}

bool HistoryDownload::isDateContains(const std::string& time, int filterDate)
{
    if (filterDate == HistoryDownload::downloaded_all)
        return true;

    QDateTime current = MyConfig.currentDateTime; // 当前时间
    QDateTime downtime = QDateTime::fromString(QString::fromStdString(time), "yyyy-MM-dd hh:mm:ss"); // 下载时间
    int days = downtime.daysTo(current); // 相差天数
    return days <= filterDate;
}

bool HistoryDownload::checkColumnExist(const SQLiteConnection& conn, const std::string& colName)
{
    try {
        SQLiteCommand cmd(conn);

        std::string sql = "SELECT * FROM t_download limit 0,1";
        cmd.setCommandText(sql);
        SQLiteDataReader dr = cmd.executeReader();
        for (int i = 0; i < dr.getFieldCount(); i++) {
            std::string col = dr.getFieldName(i);
            if (col == colName) {
                return true;
            }
        }

    }
    catch (SQLiteException& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
    return false;
}

bool HistoryDownload::addNewStrColumn(const SQLiteConnection& conn, const std::string& colName, int strLen)
{
    try {
        SQLiteTransaction trans = conn.beginTransaction();
        SQLiteCommand cmd(conn);
        char sql[2048] = { 0 };
        sprintf(sql, "ALTER TABLE t_download ADD COLUMN '%s' VARCHAR(%d) default NULL", colName.c_str(), strLen);
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

bool HistoryDownload::updateTableStruct(const SQLiteConnection& conn)
{
    bool isUpdateSuccess = true;
    if (!checkColumnExist(conn, "output_size")) {
        isUpdateSuccess = this->addNewStrColumn(conn, "output_size", 64);
    }
    if (!checkColumnExist(conn, "priority_type")) {
        isUpdateSuccess = this->addNewIntColumn(conn, "priority_type");
    }
    if (!checkColumnExist(conn, "operate_type")) {
        isUpdateSuccess = this->addNewIntColumn(conn, "operate_type");
    }
    if (!checkColumnExist(conn, "download_job_type")) {
        isUpdateSuccess = this->addNewIntColumn(conn, "download_job_type");
    }
    if (!checkColumnExist(conn, "task_user_id")) {
        isUpdateSuccess = this->addNewIntColumn(conn, "task_user_id");
    }
    if (!checkColumnExist(conn, "output_types")) {
        isUpdateSuccess = this->addNewStrColumn(conn, "output_types", 256);
    }

    return isUpdateSuccess;
}

bool HistoryDownload::addNewIntColumn(const SQLiteConnection& conn, const std::string& colName)
{
    try {
        SQLiteTransaction trans = conn.beginTransaction();
        SQLiteCommand cmd(conn);
        char sql[2048] = { 0 };
        sprintf(sql, "ALTER TABLE t_download ADD COLUMN '%s' INT DEFAULT 0", colName.c_str());
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