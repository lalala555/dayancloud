#include "stdafx.h"
#include "JobStateMgr.h"
#include "Kernel/TaskCenterManager.h"
#include "HttpCommand/HttpCmdManager.h"
#include "HttpCommand/HttpCommon.h"
#include "Common/RBHelper.h"
#include "NewJobs/JobListViewPage.h"

JobStateMgr::JobStateMgr(void)
    : m_renderListBox(nullptr)
    , m_treeRootItem(nullptr)
    , m_dwCurPage(1)
    , m_dwTotalPage(0)
    , m_dwRunTask(0)
    , m_totalJobs(0)
    , m_pageCount(1)
    , m_pageNum(1)
    , m_pageSize(0)
    , m_renderListView(nullptr)
{
}

JobStateMgr::~JobStateMgr(void)
{
    qDeleteAll(m_lstSubAccountName);

    auto it = m_lstTask.begin();
    while (it != m_lstTask.end()) {
        delete it->second;
        it = m_lstTask.erase(it);
    }

    m_taskIdAndAliasMap.clear();
}

// 获取本地已经刷新过的state信息
JobState* JobStateMgr::LookupTaskstate(t_taskid taskId)
{
    CGuard guard(&m_lock);

    auto it = m_lstTask.find(taskId);
    if(it == m_lstTask.end()) {
        return nullptr;
    }
    JobState *pTask = it->second;

    return pTask;
}

void JobStateMgr::AddTaskstate(JobState *pTaskState)
{
    CGuard guard(&m_lock);
    m_lstTask[pTaskState->getTaskId()] = pTaskState;
}

bool JobStateMgr::RemoveTaskstate(t_taskid taskId)
{
    CGuard guard(&m_lock);

    // m_renderListBox->removeTaskState(taskId);
    m_renderListView->removeTaskState(taskId);

    auto itState = m_lstTask.find(taskId);
    if(itState == m_lstTask.end() || itState->second == nullptr) {
        return false;
    }

    m_lstTask.erase(itState);

    // 从组中删除对象
    std::vector<qint32>::iterator it;
    for(it = m_vtPage.begin(); it != m_vtPage.end();) {
        if(*it == taskId) {
            m_vtPage.erase(it);
            break;
        } else {
            it++;
        }
    }

    return true;
}

void JobStateMgr::SetListBox(JobViewPage *renderListbox)
{
    m_renderListBox = renderListbox;
}

void JobStateMgr::SetListView(JobListViewPage *renderListbox)
{
    m_renderListView = renderListbox;
}

void JobStateMgr::InitTaskState()
{
    m_bClean = true;
    m_dwCurPage = 1;
    m_bListEvent = false;
    m_dwActiveTick = GetTickCount();

    /*
    int nShow = MyConfig.client.showType;
    SendRefreshTasklist(-1, nShow);
    */
}

void JobStateMgr::SetClean(bool bClean)
{
    m_bClean = bClean;
}

void JobStateMgr::TaskListEventRet(qint32 dwCurPage)
{
    m_bListEvent = false;

    CGlobalHelp::Response(UM_TASKLIST_UI, GetCurPage(), (m_bClean == true) ? 1 : 0);
}

bool JobStateMgr::IsTimeout(qint32 dwElapsed)
{
    qint32 dwCur = GetTickCount();

    return ((dwCur - m_dwActiveTick) > dwElapsed) ? true : false;
}

