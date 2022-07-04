/***********************************************************************
* Module:  HistoryDbMgr.h
* Author:  hqr
* Modified: 2016/07/03 17:43:21
* Purpose: Declaration of the class
***********************************************************************/
#ifndef HISTORYDB_MGR_H_
#define HISTORYDB_MGR_H_
#include <map>
#include "HistoryTrans.h"
#include "HistoryReportLog.h"
#include "HistoryDownload.h"

class HistoryDbMgr : public CSqliteDb, public IQueueSink<SyncData>
{
    HistoryDbMgr(void);
public:
    ~HistoryDbMgr(void);
    static HistoryDbMgr* getInstance()
    {
        static HistoryDbMgr instance;
        return &instance;
    }

    virtual void on_queue_data(SyncData* pData);
public:
    bool beepOn();
    void beepOff();
    bool asyncTask(void *data, int size);

    bool onAddTask(int taskId, const SyncData* data);
    bool onRemoveTask(int taskId, const SyncData* data);
    bool onUpdateTransTaskScenePath(int taskId, const SyncData* data);
    bool onUpdateTransTaskDownloadPath(int taskId, const SyncData* data);
    bool onUpdateTransTask(int taskId, const SyncData* data);
    bool onUpdateTransTaskState(int taskId, const SyncData* data);
    bool onUpdateTransTaskLocalSavePath(int taskId, const SyncData* data);
    bool onUpdateDownloadStatus(int taskId, const SyncData* data);

    // trans_histroy
    bool addTransTask(int taskId, const TransHistoryInfo& info);
    bool removeTransTask(int taskId);
    bool updateTransTask(int taskId, const TransHistoryInfo& info);
    bool updateTaskTransState(int taskId, int transState);
    bool updateTransTaskScenePath(int taskId, std::string& scenePath);
    bool updateTransTaskDownloadPath(int taskId, std::string& downloadPath);
    int  getUnfinishedUploadTransTaskCount();
    void getUnfinishedUploadTransTaskList(std::map<int, TransHistoryInfo>& list);
    int  getUnfinishedDownloadTransTaskCount();
    void getUnfinishedDownloadTransTaskList(std::map<int, TransHistoryInfo>& list);
    void getUnstoppedDownloadTransTaskList(std::map<int, TransHistoryInfo>& list);
    int  getDownloadTransTaskStatus(int taskId);
    void getAllTransTaskHistory(std::map<int, TransHistoryInfo>& list);
    void getTransDownloadHistroy(std::map<int, TransHistoryInfo>& list);

    // Download
    bool addDownloadTask(int taskId, const DownloadHistoryInfo& info);
    bool removeDownloadTask(int taskId);
    bool updateDownloadTaskStatus(int taskId, int status, int nchecking, const std::string& completeTime, const std::string& downloadList);
    bool updateDownloadTaskPriority(int taskId, int priority, int priorityTime, int prorityType);
    bool updateDownLoadTaskScene(int taskId, const std::string &strScene);
    int  getUnfinishDownloadCount(int filterDate);
    void getDownloadTaskList(std::map<int, DownloadHistoryInfo>& list, int filterDate);
    bool updateDownloadTaskSavePath(int taskId, const std::string &path);

    void getUnstoppedDownloadTaskList(std::map<int, DownloadHistoryInfo>& list, int filterDate);
    int getDownloadTaskStatus(int taskId);
    bool isTaskHistoryExist(int taskId);
    void getDownloadUnfinishedTaskIds(std::list<int>& list, int filterDate);
    bool updateTaskParentInfo(int taskId, int parentId, const std::string& parentAlias);
    void getDownloadTaskIds(std::list<int>& list, int filterDate);

private:
    bool readTasklist();
    bool openDb(const QString& szPathDb);
public:
    IQueue<SyncData> m_queue;
    HistoryTrans m_transHistory;
    HistoryDownload m_downloadHistory;
};

#endif //_HISTORYDB_MGR_H_ 