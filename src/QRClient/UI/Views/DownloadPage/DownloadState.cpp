#include "stdafx.h"
#include "DownloadState.h"
#include "Common/StringUtil.h"
#include "Common/SystemUtil.h"

CDownloadState::CDownloadState(qint32 jobId, CDownloadState* parentItem)
{
    m_transSpeed        = 0.0f;
    m_dwProgress        = 0;
    m_nTransState       = enNothing;
    m_nStatus           = enNothing;
    m_strShortScene     = "Not found";
    m_strOrgScene       = "";
    m_strError          = "";
    m_strDownloadFileName = "";
    m_strSpeed          = "";
    m_nFrameStatus      = enCheckingWaitting;
    m_hasError          = false;
    m_priority          = enPriorityMiddle;
    m_priorityTime      = 0;
    m_sPriority         = "M";
    m_bVisible          = true;
    m_sPathCmd          = "";
    m_strLayerName      = "-";
    m_parentItem        = parentItem;
    m_isParentItem      = false;
    m_storageId         = 0;
    m_isInDb            = false;
    m_isAutoDownload    = false;
    m_isOpen            = false;
    m_strStartDate      = "";
    m_strStopDate       = "";
    m_startRenderDate   = -1;
    m_parentId          = 0;
    m_rowNum            = -1;
    m_taskFileSize      = 0;
    m_prorityType       = enDefaultPriority;
    m_taskLastExecTime  = 0;
    m_timeElapsed       = 0;
    m_jobType           = enSceneDownloadJob;
    m_jobOperateType    = enAutoDownload;
    m_nCustomerId       = 0;
    SetTaskId(jobId); 
}

CDownloadState::~CDownloadState(void)
{
}

void CDownloadState::SetTaskId(qint32 dwTaskId)
{
    m_dwTaskId = dwTaskId;
}

void CDownloadState::setScene(const QString& strScene)
{
    m_strShortScene = strScene;
}

void CDownloadState::setOrgScene(const QString& strScene)
{
    m_strOrgScene = strScene;
}

void CDownloadState::SetLayerName(const QString& strLayer)
{
    m_strLayerName = strLayer;
}

void CDownloadState::SetProjectSymbol(const QString& strSymbol)
{
    m_strProjectSymbol = strSymbol;
}

void CDownloadState::SetProgress(float dwProgress, bool bReset)
{
    if(bReset == true) {
        m_dwProgress = 0;
        return;
    }

    if(dwProgress > m_dwProgress) {
        m_dwProgress = dwProgress;

        if(m_dwProgress >= 100) {
            m_dwProgress = 100;
        }
    } else {
        m_dwProgress = dwProgress;
    }
    InvalidateTask();
}

void CDownloadState::setStartDate(const QString& strStartDate)
{
    m_strStartDate = strStartDate;
}

void CDownloadState::SetStopDate(const QString& pszStopDate)
{
    m_strStopDate = pszStopDate;
}

void CDownloadState::setCompleteDate(const QString& pszCompleteDate)
{
    m_strCompletedDate = pszCompleteDate;
}

void CDownloadState::SetStatus(int status)
{
    m_nStatus = status;
}

void CDownloadState::setLocalSavePath(const QString& strSavePath)
{
    if(strSavePath.isEmpty())
        return;

    m_strLocalSavePath = strSavePath;
}

qint32 CDownloadState::getJobId()
{
    return m_dwTaskId;
}

QString CDownloadState::GetScene()
{
    return m_strShortScene;
}

QString CDownloadState::GetOrgScene()
{
    return m_strOrgScene;
}

QString CDownloadState::GetLayerName()
{
    return m_strLayerName;
}

QString CDownloadState::getProject()
{
    return m_strProjectSymbol;
}

float CDownloadState::GetProgress()
{
    return m_dwProgress;
}

QString CDownloadState::GetStartDate()
{
    return m_strStartDate;
}

QString CDownloadState::GetStopDate()
{
    return m_strStopDate;
}

