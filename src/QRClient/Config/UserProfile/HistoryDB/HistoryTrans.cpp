#include "stdafx.h"
#include "HistoryTrans.h"


HistoryTrans::HistoryTrans(void)
{
}


HistoryTrans::~HistoryTrans(void)
{
}

bool HistoryTrans::addTask(int taskid, const TransHistoryInfo& taskinfo)
{
    CGuard guard(&m_lock);
    std::map<int, TransHistoryInfo>::iterator it = m_translist.find(taskid);

    if(it != m_translist.end()) {
        m_translist[taskid] = taskinfo;
        return postAsyncUpdateTask(taskid, taskinfo);
    }
    m_translist[taskid] = taskinfo;
    return postAsyncAddTask(taskid, taskinfo);
}

bool HistoryTrans::removeTask(int taskid)
{
    CGuard guard(&m_lock);
    std::map<int, TransHistoryInfo>::iterator it = m_translist.find(taskid);
    if(it == m_translist.end()) {
        return false;
    }

    m_translist.erase(it);

    return postAsyncDelTask(taskid);
}

bool HistoryTrans::updateTask(int taskid, const TransHistoryInfo& taskinfo)
{
    CGuard guard(&m_lock);
    TransHistoryInfo info = findTaskInfo(taskid);
    if(info.taskId == -1) {
        return false;
    }

    info.jobType = taskinfo.jobType;
    info.transState = taskinfo.transState;
    info.sceneName = taskinfo.sceneName;
    info.scenePath = taskinfo.scenePath;
    info.downloadPath = taskinfo.downloadPath;
    info.loaclSavePath = taskinfo.loaclSavePath;
    info.artist = taskinfo.artist;
    info.camera = taskinfo.artist;
    info.vipTask = taskinfo.vipTask;
    info.autoDownload = taskinfo.autoDownload;
    info.startTime = taskinfo.startTime;
    info.endTime = taskinfo.endTime;
    info.taskAlias = taskinfo.taskAlias;
    info.download_mode = taskinfo.download_mode;
    info.unicode_download = taskinfo.unicode_download;

    std::map<int, TransHistoryInfo>::iterator it = m_translist.find(taskid);
    if(it == m_translist.end()) {
        return false;
    }
    it->second = info;

    return postAsyncUpdateTask(taskid, info);
}

// 更新任务的状态（传输状态）
bool HistoryTrans::updateTaskTransState(int taskid, int state)
{
    CGuard guard(&m_lock);
    std::map<int, TransHistoryInfo>::iterator it = m_translist.find(taskid);
    if(it == m_translist.end()) {
        return false;
    }

    it->second.transState = state;

    return postAsyncUpdateTaskTransState(taskid, state);
}

int  HistoryTrans::getUnfinishedCount() //获取未完成的数量
{
    CGuard guard(&m_lock);
    int count = 0;
    std::map<int, TransHistoryInfo>::iterator it = m_translist.begin();
    while(it != m_translist.end()) {
        int status = it->second.transState;
        if(status == UPLOAD_UNFINISH || status == enDownloadUnfinish 
               || status == enUploadUnfinished) {
            count++;
        }
        ++it;
    }

    return count;
}

bool HistoryTrans::addOldTask(const SQLiteConnection& connection, const TransHistoryInfo& taskinfo)
{
    try {
        SQLiteCommand cmd(connection);
        SQLiteTransaction trans = connection.beginTransaction();
        char sql[1024] = {0};
        sprintf_s(sql, sizeof(sql), "insert into trans_history(taskid,tasktype,transstate,viptask,autodownload,scenename,\
            scenepath,downloadpath,localsavepath,camera,artist,starttime,endtime,taskalias)values(%d,%d,%d,%d,%d,'%s','%s',\
            '%s','%s','%s','%s','%s','%s','%s')",
                  taskinfo.taskId,
                  taskinfo.jobType,
                  taskinfo.transState,
                  taskinfo.vipTask,
                  taskinfo.autoDownload,
                  taskinfo.sceneName.c_str(),
                  taskinfo.scenePath.c_str(),
                  taskinfo.downloadPath.c_str(),
                  taskinfo.loaclSavePath.c_str(),
                  taskinfo.camera.c_str(),
                  taskinfo.artist.c_str(),
                  taskinfo.startTime.c_str(),
                  taskinfo.endTime.c_str(),
                  taskinfo.taskAlias.c_str());
        cmd.setCommandText(sql);
        cmd.executeNonQuery();
        trans.commit();

    } catch(SQLiteException& e) {
        return false;
    }

    return true;
}