// 单页刷新
void JobStateMgr::SendRefreshTasklist(int nPage, int nShowTpe)
{
    if(nShowTpe >= TYPE_SHOW_UPLOADING) {
        CGlobalHelp::Response(UM_TASKLIST_UI, (intptr_t)GetCurPage(), (intptr_t)(m_bClean == true) ? 1 : 0);
        return;
    }

    if(m_bListEvent == true) {
        if(IsTimeout() == false) {
            return;
        }
    }
    CGlobalHelp::Response(UM_SHOW_LOADING, PAGE_TASK, 0);

    m_bListEvent = true;
    m_dwActiveTick = GetTickCount();

    if(nPage == -1) {
        nPage = m_dwCurPage;
    }

    int userId = MyConfig.accountSet.userId;
    int parentId = MyConfig.accountSet.mainUserId;
    bool bIncChild = (parentId == 0) ? true : false;

    // CGlobalHelp::Response(WM_TASKLIST_UI, (WPARAM)GetCurPage(), (LPARAM)(m_bClean == true) ? 1 : 0);
    // RequestManager::Inst()->SendQueryTaskList(nPage, userId, bIncChild, strSearchKey, strSearch, nShowTpe);

    FilterContent filterContent = HttpCmdManager::getInstance()->filterContent;
    if(filterContent.filterType == TIME_STATE_FILTER){
        filterContent.content = RBHelper::getFilterTime(filterContent.filterTimeType);
    }
    m_dwCurPage = filterContent.pageNum;
    
    // 轮训查询所有已经加载过的页码
    for(int i = 1; i <= m_dwCurPage; i++){
        filterContent.pageNum = i;
        // 避免重复刷新导致界面卡顿
        filterContent.isRepeat = true;
        if(i == m_dwCurPage){
            filterContent.isRepeat = false;
        }
        HttpCmdManager::getInstance()->renderList(filterContent);
    }   
}
// 刷新全部下载任务
void JobStateMgr::SendRefreshTasklistByTaskId(EVENT_TYPE eventType, int taskId, bool subOnce)
{
    int count = 0;
    std::string taskidList;

    if(eventType == TYPE_DOWNLOAD) {
        clearAllDownTaskstate();
        std::map<int, DownloadHistoryInfo> downloadList;
        CTaskCenterManager::Inst()->getDownloadHistory(downloadList, 0);
        std::map<int, DownloadHistoryInfo>::const_iterator it = downloadList.begin();
        while(it != downloadList.end()) {
            char szTid[64] = {0};
            sprintf(szTid, "%d,", it->first);
            taskidList += szTid;
            ++it;
        }
    } else if(eventType == TYPE_UPLOAD) {
        clearAllUploadTaskstate();
        std::map<int, UploadHistoryInfo> uploadList;
        CTaskCenterManager::Inst()->getUploadHistory(uploadList);
        auto it = uploadList.begin();
        while(it != uploadList.end()) {
            char szTid[64] = {0};
            sprintf(szTid, "%d,", it->first);
            taskidList += szTid;
            ++it;
        }
    } else {
        char szTid[64] = {0};
        sprintf(szTid, "%d,", taskId);
        taskidList += szTid;
    }

    taskidList.erase(taskidList.find_last_not_of(',') + 1);//删除末尾，

    //RequestManager::Inst()->SendQueryTaskIdsList(0, false, taskidList, subOnce);

    FilterContent filterContent = HttpCmdManager::getInstance()->filterContent;
	//新增刷新任务会清空关键词
	filterContent.searchWord = "";
	filterContent.content = "";
    HttpCmdManager::getInstance()->renderList(filterContent);
}

void JobStateMgr::GetFirstPage()
{
    if(JobStateMgr::Inst()->GetCurPage() > 1) {
        m_dwCurPage = 1;
        m_bClean = true;
        int nShow = MyConfig.client.showType;
        SendRefreshTasklist(-1, nShow);
    }
}

void JobStateMgr::GetLastPage()
{
    if(GetCurPage() < GetTotalPage()) {
        m_dwCurPage = GetTotalPage();
        m_bClean = true;
        int nShow = MyConfig.client.showType;
        SendRefreshTasklist(-1, nShow);
    }
}

void JobStateMgr::GetPrevPage()
{
    if(JobStateMgr::Inst()->GetCurPage() > 1) {
        m_dwCurPage--;
        m_bClean = true;
        int nShow = MyConfig.client.showType;
        SendRefreshTasklist(-1, nShow);
    }
}

void JobStateMgr::GetNextPage()
{
    if(GetCurPage() < GetTotalPage()) {
        m_dwCurPage++;
        m_bClean = true;
        int nShow = MyConfig.client.showType;
        SendRefreshTasklist(-1, nShow);
    }
}

void JobStateMgr::NewTaskEventRet(t_taskid taskId)
{
    if(taskId == -1) return;

    JobState *jobState = nullptr;

    jobState = LookupTaskstate(taskId);

    if (jobState == nullptr && m_renderListView) {
        jobState = new JobState(taskId);
        AddTaskstate(jobState);
    }

    m_bClean = true;
}

void JobStateMgr::SubmitEventRet(t_taskid taskId, intptr_t data)
{
    MSGBOXPARAM *param = (MSGBOXPARAM*)data;
    int retCode = SUBMIT_TASK_NETWORK_ERROR;
    if (param != nullptr) {
        retCode = param->code;
    }

    LOGFMTI("[JobStateMgr] submit task return taskid = %d, code = %d", taskId, retCode);
    if (retCode == SUBMIT_TASK_OK) {
        // 刷新页面
        m_bClean = true;
        int showType = MyConfig.client.showType;
        JobStateMgr::Inst()->SendRefreshTasklist(-1, showType);
    }
    // 界面通知
    CGlobalHelp::Response(UM_SUBMIT_TASK_RET, taskId, (intptr_t)param);
}

