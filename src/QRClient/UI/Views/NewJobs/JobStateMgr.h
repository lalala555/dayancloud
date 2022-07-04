/***********************************************************************
* Module:  JobStateMgr.h
* Author:  hqr
* Created: 2017/12/25 15:31:37
* Modifier: hqr
* Modified: 2017/12/25 15:31:37
* Purpose: Declaration of the class
***********************************************************************/
#pragma once
#ifndef JOBSTATEMGR_H
#define JOBSTATEMGR_H
#include "JobState.h"
#include "Common/MainFrame.h"

class JobViewPage;
struct UserName;
class JobListViewPage;

class JobStateMgr : public TSingleton<JobStateMgr>
{
public:
    JobStateMgr();
    virtual ~JobStateMgr();
public:
    JobState* LookupTaskstate(t_taskid taskId);
    void AddTaskstate(JobState *pTaskState);
    bool RemoveTaskstate(t_taskid taskId);
    void InitTaskState();
    void SetClean(bool bClean);
    void SetListBox(JobViewPage *pRenderListbox);
    std::map<int, TransHistoryInfo> LocalTaskHistroyFilter(int filterType, QString& content);
    void LoadLocalHistory();
    void UpdateDownloadHistroy();
    std::map<int, TransHistoryInfo> LocalUploadTaskHistroyFilter(int nShowType);
    int UploadStateToShowType(int state);
    void UpdateUploadHistory(std::map<int, TransHistoryInfo>& uploadHistory); 
    void SetListView(JobListViewPage *renderListbox);

    inline quint32 GetTotalPage()
    {
        return m_pageCount;
    }
    inline quint32 GetCurPage()
    {
        return m_pageNum;
    }
    inline quint32 GetCurTaskCount()
    {
        return (quint32)m_vtPage.size();
    }
    inline quint32 GetTaskId(int nTaskIndex)
    {
        return m_vtPage[nTaskIndex];
    }
    inline quint32 GetTotalJobs()
    {
        return m_totalJobs;
    }

    void SendRefreshTasklist(int nPage = -1, int nShowType = TYPE_SHOW_ALLD);
    void SendRefreshTasklistByTaskId(EVENT_TYPE eventType, int taskId = -1, bool subOnce = 0);
    void TaskListEventRet(qint32  dwCurPage);
    void SubmitEventRet(t_taskid taskId, intptr_t data);
    void TaskOperatorRet();
    void DependTaskRet();
    void DeleteTaskRet(t_taskid taskId);
    void NewTaskEventRet(t_taskid taskId);
    ///
    void RequestDetailInfoTask(JobState* pState);
    void DetailInfoEventRet(t_taskid taskId, int type);

    void GetFirstPage();
    void GetLastPage();
    void GetPrevPage();
    void GetNextPage();
    bool IsTimeout(qint32 dwElapsed = 1500);

    void        pushToAlDownTaskstate(t_taskid tid, JobState*pState);
    JobState* LookupAllDownloadTaskstate(t_taskid taskId);
    void        clearAllDownTaskstate();
    void        pushToAlUploadTaskstate(t_taskid tid, JobState*pState);
    //刷新下载场景名
    //获取所有的下载任务
    std::map<t_taskid, JobState*> getAllDownloadTask();

    JobState* LookupAllUploadTaskstate(t_taskid taskId);
    void      clearAllUploadTaskstate();

    bool TaskIsAdle(int transStatus);
    bool AllTaskAdle();

    // 设置搜索
    void setSearchString();
    void setCurPage(qint32 nCurrPage);

    // 设置正在渲染的任务数
    void setRunTaskCount(qint32 nRunTask);
    int getRunTaskCount();

    bool checkDownloadFlag(t_taskid taskId);
    JobState* resetTreeRootItem();
    JobState* getTreeRootItem()
    {
        return m_treeRootItem;
    }
    void insertTaskIdAndAlias(t_taskid taskId, const QString& taskIdAlias);
    void removeTaskIdAndAlias(t_taskid taskId);
    QString getTaskIdAlias(t_taskid taskId);
    t_taskid getTaskIdFromAlias(const QString& alias);
    void setPageSummury(quint32 totalJobs, quint32 pageCount, quint32 pageNum, quint32 pageSize);

    // 提交账户相关
    UserName* getSubAccountName(qint64 userId);
    void addSubAccountName(UserName* name);
    QList<UserName*> getSubAccountNameList();
    void clearSubAccountNameList();
    // 制作人相关
    QList<QString> getProducerList();
    void setProducerList(const QList<QString>& list);
    // 请求所有子节点下载数
    void queryDownloadCount();
    // 获取任务输出大小
    void getTasksOutputSize();

    bool isAllCompleted();
    void queryDownloadState(t_taskid dwTaskid, bool freshView = false);

    // upload
    QList<JobState*> getUploadingJobs();
    float uploadJobsTotalSpeed();
    float uploadJobsAvgSpeed();
    QString uploadJosSpeedStr();
    bool isLowUploadSpeed(float lowSpeed = 10, int timeElapse = 1000 * 60 * 2); // 低于10KB/s 提示告警（统计两分钟）

    // download
    QList<JobState*> getDownloadingJobs();
    float downloadingJobsTotalSpeed();
    float downloadingJobsAvgSpeed();
    QString downloadJosSpeedStr();
public:
    bool m_bListEvent;
    bool m_bClean;
    CLock m_lock;
    qint32 m_dwActiveTick;
    qint32 m_dwCurPage;
    qint32 m_dwTotalPage;
    qint32 m_dwRunTask;           // 正在渲染的任务数
    JobViewPage* m_renderListBox;
    JobListViewPage *m_renderListView;

    std::map<t_taskid, JobState*> m_lstTask;
    std::map<t_taskid, JobState*> m_mapAllDownloadTask;
    std::map<t_taskid, JobState*> m_mapAllUploadTask;
    std::map<t_taskid, QString>   m_taskIdAndAliasMap;

    std::vector<qint32> m_vtPage;
    JobState* m_treeRootItem;

    // 数据总览
    quint32 m_totalJobs;
    quint32 m_pageCount;
    quint32 m_pageNum;
    quint32 m_pageSize;

    // 提交人
    QMap<qint64, UserName*> m_lstSubAccountName;
    // 制作人
    QList<QString> m_producerList;

    QList<float> m_avgSpeedStatistic;
    qint64 m_timeRecord;
};

#endif // JOBSTATEMGR_H