QString CDownloadState::GetCompleteDate()
{
    return m_strCompletedDate;
}

QString CDownloadState::GetStatusString()
{
    QString wstrTip;
    int status = m_nTransState;

    switch (status) {
    case enTransDownloading:
        wstrTip = QObject::tr("正在下载");
        break;
    case enDownloadCompleted:
        wstrTip = QObject::tr("下载完成");
        break;
    case enDownloadStop:
        wstrTip = QObject::tr("停止下载");
        break;
    case enDownloadWaiting:
    case enDownloadLasttime:
        wstrTip = QObject::tr("等待下载");
        break;
    case enDownloadQueued:
        wstrTip = QObject::tr("等待下载");
        break;
    case enDownloadCompletedWithError:
        wstrTip = QObject::tr("下载完成，有错误");
        break;
    case enDownloadError:
        wstrTip = QObject::tr("下载失败");
        break;
    case enDownloadUnfinish:
        wstrTip = QObject::tr("下载未完成");
        break;
    default:
        wstrTip = "-";
        break;
    }

    return wstrTip;
}

int CDownloadState::GetStatus()
{
    int status = m_nStatus;
    return status;
}

QString CDownloadState::GetLocalSavePath()
{
    return m_strLocalSavePath;
}

void CDownloadState::setTransState(int nTransState, bool invalidate)
{
    m_nTransState = nTransState;
    if (m_nTransState == enDownloadCompleted) {
        SetProgress(100);
    }

    if(invalidate) InvalidateTask();
}

/*
    1. 所有的子任务都是等待的时候，壳任务是等待状态
    2. 所有的子任务都是完成的时候，壳任务是完成状态
    3. 只要有一个子任务是下载状态，壳任务是下载状态
    4. 所有的子任务都是停止状态，壳任务是停止状态
    5. 所有的子任务都是排队状态时，壳任务是排队状态
*/
int CDownloadState::GetTransState()
{
    int status = m_nTransState;

#if DOWNLOAD_PAGE_TREE_STRUCT
    if(this->isParentItem()) {
        QList<CDownloadState*> children = getChildren();
        int downloading = 0;
        int completed = 0;
        int waitting  = 0;
        int stoped = 0;
        int queued = 0;
        int otherStatus = 0;
        foreach(CDownloadState* child, children) {
            if(child->GetTransState() == enTransDownloading) {
                downloading++;
            } else if(child->GetTransState() == enDownloadCompleted) {
                completed++;
            } else if(child->GetTransState() == enDownloadWaiting) {
                waitting++;
            } else if(child->GetTransState() == enDownloadStop) {
                stoped++;
            } else if(child->GetTransState() == enDownloadQueued) {
                queued++;
            } else {
                otherStatus++;
            }
        }

        if(downloading > 0) {
            status = enTransDownloading;
        } else if(completed == childCount()) {
            status = enDownloadCompleted;
        } else if(waitting == childCount()) {
            status = enDownloadWaiting;
        } else if(stoped == childCount()) {
            status = enDownloadStop;
        } else if(queued == childCount()) {
            status = enDownloadQueued;
        } else {
            status = enTransDownloading;
        }
    }
#endif

    return status;
}

void CDownloadState::SetDownloadFileName(const QString& strName)
{
    m_strDownloadFileName = strName;
}

QString CDownloadState::GetDownloadFileName()
{
    return m_strDownloadFileName;
}

void CDownloadState::SetSpeed(const QString& pszSpeed)
{
    CGuard guard(&m_lockSpeed);
    m_strSpeed = pszSpeed;
}

void CDownloadState::SetSpeed(float fSpeed)
{
    {
        CGuard guard(&m_lockSpeed);
        m_transSpeed = fSpeed;
        char szSpeed[20] = { 0 };
        QString qspeed = "";
        if (RBHelper::isFloatValid(fSpeed)) {
            qspeed = RBHelper::convertTransSpeedToStr(fSpeed, DISPLAY_UNIT_BIT);
        }
        m_strSpeed = qspeed;
    }

    InvalidateTask();
}