void JobStateMgr::TaskOperatorRet()
{
    LOGFMTI("[JobStateMgr] Task Operator success!!!");

    // 刷新列表
    int showType = MyConfig.client.showType;
    m_bListEvent = false;
    SendRefreshTasklist(-1, showType);
}

void JobStateMgr::DependTaskRet()
{
    CGlobalHelp::Response(UM_DEPENDTASK_COMPLETED, 1, 0);
}

void JobStateMgr::DeleteTaskRet(t_taskid taskId)
{
    JobState *pTaskState = LookupTaskstate(taskId);
    if(pTaskState == 0) return;
    CGlobalHelp::Response(UM_TASKDELETE_UI, (intptr_t)taskId, 0);
}

void JobStateMgr::DetailInfoEventRet(t_taskid taskId, int type)
{
    // 抛送消息到请窗口让它展示数据
    CGlobalHelp::Response(UM_INFOLIST_UI, (intptr_t)taskId, (intptr_t)type);
}

void JobStateMgr::RequestDetailInfoTask(JobState* pState)
{
    JobState *pTaskState = LookupTaskstate(pState->getTaskId());
    if(pTaskState == 0) {
        // 说明是子项目
        pTaskState = pState;
    }

    // 清理旧数据
    pState->m_frameDetails.detailItemList.clear();
    pState->m_taskParam.clear();
    pState->m_frameDetails.reqestPageNums.clear();

    QString searchKey = pState->m_frameDetails.searchKey;
    QList<int>& stateList = pState->m_frameDetails.stateList;
    if(searchKey.isEmpty() && stateList.isEmpty()) {
        HttpCmdManager::getInstance()->qureyTaskFrameRenderingInfo(pTaskState->getTaskId(), 1, 50);
    } else {
        HttpCmdManager::getInstance()->searchTaskFrameRenderingInfo(pTaskState->getTaskId(), searchKey, stateList, 1, 50);
    }
    pState->m_frameDetails.reqestPageNums.append(1); // 记录请求的页码,用于防止重复请求

    HttpCmdManager::getInstance()->queryTaskRenderingConsume(pTaskState->getTaskId());

    LOGFMTI("[JobStateMgr] Task Request DetailInfo, taskid=%lld", pTaskState->getTaskId());
}

bool JobStateMgr::AllTaskAdle()
{
    auto it = m_lstTask.begin();
    while(it != m_lstTask.end()) {
        JobState* pTaskState = it->second;
        if(!TaskIsAdle(pTaskState->getTransStatus())) {
            LOGFMTI("[JobStateMgr] Task is not idle, taskid=%lld, status=%d", 
                pTaskState->getTaskId(), pTaskState->getTransStatus());
            return false;
        }
        ++it;
    }

    return true;
}

bool JobStateMgr::TaskIsAdle(int transStatus)
{
    switch(transStatus) {
    case enNothing:
    case enDownloadCompleted:
    case enUploadCompleted:
    case enUploadError:
    case enDownloadError:
    case enUploadStop:
    case enDownloadStop:
        return true;
        break;
    default:
        break;
    }

    return false;
}

void JobStateMgr::pushToAlDownTaskstate(t_taskid tid, JobState*pState)
{
    m_mapAllDownloadTask[tid] = pState;
}

// 获取本地下载过的taskid的state信息
JobState* JobStateMgr::LookupAllDownloadTaskstate(t_taskid taskId)
{
    JobState *pTask = LookupTaskstate(taskId);
    if(pTask != nullptr) {
        return pTask;
    }

    std::map<t_taskid, JobState*>::iterator it = m_mapAllDownloadTask.find(taskId);
    if(it == m_mapAllDownloadTask.end()) {
        return nullptr;
    }

    return it->second;
}

void JobStateMgr::clearAllDownTaskstate()
{
    JobState *pState = nullptr;
    std::map<t_taskid, JobState*>::iterator it = m_mapAllDownloadTask.begin();
    while(it != m_mapAllDownloadTask.end()) {
        delete it->second;
        pState = nullptr;
        it = m_mapAllDownloadTask.erase(it);
    }
}

void JobStateMgr::pushToAlUploadTaskstate(t_taskid tid, JobState*pState)
{
    m_mapAllUploadTask[tid] = pState;
}