bool HistoryTrans::readOldData(const SQLiteConnection& connection)
{
    try {
        SQLiteCommand cmd(connection);
        char sql[512] = { 0 };
        sprintf_s(sql, 512, "select * from trans_history");
        cmd.setCommandText(sql);
        SQLiteDataReader dr3 = cmd.executeReader();

        while(dr3.read()) {
            TransHistoryInfo taskInfo2;
            for(int i = 0; i < dr3.getFieldCount(); i++) {
                if(!dr3.isDbNull(i)) {
                    switch(i) {
                    case 0:
                        taskInfo2.index = dr3.getInt32(i);
                        break;
                    case 1:
                        taskInfo2.taskId = dr3.getInt32(i);
                        break;
                    case 2:
                        taskInfo2.jobType = dr3.getInt32(i);
                        break;
                    case 3:
                        taskInfo2.transState = dr3.getInt32(i);
                        break;
                    case 4:
                        taskInfo2.vipTask = dr3.getInt32(i) > 0 ? true : false;
                        break;
                    case 5:
                        taskInfo2.autoDownload = dr3.getInt32(i) > 0 ? true : false;
                        break;
                    case 6:
                        taskInfo2.sceneName = dr3.getString(i);
                        break;
                    case 7:
                        taskInfo2.scenePath = dr3.getString(i);
                        break;
                    case 8:
                        taskInfo2.downloadPath = dr3.getString(i);
                        break;
                    case 9:
                        taskInfo2.loaclSavePath = dr3.getString(i);
                        break;
                    case 10:
                        taskInfo2.camera = dr3.getString(i);
                        break;
                    case 11:
                        taskInfo2.artist = dr3.getString(i);
                        break;
                    case 12:
                        taskInfo2.startTime = dr3.getString(i);
                        break;
                    case 13:
                        taskInfo2.endTime = dr3.getString(i);
                        break;
                    case 14:
                        taskInfo2.taskAlias = dr3.getString(i);
                        break;
                    }
                }
            }
            m_translist[taskInfo2.taskId] = taskInfo2;
        }
    } catch(SQLiteException& e) {
        return false;
    }

    return true;
}

bool HistoryTrans::saveOldData(const SQLiteConnection& connection)
{
    std::map<int, TransHistoryInfo>::iterator dit = m_translist.begin();
    for(dit; dit != m_translist.end(); ++dit) {
        addOldTask(connection, dit->second);
    }

    return true;
}

bool HistoryTrans::readTasklist(const SQLiteConnection& connection)
{
    try {
        SQLiteCommand cmd(connection);

        char sql[512] = {0};
        sprintf_s(sql, 512, "select * from trans_history");
        cmd.setCommandText(sql);
        SQLiteDataReader dr3 = cmd.executeReader();

        while(dr3.read()) {
            TransHistoryInfo taskInfo2;
            for(int i = 0; i < dr3.getFieldCount(); i++) {
                if(!dr3.isDbNull(i)) {
                    switch(i) {
                    case 0:
                        taskInfo2.index = dr3.getInt32(i);
                        break;
                    case 1:
                        taskInfo2.taskId = dr3.getInt32(i);
                        break;
                    case 2:
                        taskInfo2.jobType = dr3.getInt32(i);
                        break;
                    case 3:
                        taskInfo2.transState = dr3.getInt32(i);
                        break;
                    case 4:
                        taskInfo2.vipTask = dr3.getInt32(i) > 0 ? true : false;
                        break;
                    case 5:
                        taskInfo2.autoDownload = dr3.getInt32(i) > 0 ? true : false;
                        break;
                    case 6:
                        taskInfo2.sceneName = dr3.getString(i);
                        break;
                    case 7:
                        taskInfo2.scenePath = dr3.getString(i);
                        break;
                    case 8:
                        taskInfo2.downloadPath = dr3.getString(i);
                        break;
                    case 9:
                        taskInfo2.loaclSavePath = dr3.getString(i);
                        break;
                    case 10:
                        taskInfo2.camera = dr3.getString(i);
                        break;
                    case 11:
                        taskInfo2.artist = dr3.getString(i);
                        break;
                    case 12:
                        taskInfo2.startTime = dr3.getString(i);
                        break;
                    case 13:
                        taskInfo2.endTime = dr3.getString(i);
                        break;
                    case 14:
                        taskInfo2.taskAlias = dr3.getString(i);
                        break;
                    case 15:
                        taskInfo2.download_mode = dr3.getInt32(i);
                        break;
                    case 16:
                        taskInfo2.unicode_download = dr3.getInt32(i);
                        break;
                    case 17:
                        taskInfo2.coordCount = dr3.getInt32(i);
                        break;
                    case 18:
                        taskInfo2.pictureCount = dr3.getInt32(i);
                        break;
                    }
                }
            }
            m_translist[taskInfo2.taskId] = taskInfo2;
        }
    } catch(SQLiteException& e) {
        return false;
    }
    return true;
}