QString CDownloadState::GetSpeed()
{
    return m_strSpeed;
}

void CDownloadState::setArtist(const QString& pszArtist)
{
    m_artist = pszArtist;
}

QString CDownloadState::GetArtist()
{
    return m_artist;
}

QString CDownloadState::getOutputPath(int ptid, int uid, int puid, const QString& projectSavePath)
{
    m_currProjectSavePath = projectSavePath;

    m_sPathCmd = m_outputPath;

    return m_outputPath;
}

QString CDownloadState::getOutputPath(const QString& projectSavePath)
{
    m_outputPath = QString("%1/%2").arg(projectSavePath).arg(this->getOutputLabel());
    return m_outputPath;
}

QString CDownloadState::getCurrProjectSavePath()
{
    return m_currProjectSavePath;
}

CG_ID CDownloadState::getCGId()
{
    return RBHelper::cgidOfScene(m_strShortScene);
}

void CDownloadState::SetCheckingStatus(int status, bool invalidate)
{
    m_nFrameStatus = status;
    if(invalidate) InvalidateTask();
}

int CDownloadState::GetCheckingStatus()
{
    return m_nFrameStatus;
}

int CDownloadState::GetCheckingStatusIcon()
{
    int nStatus = 2;
    switch(m_nFrameStatus) {
    case enCheckingFailed:
        nStatus = 1;
        break;
    case enCheckingSuccess:
        nStatus = 0;
        break;
    default:
        nStatus = 2;
        break;
    }
    return nStatus;
}

void CDownloadState::setVisible(bool bVisible)
{
    m_bVisible = bVisible;
}

bool CDownloadState::getVisible(bool bVisible)
{
    return m_bVisible;
}

void CDownloadState::addChildDownloadState(CDownloadState* child)
{
    QMutexLocker locker(&m_mutex);
    if(child == nullptr)
        return;
    //auto it = m_children.find(child->GetTaskId());
    //if(it == m_children.end()) {
    //    m_children.insert(child->GetTaskId(), child);
    //} else {
    m_children[child->getJobId()] = child;
    //}
}

QHash<qint64, CDownloadState*> CDownloadState::getChildDownloadState()
{
    return m_children;
}

CDownloadState* CDownloadState::child(int row)
{
    QMutexLocker locker(&m_mutex);
    QList<CDownloadState*> jobs = m_children.values();
    if(jobs.isEmpty() || jobs.count() <= row)
        return NULL;
    return jobs.value(row);
}

CDownloadState* CDownloadState::getChildByTaskId(qint64 taskid)
{
    QMutexLocker locker(&m_mutex);
    if(m_children.isEmpty())
        return NULL;

    return m_children.value(taskid);
}

int CDownloadState::childCount() const
{
    return m_children.count();
}

int CDownloadState::columnCount() const
{
    return COLUMN_DOWNLOAD_TOTAL;
}

int CDownloadState::row()
{
    QMutexLocker locker(&m_mutex);
    if (m_parentItem) {
        QList<CDownloadState*> jobs = m_parentItem->m_children.values();
        if(!jobs.isEmpty())
            return jobs.indexOf(const_cast<CDownloadState*>(this));
    }
    return 0;
}

void CDownloadState::clearAllChildren()
{
    QMutexLocker locker(&m_mutex);
    if(!m_children.isEmpty()) {
        foreach(CDownloadState *job, m_children.values()) {
            m_children.remove(job->getJobId());
        }
    }
}

QList<CDownloadState*> CDownloadState::getChildren()
{
    QList<CDownloadState*> downloads;
    downloads = m_children.values();
    return downloads;
}

qint32 CDownloadState::getRowCount()
{
    qint32 count = m_children.count();
    for (int i = 0; i < m_children.count(); i++) {
        CDownloadState* job = this->child(i);
        if(job != nullptr)
            count += job->childCount();
    }
    return count;
}

