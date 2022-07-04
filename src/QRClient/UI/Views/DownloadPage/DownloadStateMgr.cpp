#include "stdafx.h"
#include "DownloadStateMgr.h"
#include "DownloadViewPage.h"
#include "Kernel/TaskCenterManager.h"
#include "NewJobs/JobStateMgr.h"
#include "PluginConfig/ProjectMgr.h"
#include "Common/StringUtil.h"
#include "HttpCommand/HttpCmdManager.h"
#include <QtConcurrent/QtConcurrent>

CDownloadStateMgr::CDownloadStateMgr(void)
    : m_pDownloadListBox(nullptr)
    , m_rootItem(nullptr)
{
}

CDownloadStateMgr::~CDownloadStateMgr(void)
{
    auto it = m_lstTask.begin();
    while (it != m_lstTask.end()) {
        delete it->second;
        it = m_lstTask.erase(it);
    }

    delete m_rootItem;
}

CDownloadState* CDownloadStateMgr::LookupTaskstate(t_taskid taskId)
{
    CGuard guard(&m_lock);
    CDownloadState *pTask = nullptr;

    auto it = m_lstTask.find(taskId);
    if(it == m_lstTask.end()) {
        return nullptr;
    }

    return it->second;
}

void CDownloadStateMgr::AddDownloadState(CDownloadState *pState, int nHead)
{
    CGuard guard(&m_lock);
    m_lstTask[pState->getJobId()] = pState;
}

void CDownloadStateMgr::AddDownloadState(CDownloadState *pState)
{
    if(pState == nullptr)
        return;

    if(LookupTaskstate(pState->getJobId()) == nullptr) {
        AddDownloadState(pState, 1);
        if(m_pDownloadListBox && pState->getVisible(true)) {
            //m_pDownloadListBox->addDownloadState(pState);
        }
        //m_pDownloadListBox->updateRunTaskTable();
    }
}

bool CDownloadStateMgr::removeDownloadState(t_taskid taskId, bool bPermanent)
{
    CGuard guard(&m_lock);
    CDownloadState *state = CDownloadStateMgr::Inst()->LookupTaskstate(taskId);
    if(state == nullptr) return false;

    CTaskCenterManager::Inst()->removeDownload(taskId);
    m_pDownloadListBox->removeDownloadState(taskId);
    this->removeTaskList(taskId);

    return true;
}

//刷新下载场景名
void CDownloadStateMgr::updateDownLoadScene(int taskid, const QString &strScene)
{
    // HistoryDbMgr::getInstance()->updateDownLoadTaskScene(taskid, strScene.toStdString());
}

void CDownloadStateMgr::SetListBox(DownloadViewPage *pRenderListbox)
{
    m_pDownloadListBox = pRenderListbox;
}

void CDownloadStateMgr::LoadlocalHistory()
{
    std::map<int, DownloadHistoryInfo> downloadList;
    int days = LocalSetting::getInstance()->getDownloadShowInDay();
    CTaskCenterManager::Inst()->getDownloadHistory(downloadList, days);

    auto it = downloadList.begin();
    while(it != downloadList.end()) {
        CDownloadState* parentState = nullptr;
        CDownloadState* downloadState = buildDownloadState(it->first, it->second);

        // 兼容老版本下载路径
        // 更新数据库本地下载路径(savepath)，只在客户端版本更新后首次使用此功能
        if(!CConfigDb::Inst()->isUpdateDownloadList()){
            QString projPath = ProjectMgr::getInstance()->getProjectOutput(downloadState->getCGId());
            downloadState->setLocalSavePath(projPath);
            // HistoryDbMgr::getInstance()->updateDownloadTaskSavePath(downloadState->getJobId(), projPath.toStdString());
        }

        // 父任务不加入列表
        if(parentState != nullptr) {
            AddDownloadState(downloadState, 0);
        } else {
            AddDownloadState(downloadState);
        }

        if(downloadState->GetCheckingStatus() == CDownloadState::enCheckingFailed) {
            CGlobalHelp::Response(UM_WARN_SET_REDHOT, PAGE_DOWNLOAD, true);
        }
        ++it;
    }

    CConfigDb::Inst()->setUpdateDownloadList(true);

    downloadList.clear();
}

std::map<t_taskid, CDownloadState*> CDownloadStateMgr::getAllDownloadTask()
{
    return m_lstTask;
}

bool CDownloadStateMgr::isAllCompleted()
{
    auto it = m_lstTask.begin();
    while(it != m_lstTask.end()) {
        if(it->second) {
            CDownloadState* state = it->second;
            if(state->GetTransState() == enDownloadStop
                    || state->GetTransState() == enDownloadCompleted) {
                ++it;
            } else {
                return false;
            }
        }
    }

    return true;
}

bool CDownloadStateMgr::removeTaskList(t_taskid dwTaskid)
{
    std::map<t_taskid, CDownloadState*>::iterator itState = m_lstTask.find(dwTaskid);
    if(itState == m_lstTask.end() || itState->second == nullptr) {
        return false;
    }
    // delete itState->second;
    // itState->second = nullptr;
    m_lstTask.erase(itState);
    return true;
}

QList<CDownloadState*> CDownloadStateMgr::getDownloadingJobs()
{
    CGuard guard(&m_lock);
    QList<CDownloadState*> downJobs;

    auto list = m_lstTask;
    auto it = list.begin();
    while(it != list.end()) {
        CDownloadState* job = it->second;
        if(job->GetTransState() == enTransDownloading && !job->isObsoleteData()) {
            downJobs.append(it->second);
        }

        it++;
    }
    return downJobs;
}