void HistoryTrans::getAllTaskList(std::map<int, TransHistoryInfo>& alltask)
{
    alltask = m_translist;
}

bool HistoryTrans::findTask(int taskid)
{
    CGuard guard(&m_lock);
    auto it = m_translist.find(taskid);
    if(it != m_translist.end()) {
        return true;
    }
    return false;
}

TransHistoryInfo HistoryTrans::findTaskInfo(int taskid)
{
    CGuard guard(&m_lock);
    TransHistoryInfo info;
    auto it = m_translist.find(taskid);
    if(it != m_translist.end()) {
        return it->second;
    }
    return info;
}

int  HistoryTrans::getUploadUnfinishedCount(std::map<int, TransHistoryInfo>& unuploadtasks) //获取上传未完成的数量
{
    CGuard guard(&m_lock);
    int count = 0;
    std::map<int, TransHistoryInfo> list;
    std::map<int, TransHistoryInfo>::iterator it = m_translist.begin();

    while(it != m_translist.end()) {
        int status = it->second.transState;
        int jobType = it->second.jobType;

        if(jobType == UPLOAD_JOB && (status != enUploadCompleted || status != enUploadCheckCompleted)) {
            count++;
            list.insert(std::make_pair(it->first, it->second));
        }
        ++it;
    }
    unuploadtasks = list;
    return count;
}

bool HistoryTrans::updateTaskScenePath(int taskid, const std::string& path)
{
    CGuard guard(&m_lock);
    std::map<int, TransHistoryInfo>::iterator it = m_translist.find(taskid);
    if(it == m_translist.end()) {
        return false;
    }

    it->second.scenePath = path;

    return postAsyncUpdateLocalScenePath(taskid, path);
}

void HistoryTrans::getUploadTaskList(std::map<int, TransHistoryInfo>& uploadtasks)
{
    CGuard guard(&m_lock);
    std::map<int, TransHistoryInfo> list;
    std::map<int, TransHistoryInfo>::iterator it = m_translist.begin();
    while(it != m_translist.end()) {
        int status = it->second.transState;
        int jobType = it->second.jobType;
        if(jobType == UPLOAD_JOB) {
            list.insert(std::make_pair(it->first, it->second));
        }
        ++it;
    }
    uploadtasks = list;
}

int  HistoryTrans::getDownloadUnfinishedCount(std::map<int, TransHistoryInfo>& undownloadtasks) //获取下载未完成的数量
{
    CGuard guard(&m_lock);
    int count = 0;
    std::map<int, TransHistoryInfo> list;
    std::map<int, TransHistoryInfo>::iterator it = m_translist.begin();
    while(it != m_translist.end()) {
        int status = it->second.transState;
        int jobType = it->second.jobType;
        if(status != enDownloadCompleted) {
            count++;
            list.insert(std::make_pair(it->first, it->second));
        }
        ++it;
    }
    undownloadtasks = list;
    return count;
}