QList<qint64> CDownloadState::getChildrenIds()
{
    QList<qint64> childrenIds;
    childrenIds = m_children.keys();
    return childrenIds;
}

// 获取父节点
CDownloadState* CDownloadState::getParentItem()
{
    return m_parentItem;
}

void CDownloadState::setParentItem(CDownloadState *parent)
{
    m_parentItem = parent;
}

void CDownloadState::setIsParentItem(bool isParent)
{
    m_isParentItem = isParent;
}

bool CDownloadState::isParentItem()
{
    return m_isParentItem;
}

void CDownloadState::removeChildState(CDownloadState* state)
{
    if(state == nullptr)
        return;

    QMutexLocker locker(&m_mutex);
    auto it = m_children.find(state->getJobId());
    if(it == m_children.end() && !state->isParentItem()) {
        CDownloadState* parent = state->getParentItem();
        parent->removeChildState(state);
    } else {
        m_children.remove(state->getJobId());
    }
}

int CDownloadState::getDownloadCompleteChildCount()
{
    int count = 0;
    if(this->isParentItem()) {
        foreach(CDownloadState* child, getChildren()) {
            if(child->GetTransState() == enDownloadCompleted)
                count++;
        }
    }
    return count;
}

QString CDownloadState::getTransProgress()
{
    return "";
}

void CDownloadState::setStorageId(qint32 bid)
{
    m_storageId = bid;
}

qint32 CDownloadState::getStorageId()
{
    // 任务的userid和登录的账户ID不同时
    if (this->isSubAccountJob()) {
        qint32 bid = MyConfig.storageSet.getChildOutputBid(this->GetCustomerId());
        if (bid == 0) {
            LOGFMTE("[CDownloadState::getStorageId] User (%I64d) output bid not exist!", this->GetCustomerId());
        } else {
            return bid;
        }
    }

    return m_storageId;
}

void CDownloadState::setOutputLabel(const QString& outputLabel)
{
    m_outputLabel = outputLabel;
}

QString CDownloadState::getOutputLabel()
{
    if(m_outputLabel.isEmpty())
        return m_parentOutputLabel;

    return  m_outputLabel;
}

bool CDownloadState::isCompleted()
{
    return true;
    if(((GetStatus() == JobState::enRenderTaskFinished))
            || GetStatus() == JobState::enRenderTaskFinishHasFailed)
        return true;

    return false;
}

void CDownloadState::setTaskProgress(int excuteCount, int doneCount, int failedCount, int abortCount, int totalCount)
{
    m_executingFrames = excuteCount;
    m_doneFrames = doneCount;
    m_failedFrames = failedCount;
    m_abortFrames = abortCount;
    m_totalFrames = totalCount;
    m_waitFrames = totalCount - (excuteCount + doneCount + failedCount + abortCount);
}

void CDownloadState::setIsDelete(bool isDelete)
{
    m_isDelete = isDelete;
}

void CDownloadState::setIsOpen(bool isOpen)
{
    m_isOpen = isOpen;
}

bool CDownloadState::isDelete()
{
    return m_isDelete;
}

bool CDownloadState::isOpen()
{
    return m_isOpen;
}

void CDownloadState::setTaskAlias(const QString& alias)
{
    m_taskAlias = alias;
}

QString CDownloadState::getJobIdAlias()
{
    return m_taskAlias;
}

void CDownloadState::SetLocalSaveAsPath(const QString& pszSaveAsPath)
{
    m_localSaveAsPath = pszSaveAsPath;
}

QString CDownloadState::GetLocalSaveAsPath()
{
    return m_localSaveAsPath;
}

bool CDownloadState::isChildItem()
{
    return ((m_isOpen == false) && (m_parentItem != nullptr) && (m_parentItem->getJobId() != -1));
}

