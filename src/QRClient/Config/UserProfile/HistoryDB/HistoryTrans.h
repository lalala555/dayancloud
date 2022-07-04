#pragma once
#ifndef HISTORYTRANS_H
#define HISTORYTRANS_H
#include "HistoryDbBase.h"

class HistoryTrans : public CHistoryDbBase
{
public:
    HistoryTrans(void);
    ~HistoryTrans(void);

    // 公共部分
public:
    bool addTask(int taskid, const TransHistoryInfo& taskinfo);
    bool removeTask(int taskid);
    bool updateTask(int taskid, const TransHistoryInfo& taskinfo);
    bool updateTaskTransState(int taskid, int state);   // 更新任务的状态（传输状态）
    int  getUnfinishedCount(); //获取未完成的数量
    bool addOldTask(const SQLiteConnection& connection, const TransHistoryInfo& taskinfo);
    bool readOldData(const SQLiteConnection& connection);
    bool saveOldData(const SQLiteConnection& connection);
    bool readTasklist(const SQLiteConnection& connection);

    void getAllTaskList(std::map<int, TransHistoryInfo>& alltask);
    bool findTask(int taskid);
    TransHistoryInfo findTaskInfo(int taskid);

    // 上传部分
public:
    int  getUploadUnfinishedCount(std::map<int, TransHistoryInfo>& unuploadtasks); //获取上传未完成的数量
    bool updateTaskScenePath(int taskid, const std::string& path);
    void getUploadTaskList(std::map<int, TransHistoryInfo>& uploadtasks);

    // 下载部分
public:
    int  getDownloadUnfinishedCount(std::map<int, TransHistoryInfo>& undownloadtasks); //获取下载未完成的数量
    bool updateTaskDownloadPath(int taskid, const std::string& downpath);
    void getDownloadTaskList(std::map<int, TransHistoryInfo>& downloadtasks);
    int  getDownloadTaskStatus(int taskId);
    void getDownloadUnstoppedTaskList(std::map<int, TransHistoryInfo>& list);
    // 更新表结构
    bool updateTableStruct(const SQLiteConnection& conn);
    bool checkColumnExist(const SQLiteConnection& conn, const std::string& colName);
    bool addNewColumnInt(const SQLiteConnection& conn, const std::string& colName, int defaultValue);
    bool addNewColumnString(const SQLiteConnection& conn, const std::string& colName);
public:
    //添加任务同步到数据库
    bool postAsyncAddTask(int taskId,const TransHistoryInfo& info);
    //修改任务同步到数据库
    bool postAsyncUpdateTask(int taskId, const TransHistoryInfo& info);
    //删除任务同步到数据库
    bool postAsyncDelTask(int taskId);
    //修改传输状态 同步到数据库
    bool postAsyncUpdateTaskTransState(int taskId, int state);
    //修改任务的本地场景路径
    bool postAsyncUpdateLocalScenePath(int taskId, const std::string& localpath);
    //修改任务的场景下载路径
    bool postAsyncUpdateDownloadPath(int taskId, const std::string& downloadPath);
    //修改另存为路径
    bool postAsyncUpdateLocalSavePath(int taskId, const std::string& localSavePath);
public:
    bool onAddTask(const SQLiteConnection& conn, int taskId, const SyncData* data);
    bool onRemoveTask(const SQLiteConnection& conn, int taskId, const SyncData* data);
    bool onUpdateTask(const SQLiteConnection& conn, int taskId, const SyncData* data);
    bool onUpdateTaskTransState(const SQLiteConnection& conn, int taskId, const SyncData* data);
    bool onUpdateTaskScenePath(const SQLiteConnection& conn, int taskId, const SyncData* data);
    bool onUpdateTaskDownloadPath(const SQLiteConnection& conn, int taskId, const SyncData* data);
    bool onUpdateTaskLoaclSavePath(const SQLiteConnection& conn, int taskId, const SyncData* data);

private:
    CLock m_lock;
    std::map<int, TransHistoryInfo> m_translist;
};



#endif // _HISTORYTRANS_H