bool HistoryTrans::updateTaskDownloadPath(int taskid, const std::string& downpath)
{
    CGuard guard(&m_lock);
    std::map<int, TransHistoryInfo>::iterator it = m_translist.find(taskid);
    if(it == m_translist.end()) {
        return false;
    }

    it->second.downloadPath = downpath;

    return postAsyncUpdateLocalScenePath(taskid, downpath);
}

void HistoryTrans::getDownloadTaskList(std::map<int, TransHistoryInfo>& downloadtasks)
{
    CGuard guard(&m_lock);
    std::map<int, TransHistoryInfo> list;
    std::map<int, TransHistoryInfo>::iterator it = m_translist.begin();
    while(it != m_translist.end()) {
        int status = it->second.transState;
        int jobType = it->second.jobType;
        if(jobType == DOWNLOAD_JOB || jobType == RENDER_JOB) {
            list.insert(std::make_pair(it->first, it->second));
        }
        ++it;
    }
    downloadtasks = list;
}

int  HistoryTrans::getDownloadTaskStatus(int taskId)
{
    CGuard guard(&m_lock);
    std::map<int, TransHistoryInfo> list;
    this->getDownloadTaskList(list);

    std::map<int, TransHistoryInfo>::const_iterator it = list.find(taskId);
    if(it != list.end()) {
        return it->second.transState;
    }

    return INVALID_STATUS;
}

void HistoryTrans::getDownloadUnstoppedTaskList(std::map<int, TransHistoryInfo>& list)
{
    CGuard guard(&m_lock);
    std::map<int, TransHistoryInfo> downloadList;
    std::map<int, TransHistoryInfo> unstopedList;
    this->getDownloadTaskList(downloadList);

    std::map<int, TransHistoryInfo>::iterator it = downloadList.begin();
    while(it != downloadList.end()) {
        int status = it->second.transState;
        if(status != enDownloadStop) {
            unstopedList[it->first] = it->second;
        }
        ++it;
    }
    list = unstopedList;
}

//添加任务同步到数据库
bool HistoryTrans::postAsyncAddTask(int taskId,const TransHistoryInfo& info)
{
    SyncAddTask<TransHistoryInfo>* syncData = new SyncAddTask<TransHistoryInfo>;
    syncData->table = TABLE_TRANS;
    syncData->type = SYNC_ADD_TASK;
    syncData->taskId = taskId;
    syncData->info = new TransHistoryInfo;
    *(syncData->info) = info;
    return HistoryDbMgr::getInstance()->asyncTask(syncData, sizeof(SyncAddTask<TransHistoryInfo>*));
}

//修改任务同步到数据库
bool HistoryTrans::postAsyncUpdateTask(int taskId, const TransHistoryInfo& info)
{
    SyncAddTask<TransHistoryInfo>* syncData = new SyncAddTask<TransHistoryInfo>;
    syncData->table = TABLE_TRANS;
    syncData->type = SYNC_UPDATE_TASK;
    syncData->taskId = taskId;
    syncData->info = new TransHistoryInfo;
    *(syncData->info) = info;
    return HistoryDbMgr::getInstance()->asyncTask(syncData, sizeof(SyncAddTask<TransHistoryInfo>*));
}

//删除任务同步到数据库
bool HistoryTrans::postAsyncDelTask(int taskId)
{
    SyncDelTask* syncData = new SyncDelTask;
    syncData->table = TABLE_TRANS;
    syncData->type = SYNC_DEL_TASK;
    syncData->taskId = taskId;
    return HistoryDbMgr::getInstance()->asyncTask(syncData, sizeof(SyncDelTask*));
}

//修改传输状态 同步到数据库
bool HistoryTrans::postAsyncUpdateTaskTransState(int taskId, int state)
{
    SyncUpdateTaskTransState* syncData = new SyncUpdateTaskTransState;
    syncData->table = TABLE_TRANS;
    syncData->type = SYNC_UPDATE_TASK_TRANS_STATE;
    syncData->taskId = taskId;
    syncData->transState = state;
    return HistoryDbMgr::getInstance()->asyncTask(syncData, sizeof(SyncUpdateTaskTransState*));
}