QString CDownloadState::getDownloadFullPath(const QString& projSavePath)
{
    // 客户选择了另存 则需要使用客户另存的路径
    QString projPath = this->GetLocalSavePath();
    if(!this->GetLocalSaveAsPath().isEmpty()) {
        projPath = this->GetLocalSaveAsPath();
    }
    return projPath; //todo 适配路径

#if 0
    QString fullPath;
    QStringList pSenceList;
    pSenceList << QFileInfo(this->GetScene()).baseName();

    // 如果是带壳任务的子作业,拿到壳任务并组成一级目录
    if (this->isChildItem()) {
        CDownloadState* parent = this->getParentItem();
        QString parentDir;
        if (pSenceList.count() > 0) {
            parentDir = QString("%1_%2").arg(parent->GetTaskId()).arg(pSenceList.at(0));
        }
        fullPath = QString("%1/%2/").arg(projPath).arg(parentDir);

    }
    else if (this->isParentItem()) {
        QString parentDir;
        if (pSenceList.count() > 0) {
            parentDir = QString("%1_%2").arg(this->GetTaskId()).arg(pSenceList.at(0));
        }
        fullPath = QString("%1/%2/").arg(projPath).arg(parentDir);

    }
    else if (!this->getParentOutputLabel().isEmpty()) {
        fullPath = QString("%1/%2/").arg(projPath).arg(this->getParentOutputLabel());

    }
    else {
        fullPath = projPath;
    }

    return fullPath;
#endif
}

bool CDownloadState::isPhotonJob()
{
    if(m_taskType == "RenderPhoton" && this->isChildItem()) {
        return true;
    }
    return false;
}

QString CDownloadState::getTaskType()
{
    return m_taskType;
}

void CDownloadState::setTaskType(const QString& type)
{
    m_taskType = type;
}

bool CDownloadState::isDownloadExpired()
{
    QString strFinishedDate = this->getFinishedRenderDate().split('.')[0];
    QDateTime finishDate = QDateTime::fromString(strFinishedDate, "yyyy-MM-dd HH:mm:ss");
    QDateTime currentDate = MyConfig.currentDateTime;
    qint64 days = finishDate.daysTo(currentDate);

    if (MyConfig.renderSet.downloadLimit == 0) {
        if (days > MyConfig.renderSet.downloadLimitDay){
            LOGFMTE("[CDownloadState]isDownloadExpired::job %d finished date is %s, current date is %s",
                m_dwTaskId,
                qPrintable(strFinishedDate), qPrintable(currentDate.toString("yyyy-MM-dd HH:mm:ss")));

            return true;
        }       
    }

    return false;
}

void CDownloadState::setStartRenderDate(qint64 startDate)
{
    m_startRenderDate = startDate;
}

QString CDownloadState::getStartRenderDate()
{
    if(m_startRenderDate <= 0 ) {
        return MyConfig.currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
    }

    // 时间转换
    QDateTime starttime = QDateTime::fromMSecsSinceEpoch(m_startRenderDate);
    QString time = starttime.toString("yyyy-MM-dd hh:mm:ss");
    return time;
}

void CDownloadState::setFinishedRenderDate(qint64 finishedDate)
{
    m_finishedRenderDate = finishedDate;
}

QString CDownloadState::getFinishedRenderDate()
{
    if(m_finishedRenderDate <= 0 ) {
        return MyConfig.currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
    }

    // 时间转换
    QDateTime finishedtime = QDateTime::fromMSecsSinceEpoch(m_finishedRenderDate);
    QString time = finishedtime.toString("yyyy-MM-dd hh:mm:ss");
    return time;
}

float CDownloadState::GetTransSpeed()
{
    return m_transSpeed;
}

void CDownloadState::setParentOutputLabel(const QString& outlabel)
{
    m_parentOutputLabel = outlabel;
}

QString CDownloadState::getParentOutputLabel()
{
    return m_parentOutputLabel;
}

void CDownloadState::setRowNum(int rowNum)
{
    m_rowNum = rowNum;
}

int CDownloadState::getRowNum()
{
    return m_rowNum;
}

