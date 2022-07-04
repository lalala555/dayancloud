#pragma once
#include <QList>
#include <map>
#include "DownloadState.h"
#include "Common/MainFrame.h"

class DownloadViewPage;

class CDownloadStateMgr : public TSingleton<CDownloadStateMgr>
{
public:
    CDownloadStateMgr();
    virtual ~CDownloadStateMgr();
public:
    CDownloadState* LookupTaskstate(t_taskid dwTaskid);
    void AddDownloadState(CDownloadState *pState, int nHead); //head = 0
    void AddDownloadState(CDownloadState *pState);
    bool removeDownloadState(t_taskid dwTaskid, bool bPermanent = false);
    void SetListBox(DownloadViewPage* pRenderListbox);
    void LoadlocalHistory();
    std::map<t_taskid, CDownloadState*> getAllDownloadTask();
    //刷新下载场景名
    void updateDownLoadScene(int taskid, const QString &strScene);
    bool isAllCompleted();
    bool removeTaskList(t_taskid dwTaskid);
    QList<CDownloadState*> getDownloadingJobs();
    QList<t_taskid> getAllOrderedJobIdList();

    void refreshBadgeNumber();

    void taskListEventRet(bool freshView = false);
    void setClean(bool clean);

    CDownloadState* buildDownloadState(t_taskid jobId, const DownloadHistoryInfo& info);
    QString getOutputPath(t_taskid jobId);
    QString getDownloadLocalPath(t_taskid jobId);

    CDownloadState* resetRootItem();
    CDownloadState* getRootItem()
    {
        return m_rootItem;
    }

    qint32 getTotalDownloadCount();
    void refreshView();

    void runAllHistoryDownload();
public:
    CLock m_lock;
    std::map<t_taskid, CDownloadState*> m_lstTask;
    DownloadViewPage* m_pDownloadListBox;
    bool m_bListEvent;
    CDownloadState* m_rootItem;
    QList<int> filterStatus;
    bool m_bClean;

    qint64 m_timeRecord;
    QList<float> m_avgSpeedStatistic;
};