std::map<t_taskid, JobState*> JobStateMgr::getAllDownloadTask()
{
    return std::map<t_taskid, JobState*>();
}

// 获取本地上传中的taskid的state信息
// 获取本地下载过的taskid的state信息
JobState* JobStateMgr::LookupAllUploadTaskstate(t_taskid taskId)
{
    JobState *pTask = LookupTaskstate(taskId);
    if(pTask != nullptr) {
        return pTask;
    }

    std::map<t_taskid, JobState*>::iterator it = m_mapAllUploadTask.find(taskId);
    if(it == m_mapAllUploadTask.end()) {
        return NULL;
    }

    return it->second;
}

void JobStateMgr::clearAllUploadTaskstate()
{
    JobState *pState = NULL;
    std::map<t_taskid, JobState*>::iterator it = m_mapAllUploadTask.begin();
    while(it != m_mapAllUploadTask.end()) {
        delete it->second;
        pState = NULL;
        it = m_mapAllUploadTask.erase(it);
    }
}

void JobStateMgr::setSearchString()
{
    QString strContent = HttpCmdManager::getInstance()->filterContent.content;
    HttpCmdManager::getInstance()->setSearchContent(strContent);
    if(!strContent.isEmpty()) {
        setCurPage(0);
    }
}

void JobStateMgr::setCurPage(qint32 nCurrPage)
{
    m_dwCurPage = nCurrPage;
}
void JobStateMgr::setRunTaskCount(qint32 nRunTask)
{
    m_dwRunTask = nRunTask;
}
int JobStateMgr::getRunTaskCount()
{
    return m_dwRunTask;
}

bool JobStateMgr::checkDownloadFlag(t_taskid taskId)
{
    int flag = MyConfig.accountSet.downloadDisable;
    if(flag == ERR_TYPE_DOWNLOAD_NORMAL)
        return true;

    if(flag == ERR_TYPE_BALANCE_LIMITED) {           // 2 欠费禁止下载
        LOGFMTE("[TaskStateMgr] Download is disabled! type = %d", flag);
        CGlobalHelp::Response(UM_MESSAGE_FLOAT, MSGBOX_TYPE_BALANCE_LIMITED, taskId);
    } else if(flag == ERR_TYPE_DOWNLOAD_DISABLE) {   // 1 强制禁止下载
        LOGFMTE("[TaskStateMgr] Download is disabled! type = %d", flag);
        CGlobalHelp::Response(UM_MESSAGE_FLOAT, MSGBOX_TYPE_DOWNLOAD_DISABLE, taskId);
    }

    return false;
}

void JobStateMgr::LoadLocalHistory()
{
    std::map<int, TransHistoryInfo> transList;
    // CTaskCenterManager::Inst()->getTransUploadHistory(transList);
    int type = HttpCmdManager::getInstance()->filterContent.filterType;
    QString content = HttpCmdManager::getInstance()->filterContent.content;
    transList = LocalTaskHistroyFilter(type, content);

    std::map<int, TransHistoryInfo>::const_iterator it = transList.begin();
    while(it != transList.end()) {
        JobState* taskState = nullptr;
        taskState = this->LookupTaskstate(it->first);
        if(taskState == nullptr) {
            taskState = new JobState(it->first);
            taskState->setTransStatus(it->second.transState);
        }
        // 避免任务提交成功但本地记录未删除的情况
        if(taskState->getJobType() == RENDER_JOB) {
            CTaskCenterManager::Inst()->removeUploadHistory(it->first);
            ++it;
            continue;
        }

        taskState->setCamera(QString::fromLocal8Bit(it->second.camera.c_str()));
        taskState->setScene(QString::fromLocal8Bit(it->second.sceneName.c_str()));
        taskState->setScenePath(QString::fromLocal8Bit(it->second.scenePath.c_str()));
        taskState->setDownloadPathCode(it->second.unicode_download);
        taskState->setPicCount(QString::number(it->second.pictureCount));
        taskState->setCoordCount(QString::number(it->second.coordCount));
        taskState->setProjectName(QString::fromLocal8Bit(it->second.sceneName.c_str()));

        if(taskState->getDownloadPathCode() == DOWNLOAD_USE_ANSI){
            taskState->setAutoDownPath(QString::fromLocal8Bit(it->second.downloadPath.c_str()));
        }else if(taskState->getDownloadPathCode() == DOWNLOAD_USE_UNICODE){
            taskState->setAutoDownPath(QString::fromStdString(it->second.downloadPath));
        }else{
            taskState->setAutoDownPath(QString::fromLocal8Bit(it->second.downloadPath.c_str()));
        }  

        if(taskState->getDownloadPathCode() == DOWNLOAD_USE_ANSI){
            taskState->setLocalSavePath(QString::fromLocal8Bit(it->second.loaclSavePath.c_str()));
        }else if(taskState->getDownloadPathCode() == DOWNLOAD_USE_UNICODE){
            taskState->setLocalSavePath(QString::fromStdString(it->second.loaclSavePath));
        }else{
            taskState->setLocalSavePath(QString::fromLocal8Bit(it->second.loaclSavePath.c_str()));
        }

        // taskState->setAutoDownloadPath(QString::fromLocal8Bit(it->second.downloadPath.c_str()));
        // taskState->SetLocalSavePath(QString::fromLocal8Bit(it->second.loaclSavePath.c_str()));
        taskState->setStartDateString(QString::fromLocal8Bit(it->second.startTime.c_str()));
        taskState->setCompelteDateString(QString::fromLocal8Bit(it->second.endTime.c_str()));
        taskState->setArtistName(QString::fromLocal8Bit(it->second.artist.c_str()));        
        taskState->setJobType(it->second.jobType);
        taskState->setUploadProgress(0);
        taskState->setTaskIdAlias(QString::fromStdString(it->second.taskAlias));
        taskState->setDownloadMode(it->second.download_mode);
        // taskState->setUploadedFileSize(0);
        // taskState->setDownloadType(QString::fromStdString(it->second.downloadType));
        
        this->insertTaskIdAndAlias(it->first, QString::fromStdString(it->second.taskAlias));

        AddTaskstate(taskState);
        m_vtPage.push_back(it->first);
        ++it;
    }
}