void CDownloadState::InvalidateTask()
{
    CGuard guard(&m_lock);
    if (GetTransState() == enDownloadCompleted) {// 防止完成的时候没有及时更新
        CGlobalHelp::Response(UM_CHANGE_STATE_VIEW, VIEW_REFRESH_DOWNLOAD_PAGE_ROWS, this->getRowNum());
        CGlobalHelp::Response(UM_UPDATE_TRANS_SPEED, UM_UPDATE_TRANS_SPEED_DOWNLOAD, 0);
        return;
    }

    if (GetTickCount() - m_timeElapsed < 1500)
        return;
    m_timeElapsed = GetTickCount();

    CGlobalHelp::Response(UM_CHANGE_STATE_VIEW, VIEW_REFRESH_DOWNLOAD_PAGE_ROWS, this->getRowNum());
}

QString CDownloadState::getLogFileState()
{
    return m_logFileState;
}

QJsonObject CDownloadState::dumpState(int errCode, const QString& localPath, const QString& remotePath)
{
    t_taskid jobId = getJobId();
    m_logFileState = RBHelper::getReportPathBatch("download", jobId, TYPE_LOG_TRANSMIT, "transmit.json");

    QString transEngine = LocalSetting::getInstance()->getTransEngine();
    QString hostName = LocalSetting::getInstance()->getNetWorkName();

    QJsonObject download_json;
    download_json.insert("keyValue", "type=download");
    download_json.insert("userId", QString::number(MyConfig.accountSet.userId));
    download_json.insert("userName", MyConfig.accountSet.userName);
    download_json.insert("clientVersion", STRFILEVERSION);
    download_json.insert("taskId", QString::number(jobId));
    download_json.insert("scenePath", this->GetOrgScene());
    download_json.insert("localPath", localPath);
    download_json.insert("remotePath", remotePath);
    download_json.insert("engineName", transEngine);
    download_json.insert("lineName", hostName);
    download_json.insert("lanIP", System::getLanIP());
    download_json.insert("macAddress", System::getMacAddress());
    download_json.insert("errorCode", QString::number(errCode));

    RBHelper::saveJsonFile(m_logFileState, download_json);

    return download_json;
}

void CDownloadState::SetParentTaskId(t_taskid id)
{
    m_parentId = id;
}

t_taskid CDownloadState::GetParentTaskId()
{
    return m_parentId;
}

qint64 CDownloadState::getStartDateTime()
{
    QDateTime date = QDateTime::fromString(this->GetStartDate(), "yyyy-MM-dd hh:mm:ss");
    return date.toTime_t();
}

void CDownloadState::SetTaskOutputSize(qint64 size)
{
    m_taskFileSize = size;
}

qint64 CDownloadState::GetTaskOutputSize()
{
    return m_taskFileSize;
}

QString CDownloadState::GetTaskOutputSizeStr()
{
    QString size = RBHelper::ramUsageToString(m_taskFileSize);
    if (size.isEmpty() || size == "-") {
        size = "";
    }

    return size;
}

void CDownloadState::SetCustomerId(qint64 customerId)
{
    m_nCustomerId = customerId;
}

qint64 CDownloadState::GetCustomerId()
{
    return m_nCustomerId;
}

bool CDownloadState::isSubAccountJob()
{
    // 主账号，且任务的uid与登录的uid不同，则判断为子账号
    if (MyConfig.accountSet.isMainAccount()
        && MyConfig.accountSet.userId != this->GetCustomerId()) {
        return true;
    }
    return false;
}

void CDownloadState::setProrityType(int prorityType)
{
    m_prorityType = prorityType;
}

int CDownloadState::getProrityType()
{
    return m_prorityType;
}

void CDownloadState::updateTaskLastExecTime(bool reset)
{
    if (reset) {
        m_taskLastExecTime = 0;
        return;
    }
    m_taskLastExecTime = QDateTime::currentMSecsSinceEpoch();
}

qint64 CDownloadState::getTaskLastExecTime()
{
    return m_taskLastExecTime;
}

bool CDownloadState::isObsoleteData()
{
    // 数据更新时间间隔超过5s 表示无用数据
    return GetTickCount() - m_timeElapsed > 1000 * 5;
}