QString CDownloadStateMgr::getDownloadLocalPath(t_taskid jobId)
{
    return getOutputPath(jobId);
}

void CDownloadStateMgr::refreshBadgeNumber()
{
    int days = LocalSetting::getInstance()->getDownloadShowInDay();
    int count = HistoryDbMgr::getInstance()->getUnfinishDownloadCount(days);
    CGlobalHelp::Response(UM_BADGENUMBER, count, (intptr_t)PAGE_DOWNLOAD);
}

void CDownloadStateMgr::taskListEventRet(bool freshView)
{
    CGlobalHelp::Response(UM_DOWNLOADLIST_UI, freshView ? 1 : 0, (m_bClean == true) ? 1 : 0);
}

void CDownloadStateMgr::setClean(bool clean)
{
    m_bClean = clean;
}

CDownloadState* CDownloadStateMgr::buildDownloadState(t_taskid jobId, const DownloadHistoryInfo& info)
{
    CDownloadState* downloadState = new CDownloadState(jobId);

    /*switch (info.status) {
    case DOWNLOAD_FINISHED:
        downloadState->setTransState(enDownloadCompleted, false);
        break;
    case DOWNLOAD_UNFINISH:
    case DOWNLOAD_LASTTIME:
        downloadState->setTransState(enDownloadQueued, false);
        break;
    case DOWNLOAD_STOP:
        downloadState->setTransState(enDownloadStop, false);
        break;
    }*/
    downloadState->setTransState(info.status, false);
    downloadState->setProjectName(QString::fromStdString(info.project));
    downloadState->setScene(QString::fromStdString(info.sceneName));
    downloadState->setOrgScene(QString::fromStdString(info.sceneName));
    downloadState->SetLayerName(QString::fromStdString(info.layerName));
    downloadState->setArtist(QString::fromStdString(info.artist));
    downloadState->setStartDate(QString::fromStdString(info.startTime));
    downloadState->SetStopDate(QString::fromStdString(info.stopTime));
    downloadState->SetCheckingStatus(info.ncheckingstatus, false);
    downloadState->setPriority(info.priority);
    downloadState->setPriorityTime(info.priorityTime);
    downloadState->setStorageId(info.storageId);
    downloadState->setOutputLabel(QString::fromStdString(info.outputLabel));
    downloadState->SetLocalSaveAsPath(QString::fromStdString(info.saveAsPath));
    downloadState->setLocalSavePath(QString::fromStdString(info.savePath));
    downloadState->setTaskAlias(QString::fromStdString(info.taskAlias));
    downloadState->setIsOpen(false);
    downloadState->SetParentTaskId(info.parentId);
    downloadState->SetTaskOutputSize(QString::fromStdString(info.outputSize).toLongLong());
    downloadState->setProrityType(info.priorityType);
    downloadState->setDownloadJobType(info.downloadJobType);
    downloadState->setJobOperateType(info.operateType);
    downloadState->setFrameName("");
    downloadState->SetCustomerId(info.taskUserId);
    QString output = QString::fromStdString(info.downloadType);
    QStringList outList = output.split("|");
    QStringList outputs;
    foreach(QString out, outList) {
        if (out.isEmpty()) continue;
        outputs.append(out);
    }
    downloadState->setOutputType(outputs);
    downloadState->setDownloadFiles(outputs);

    return downloadState;
}


QString CDownloadStateMgr::getOutputPath(t_taskid jobId)
{
    CDownloadState *downloadState = this->LookupTaskstate(jobId);
    if (downloadState == nullptr)
        return "";

    QString projectSavePath = ProjectMgr::getInstance()->getProjectOutput(downloadState->getProject());
    projectSavePath = downloadState->getDownloadFullPath(projectSavePath);

    t_taskid taskId = downloadState->getJobId();
    t_taskid parentId = downloadState->GetParentTaskId();

    QString strpath = RBHelper::getOutputPath(taskId, parentId, downloadState->GetScene(), projectSavePath);

    return strpath;
}

CDownloadState* CDownloadStateMgr::resetRootItem()
{
    if (m_rootItem == nullptr) {
        m_rootItem = new CDownloadState(-1);
    }

    return m_rootItem;
}

qint32 CDownloadStateMgr::getTotalDownloadCount()
{
    return m_lstTask.size();
}

QList<t_taskid> CDownloadStateMgr::getAllOrderedJobIdList()
{
    QMap<t_taskid, CDownloadState*> jobs(getAllDownloadTask());
    QList<t_taskid> jobList = jobs.keys();

    qSort(jobList.begin(), jobList.end(), qGreater<t_taskid>());

    return jobList;
}

void CDownloadStateMgr::refreshView()
{
    if (m_pDownloadListBox)
        m_pDownloadListBox->refreshView();
}

void CDownloadStateMgr::runAllHistoryDownload()
{
    auto downloads = this->getAllDownloadTask();
    auto it = downloads.begin();
    while (it != downloads.end()) {
        if (it->second->GetTransState() != enDownloadCompleted && it->second->GetTransState() != enDownloadStop) {
            CTaskCenterManager::Inst()->manualStopDownload(it->first);
            CTaskCenterManager::Inst()->startDownloadHistoryTask(it->first);
        }       
        ++it;
    }
}