void JobStateMgr::UpdateDownloadHistroy()
{
    std::map<int, TransHistoryInfo> transList;
    CTaskCenterManager::Inst()->getTransDownloadHistory(transList);
    std::map<int, TransHistoryInfo>::const_iterator it = transList.begin();
    while(it != transList.end()) {
        JobState* taskState = NULL;
        taskState = this->LookupTaskstate(it->first);
        if(taskState == NULL) {
            taskState = new JobState(it->first);
        }
        taskState->setCamera(QString::fromLocal8Bit(it->second.camera.c_str()));
        taskState->setScene(QString::fromLocal8Bit(it->second.sceneName.c_str()));
        taskState->setScenePath(QString::fromLocal8Bit(it->second.scenePath.c_str()));
        taskState->setDownloadMode(it->second.download_mode);
        taskState->setDownloadPathCode(it->second.unicode_download);

        if(!it->second.downloadPath.empty()){
            if(taskState->getDownloadPathCode() == DOWNLOAD_USE_ANSI){
                taskState->setAutoDownPath(QString::fromLocal8Bit(it->second.downloadPath.c_str()));
            }else if(taskState->getDownloadPathCode() == DOWNLOAD_USE_UNICODE){
                taskState->setAutoDownPath(QString::fromStdString(it->second.downloadPath));
            }else{
                taskState->setAutoDownPath(QString::fromLocal8Bit(it->second.downloadPath.c_str()));
            }  
        }
            
        if(!it->second.loaclSavePath.empty()){
            if(taskState->getDownloadPathCode() == DOWNLOAD_USE_ANSI){
                taskState->setLocalSavePath(QString::fromLocal8Bit(it->second.loaclSavePath.c_str()));
            }else if(taskState->getDownloadPathCode() == DOWNLOAD_USE_UNICODE){
                taskState->setLocalSavePath(QString::fromStdString(it->second.loaclSavePath));
            }else{
                taskState->setLocalSavePath(QString::fromLocal8Bit(it->second.loaclSavePath.c_str()));
            }
        }
            
        //taskState->SetStartDate(QString::fromLocal8Bit(it->second.startTime.c_str()));
        //taskState->SetCompleteDate(QString::fromLocal8Bit(it->second.endTime.c_str()));
        taskState->setArtistName(QString::fromLocal8Bit(it->second.artist.c_str()));
        taskState->setTransStatus(it->second.transState);
        taskState->setJobType(it->second.jobType);

        taskState->setAutoDownload(it->second.autoDownload);
        quint32 downprogress = taskState->getDownloadProgress(); // 避免刷新时清0
        taskState->setDownloadProgress(downprogress > 0 ? downprogress : 0);
        if(taskState->getTransStatus() == enDownloadCompleted) {
            taskState->setDownloadProgress(100);
        }
        // 如果任务被重提，则需要显示渲染状态
        taskState->updateTaskType();

        ++it;
    }

    // 清零下载速度
    CGlobalHelp::Response(UM_UPDATE_TRANS_SPEED, UM_UPDATE_TRANS_SPEED_DOWNLOAD, 0);
}