void CDownloadState::setFrameOutput(const QStringList& frames)
{
    m_frameOutput = frames;
}

QStringList CDownloadState::getFrameOutput()
{
    return m_frameOutput;
}

void CDownloadState::setDownloadJobType(int jobType)
{
    m_jobType = jobType;
}

int CDownloadState::getDownloadJobType()
{
    return m_jobType;
}

bool CDownloadState::isFrameDownloadJob()
{
    return m_jobType == enFrameDownloadJob;
}

void CDownloadState::setFrameName(const QString& name)
{
    m_frameName = name;
}

QString CDownloadState::getFrameName()
{
    return m_frameName;
}

QString CDownloadState::getJobOperateTypeString()
{
    if (m_jobOperateType == enAutoDownload) {
        return QObject::tr("自动");
    }
    return QObject::tr("手动");
}

void CDownloadState::setJobOperateType(int type)
{
    m_jobOperateType = type;
}

int CDownloadState::getJobOperateType()
{
    return m_jobOperateType;
}

void CDownloadState::setFrameFileDownloadState(const QString& remote, int status)
{
    QStringList frames = this->getFrameOutput();
    QString temp = remote.mid(1);// 去除 / 
    if (frames.contains(temp)) {
        m_framesDownloadState[remote] = status;
    }
}

bool CDownloadState::isFrameDownloadFinished()
{
    int finishedCount = 0;
    auto frames = m_framesDownloadState.begin();
    while (frames != m_framesDownloadState.end()){
        if (frames.value() == enDownloadCompleted) {
            finishedCount++;
        }
        frames++;
    }

    return finishedCount == getFrameOutput().count();
}

// 下载文件的大小
void CDownloadState::setFileTotalSize(qint64 total)
{

}

qint64 CDownloadState::getFileTotalSize()
{
    return 0;
}

// 已经传输的大小
void CDownloadState::setFileTransferedSize(qint64 total)
{

}

qint64 CDownloadState::getFileTransferedSize()
{
    return 0;
}

void CDownloadState::setProjectName(const QString& name)
{
    m_projectName = name;
}

QString CDownloadState::getProjectName()
{
    return m_projectName;
}

void CDownloadState::setFinishedTimes(int times)
{
    m_downloadFinishTimes = times;
}

int CDownloadState::getFinishedTimes()
{
    return m_downloadFinishTimes;
}

void CDownloadState::setOutputType(const QStringList& type)
{
    m_outputType = type;
}

QStringList CDownloadState::getOutputType()
{
    return m_outputType;
}

void CDownloadState::addDownloadFile(const QString& outputPath)
{
    if (!m_downloadList.contains(outputPath)) {
        m_downloadList.append(outputPath);
    }
}

QStringList CDownloadState::getDownloadFileList()
{
    return m_downloadList;
}

void CDownloadState::setDownloadFiles(const QStringList& types)
{
    if (types.isEmpty())
        return;

    foreach(QString path, types) {
        this->addDownloadFile(path);
    }
}

void CDownloadState::setDownloadedFile(const QString& filename)
{
    QString name = QFileInfo(filename).fileName();
    if (!m_downloadedFiles.contains(name)) {
        m_downloadedFiles.append(name);
    }
}

int CDownloadState::getDownloadedFileCount()
{
    return m_downloadedFiles.size();
}

void CDownloadState::resetDownloadedInfo()
{
    m_downloadedFiles.clear();
}

bool CDownloadState::isDownloadFinished()
{
    return getDownloadedFileCount() == m_downloadList.size();
}

bool CDownloadState::isDownloadedFileFinished(const QString& filename)
{
    foreach(QString file, m_downloadedFiles) {
        QString dname = QFileInfo(file).fileName();
        QString name = QFileInfo(filename).fileName();

        if (dname.contains(name))
            return true;
    }

    return false;
}

void CDownloadState::resetDownloadList()
{
    m_downloadList.clear();
    m_downloadedFiles.clear();
}