//修改任务的本地场景路径
bool HistoryTrans::postAsyncUpdateLocalScenePath(int taskId, const std::string& localpath)
{
    SyncUpdateTaskScenePath* syncData = new SyncUpdateTaskScenePath;
    syncData->table = TABLE_TRANS;
    syncData->type  = SYNC_UPDATE_TASK_SCENE_PATH;
    syncData->taskId = taskId;
    syncData->strScenePath = localpath;
    return HistoryDbMgr::getInstance()->asyncTask(syncData, sizeof(SyncUpdateTaskScenePath*));
}

//修改任务的场景下载路径
bool HistoryTrans::postAsyncUpdateDownloadPath(int taskId, const std::string& downloadPath)
{
    SyncUpdateTaskDownloadPath* syncData = new SyncUpdateTaskDownloadPath;
    syncData->table = TABLE_TRANS;
    syncData->type  = SYNC_UPDATE_TASK_DOWNLOAD_PATH;
    syncData->taskId = taskId;
    syncData->strDownloadPath = downloadPath;
    return HistoryDbMgr::getInstance()->asyncTask(syncData, sizeof(SyncUpdateTaskDownloadPath*));
}

bool HistoryTrans::onAddTask(const SQLiteConnection& conn, int taskId, const SyncData* data)
{
    const SyncAddTask<TransHistoryInfo>* syncData = dynamic_cast<const SyncAddTask<TransHistoryInfo>*>(data);
    TransHistoryInfo info = *(syncData->info);

    try {
        SQLiteCommand cmd(conn);

        SQLiteTransaction trans = conn.beginTransaction();
        char sql[1024] = {0};
        sprintf_s(sql, sizeof(sql), "insert into trans_history(taskid,tasktype,transstate,viptask,autodownload,scenename,\
              scenepath,downloadpath,localsavepath,camera,artist,starttime,endtime,taskalias,download_mode,unicode_download,\
              downloadtype,coord_count,picture_count)values(%d,%d,%d,%d,%d,'%s','%s','%s','%s','%s','%s','%s','%s','%s',%d,%d,'%s',%d,%d)",
                  info.taskId,
                  info.jobType,
                  info.transState,
                  info.vipTask,
                  info.autoDownload,
                  info.sceneName.c_str(),
                  info.scenePath.c_str(),
                  info.downloadPath.c_str(),
                  info.loaclSavePath.c_str(),
                  info.camera.c_str(),
                  info.artist.c_str(),
                  info.startTime.c_str(),
                  info.endTime.c_str(),
                  info.taskAlias.c_str(),
                  info.download_mode,
                  info.unicode_download,
                  info.downloadType.c_str(),
                  info.coordCount,
                  info.pictureCount);
        cmd.setCommandText(sql);
        cmd.executeNonQuery();
        trans.commit();

    } catch(SQLiteException& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
    return true;
}

bool HistoryTrans::onRemoveTask(const SQLiteConnection& conn, int taskId, const SyncData* data)
{
    try {
        SQLiteTransaction trans = conn.beginTransaction();
        SQLiteCommand cmd(conn);

        char sql[4096] = { 0 };
        sprintf(sql, "delete from trans_history where taskid=%d", taskId);

        cmd.setCommandText(sql);

        cmd.executeNonQuery();

        trans.commit();

    } catch(SQLiteException& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    return true;
}

bool HistoryTrans::onUpdateTask(const SQLiteConnection& conn, int taskId, const SyncData* data)
{
    const SyncAddTask<TransHistoryInfo>* syncData = dynamic_cast<const SyncAddTask<TransHistoryInfo>*>(data);

    try {
        SQLiteTransaction trans = conn.beginTransaction();
        SQLiteCommand cmd(conn);

        char sql[4096] = { 0 };
        sprintf(sql, "update trans_history set taskid=?,tasktype=?,transstate=?,viptask=?,autodownload=?,scenename=?,\
            scenepath=?,downloadpath=?,localsavepath=?,camera=?,artist=?,starttime=?,endtime=?,taskalias=?,download_mode=?,\
            unicode_download=?,downloadtype=?,coord_count=?,picture_count=? where taskid=?");

        cmd.setCommandText(sql);

        SQLiteParameter param1 = cmd.createParameter();
        param1.set(1, syncData->info->taskId);
        SQLiteParameter param2 = cmd.createParameter();
        param2.set(2, syncData->info->jobType);
        SQLiteParameter param3 = cmd.createParameter();
        param3.set(3, syncData->info->transState);
        SQLiteParameter param4 = cmd.createParameter();
        param4.set(4, syncData->info->vipTask);
        SQLiteParameter param5 = cmd.createParameter();
        param5.set(5, syncData->info->autoDownload);
        SQLiteParameter param6 = cmd.createParameter();
        param6.set(6, syncData->info->sceneName);
        SQLiteParameter param7 = cmd.createParameter();
        param7.set(7, syncData->info->scenePath);
        SQLiteParameter param8 = cmd.createParameter();
        param8.set(8, syncData->info->downloadPath);
        SQLiteParameter param9 = cmd.createParameter();
        param9.set(9, syncData->info->loaclSavePath);
        SQLiteParameter param10 = cmd.createParameter();
        param10.set(10, syncData->info->camera);
        SQLiteParameter param11 = cmd.createParameter();
        param11.set(11, syncData->info->artist);
        SQLiteParameter param12 = cmd.createParameter();
        param12.set(12, syncData->info->startTime);
        SQLiteParameter param13 = cmd.createParameter();
        param13.set(13, syncData->info->endTime);
        SQLiteParameter param14 = cmd.createParameter();
        param14.set(14, syncData->info->taskAlias);
        SQLiteParameter param15 = cmd.createParameter();
        param15.set(15, syncData->info->download_mode);
        SQLiteParameter param16 = cmd.createParameter();
        param16.set(16, syncData->info->unicode_download);
        SQLiteParameter param17 = cmd.createParameter();
        param17.set(17, syncData->info->downloadType);
        SQLiteParameter param18 = cmd.createParameter();
        param18.set(18, syncData->info->coordCount);
        SQLiteParameter param19 = cmd.createParameter();
        param19.set(19, syncData->info->pictureCount);
        SQLiteParameter param20 = cmd.createParameter();
        param20.set(20, taskId);


        cmd.executeNonQuery();

        trans.commit();

    } catch(SQLiteException& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    return true;
}

bool HistoryTrans::onUpdateTaskTransState(const SQLiteConnection& conn, int taskId, const SyncData* data)
{
    const SyncUpdateTaskTransState* syncData = dynamic_cast<const SyncUpdateTaskTransState* >(data);
    int transState = syncData->transState;

    try {
        SQLiteTransaction trans = conn.beginTransaction();
        SQLiteCommand cmd(conn);

        char sql[4096] = { 0 };
        sprintf(sql, "update trans_history set transstate=? where taskid=?");

        cmd.setCommandText(sql);

        SQLiteParameter param1 = cmd.createParameter();
        param1.set(1, transState);
        SQLiteParameter param2 = cmd.createParameter();
        param2.set(2, taskId);

        cmd.executeNonQuery();

        trans.commit();

    } catch(SQLiteException& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    return true;
}

bool HistoryTrans::onUpdateTaskScenePath(const SQLiteConnection& conn, int taskId, const SyncData* data)
{
    const SyncUpdateTaskScenePath* syncData = dynamic_cast<const SyncUpdateTaskScenePath* >(data);
    std::string strScenePath = syncData->strScenePath;

    try {
        SQLiteTransaction trans = conn.beginTransaction();
        SQLiteCommand cmd(conn);

        char sql[4096] = { 0 };
        sprintf(sql, "update trans_history set scenepath=? where taskid=?");

        cmd.setCommandText(sql);

        SQLiteParameter param1 = cmd.createParameter();
        param1.set(1, strScenePath);
        SQLiteParameter param2 = cmd.createParameter();
        param2.set(2, taskId);

        cmd.executeNonQuery();

        trans.commit();

    } catch(SQLiteException& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    return true;
}

bool HistoryTrans::onUpdateTaskDownloadPath(const SQLiteConnection& conn, int taskId, const SyncData* data)
{
    const SyncUpdateTaskDownloadPath* syncData = dynamic_cast<const SyncUpdateTaskDownloadPath* >(data);
    std::string strDownloadPath = syncData->strDownloadPath;

    try {
        SQLiteTransaction trans = conn.beginTransaction();
        SQLiteCommand cmd(conn);

        char sql[4096] = { 0 };
        sprintf(sql, "update trans_history set downloadpath=? where taskid=?");

        cmd.setCommandText(sql);

        SQLiteParameter param1 = cmd.createParameter();
        param1.set(1, strDownloadPath);
        SQLiteParameter param2 = cmd.createParameter();
        param2.set(2, taskId);

        cmd.executeNonQuery();

        trans.commit();

    } catch(SQLiteException& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    return true;
}

bool HistoryTrans::postAsyncUpdateLocalSavePath(int taskId, const std::string& localSavePath)
{
    SyncUpdateTaskLocalSavePath* syncData = new SyncUpdateTaskLocalSavePath;
    syncData->table = TABLE_TRANS;
    syncData->type  = SYNC_UPDATE_TASK_LOCALSAVE_PATH;
    syncData->taskId = taskId;
    syncData->strLocalSavePath = localSavePath;
    return HistoryDbMgr::getInstance()->asyncTask(syncData, sizeof(SyncUpdateTaskLocalSavePath*));
}

bool HistoryTrans::onUpdateTaskLoaclSavePath(const SQLiteConnection& conn, int taskId, const SyncData* data)
{
    const SyncUpdateTaskLocalSavePath* syncData = dynamic_cast<const SyncUpdateTaskLocalSavePath* >(data);
    std::string localpath = syncData->strLocalSavePath;

    try {
        SQLiteTransaction trans = conn.beginTransaction();
        SQLiteCommand cmd(conn);

        char sql[4096] = { 0 };
        sprintf(sql, "update trans_history set localsavepath=? where taskid=?");

        cmd.setCommandText(sql);

        SQLiteParameter param1 = cmd.createParameter();
        param1.set(1, localpath);
        SQLiteParameter param2 = cmd.createParameter();
        param2.set(2, taskId);

        cmd.executeNonQuery();

        trans.commit();

    } catch(SQLiteException& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    return true;
}

bool HistoryTrans::checkColumnExist(const SQLiteConnection& conn, const std::string& colName)
{
    try {
        SQLiteCommand cmd(conn);

        std::string sql = "SELECT * FROM trans_history limit 0,1";
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

bool HistoryTrans::addNewColumnInt(const SQLiteConnection& conn, const std::string& colName, int defaultValue)
{
    try {
        SQLiteTransaction trans = conn.beginTransaction();
        SQLiteCommand cmd(conn);
        char sql[2048] = { 0 };
        sprintf(sql, "ALTER TABLE trans_history ADD COLUMN '%s' INT DEFAULT %d", colName.c_str(), defaultValue);
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

bool HistoryTrans::addNewColumnString(const SQLiteConnection& conn, const std::string& colName)
{
    try {
        SQLiteTransaction trans = conn.beginTransaction();
        SQLiteCommand cmd(conn);
        char sql[2048] = { 0 };
        sprintf(sql, "ALTER TABLE trans_history ADD COLUMN '%s' VARCHAR(1024)", colName.c_str());
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

bool HistoryTrans::updateTableStruct(const SQLiteConnection& conn)
{
    bool isUpdateSuccess = true;
    if (!checkColumnExist(conn, "download_mode")) {
        isUpdateSuccess = this->addNewColumnInt(conn, "download_mode", 1);
    }
    if (!checkColumnExist(conn, "unicode_download")) {
        bool isOk = this->addNewColumnInt(conn, "unicode_download", 0);
        isUpdateSuccess = isUpdateSuccess && isOk;
    }
    if (!checkColumnExist(conn, "unicode_download")) {
        bool isOk = this->addNewColumnString(conn, "unicode_download");
        isUpdateSuccess = isUpdateSuccess && isOk;
    }
    if (!checkColumnExist(conn, "coord_count")) {
        bool isOk = this->addNewColumnInt(conn, "coord_count", 0);
        isUpdateSuccess = isUpdateSuccess && isOk;
    }
    if (!checkColumnExist(conn, "picture_count")) {
        bool isOk = this->addNewColumnInt(conn, "picture_count", 0);
        isUpdateSuccess = isUpdateSuccess && isOk;
    }
    return isUpdateSuccess;
}