std::map<int, TransHistoryInfo> JobStateMgr::LocalTaskHistroyFilter(int filterType, QString& content)
{
    std::map<int, TransHistoryInfo> transList;
    int showType = HttpCmdManager::getInstance()->filterContent.showType;
#if 0
    if (showType < TYPE_SHOW_UPLOADING) {
        if (/*showType == TYPE_SHOW_ALLAD || */showType == TYPE_SHOW_ALLD)
            CTaskCenterManager::Inst()->getTransUploadHistory(transList);
    }
    else {
        m_vtPage.clear();
        transList = LocalUploadTaskHistroyFilter(showType);
    }

    if (content.isEmpty())
        return transList;

    std::map<int, TransHistoryInfo> filterList;
    std::map<int, TransHistoryInfo>::const_iterator it = transList.begin();
    while (it != transList.end()) {
        if (filterType == CONTENT_FILTER) {
            quint32 id = content.toUInt();
            QString scene = QString::fromLocal8Bit(it->second.sceneName.c_str());
            if (id == it->first || scene.contains(content)) {
                filterList.insert(std::make_pair(it->first, it->second));
            }
        }
        else if (filterType == TIME_STATE_FILTER) {
            QString subTime = QString::fromLocal8Bit(it->second.startTime.c_str());
            QDateTime subDate = QDateTime::fromString(subTime, "yyyy-MM-dd hh:mm:ss");

            QStringList timeRound = content.split("::");
            QDateTime startDate = QDateTime::fromString(timeRound.at(0), "yyyy-MM-dd hh:mm:ss");
            QDateTime endDate = QDateTime::fromString(timeRound.at(timeRound.size() - 1), "yyyy-MM-dd hh:mm:ss");

            if (subDate.toTime_t() >= startDate.toTime_t()
                && subDate.toTime_t() <= endDate.toTime_t()) {
                filterList.insert(std::make_pair(it->first, it->second));
            }
        }
        ++it;
    }
    return filterList;
#else
    if (showType >= TYPE_SHOW_UPLOADING) {
        m_vtPage.clear();
        CTaskCenterManager::Inst()->getTransHistory(transList);
    } else if (showType == TYPE_SHOW_ALLD) {
        CTaskCenterManager::Inst()->getTransHistory(transList);
    }
    return transList;
#endif
}

int JobStateMgr::UploadStateToShowType(int state)
{
    int showType = -1;
    switch(state) {
    case enTransUploading:
        showType = TYPE_SHOW_UPLOADING;
        break;
    case enUploadUnfinished:
        showType = TYPE_SHOW_UPLOADUNFINIFSHED;
        break;
    case enUploadStop:
        showType = TYPE_SHOW_UPLOADSTOPED;
        break;
    case enUploadQueued:
    case enUploadRetrying:
    case enUploadWaiting:
        showType = TYPE_SHOW_UPLOADWAITING;
        break;
    case enUploadError:
        showType = TYPE_SHOW_UPLOADFAIL;
        break;
    case enUploadCheckCompleted:
    case enUploadCompleted:
        break;
    default:
        break;
    }
    return showType;
}

std::map<int, TransHistoryInfo> JobStateMgr::LocalUploadTaskHistroyFilter(int nShowType)
{
    std::map<int, TransHistoryInfo> transList;
    CTaskCenterManager::Inst()->getTransUploadHistory(transList);
    UpdateUploadHistory(transList);

    std::map<int, TransHistoryInfo> filterList;
    std::map<int, TransHistoryInfo>::const_iterator it = transList.begin();
    while(it != transList.end()) {
        if(nShowType == UploadStateToShowType(it->second.transState)) {
            filterList.insert(std::make_pair(it->first, it->second));
        }
        ++it;
    }

    return filterList;
}

void JobStateMgr::UpdateUploadHistory(std::map<int, TransHistoryInfo>& uploadHistory)
{
    if(uploadHistory.empty())
        return;

    std::map<int, TransHistoryInfo>::iterator it = uploadHistory.begin();
    while(it != uploadHistory.end()) {
        JobState* taskState = nullptr;
        taskState = this->LookupTaskstate(it->first);
        if(taskState == nullptr) {
            ++it;
            continue;
        }
        // 更新传输状态
        it->second.transState = taskState->getTransStatus();

        ++it;
    }
}

JobState* JobStateMgr::resetTreeRootItem()
{
    if (m_treeRootItem == nullptr) {
        m_treeRootItem = new JobState(-1);
    } else {
        m_treeRootItem->clearAllChildren();
    }
    return m_treeRootItem;
}

void JobStateMgr::insertTaskIdAndAlias(t_taskid taskId, const QString& taskIdAlias)
{
    CGuard guard(&m_lock);
    m_taskIdAndAliasMap[taskId] = taskIdAlias;
}

void JobStateMgr::removeTaskIdAndAlias(t_taskid taskId)
{
    CGuard guard(&m_lock);

    auto itState = m_taskIdAndAliasMap.find(taskId);
    if(itState == m_taskIdAndAliasMap.end() || itState->second == "") {
        return;
    }

    m_taskIdAndAliasMap.erase(itState);
}

QString JobStateMgr::getTaskIdAlias(t_taskid taskId)
{
    CGuard guard(&m_lock);

    auto it = m_taskIdAndAliasMap.find(taskId);
    if(it == m_taskIdAndAliasMap.end()) {
        return QString::number(taskId);
    }
    return it->second;
}

t_taskid JobStateMgr::getTaskIdFromAlias(const QString& alias)
{
    CGuard guard(&m_lock);
    if(m_taskIdAndAliasMap.empty()) return 0;
    auto it = m_taskIdAndAliasMap.begin();
    while(it != m_taskIdAndAliasMap.end()) {
        if(it->second == alias) {
            return it->first;
        }
        ++it;
    }
    return 0;
}

void JobStateMgr::setPageSummury(quint32 totalJobs, quint32 pageCount, quint32 pageNum, quint32 pageSize)
{
    m_totalJobs = totalJobs;
    m_pageCount = pageCount;
    m_pageNum   = pageNum;
    m_pageSize  = pageSize;
}

UserName* JobStateMgr::getSubAccountName(qint64 userId)
{
    UserName* producer = nullptr;
    auto it = m_lstSubAccountName.find(userId);
    if(it == m_lstSubAccountName.end())
        return nullptr;
    producer = it.value();
    return producer;
}

void JobStateMgr::addSubAccountName(UserName* name)
{
    if(name == nullptr)
        return;
    m_lstSubAccountName.insert(name->userId, name);
}

QList<UserName*> JobStateMgr::getSubAccountNameList()
{
    QList<UserName*> producers;
    producers = m_lstSubAccountName.values();
    return producers;
}

void JobStateMgr::clearSubAccountNameList()
{
    m_lstSubAccountName.clear();
}

QList<QString> JobStateMgr::getProducerList()
{
    return m_producerList;
}

void JobStateMgr::setProducerList(const QList<QString>& list)
{
    m_producerList.clear();
    m_producerList.append(list);
}

void JobStateMgr::queryDownloadCount()
{
    JobState* jobState = getTreeRootItem();
    if (jobState == nullptr)
        return;

    QList<qint64> taskIds;
    foreach(JobState* job, jobState->getChildJobState()) {
        if (job->IsParentItem()) {
            foreach(JobState* subJob, job->getChildJobState()) {
                taskIds.append(subJob->getTaskId());
            }
        } else {
            taskIds.append(job->getTaskId());
        }
    }

    HttpCmdManager::getInstance()->queryTaskDownloadCount(taskIds);
}

void JobStateMgr::getTasksOutputSize()
{
    JobState* jobState = getTreeRootItem();
    if (jobState == nullptr)
        return;

    QList<qint64> taskIds;
    foreach(JobState* job, jobState->getChildJobState()) {
        if (job->IsParentItem()) {
            foreach(JobState* subJob, job->getChildJobState()) {
                if (subJob->TaskIsFinished() && job->getTaskOutputSize() <= 0) {
                    taskIds.append(subJob->getTaskId());
                }      
            }
        } else {
            if (job->TaskIsFinished() && job->getTaskOutputSize() <= 0) {
                taskIds.append(job->getTaskId());
            }    
        }
    }

    HttpCmdManager::getInstance()->getTasksOutputSize(taskIds, PAGE_TASK);
}

bool JobStateMgr::isAllCompleted()
{
    auto it = m_lstTask.begin();
    while (it != m_lstTask.end()) {
        if (it->second) {
            JobState* state = it->second;
            if (state->getTransStatus() == enUploadStop ||
                state->getTransStatus() == enUploadCompleted) {
                ++it;
            } else {
                return false;
            }
        }
    }

    return true;
}


void JobStateMgr::queryDownloadState(t_taskid dwTaskid, bool freshView)
{
    QList<qint64> taskIds;
    taskIds.append(dwTaskid);
    HttpCmdManager::getInstance()->queryTaskInfo(taskIds, freshView);
}

QList<JobState*> JobStateMgr::getUploadingJobs()
{
    CGuard guard(&m_lock);
    QList<JobState*> uploadjobs;

    auto list = m_lstTask;
    auto it = list.begin();
    while (it != list.end()) {
        if (it->second->getTransStatus() == enTransUploading) {
            uploadjobs.append(it->second);
        }

        it++;
    }
    return uploadjobs;
}

float JobStateMgr::uploadJobsTotalSpeed()
{
    auto uploadjobs = getUploadingJobs();
    float totalspeed = 0.0f;
    float avgSpeed = totalspeed;

    foreach(JobState* job, uploadjobs) {
        // 只统计正在传输的
        if (job->IsTransform()) {
            totalspeed += job->getTransSpeed();
        }
    }

    return totalspeed;
}

float JobStateMgr::uploadJobsAvgSpeed()
{
    float total = uploadJobsTotalSpeed();
    int count = getUploadingJobs().count();
    float avg = 0;

    if (count != 0) {
        avg = total / count;
    }

    return avg;
}

QString JobStateMgr::uploadJosSpeedStr()
{
    float speed = uploadJobsTotalSpeed();
    return RBHelper::convertTransSpeedToStr(speed, DISPLAY_UNIT_BIT);
}

bool JobStateMgr::isLowUploadSpeed(float lowSpeed, int timeElapsed)
{
    // 如果得到的值为0，则表示传输出故障了
    m_avgSpeedStatistic.append(this->uploadJobsTotalSpeed());

    if (GetTickCount() - m_timeRecord < timeElapsed)
        return false;
    m_timeRecord = GetTickCount();

    // 统计全部
    float total = 0;
    foreach(float speed, m_avgSpeedStatistic) {
        total += speed;
    }

    // 获取中位数
    qSort(m_avgSpeedStatistic.begin(), m_avgSpeedStatistic.end());
    int mid_index = m_avgSpeedStatistic.count() / 2;

    float mid_value = 0.0f;
    float avg = 0.0f;
    if (!m_avgSpeedStatistic.isEmpty()) {
        avg = total / m_avgSpeedStatistic.count() * 1.0f;
        mid_value = m_avgSpeedStatistic.at(mid_index);
    }

    // 计算众数
    float mo = avg - 3.0f * (avg - mid_value);

    // 转换成KB/S
    mo = RBHelper::convertTransSpeed(mo, SPEED_UNIT_KB);
    // 如果计算得到的数据小于最低限制，则返回true, 并清理缓存
    if (this->getUploadingJobs().count() > 0 && mo > 0 && mo < lowSpeed) {
        m_avgSpeedStatistic.clear();
        return true;
    }

    m_avgSpeedStatistic.clear();
    return false;
}

QList<JobState*> JobStateMgr::getDownloadingJobs()
{
    CGuard guard(&m_lock);
    QList<JobState*> downJobs;

    auto list = m_lstTask;
    auto it = list.begin();
    while (it != list.end()) {
        if (it->second->getTransStatus() == enTransDownloading) {
            downJobs.append(it->second);
        }

        it++;
    }
    return downJobs;
}

float JobStateMgr::downloadingJobsTotalSpeed()
{
    auto downloadjobs = getDownloadingJobs();
    float totalspeed = 0.0f;
    float avgSpeed = totalspeed;

    foreach(JobState* job, downloadjobs) {
        if (job->IsTransform()) {
            totalspeed += job->getTransSpeed();
        }
    }

    return totalspeed;
}

float JobStateMgr::downloadingJobsAvgSpeed()
{
    float total = downloadingJobsTotalSpeed();
    int count = getDownloadingJobs().count();
    float avg = 0;

    if (count != 0) {
        avg = total / count;
    }

    return avg;
}

QString JobStateMgr::downloadJosSpeedStr()
{
    float speed = downloadingJobsTotalSpeed();
    return RBHelper::convertTransSpeedToStr(speed, DISPLAY_UNIT_BIT);
}