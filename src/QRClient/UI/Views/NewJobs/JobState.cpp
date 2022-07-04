#include "stdafx.h"
#include "JobState.h"
#include "Common/LangTranslator.h"
#include "Common/SystemUtil.h"

JobState::JobState(quint64 taskId, JobState* parentItem)
    : m_taskId(taskId)
{
    m_uploadProgress        = 0;
    m_downloadProgress      = 0;
    m_renderingCount        = 0;
    m_taskLevel             = 0;
    m_shortScene            = "-";
    m_strError              = "";
    m_strSpeed              = "";
    m_camera                = "-";
    m_outputBid             = 0;
    m_taskType              = "";
    m_overTimeRemind        = 12;
    m_bOverTime             = false;
    m_layerName             = "-";
    m_remark                = "";
    m_checkedState          = Qt::Unchecked;
    m_renderTimeCost        = 0;
    m_isAutoDownload        = true;
    m_parentItem            = parentItem;
    m_strStartDate          = "-";
    m_startDate             = -1;
    m_jobType               = UNKOWN_JOB;
    m_isOpen                = false;
    m_accountConsume        = 0.0f;
    m_couponConsume         = 0.0f;
    m_qyCouponConsume       = 0.0f;
    m_taskArrears           = 0.0f;
    m_renderConsume         = 0.0f;
    m_strLocalSavePath      = "";
    m_autoDownPath          = "";
    m_taskLevel             = 0;
    m_taskUserLevel         = 0;
    m_shareMainCapital      = 0;
    m_isDelete              = false;
    m_submitDate            = 0;
    m_cloneOriginalId       = 0;
    m_cloneParentId         = 0;
    m_layerParentId         = 0;
    m_cgId                  = 0;
    m_taskLimit             = 0;
    m_completedDate         = -1;
    m_jobSubChannel         = -1;
    m_isDownloadCompleted   = false;
    m_remark                = "";
    m_downloadCount         = 0;
    m_downloading           = false;
    m_parentId              = 0;
    m_taskOutputSize        = 0;
    m_isClone               = false;
    m_sceneLocalPath        = "";
    m_jobPreState           = enRenderTaskFinished;
    m_transState            = enNothing;
    m_completedCount        = 0;
    m_totalCount            = 0;
    m_renderProgress        = 0;
    m_rowNum                = 0;
    m_downloadFinishTimes   = 0;
    m_uploadedFileSize      = 0;
    m_completedFileSize     = 0;
    m_totalFileSize         = 0;
    m_downloadMode          = 0;
    m_downloadType          = "";
    m_baseTaskInfo          = new BaseTaskInfo;
    m_projectType           = enImageProject;
    m_blockInfo             = nullptr;
    m_munuTaskId            = "";
    m_isSubmitSuccess       = false;
    m_transSpeed            = 0;
    m_totalPixel            = "";
    m_transFileCount        = 0;
    m_perSecondUploadedFile = 1;
}

JobState::~JobState(void)
{
}

qint32 JobState::childCount() const
{
    return m_children.count();
}

qint32 JobState::rowCount()
{
    qint32 count = m_children.count();
    for (qint32 i = 0; i < m_children.count(); i++) {
        JobState* job = this->childItem(i);
        if (job != nullptr)
            count += job->childCount();
    }
    return count;
}

qint32 JobState::row()
{
    QMutexLocker locker(&m_mutex);
    if (m_parentItem) {
        QList<JobState*> jobs = m_parentItem->m_children.values();
        if (!jobs.isEmpty())
            return jobs.indexOf(const_cast<JobState*>(this));
    }
    return 0;
}

void JobState::addChildJobState(JobState * child)
{
    QMutexLocker locker(&m_mutex);
    if (child == nullptr)
        return;

    m_children.insert(child->getTaskId(), child);
}

void JobState::setParentItem(JobState * parent)
{
    m_parentItem = parent;
}

void JobState::removeChildState(JobState * job)
{
    QMutexLocker locker(&m_mutex);
    if (job == nullptr)
        return;

    // 1. job是父节点，且job的父节点和调用remove的节点相同，则直接移除该节点的所有子节点，并通过父节点移除自身
    // 2. 如果job不是父节点，则找到其父节点移除自身，通过调用此方法的节点是不是自身来判断移除子节点
    if (job->IsParentItem() && job->getParentItem() == this) {
        job->clearAllChildren();
        // 找root节点，删除自身
        m_children.remove(job->getTaskId());
    } else if (job->IsChildItem()) {
        if (this->IsParentItem() && job->getParentItem() == this) { // 调用此函数的job也是父节点, 则直接移除子项目
            m_children.remove(job->getTaskId());
            return;
        }
        JobState* parent = job->getParentItem();
        parent->removeChildState(job);

    } else {
        m_children.remove(job->getTaskId());
    }
}

void JobState::clearAllChildren()
{
    QMutexLocker locker(&m_mutex);
    if (!m_children.isEmpty()) {
        foreach(JobState *job, m_children.values()) {
            m_children.remove(job->getTaskId());
        }
    }
}

JobState* JobState::childItem(qint32 row)
{
    QMutexLocker locker(&m_mutex);
    QList<JobState*> jobs = m_children.values();
    if (jobs.isEmpty() || jobs.count() <= row)
        return NULL;
    return jobs.value(row);
}

JobState* JobState::getChild(qint64 taskid)
{
    QMutexLocker locker(&m_mutex);
    if (m_children.isEmpty())
        return NULL;

    return m_children.value(taskid);
}

JobState * JobState::getParentItem()
{
    return m_parentItem;
}

QList<JobState*> JobState::getChildren()
{
    QMutexLocker locker(&m_mutex);
    QList<JobState*> children;
    children = m_children.values();
    return children;
}

QList<qint64> JobState::getChildrenIds()
{
    QMutexLocker locker(&m_mutex);
    QList<qint64> childrenid;
    childrenid = m_children.keys();
    return childrenid;
}

QHash<qint64, JobState*> JobState::getChildJobState()
{
    return m_children;
}

void JobState::setTaskId(qint64 taskId)
{
    m_taskId = taskId;
}

void JobState::setTaskParentId(qint64 pId)
{
    m_parentId = pId;
}

void JobState::setSubJobUserId(qint64 uId)
{
    m_subJobUserId = uId;
}

void JobState::setCloneOriginalId(qint64 orgCloneId)
{
    m_cloneOriginalId = orgCloneId;
}

void JobState::setCloneParentId(qint64 cloneParentId)
{
    m_cloneParentId = cloneParentId;
}

void JobState::setLayerParentId(qint64 layerPid)
{
    m_layerParentId = layerPid;
}

void JobState::setStatus(qint32 status)
{
    m_renderState = status;
}

void JobState::setTransStatus(qint32 status)
{
    m_transState = status;
    invalidateTask();
}

void JobState::setCheckedState(qint32 status)
{
    m_checkedState = status;
}

void JobState::setScene(const QString & strScene)
{
    m_shortScene = strScene;
}

void JobState::setProjectName(const QString & projectName)
{
    m_projectName = projectName;
}

void JobState::setTaskIdAlias(const QString & alias)
{
    m_taskAlias = alias;
}

void JobState::setCamera(const QString & camara)
{
    if (camara == "-" || camara.isEmpty())
        return;
    m_camera = camara;
}

void JobState::setJobSubmitter(const QString & submitter)
{
    m_jobSubmitter = submitter;
}

void JobState::setLayerName(const QString & layerName)
{
    m_layerName = layerName;
}

void JobState::setArtistName(const QString & artistName)
{
    m_artistName = artistName;
}

void JobState::setRemark(const QString & remark)
{
    m_remark = remark;
}

void JobState::setFrameScope(const QString & frameScope)
{
    m_frameScope = frameScope;
}

void JobState::setTaskType(const QString & taskType)
{
    m_taskType = taskType;
}

void JobState::setJobPreStateText(const QString & preState)
{
    
}

void JobState::setErrorString(const QString & errorString)
{
    m_strError = errorString;
}

void JobState::setSpeedString(const QString & speed)
{
    m_strSpeed = speed;
}

void JobState::setDependentTask(const QString & dependTask)
{
    m_strDependentTask = dependTask;
}

void JobState::setRenderTiles(const QString & tiles)
{
    m_tiles = tiles;
}

void JobState::setScenePath(const QString & path)
{
    m_sceneLocalPath = path;
}

void JobState::setOutputLabel(const QString & outputLabel)
{
    m_strOutputLabel = outputLabel;
}

void JobState::setLocalSavePath(const QString & saveAsPath)
{
    m_strLocalSavePath = saveAsPath;
}

void JobState::setAutoDownPath(const QString & downPath)
{
    m_autoDownPath = downPath;
}

void JobState::setUploadFileName(const QString & name)
{
    m_uploadFileName = name;
}

void JobState::setDownloadFileName(const QString & name)
{
}

void JobState::setStartDate(qint64 startDate)
{
    m_startDate = startDate;
}

void JobState::setCompelteDate(qint64 endDate)
{
    m_completedDate = endDate;
}

void JobState::setSubmitDate(qint64 submitDate)
{
    m_submitDate = submitDate;
}

void JobState::setRenderTimeCost(qint64 cost)
{
    m_renderTimeCost = cost;
}

void JobState::setAvgTimeCost(qint64 cost)
{
    m_avgTimeCost = cost;
}

void JobState::setOverTimeRemind(qint64 remind)
{
    m_overTimeRemind = remind;
}

void JobState::setOverTimeStop(qint64 timeStop)
{
    m_overTimeStop = timeStop;
}

void JobState::setStartDateString(const QString & startDate)
{
    m_strStartDate = startDate;
}

void JobState::setCompelteDateString(const QString & endDate)
{
    m_strCompletedDate = endDate;
}

void JobState::setSubmitDateString(const QString & submitDate)
{
    m_strSubmitDate = submitDate;
}

void JobState::setDownloadStopDate(const QString & stopDate)
{
    m_strStopDate = stopDate;
}

void JobState::setUploadProgress(float upload, bool bReset)
{
    if (bReset == true) {
        m_uploadProgress = 0.0f;
        invalidateTask();
        return;
    }

    if (upload > m_uploadProgress) {
        m_uploadProgress = upload;

        if (m_uploadProgress >= 1.0f) {
            m_uploadProgress = 1.0f;
        }        
    }

    invalidateTask();
}

void JobState::setDownloadProgress(qint64 download, bool bReset)
{
    if (bReset == true) {
        m_downloadProgress = 0;
        invalidateTask();
        return;
    }

    if (m_jobType == DOWNLOAD_JOB) {
        m_downloadProgress = download;
        if (m_downloadProgress < 0)
            m_downloadProgress = 0;
        if (m_downloadProgress >= 100)
            m_downloadProgress = 100;
        invalidateTask();
    }
}

void JobState::setGroup(qint32 group)
{
    m_dwGroup = group;
}

void JobState::setDownloadCount(qint32 count)
{
    m_downloadCount = count;
}

void JobState::setMemorySize(qint32 size)
{
    m_memorySize = size;
}

void JobState::setRenderProgress(int excuteCount, int doneCount, int failedCount, int abortCount, int totalCount)
{
    if (totalCount == 0)
        return;
    m_renderProgress = (double)(doneCount) / totalCount;

    m_dwExcuteCount = excuteCount;
    m_dwDoneCount = doneCount;
    m_dwFailedCount = failedCount;
    m_dwAbortCount = abortCount;
    m_dwWaitCount = totalCount - (excuteCount + doneCount + failedCount + abortCount);
    if (m_dwWaitCount < 0) m_dwWaitCount = 0;
}

void JobState::setProgress(qint32 completed, qint32 total)
{
    m_completedCount = completed;
    m_totalCount = total;
    invalidateTask();
}

void JobState::setTaskLimit(qint32 limit)
{
    m_taskLimit = limit;
}

void JobState::setCgId(qint32 cgId)
{
    m_cgId = cgId;
}

void JobState::setTaskOutputSize(qint64 size)
{
    m_taskOutputSize = size;
}

void JobState::setTaskLevel(qint32 level)
{
    m_taskLevel = level;
}

void JobState::setTaskUserLevel(qint32 userLevel)
{
    m_taskUserLevel = userLevel;
}

void JobState::setShareMainCapital(qint32 share)
{
    m_shareMainCapital = share;
}

void JobState::setJobOperable(qint32 operable)
{
    m_jobOperable = operable;
}

void JobState::setOutputBid(qint32 bid)
{
    m_outputBid = bid;
}

void JobState::setJobSubChannel(qint32 channel)
{
    m_jobSubChannel = channel;
}

void JobState::setJobType(qint32 jobType)
{
    m_jobType = jobType;
}

void JobState::setUserAccountConsume(float accountConsume)
{
    m_accountConsume = accountConsume;
}

void JobState::setCouponConsume(float couponConsume)
{
    m_couponConsume = couponConsume;
}

void JobState::setQYCouponConsume(float couponConsume)
{
    m_qyCouponConsume = couponConsume;
}

void JobState::setTaskArrears(float taskArrears)
{
    m_taskArrears = taskArrears;
}

void JobState::setTaskRenderConsume(float renderConsume)
{
    m_renderConsume = renderConsume;
}

void JobState::setSpeed(float fSpeed)
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

    invalidateTask();
}

void JobState::setDownloadMode(qint32 mode)
{
    m_downloadMode = mode;
}

void JobState::setDownloadPathCode(qint32 code)
{
    m_downloadCode = code;
}

void JobState::setIsOpen(bool isOpen)
{
    m_isOpen = isOpen;
}

void JobState::setIsDelete(bool isDelete)
{
    m_isDelete = isDelete;
}

void JobState::setIsCloneTask(bool isClone)
{
    m_isClone = isClone;
}

void JobState::setDownloadError(bool error)
{
    m_downloadError = error;
}

void JobState::setAutoDownload(bool isAuto)
{
    m_isAutoDownload = isAuto;
}

void JobState::setDownloadCompleted(bool compelete)
{
    m_isDownloadCompleted = compelete;
}

void JobState::setIsOverTime(bool overTime)
{
    m_bOverTime = overTime;
}

void JobState::setDependency(bool isDepend)
{
    m_bDependency = isDepend;
}

qint64 JobState::getTaskId()
{
    return m_taskId;
}

qint64 JobState::getTaskParentId()
{
    return m_parentId;
}

qint64 JobState::getSubJobUserId()
{
    return m_subJobUserId;
}

qint64 JobState::getCloneOriginalId()
{
    return m_cloneOriginalId;
}

qint64 JobState::getCloneParentId()
{
    return m_cloneParentId;
}

qint64 JobState::getLayerParentId()
{
    return m_layerParentId;
}

qint32 JobState::getStatus()
{
    return m_renderState;
}

qint32 JobState::getTransStatus()
{
    return m_transState;
}

qint32 JobState::getCheckedState()
{
    return m_checkedState;
}

QString JobState::getScene()
{
    return m_shortScene;
}

QString JobState::getProjectName()
{
    return m_projectName;
}

QString JobState::getTaskIdAlias()
{
    return m_taskAlias;
}

QString JobState::getCamera()
{
    // 壳任务没有相机，返回子作业的相机
    if (this->IsParentItem()) {
        QList<JobState*> childs = this->getChildren();
        if (!childs.isEmpty()) {
            JobState *child = childs.at(0);
            return child->getCamera();
        }
    }
    return m_camera;
}

QString JobState::getJobSubmitter()
{
    return m_jobSubmitter;
}

QString JobState::getLayerName()
{
    return m_layerName;
}

QString JobState::getArtistName()
{
    return m_artistName;
}

QString JobState::getRemark()
{
    return m_remark;
}

QString JobState::getFrameScope()
{
    return m_frameScope;
}

QString JobState::getTaskType()
{
    return m_taskType;
}

QString JobState::getJobPreStateText()
{
    return m_jobPreStateText;
}

QString JobState::getErrorString()
{
    return m_strError;
}

QString JobState::getSpeedString()
{
    return m_strSpeed;
}

QString JobState::getDependentTask()
{
    return m_strDependentTask;
}

QString JobState::getRenderTiles()
{
    if (m_tiles.isEmpty())
        return "-";

    return m_tiles + QObject::tr("台机器");
}

float JobState::getConsumFeeNum()
{
    float renderConsume = this->getTaskRenderConsume();
    float renderCoupon = this->getCouponConsume();

    float totalConsume = 0.0f;
    // 全部合并到一起
    totalConsume += renderConsume;
    totalConsume += renderCoupon;


    return totalConsume;
}

QString JobState::getConsumpFee()
{
    if (MyConfig.userSet.isChildAccount() && MyConfig.accountSet.hideJobCharge)
        return "-";

    float totalConsume = getConsumFeeNum();
    QString consume = QString("%1").arg(QString::number(totalConsume, 'f', 3));

    return totalConsume == 0 ? "" : consume;
}

QString JobState::getScenePath()
{
    return m_orgScenePath;
}

QString JobState::getOutputLabel()
{
	if (this->IsParentItem()) {
		foreach(JobState* child, this->getChildren()) {
			if (child->getTaskType().toLower() == "render") {
				return child->getOutputLabel();
			}
		}
	}
	QString plat = WSConfig::getInstance()->get_host_url();
	if (plat == "https://task-beta.dayancloud.com")
	{
		return QString::number(m_parentId) + QString::fromStdString("/") + m_strOutputLabel;
	}
	else
	{
		if (m_taskType == "Triangulation")
		{
			return QString::number(m_parentId) + QString::fromStdString("/") + "at_result";
		}
		/*  return m_strOutputLabel;   //平台数据输出路径层级修改，添加了壳ID*/
		//  return QString::number(m_parentId) + QString::fromStdString("/") + m_strOutputLabel;  
		return QString::number(m_parentId) + QString::fromStdString("/") + "render_result";  //存储后缀改未render_result
	}
}

QString JobState::getLocalSavePath()
{
    return m_strLocalSavePath;
}

QString JobState::getAutoDownPath()
{
    return m_autoDownPath;
}

QString JobState::getOutputPath(const QString & projectSavePath)
{
    // 如果不启用下载新建文件夹，则直接返回根目录
    if (this->getDownloadMode() == DOWNLOAD_WITHOUT_DIR) {
        return projectSavePath;
    }

    QString label = getOutputLabel();
    QString output = "";

#if 0
    if (m_subTaskState.count() > 0) {
        output = QString("%1%2_%3").arg(projectSavePath).arg(this->GetTaskId()).arg(label);
    }else {
        output = QString("%1%2").arg(projectSavePath).arg(label);
    }
#endif
    output = QString("%1/%2").arg(projectSavePath).arg(label);

    return output;
}

QString JobState::getUploadFileName()
{
    return m_uploadFileName;
}

QString JobState::getDownloadFileName()
{
    return QString();
}

QString JobState::getDownloadFullPath(const QString & projSavePath)
{
    // 客户选择了另存 则需要使用客户另存的路径
    QString projPath = this->getAutoDownPath();
    if (!this->getLocalSavePath().isEmpty()) {
        projPath = this->getLocalSavePath();
    }
    return projPath; //todo 适配路径
}

qint64 JobState::getStartDate()
{
    if (m_startDate == 0 && m_jobType != UPLOAD_JOB) {
        return this->getSubmitDate();
    }
    return m_startDate;
}

qint64 JobState::getCompelteDate()
{
    return m_completedDate;
}

qint64 JobState::getSubmitDate()
{
    return m_submitDate;
}

qint64 JobState::getRenderTimeCost()
{
    return m_renderTimeCost;
}

qint64 JobState::getAvgTimeCost()
{
    return m_avgTimeCost;
}

qint64 JobState::getOverTimeRemind()
{
    return m_overTimeRemind;
}

qint64 JobState::getOverTimeStop()
{
    return m_overTimeStop;
}

QString JobState::getStartDateString(const QString& format)
{
    if (getStartDate() <= 0) {
        QDateTime starttime = QDateTime::fromString(m_strStartDate, "yyyy-MM-dd hh:mm:ss");
        return starttime.toString(format);
    }
        
    // 时间转换
    QDateTime starttime = QDateTime::fromMSecsSinceEpoch(getStartDate());
    QString time = starttime.toString(format);
    return time;
}

QString JobState::getCompelteDateString(const QString& format)
{
    if (m_completedDate <= 0) {
        return "-";
    }

    // 时间转换
    QDateTime endtime = QDateTime::fromMSecsSinceEpoch(m_completedDate);
    return endtime.toString(format);
}

QString JobState::getSubmitDateString()
{
    return m_strSubmitDate;
}

QString JobState::getDownloadStopDate()
{
    return m_strStopDate;
}

QString JobState::getRenderTimeCostString()
{
    if (this->getRenderTimeCost() <= 0) {
        return "-";
    }
    return RBHelper::timeCostToString(this->getRenderTimeCost());
}

float JobState::getUploadProgress()
{
    return m_uploadProgress;
}

qint64 JobState::getDownloadProgress()
{
    return m_downloadProgress;
}

double JobState::getRenderProgress()
{
    return m_renderProgress;
}

qint32 JobState::getCompleteFileCount()
{
    return m_completedCount;
}

qint32 JobState::getTotalFileCount()
{
    return m_totalCount;
}

qint32 JobState::getGroup()
{
    return m_dwGroup;
}

qint32 JobState::getDownloadCount()
{
    return m_downloadCount;
}

qint32 JobState::getMemorySize()
{
    return m_memorySize;
}

qint32 JobState::getTaskLimit()
{
    return m_taskLimit;
}

qint32 JobState::getCgId()
{
    return m_cgId;
}

qint64 JobState::getTaskOutputSize()
{
    return m_taskOutputSize;
}

qint32 JobState::getTaskLevel()
{
    return m_taskLevel;
}

qint32 JobState::getTaskUserLevel()
{
    return m_taskUserLevel;
}

qint32 JobState::getShareMainCapital()
{
    return m_shareMainCapital;
}

qint32 JobState::getJobOperable()
{
    return m_jobOperable;
}

qint32 JobState::getOutputBid()
{
    // 任务的userid和登录的账户ID不同时
    if (this->isSubAccountJob()) {
        qint32 bid = MyConfig.storageSet.getChildOutputBid(this->getSubJobUserId());
        if (bid == 0) {
            LOGFMTE("[JobState::getOutputBid] User (%I64d) output bid not exist!", this->getSubJobUserId());
        } else {
            return bid;
        }
    }

    return m_outputBid;
}

qint32 JobState::getJobSubChannel()
{
    return m_jobSubChannel;
}

qint32 JobState::getJobType()
{
    return m_jobType;
}

qint32 JobState::getSceneType()
{
    return RBHelper::typeOfScene(this->getScene());
}

qint32 JobState::getDownloadMode()
{
    return m_downloadMode;
}

qint32 JobState::getDownloadPathCode()
{
    return m_downloadCode;
}

qint32 JobState::getDownloadCompleteChildCount()
{
    int count = 0;
    if (this->IsParentItem()) {
        foreach(JobState* child, getChildren()) {
            if (child->getTransStatus() == enDownloadCompleted)
                count++;
        }
    }
    return count;
}

float JobState::getUserAccountConsume()
{
    return m_accountConsume;
}

float JobState::getCouponConsume()
{
    return m_couponConsume;
}

float JobState::getQYCouponConsume()
{
    return m_qyCouponConsume;
}

float JobState::getTaskArrears()
{
    return m_taskArrears;
}

float JobState::getTaskRenderConsume()
{
    return m_renderConsume;
}

float JobState::getTotalConsume()
{
    // 总费用=余额+渲染券 用于排序
    float renderConsume = this->getTaskRenderConsume();
    float renderCoupon = this->getCouponConsume();
    return renderConsume + renderCoupon;
}

float JobState::getTransSpeed()
{
    return m_transSpeed;
}

bool JobState::isOpen()
{
    return m_isOpen;
}

bool JobState::isDelete()
{
    return m_isDelete;
}

bool JobState::isCloneTask()
{
    return m_isClone;
}

bool JobState::getDownloadError()
{
    return m_downloadError;
}

bool JobState::isAutoDownload()
{
    return m_isAutoDownload;
}

bool JobState::isDownloadCompleted()
{
    return true;

    if (this->IsParentItem()) {
        QList<JobState*> jobs = this->getChildren();
        int completed = 0;
        foreach(JobState* job, jobs) {
            if (job->isDownloadCompleted()) {
                completed++;
            }
        }
        return completed == jobs.count();
    }
    return m_isDownloadCompleted;
}

bool JobState::isOverTime()
{
    return m_bOverTime;
}

bool JobState::isDependency()
{
    return m_bDependency;
}

bool JobState::isSupportScene()
{
    SCENE_TYPE type = RBHelper::typeOfScene(this->getScenePath());
    return type != CG_ID_UNKNOWN;
}

bool JobState::isChecked()
{
    return m_checkedState == Qt::Checked;
}

bool JobState::hasPhotonJob()
{
    int photonCount = 0;
    QList<JobState*> list = this->getChildren();
    foreach(JobState* state, list) {
        if (state->getTaskType().toLower() == "renderphoton") {
            photonCount++;
        }
    }
    return photonCount > 0;
}

bool JobState::isPhotonJob()
{
    if (m_taskType == "RenderPhoton" && this->IsChildItem()) {
        if (this->isContainsMajor()) { // 1. 任务是光子且是子任务，2. 壳任务中包含主图任务，则可以判断该任务是主图的光子，主图光子不可下载
            return true;
        }
    }
    return false;
}

bool JobState::isContainsMajor()
{
    // 判断壳任务下面是否存在主图任务
    if (!this->IsChildItem()) {
        return m_taskType == "Render";
    }

    int majorCount = 0;
    JobState* parent = this->getParentItem();
    if (parent != NULL) {
        QList<JobState*> list = parent->getChildren();
        foreach(JobState* state, list) {
            if (state->getTaskType() == "Render") {
                majorCount++;
            }
        }
    }

    return majorCount > 0;
}

bool JobState::isContainsRebuildJob()
{
    // 判断壳任务下面是否存在主图任务
    if (!this->IsChildItem()) {
        return m_taskType == "Render";
    }

    int rebuildCount = 0;
	QList<JobState*> list = getChildren();
	foreach(JobState* state, list) {
		if (state->getTaskType().toLower() == "rebuild") {
			rebuildCount++;
		}
	}

    return rebuildCount > 0;
}

bool JobState::isContains3DTilesJob()
{
    int tilesCount = 0;
    QList<JobState*> list = getChildren();
    foreach(JobState* state, list) {
        if (state->is3DTilesJob()) {
            tilesCount++;
        }
    }
    
    return tilesCount > 0;
}

bool JobState::isContainsRenderJob()
{
    int renderCount = 0;
    QList<JobState*> list = getChildren();
    foreach(JobState* state, list) {
        if (state->isMajorJob()) {
            renderCount++;
        }
    }

    return renderCount > 0;
}

void JobState::invalidateTask()
{
    if (getJobType() == UPLOAD_JOB) {
        static qint64 timeElapsed;
        CGuard guard(&m_lockSpeed);
        if (this->getTransStatus() == enUploadCompleted) {
            CGlobalHelp::Response(UM_CHANGE_STATE_VIEW, VIEW_REFRESH_UPLOAD_PAGE_ROWS, this->getRowNum());
            CGlobalHelp::Response(UM_UPDATE_TRANS_SPEED, UM_UPDATE_TRANS_SPEED_UPLOAD, 0);
            return;
        }

        if (GetTickCount() - timeElapsed < 1000)
            return;
        timeElapsed = GetTickCount();

        CGlobalHelp::Response(UM_CHANGE_STATE_VIEW, VIEW_REFRESH_UPLOAD_PAGE_ROWS, this->getRowNum());
    
    } else if (getJobType() == DOWNLOAD_JOB /*|| getJobType() == RENDER_JOB*/) {
        static qint64 timeElapsed;
        CGuard guard(&m_lockSpeed);
        if (getTransStatus() == enDownloadCompleted) {// 防止完成的时候没有及时更新
            CGlobalHelp::Response(UM_UPDATE_TRANS_SPEED, UM_UPDATE_TRANS_SPEED_DOWNLOAD, 0);
            return;
        }

        if (GetTickCount() - timeElapsed < 1000)
            return;
        timeElapsed = GetTickCount();
    }
}

void JobState::setPreTaskStatus(int preState)
{
    m_jobPreState = preState;
}

int JobState::getPreTaskStatus()
{
    return m_jobPreState;
}

void JobState::setPreTaskStatusText(const QString& text)
{
    m_jobPreStateText = text;
}

QVariant JobState::getJobFeeTips()
{
    if (MyConfig.userSet.isChildAccount() && MyConfig.accountSet.hideJobCharge)
        return QVariant("-");

    float renderConsume  = this->getTaskRenderConsume();
    float renderCoupon   = this->getCouponConsume();
    float renderArrears  = this->getTaskArrears();
    float renderQYCoupon = this->getQYCouponConsume();
    float userAccountConsum = this->getUserAccountConsume();

    QString consume;

    if (userAccountConsum > 0) {
#ifdef FOXRENDERFARM
        consume = "$: ";
#else
        consume = "余额：";
        // consume = "￥: ";
#endif
        consume += QString::number(userAccountConsum);
        consume += " ";
    }

    int ptid = LocalSetting::getInstance()->getDefaultsite();
    if (WSConfig::getInstance()->is_qingyun_platform(ptid)) {
        if (renderCoupon > 0) {
            consume += QObject::tr(" 青云券：");
            consume += QString::number(renderCoupon);
            consume += " ";
        }
    } else {
        if (renderCoupon > 0) {
            consume += QObject::tr(" 代金券：");
            consume += QString::number(renderCoupon);
            consume += " ";
        }
    }

    if (renderArrears > 0) {
        consume += QObject::tr(" 欠费：");
        consume += QString::number(this->getTaskArrears());
    }

    return QVariant(consume);
}

bool JobState::isDownloadExpired()
{
    QString strFinishedDate = this->getCompelteDateString().split('.')[0];
    if (this->IsParentItem()) {
        strFinishedDate = this->getSubmitDateString().split('.')[0];
    }
    QDateTime startDate = QDateTime::fromString(strFinishedDate, "yyyy-MM-dd HH:mm:ss");
    QDateTime currentDate = MyConfig.currentDateTime;
    qint64 days = startDate.daysTo(currentDate);

	qDebug() << days;
	qDebug() << MyConfig.renderSet.downloadLimitDay;
	qDebug() << MyConfig.renderSet.downloadLimit;

#ifndef _DEBUG
    if (days > MyConfig.renderSet.downloadLimitDay && MyConfig.renderSet.downloadLimit == 0) {
        return true;
    }
#endif // !_DEBUG

    return false;
}

QList<JobState*> JobState::getCanDownloadChildJob()
{
    QList<JobState*> childJobs;
    if (!this->IsParentItem()) {
        return childJobs;
    }

    QList<JobState*> jobs = this->getChildren();
    foreach(JobState* job, jobs) {
        if (job && !job->isDownloadExpired()) {
            childJobs.append(job);
        }
    }
    return childJobs;
}

QList<JobState*> JobState::getExpiredChildJob()
{
    QList<JobState*> expiredJobs;
    if (!this->IsParentItem()) {
        return expiredJobs;
    }

    QList<JobState*> jobs = this->getChildren();
    foreach(JobState* job, jobs) {
        if (job && job->isDownloadExpired()) {
            expiredJobs.append(job);
        }
    }
    return expiredJobs;
}

QList<JobState*> JobState::getSubTasks()
{
    return QList<JobState*>();
}

bool JobState::showPreState()
{
    // 如果是子任务 直接返回false
    if (this->IsChildItem()) {
        return false;
    }

    // MAX任务，当预处理状态不为空，渲染状态为0 并且是预处理状态 0或8 时，显示显示任务的预处理状态
    return /*this->IsParentItem() && */
        !m_jobPreStateText.isEmpty() &&
        getStatus() == enRenderTaskWaitting &&
        (getPreTaskStatus() == enRenderTaskWaitting || getPreTaskStatus() == enRenderTaskPreRendering);
}

QString JobState::getStatusText()
{
    QString wstrTip;
    int status = enNothing;

    // 只显示上传和渲染的状态，下载的状态用渲染状态
    if (m_jobType == RENDER_JOB) {
        status = m_renderState;
    } else if (m_jobType == UPLOAD_JOB || m_jobType == DOWNLOAD_JOB) {
        status = m_transState;
    }

    if(status == enRenderTaskSubmitFailed)
        return QObject::tr("提交失败");
    else if(status == enRenderTaskSubmitting)
        return QObject::tr("提交中");

    /*if (showPreState()) {
        if ((m_jobType == RENDER_JOB)
            && m_jobPreState != enRenderTaskFinished)
            return getPreTaskStatusText();
    }*/

    if (m_jobType == RENDER_JOB && !this->isATFinished()) {
        return getATStateText();
    } 
    
    if (m_jobType == RENDER_JOB && !this->isRebuildFinished()) {
        return getRebuildStateText();
    }

    if (m_jobType == RENDER_JOB && !this->isProductionFinished()) {
        return getProductionStateText();
    }

    if (m_jobType == RENDER_JOB && this->isRebuildFinished() && this->isProductionFinished()) {
        return getProductionStateText();
    }

    switch (status) {
    case enTransUploading:
        wstrTip = QObject::tr("正在上传...");
        break;
    case enUploadCompleted:
        wstrTip = QObject::tr("上传完成");
        break;
    case enUploadUnfinished:
        wstrTip = QObject::tr("上传未完成");
        break;
    case enUploadError:
        wstrTip = QObject::tr("上传出错");
        break;
    case enUploadWaiting:
        wstrTip = QObject::tr("上传等待");
        break;
    case enUploadQueued:
        wstrTip = QObject::tr("上传等待");
        break;
    case enUploadRetrying:
        wstrTip = QObject::tr("上传重试");
        break;
    case enUploadCheckCompleted:
        wstrTip = QObject::tr("上传校验完成");
        break;
    case enUploadStop:
        wstrTip = QObject::tr("上传停止");
        break;
    case enRenderTaskPreRendering:
        wstrTip = QObject::tr("准备生产");
        break;
    case enRenderTaskFinished:
        wstrTip = QObject::tr("生产完成");
        break;
    case enRenderTaskFailed:
        wstrTip = QObject::tr("生产失败");
        break;
    case enRenderTaskStop:
        wstrTip = QObject::tr("生产停止");
        break;
    case enRenderTaskRendering:
        wstrTip = QObject::tr("正在生产");
        break;
    case enRenderTaskAbandon:
        wstrTip = QObject::tr("放弃生产");
        break;
    case enRenderTaskFinishTest:
        wstrTip = QObject::tr("测试完成");
        break;
    case enRenderTaskAnalyse:
        wstrTip = QObject::tr("分析中");
        break;
    case enRenderTaskFinishHasFailed:
        wstrTip = QObject::tr("生产失败");
        break;
    case enRenderTaskWaitting:
        wstrTip = QObject::tr("等待生产");
        break;
    case enRenderTaskArrearageStop:
        wstrTip = QObject::tr("欠费停止");
        break;
    case enRenderOverTimeStop:
        wstrTip = QObject::tr("超时停止");
        break;
    case enRenderUserStop:
        wstrTip = QObject::tr("用户停止");
        break;
    case enRenderStateUpdating:
        wstrTip = QObject::tr("状态更新中");
        break;
    case enRenderTaskSubmitting:
        wstrTip = QObject::tr("提交中");
        break;
    case enRenderTaskSubmitFailed:
        wstrTip = QObject::tr("提交失败");
        break;
    case enRenderATWaiting:
        wstrTip = QObject::tr("空三计算中");
        break;
    case enRenderATStop:
        wstrTip = QObject::tr("取消空三");
        break;
    case enRenderATFailed:
        wstrTip = QObject::tr("空三失败");
        break;
    case enRenderATRendering:
        wstrTip = QObject::tr("空三计算中");
        break;
    case enRenderATFinished:
        wstrTip = QObject::tr("空三完成");
        break;
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
        wstrTip = QObject::tr("下载排队");
        break;
    default:
        wstrTip = "-";
        break;
    }
    if (m_jobType == DOWNLOAD_JOB && status == enDownloadUnfinish) {
        wstrTip = QObject::tr("下载未完成");
    }

    return wstrTip;

}

QString JobState::getPreTaskStatusText()
{
    QString text = "-";
    switch (m_jobPreState) {
    case enRenderTaskWaitting:
        text = QObject::tr("等待预处理");
        break;
    case enRenderTaskPreRendering:
        text = QObject::tr("正在预处理");
        break;
    case enRenderTaskStop:
        text = QObject::tr("停止预处理");
        break;
    case enRenderTaskFinished:
        text = QObject::tr("预处理完成");
        break;
    case enRenderTaskFailed:
        text = QObject::tr("预处理失败");
        break;
    case enRenderUserStop:
        text = QObject::tr("用户停止");
        break;
    case enRenderTaskArrearageStop:
        text = QObject::tr("欠费停止");
        break;
    case enRenderOverTimeStop:
        text = QObject::tr("超时停止");
        break;
    case enRenderTaskAbandon:
        text = QObject::tr("任务放弃");
        break;
    case enRenderTaskFinishTest:
        text = QObject::tr("测试完成");
        break;
    case enRenderTaskAnalyse:
        text = QObject::tr("分析中");
        break;
    case enRenderStateUpdating:
        text = QObject::tr("状态更新中");
        break;
    default:
        text = m_jobPreStateText;
        break;
    }
    return text;
}

bool JobState::isSubAccountJob()
{
    // 主账号，且任务的uid与登录的uid不同，则判断为子账号
    if (MyConfig.accountSet.isMainAccount()
        && MyConfig.accountSet.userId != this->getSubJobUserId()) {
        return true;
    }
    return false;
}

QColor JobState::getRowColor()
{
    int status = this->getStatus();
    int preState = this->getPreTaskStatus();
    QColor textColor(qRgb(0x8f, 0xf9, 0x8a));
    if (status == JobState::enRenderTaskFailed || status == JobState::enRenderTaskSubFailed || status == JobState::enRenderTaskFailed
        || preState == JobState::enRenderTaskFailed) {
        textColor = QColor(qRgb(0xe2, 0x5c, 0x59));
    } else if (this->isOverTime()) {
        // textColor = QColor(qRgb(0xff, 0x8a, 0x0));
    } else if (status == JobState::enRenderTaskArrearageStop) {
        textColor = QColor(qRgb(0xff, 0x8a, 0x0));
    } else if (status == JobState::enRenderOverTimeStop) {
        textColor = QColor(qRgb(0xff, 0x8a, 0x0));
    } else if (status == JobState::enRenderTaskFinishTest) {
        textColor = QColor(qRgb(0x2, 0xa6, 0xbc));
    } else if (status == JobState::enRenderTaskStop || status == JobState::enRenderTaskAbandon) {
        textColor = QColor(qRgb(0x66, 0x66, 0x66));
    } else if (status == enRenderTaskWaitting) {
        if (!IsParentItem())
            textColor = QColor(qRgb(0x8f, 0xf9, 0x8a));
    }/*else if(this->isChecked()) {
     textColor = QColor(qRgb(255, 255, 255));
     }*/
    return textColor;
}

void JobState::setStopType(JobState::enTaskStopReason type)
{
    m_stopReason = type;
}

JobState::enTaskStopReason JobState::getStopType()
{
    if (this->IsParentItem()) {
        QList<JobState*> children = this->getChildren();
        QMap<int, int> stopReasons;
        foreach(JobState* job, children) {
            stopReasons[job->getStopType()]++;
        }

        if (stopReasons[JobState::enUserStop] > 0)
            return JobState::enUserStop;
        else if (stopReasons[JobState::enAdminStop] > 0)
            return JobState::enAdminStop;
        else
            return JobState::enUnknownError;
    }
    return m_stopReason;
}

QString JobState::getStopReasonText()
{
    QString txt = "异常停止";
    switch (this->getStopType())
    {
    case JobState::enUnknownError:
        txt = QObject::tr("异常停止");
        break;
    case JobState::enUserStop:
        txt = QObject::tr("用户停止");
        break;
    case JobState::enAdminStop:
        txt = QObject::tr("后台停止");
        break;
    case JobState::enOverduebillStop:
        // txt = QObject::tr("(欠费)");
        break;
    default:
        txt = QObject::tr("异常停止");
        break;
    }

    return txt;
}

QString JobState::getLogFileState()
{
    return m_logFileState;
}

QJsonObject JobState::dumpState(int errCode, const QString & localPath, const QString & remotePath)
{
    t_taskid jobId = getTaskId();
    m_logFileState = RBHelper::getReportPathBatch("upload", jobId, TYPE_LOG_TRANSMIT, "transmit.json");

    QString transEngine = LocalSetting::getInstance()->getTransEngine();
    QString hostName = LocalSetting::getInstance()->getNetWorkName();

    QJsonObject upload_json;
    upload_json.insert("keyValue", "type=upload");
    upload_json.insert("userId", QString::number(MyConfig.accountSet.userId));
    upload_json.insert("userName", MyConfig.accountSet.userName);
    upload_json.insert("clientVersion", STRFILEVERSION);
    upload_json.insert("taskId", QString::number(jobId));
    upload_json.insert("scenePath", this->getScenePath());
    upload_json.insert("localPath", localPath);
    upload_json.insert("remotePath", remotePath);
    upload_json.insert("engineName", transEngine);
    upload_json.insert("lineName", hostName);
    upload_json.insert("lanIP", System::getLanIP());
    upload_json.insert("macAddress", System::getMacAddress());
    upload_json.insert("errorCode", QString::number(errCode));

    RBHelper::saveJsonFile(m_logFileState, upload_json);

    return upload_json;
}

QList<qint64> JobState::getCanResubChildTaskList()
{
    QList<qint64> list;
    QList<JobState*> children = this->getChildren();
    foreach(JobState* child, children) {
        if (child->TaskCanReSub()) {
            list.append(child->getTaskId());
        }
    }
    return list;
}

bool JobState::isPhotonJobProccessEnable()
{
    // 如果不是父任务，直接返回true，显示光子进度
    if (!this->IsParentItem()) {
        if (this->getTaskType().toLower() == "renderphoton") {
            return true;
        }  
    }

    int stopCount = 0;
    int renderingCount = 0;
    int finishCount = 0;
    int waitCount = 0;
    QList<JobState*> list = this->getPhotonJobs();
    foreach(JobState* state, list) {
        if (state->getStatus() == JobState::enRenderTaskStop) {
            stopCount++;
        }
        if (state->IsTaskRendering()) {
            renderingCount++;
        }
        if (state->TaskIsFinished()) {
            finishCount++;
        }
        if (state->getStatus() == JobState::enRenderFrameWaiting) {
            waitCount++;
        }
    }

    // 如果光子任务和大任务都停止，显示光子进度
    if (this->getStatus() == JobState::enRenderTaskStop && stopCount > 0)
        return true;
    // 如果大任务在渲染，光子渲染，显示光子进度
    if (this->IsTaskRendering() && renderingCount > 0)
        return true;
    // 如果光子等待，显示光子进度
    if (waitCount > 0)
        return true;
    // 光子渲染完成, 则显示主图
    if (finishCount > 0)
        return false;

    return false;
}

bool JobState::isMajorJobProccessEnable()
{
    // 如果不是父任务 直接返回
    if (!this->IsParentItem()) {
        return IsTaskRendering();
    }

    int majorRendering = 0;
    QList<JobState*> list = this->getMajorJobs();
    foreach(JobState* state, list) {
        if (state->IsTaskRendering()) {
            majorRendering++;
        }
    }

    return majorRendering > 0;
}

QList<JobState*> JobState::getPhotonJobs()
{
    QList<JobState*> jobs;
    QList<JobState*> list = this->getChildren();
    foreach(JobState* state, list) {
        if (state->getTaskType().toLower() == "renderphoton") {
            jobs.append(state);
        }
    }

    return jobs;
}

QList<JobState*> JobState::getMajorJobs()
{
	QList<JobState*> jobs;
	QList<JobState*> list = this->getChildren();
	foreach(JobState* state, list) {
		QString type = state->getTaskType().toLower();
		if (state->getTaskType().toLower() == "render" || state->getTaskType().toLower() == "tiff" || state->getTaskType().toLower() == "3dtiles") {//render(before)    //  "Cesium" ->3DTiles, "Render" ->格式转换,  "Rebuild"->重建,  "Triangulation" -> 空三 , "TIFF"-> TIFF, 后台任务流程更改 20220628
			jobs.append(state);
		}
	}

	return jobs;
}

bool JobState::isPhotonJobRendering()
{
    // 如果不是父任务，直接返回true，显示光子进度
    if (!this->IsParentItem()) {
        if (this->getTaskType().toLower() == "renderphoton" 
             && (this->IsTaskRendering() || this->IsWaitting())) {
            return true;
        }
    }

    int renderingCount = 0;
    QList<JobState*> list = this->getPhotonJobs();
    foreach(JobState* state, list) {
        if (state->IsTaskRendering()) {
            renderingCount++;
        }
    }

    return renderingCount > 0;
}

void JobState::setRowNum(qint32 row)
{
    m_rowNum = row;
}

qint32 JobState::getRowNum()
{
    return m_rowNum;
}

void JobState::setFinishedTimes(int times)
{
    m_downloadFinishTimes = times;
}

int JobState::getFinishedTimes()
{
    return m_downloadFinishTimes;
}

void JobState::setProgressByFileSize(qint64 completed, qint64 total)
{
    QMutexLocker locker(&m_mutex);
    m_completedFileSize = completed;
    m_totalFileSize = total;
}

void JobState::setUploadedFileSize(qint64 uploaded)
{
    QMutexLocker locker(&m_mutex);
    m_uploadedFileSize = uploaded;
}

float JobState::getUploadProgressByFileSize()
{
    QMutexLocker locker(&m_mutex);
    // if (m_totalFileSize > 0 && m_uploadedFileSize > 0 && m_uploadedFileSize < m_totalFileSize && m_uploadedFileSize > m_completedFileSize) {
    //     return m_uploadedFileSize * 1.0f / m_totalFileSize * 1.0f;
    // }

    if (m_totalFileSize > 0 && m_completedFileSize > 0) {
        return m_completedFileSize * 1.0f / m_totalFileSize * 1.0f;
    } else {
        return 0;
    }

    return getUploadProgress();
}

qint64 JobState::getTotalFileSize()
{
    QMutexLocker locker(&m_mutex);
    return m_totalFileSize;
}

bool JobState::isMemberTask()
{
    qint64 startTime = this->getStartDate();
    qint64 compeleteTime = this->getCompelteDate();

    return MyConfig.accountSet.isMemberTask(startTime);
}

void JobState::updateTaskType()
{
    // 如果任务状态不是完成状态，则修改任务类型
    if (this->TaskIsFinished())
        return;
    this->setJobType(RENDER_JOB);
}

void JobState::setCoordCount(const QString& count)
{
    if (count.isEmpty() || count == "-1")
        return;

    m_ctrlPointCount = count;
}

QString JobState::getCoordCount()
{
    if (m_ctrlPointCount.isEmpty())
        return "-";

    return m_ctrlPointCount;
}

void JobState::setPicCount(const QString& count)
{
    if (count.isEmpty() || count == "-1")
        return;
    m_picCount = count;
}

QString JobState::getPicCount()
{
    if (m_picCount.isEmpty())
        return "-";

    return m_picCount;
}

void JobState::setMunuTaskId(const QString& munuId)
{
    m_munuTaskId = munuId;
}

QString JobState::getMunuTaskId()
{
    return m_munuTaskId;
}

void JobState::updataBaseTaskInfo(BaseTaskInfo* taskInfo)
{
    if (taskInfo == nullptr)
        return;

    m_baseTaskInfo = taskInfo;
}

BaseTaskInfo* JobState::getBaseTaskInfo()
{
    return m_baseTaskInfo;
}

void JobState::setSubmitCtrlPoint(bool isSubmit)
{
    m_baseTaskInfo->isSubmitCtrlPoint = isSubmit;
}

bool JobState::isSubmitCtrlPoint()
{
    return m_baseTaskInfo->isSubmitCtrlPoint;
}

void JobState::setSubmitPosInfo(bool isSubmit)
{
    m_baseTaskInfo->isSubmitPosInfo = isSubmit;
}

bool JobState::isSubmitPosInfo()
{
    return m_baseTaskInfo->isSubmitPosInfo;
}

qint64 JobState::getCompleteFileSize()
{
    return m_completedFileSize;
}

qint32 JobState::getUploadRemainTime()
{
    QMutexLocker locker(&m_mutex);
    qint64 remineSize = m_totalFileSize - m_completedFileSize;
    if (remineSize <= 0)
        return 0;

    float speed = getTransSpeed();
    if (speed <= 0) {
        speed = 1024 * 1024; // 1m  
    }

    return remineSize / speed; // 单位 秒
}

QList<JobState*> JobState::getATJobs()
{
    QList<JobState*> jobs;
    QList<JobState*> list = this->getChildren();
    foreach(JobState* state, list) {
        if (state->isATJob()) {
            jobs.append(state);
        }
    }

    return jobs;
}

QList<JobState*> JobState::getRebuildJobs()
{
    QList<JobState*> jobs;
    QList<JobState*> list = this->getChildren();
    foreach(JobState* state, list) {
        if (state->isRebuildJob()) {
            jobs.append(state);
        }
    }

    return jobs;
}

QList<JobState*> JobState::getRenderJobs()
{
    QList<JobState*> jobs;
    QList<JobState*> list = this->getChildren();
    foreach(JobState* state, list)
    {
        if(state->isRenderJob()) {
            jobs.append(state);
        }
    }

    return jobs;
}

QList<JobState*> JobState::get3DTilesJobs()
{
    QList<JobState*> jobs;
    QList<JobState*> list = this->getChildren();
    foreach(JobState* state, list)
    {
        if (state->is3DTilesJob()) {
            jobs.append(state);
        }
    }

    return jobs;
}

JobState* JobState::getATJob()
{
    QList<JobState*> jobs = getATJobs();
    if (jobs.isEmpty())
        return nullptr;

    return jobs[0];
}

JobState* JobState::getRebuildJob()
{
    QList<JobState*> jobs = getRebuildJobs();

    if(jobs.isEmpty())
        return nullptr;

    return jobs[0];
}  

JobState* JobState::getRenderJob()
{
    QList<JobState*> jobs = getMajorJobs();

    if(jobs.isEmpty())
        return nullptr;

    return jobs[0];
}

JobState* JobState::get3DTilesJob()
{
    QList<JobState*> jobs = get3DTilesJobs();

    if (jobs.isEmpty())
        return nullptr;

    return jobs[0];
}

bool JobState::isATJob()
{
    if (m_taskType.toLower() == "triangulation" && this->IsChildItem()) {
        //if (this->isContainsMajor()) { // 1. 任务是空三且是子任务
            return true;
        //}
    }
    return false;
}

bool JobState::isRebuildJob()
{
    if (m_taskType.toLower() == "rebuild" && this->IsChildItem()) {
        //if (this->isContainsMajor()) { // 1. 任务是重建且是子任务
            return true;
        //}
    }
    return false;
}

bool JobState::isRenderJob()
{
    if (m_taskType.toLower() == "render" && this->IsChildItem()) {
        if (this->isContainsMajor()) { // 1. 任务是重建且是子任务
            return true;
        }
    }
    return false;
}

bool JobState::is3DTilesJob()
{
    if (m_taskType.toLower() == "3dtiles" && this->IsChildItem()) {
        return true;
    }
    return false;
}

bool JobState::isATJobRendering()
{
    // 如果不是父任务，直接返回true，显示空三进度
    if(!this->IsParentItem()) {
        if(this->isATJob()
           && (this->IsTaskRendering() || this->IsRenderWaiting())) {
            return true;
        }
    }

    int renderingCount = 0;
    QList<JobState*> list = this->getATJobs();
    foreach(JobState* state, list) {
        if(state->IsTaskRendering() || state->IsRenderWaiting()) {
            renderingCount++;
        }
    }

    return renderingCount > 0;
}

bool JobState::isRebuildJobRendering()
{
    // 如果不是父任务，直接返回true，显示重建进度
    if (!this->IsParentItem()) {
        if (this->isRebuildJob()
            && (this->IsTaskRendering())) {
            return true;
        }
    }

    int renderingCount = 0;
    QList<JobState*> list = this->getRebuildJobs();
    foreach(JobState* state, list) {
        if (state->IsTaskRendering()) {
            renderingCount++;
        }
    }

    return renderingCount > 0 && this->isATFinished();
}

// 空三是否完成
bool JobState::isATFinished()
{
    if (m_projectType == enBlockProject) { // 如果是区块任务，空三直接返回真
        return true;
    }

    if (!this->IsParentItem()) {
        if (this->isATJob() && this->TaskIsFinished()) {
            return true;
        }
    }

    int finishedCount = 0;
    QList<JobState*> list = this->getATJobs();
    foreach(JobState* state, list) {
        if (state->TaskIsFinished()) {
            finishedCount++;
        }
    }

    return !list.isEmpty() && finishedCount == list.size();
}

bool JobState::isRebuildFinished()
{
    if (!this->IsParentItem()) {
        if (this->isRebuildJob() && this->TaskIsFinished()) {
            return true;
        }
    }

    int finishedCount = 0;
    QList<JobState*> list = this->getRebuildJobs();
    foreach(JobState* state, list) {
        if (state->TaskIsFinished()) {
            finishedCount++;
        }
    }

    return !list.isEmpty() && finishedCount == list.size();
}

bool JobState::isMajorJob()
{
    if (m_taskType.toLower() == "render" && this->IsChildItem()) {
        if (this->isContainsMajor()) { // 1. 任务是重建且是子任务
            return true;
        }
    }
    return false;
}

bool JobState::isMajorJobRendering()
{
    if (!this->IsParentItem()) {
        if (this->isMajorJob()
            && (this->IsTaskRendering() || this->IsRenderWaiting())) {
            return true;
        }
    }

    int renderingCount = 0;
    QList<JobState*> list = this->getMajorJobs();
    foreach(JobState* state, list) {
        if (state->IsTaskRendering() || state->IsRenderWaiting()) {
            renderingCount++;
        }
    }

    return renderingCount > 0 && this->isATFinished() && this->isRebuildFinished() && this->is3DTilesFinished();
}

bool JobState::is3DTilesJobRendering()
{
    if (!this->IsParentItem()) {
        if (this->is3DTilesJob()
            && (this->IsTaskRendering() || this->IsRenderWaiting())) {
            return true;
        }
    }

    int renderingCount = 0;
    QList<JobState*> list = this->getMajorJobs();
    foreach(JobState* state, list) {
        if (state->IsTaskRendering() || state->IsRenderWaiting()) {
            renderingCount++;
        }
    }

    return renderingCount > 0 && this->isRebuildFinished();
}

bool JobState::isMajorJobFinished()
{
    if (!this->IsParentItem()) {
        if (this->isMajorJob() && this->TaskIsFinished()) {
            return true;
        }
    }

    int finishedCount = 0;
    QList<JobState*> list = this->getMajorJobs();
    foreach(JobState* state, list) {
        if (state->TaskIsFinished()) {
            finishedCount++;
        }
    }

    return !list.isEmpty() && finishedCount == list.size();
}

bool JobState::isATFailed()
{
    if (!this->IsParentItem()) {
        if (this->isATJob() && this->TaskIsFailed()) {
            return true;
        }
    }

    int failedCount = 0;
    QList<JobState*> list = this->getATJobs();
    foreach(JobState* state, list) {
        if (state->TaskIsFailed()) {
            failedCount++;
        }
    }

    return !list.isEmpty() && failedCount == list.size();
}

bool JobState::isATStopped()
{
    if(!this->IsParentItem()) {
        if(this->isATJob() && this->IsRenderStop()) {
            return true;
        }
    }

    int failedCount = 0;
    QList<JobState*> list = this->getATJobs();
    foreach(JobState* state, list)
    {
        if(state->IsRenderStop()) {
            failedCount++;
        }
    }

    return !list.isEmpty() && failedCount == list.size();
}

bool JobState::isRebuildFailed()
{
    if (!this->IsParentItem()) {
        if (this->isRebuildJob() && this->TaskIsFailed()) {
            return true;
        }
    }

    int failedCount = 0;
    QList<JobState*> list = this->getRebuildJobs();
    foreach(JobState* state, list) {
        if (state->TaskIsFailed()) {
            failedCount++;
        }
    }

    return !list.isEmpty() && failedCount == list.size();
}

bool JobState::is3DTilesFailed()
{
    if (!this->IsParentItem()) {
        if (this->is3DTilesJob() && this->TaskIsFailed()) {
            return true;
        }
    }

    int failedCount = 0;
    QList<JobState*> list = this->get3DTilesJobs();
    foreach(JobState* state, list) {
        if (state->TaskIsFailed()) {
            failedCount++;
        }
    }

    return !list.isEmpty() && failedCount == list.size();
}

bool JobState::isRebuildStopped()
{
    if(!this->IsParentItem()) {
        if(this->isRebuildJob() && this->IsRenderStop()) {
            return true;
        }
    }

    int failedCount = 0;
    QList<JobState*> list = this->getRebuildJobs();
    foreach(JobState* state, list) {
        if(state->IsRenderStop()) {
            failedCount++;
        }
    }

    return !list.isEmpty() && failedCount == list.size();
}

bool JobState::is3DTilesStopped()
{
    if (!this->IsParentItem()) {
        if (this->is3DTilesJob() && this->IsRenderStop()) {
            return true;
        }
    }

    int failedCount = 0;
    QList<JobState*> list = this->get3DTilesJobs();
    foreach(JobState* state, list) {
        if (state->IsRenderStop()) {
            failedCount++;
        }
    }

    return !list.isEmpty() && failedCount == list.size();
}

bool JobState::isMajorFailed()
{
    if (!this->IsParentItem()) {
        if (this->isMajorJob() && this->TaskIsFailed()) {
            return true;
        }
    }

    int failedCount = 0;
    QList<JobState*> list = this->getMajorJobs();
    foreach(JobState* state, list) {
        if (state->TaskIsFailed()) {
            failedCount++;
        }
    }

    return !list.isEmpty() && failedCount == list.size();
}

// (at && rebuild && 3dtiles && render) || (rebuild && 3dtiles && render)
bool JobState::isTaskFinished()
{
    if (getJobType() == enImageProject) {
        if (this->isContainsRebuildJob()) {
            return (isATFinished() && isRebuildFinished() && isProductionFinished());
        } else {
            return (isATFinished() && isProductionFinished());
        }
        
    } else if(getJobType() == enBlockProject){
        return (isRebuildFinished() && isProductionFinished());
    }  

    if (this->isContainsRebuildJob()) {
        return (isATFinished() && isRebuildFinished() && isProductionFinished());
    } else {
        return (isATFinished() && isProductionFinished());
    }
}

bool JobState::is3DTilesFinished()
{
    // 如果不存在3dtiles，则返回render状态
    if (!isContains3DTilesJob()) {
        return isMajorJobFinished();
    }

    if (!this->IsParentItem()) {
        if (this->is3DTilesJob() && this->TaskIsFinished()) {
            return true;
        }
    }

    int finishedCount = 0;
    QList<JobState*> list = this->get3DTilesJobs();
    foreach(JobState* state, list) {
        if (state->TaskIsFinished()) {
            finishedCount++;
        }
    }

    return !list.isEmpty() && finishedCount == list.size();
}

bool JobState::isProductionFinished()
{
    int finishedCount = 0;
    QList<JobState*> list = this->get3DTilesJobs();
    list.append(this->getMajorJobs());
    foreach(JobState* state, list) {
        if (state->TaskIsFinished()) {
            finishedCount++;
        }
    }

    return !list.isEmpty() && finishedCount == list.size();
}

QString JobState::getATStateText()
{
    int status = getATStatus();
    QString wstrTip = "-";

    switch (status) {
    case enRenderTaskPreRendering:
        wstrTip = QObject::tr("准备空三计算");
        break;
    case enRenderTaskFinished:
        wstrTip = QObject::tr("空三完成");
        break;
    case enRenderTaskFailed:
        wstrTip = QObject::tr("空三失败");
        break;
    case enRenderTaskStop:
        wstrTip = QObject::tr("空三停止");
        break;
    case enRenderTaskRendering:
        wstrTip = QObject::tr("空三计算中");
        break;
    case enRenderTaskAbandon:
        wstrTip = QObject::tr("放弃空三");
        break;
    case enRenderTaskFinishTest:
        wstrTip = QObject::tr("测试完成");
        break;
    case enRenderTaskAnalyse:
        wstrTip = QObject::tr("分析中");
        break;
    case enRenderTaskFinishHasFailed:
        wstrTip = QObject::tr("空三失败");
        break;
    case enRenderTaskWaitting:
        wstrTip = QObject::tr("空三计算中");
        break;
    case enRenderTaskArrearageStop:
        wstrTip = QObject::tr("欠费停止");
        break;
    case enRenderOverTimeStop:
        wstrTip = QObject::tr("超时停止");
        break;
    case enRenderUserStop:
        wstrTip = QObject::tr("用户停止");
        break;
    case enRenderStateUpdating:
        wstrTip = QObject::tr("状态更新中");
        break;
    case enRenderTaskSubmitting:
        wstrTip = QObject::tr("提交中");
        break;
    case enRenderTaskSubmitFailed:
        wstrTip = QObject::tr("提交失败");
        break;
    default:
        wstrTip = "-";
        break;
    }

    return wstrTip;
}

void JobState::setProjectType(int type)
{
    m_projectType = type;
}

int JobState::getProjectType()
{
    return m_projectType;
}

void JobState::setProjectOutputType(const QStringList& types)
{
    m_outputType = types;
}

QStringList JobState::getProjectOutputType()
{
    return m_outputType;
}

void JobState::setProgressPercent(int progress)
{
    m_progressPercent = progress;
}

int JobState::getProgressPercent()
{
    return m_progressPercent;
}

int JobState::getATStatus()
{
    int status = 0;
    QList<JobState*> jobs = getATJobs();
    JobState* job = nullptr;
    if (!jobs.isEmpty()) {
        job = jobs.at(0);
    }
    if (job == nullptr)
        return status;

    return job->getStatus();
}

int JobState::getRebuildStatus()
{
    int status = 0;
    QList<JobState*> jobs = getRebuildJobs();
    JobState* job = nullptr;
    if (!jobs.isEmpty()) {
        job = jobs.at(0);
    }
    if (job == nullptr)
        return status;
    return job->getStatus();
}

int JobState::getRenderStatus()
{
    int status = 0;
    QList<JobState*> jobs = getMajorJobs();
    JobState* job = nullptr;
    if (!jobs.isEmpty()) {
        job = jobs.at(0);
    }

    if (job == nullptr)
        return status;

    return job->getStatus();
}

int JobState::get3DTilesStatus()
{
    // 如果不存在就使用render的状态
    if (!this->isContains3DTilesJob()) {
        return getRenderStatus();
    }

    int status = 0;
    QList<JobState*> jobs = get3DTilesJobs();
    JobState* job = nullptr;
    if (!jobs.isEmpty()) {
        job = jobs.at(0);
    }

    if (job == nullptr)
        return status;

    return job->getStatus();
}

int JobState::getProductionStatus()
{
    int status = 0;
    if (this->isContains3DTilesJob() && this->isContainsRenderJob()) {
        if (!is3DTilesFinished()) {
            status = get3DTilesStatus();
        }

        if (is3DTilesFinished() && !isMajorJobFinished()) {
            status = getRenderStatus();
        }

        if (is3DTilesFinished() && isMajorJobFinished()) {
            status = getRenderStatus();
        }

    } else if(this->isContains3DTilesJob() && !this->isContainsRenderJob()){
        status = get3DTilesStatus();     

    } else if (!this->isContains3DTilesJob() && this->isContainsRenderJob()) {
        status = getRenderStatus();
    }

    return status;
}

int JobState::getTaskStatus()
{
    int status = enNothing;

    // 只显示上传和渲染的状态，下载的状态用渲染状态
    if (m_jobType == RENDER_JOB) {
        status = m_renderState;
    } else if (m_jobType == UPLOAD_JOB || m_jobType == DOWNLOAD_JOB) {
        status = m_transState;
    }

    if (status == enRenderTaskSubmitFailed)
        return status;
    else if (status == enRenderTaskSubmitting)
        return status;


    if (m_jobType == RENDER_JOB && !this->isATFinished()) {
        return getATStatus();
    }

    if (m_jobType == RENDER_JOB && !this->isRebuildFinished()) {
        return getRebuildStatus();
    }

    if (m_jobType == RENDER_JOB && !this->isProductionFinished()) {
        return getProductionStatus();
    }

    if (m_jobType == RENDER_JOB && this->isRebuildFinished() && this->isProductionFinished()) {
        return getProductionStatus();
    }

    return status;
}

QString JobState::getRebuildStateText()
{
    int status = getRebuildStatus();
    QString wstrTip = "-";

    switch (status) {
    case enRenderTaskPreRendering:
        wstrTip = QObject::tr("准备重建");
        break;
    case enRenderTaskFinished:
        wstrTip = QObject::tr("重建完成");
        break;
    case enRenderTaskFailed:
        wstrTip = QObject::tr("重建失败");
        break;
    case enRenderTaskStop:
        wstrTip = QObject::tr("重建停止");
        break;
    case enRenderTaskRendering:
        wstrTip = QObject::tr("重建中");
        break;
    case enRenderTaskAbandon:
        wstrTip = QObject::tr("放弃重建");
        break;
    case enRenderTaskFinishTest:
        wstrTip = QObject::tr("测试完成");
        break;
    case enRenderTaskAnalyse:
        wstrTip = QObject::tr("分析中");
        break;
    case enRenderTaskFinishHasFailed:
        wstrTip = QObject::tr("重建失败");
        break;
    case enRenderTaskWaitting:
        wstrTip = QObject::tr("等待重建");
        break;
    case enRenderTaskArrearageStop:
        wstrTip = QObject::tr("欠费停止");
        break;
    case enRenderOverTimeStop:
        wstrTip = QObject::tr("超时停止");
        break;
    case enRenderUserStop:
        wstrTip = QObject::tr("用户停止");
        break;
    case enRenderStateUpdating:
        wstrTip = QObject::tr("状态更新中");
        break;
    case enRenderTaskSubmitting:
        wstrTip = QObject::tr("提交中");
        break;
    case enRenderTaskSubmitFailed:
        wstrTip = QObject::tr("提交失败");
        break;
    default:
        wstrTip = "-";
        break;
    }

    return wstrTip;
}

QString JobState::getProductionStateText()
{
    int status = getProductionStatus();
    QString wstrTip = "-";

    switch (status) {
    case enRenderTaskPreRendering:
        wstrTip = QObject::tr("准备生产");
        break;
    case enRenderTaskFinished:
        wstrTip = QObject::tr("生产完成");
        break;
    case enRenderTaskFailed:
        wstrTip = QObject::tr("生产失败");
        break;
    case enRenderTaskStop:
        wstrTip = QObject::tr("生产停止");
        break;
    case enRenderTaskRendering:
        wstrTip = QObject::tr("生产中");
        break;
    case enRenderTaskAbandon:
        wstrTip = QObject::tr("放弃生产");
        break;
    case enRenderTaskFinishTest:
        wstrTip = QObject::tr("测试完成");
        break;
    case enRenderTaskAnalyse:
        wstrTip = QObject::tr("分析中");
        break;
    case enRenderTaskFinishHasFailed:
        wstrTip = QObject::tr("生产失败");
        break;
    case enRenderTaskWaitting:
        wstrTip = QObject::tr("等待生产");
        break;
    case enRenderTaskArrearageStop:
        wstrTip = QObject::tr("欠费停止");
        break;
    case enRenderOverTimeStop:
        wstrTip = QObject::tr("超时停止");
        break;
    case enRenderUserStop:
        wstrTip = QObject::tr("用户停止");
        break;
    case enRenderStateUpdating:
        wstrTip = QObject::tr("状态更新中");
        break;
    case enRenderTaskSubmitting:
        wstrTip = QObject::tr("提交中");
        break;
    case enRenderTaskSubmitFailed:
        wstrTip = QObject::tr("提交失败");
        break;
    default:
        wstrTip = "-";
        break;
    }

    return wstrTip;
}

int JobState::pageDisplayByState()
{
    int display = enPageDisplayNone;

    if (this->getProjectType() == enImageProject) { // 影像任务
        display = getATJobDisplayState();
        if (display == enPageATFinished) {
            display = getRebuildJobDisplayState();
            if (display == enPageRebuildFinished) {
                display = getRenderJobDisplayState();
            }
        }
    } else if (this->getProjectType() == enBlockProject) { // 区块任务   
        display = getRebuildJobDisplayState();
        if (display == enPageRebuildFinished) {
            display = getRenderJobDisplayState();
        }
    }

    return display;
}

bool JobState::IsParentTaskCanStart()
{
    if (!isCanJobOperable())
        return false;

    int canStart = 0;
    if (getATJob() != nullptr && getATJob()->IsRenderCanStart())
        canStart++;
    if (getRebuildJob() != nullptr && getRebuildJob()->IsRenderCanStart())
        canStart++;
    if (getRenderJob() != nullptr && getRenderJob()->IsRenderCanStart())
        canStart++;

    return canStart > 0;
}

bool JobState::IsParentTaskCanStop()
{
    if (!isCanJobOperable())
        return false;

    int canStop = 0;
    if (getATJob() != nullptr && getATJob()->IsRenderCanStop())
        canStop++;
    if (getRebuildJob() != nullptr && getRebuildJob()->IsRenderCanStop())
        canStop++;
    if (getRenderJob() != nullptr && getRenderJob()->IsRenderCanStop())
        canStop++;

    return canStop > 0;
}

bool JobState::IsParentTaskCanDelete()
{
    int canDelete = 0;
    if (getATJob() != nullptr && getATJob()->IsRenderCanDelete())
        canDelete++;
    if (getRebuildJob() != nullptr && getRebuildJob()->IsRenderCanDelete())
        canDelete++;
    if (getRenderJob() != nullptr && getRenderJob()->IsRenderCanDelete())
        canDelete++;

    return canDelete > 0;
}

bool JobState::IsParentTaskCanResubmit()
{
    if (!isCanJobOperable())
        return false;

    int canResub = 0;
    if (getATJob() != nullptr && getATJob()->TaskCanReSub())
        canResub++;
    if (getRebuildJob() != nullptr && getRebuildJob()->TaskCanReSub())
        canResub++;
    if (getRenderJob() != nullptr && getRenderJob()->TaskCanReSub())
        canResub++;

    return canResub > 0;
}

bool JobState::IsParentTaskCanDownload()
{
    return isTaskFinished() && isCanJobOperable();
}

bool JobState::IsParentTaskSubmitFailed()
{
    if (!isCanJobOperable())
        return false;

    int submitFailed = 0;
    if (getATJob() != nullptr && getATJob()->IsTaskSubmitFailed())
        submitFailed++;
    if (getRebuildJob() != nullptr && getRebuildJob()->IsTaskSubmitFailed())
        submitFailed++;
    if (getRenderJob() != nullptr && getRenderJob()->IsTaskSubmitFailed())
        submitFailed++;

    return submitFailed > 0;
}

bool JobState::IsParentTaskSubmitting()
{
    if (!isCanJobOperable())
        return false;

    int submitFailed = 0;
    if (getATJob() != nullptr && getATJob()->IsTaskSubmitting())
        submitFailed++;
    if (getRebuildJob() != nullptr && getRebuildJob()->IsTaskSubmitting())
        submitFailed++;
    if (getRenderJob() != nullptr && getRenderJob()->IsTaskSubmitting())
        submitFailed++;

    return submitFailed > 0;
}

bool JobState::IsParentTaskStateUpdating()
{
    int submitFailed = 0;
    if (getATJob() != nullptr && getATJob()->IsTaskStateUpdating())
        submitFailed++;
    if (getRebuildJob() != nullptr && getRebuildJob()->IsTaskStateUpdating())
        submitFailed++;
    if (getRenderJob() != nullptr && getRenderJob()->IsTaskStateUpdating())
        submitFailed++;

    return submitFailed > 0;
}

int JobState::getATJobDisplayState()
{
    int display = enPageDisplayNone;

    if (this->getATStatus() == enRenderTaskRendering
        || this->getATStatus() == enRenderTaskWaitting) {
        display = enPageATProgress;

    } else if (this->getATStatus() == enRenderTaskStop
        || this->getATStatus() == enRenderUserStop
        || this->getATStatus() == enRenderTaskArrearageStop
        || this->getATStatus() == enRenderOverTimeStop) {
        display = enPageATStop;

    } else if(this->getATStatus() == enRenderTaskAbandon
              || this->getATStatus() == enRenderTaskFailed
              || this->getATStatus() == enRenderTaskFinishHasFailed) {
        display = enPageATFailed;

    } else if (this->getATStatus() == enRenderTaskFinished) {
        display = enPageATFinished;

    } else {
        display = enPageATHome;
    }

    return display;
}

int JobState::getRebuildJobDisplayState()
{
    int display = enPageDisplayNone;

    auto jobState = getRebuildJob();
    if (jobState == nullptr) return display;
	if (jobState->m_munuTaskId.isEmpty())
		return enPageRebuildHome;
//		return enPageRebuildWaiting;


    if (this->getProjectType() == enImageProject) {

        if (!this->isATFinished())
            return display;

        /*重建任务渲染等待，则显示等待页面*/
        if(this->getRebuildStatus() == enRenderTaskWaitting) {
            display = enPageRebuildWaiting;

            /*重建任务或渲染任务都是渲染中,或重建完成，但渲染等待，则显示进度页面*/
        } else if (this->getRebuildStatus() == enRenderTaskRendering) {
            display = enPageRebuildProgress;

            /*重建任务停止，切换到停止页面*/
        } else if(this->getRebuildStatus() == enRenderTaskStop
                   || this->getRebuildStatus() == enRenderUserStop
                   || this->getRebuildStatus() == enRenderTaskArrearageStop
                   || this->getRebuildStatus() == enRenderOverTimeStop) {

            display = enPageRebuildStop;

            /*重建任务失败，切换到失败页面*/
        } else if (this->getRebuildStatus() == enRenderTaskAbandon
            || this->getRebuildStatus() == enRenderTaskFailed
            || this->getRebuildStatus() == enRenderTaskFinishHasFailed) {

            display = enPageRebuildFailed;

            /*空三完成，重建完成，渲染完成 => 重建完成页面*/
        } else if (this->getATStatus() == enRenderTaskFinished
            && this->getRebuildStatus() == enRenderTaskFinished) {
            display = enPageRebuildFinished;

            /*提交中、提交失败、状态更新中、预处理中，不切换页面*/
        } else if (this->getRebuildStatus() == enRenderTaskPreRendering
            || this->getRebuildStatus() == enRenderTaskSubmitting
            || this->getRebuildStatus() == enRenderTaskSubmitFailed
            || this->getRebuildStatus() == enRenderStateUpdating) {

            display = enPageDisplayNone;

        } else {
            display = enPageRebuildHome;
        }
    
    } else if (this->getProjectType() == enBlockProject) {

        if(this->getRebuildStatus() == enRenderTaskWaitting) {

            /*重建任务等待，切换到等待页面*/
            display = enPageRebuildWaiting;
        } else if (this->getRebuildStatus() == enRenderTaskRendering) {

            /*重建任务或渲染任务都是渲染中，则显示进度页面*/
            display = enPageRebuildProgress;
        } else if (this->getRebuildStatus() == enRenderTaskStop
            || this->getRebuildStatus() == enRenderUserStop
            || this->getRebuildStatus() == enRenderTaskArrearageStop
            || this->getRebuildStatus() == enRenderOverTimeStop) {

            /*重建任务失败，切换到停止页面*/
            display = enPageRebuildStop;
        } else if(this->getRebuildStatus() == enRenderTaskAbandon
                  || this->getRebuildStatus() == enRenderTaskFailed
                  || this->getRebuildStatus() == enRenderTaskFinishHasFailed) {
            /*重建任务失败，切换到失败页面*/
            display = enPageRebuildFailed;
        } else if (this->getRebuildStatus() == enRenderTaskFinished) {
            /*重建任务和渲染任务都完成，才算完成*/
            display = enPageRebuildFinished;

        } else if (this->getRebuildStatus() == enRenderTaskPreRendering
            || this->getRebuildStatus() == enRenderTaskSubmitting
            || this->getRebuildStatus() == enRenderTaskSubmitFailed
            || this->getRebuildStatus() == enRenderStateUpdating) {

            display = enPageDisplayNone;

        } else {
            display = enPageRebuildHome;
        }   
    }
    
    return display;
}

int JobState::getRenderJobDisplayState()
{
    // 如果 3dtiles 任务状态没有完成 这直接展示 3dtiles 的状态
    // 3dtiles状态和render的状态合并，页面展示合并
    if (this->isContains3DTilesJob() && get3DTilesJobDisplayState() != enPageRenderFinished) {
        return get3DTilesJobDisplayState();
    }

    int display = enPageDisplayNone;

    auto jobState = getRenderJob();
    if (jobState == nullptr) return display;
    if (jobState->m_munuTaskId.isEmpty())
        return enPageDisplayNone;

    if (this->getProjectType() == enImageProject) {

        if (!this->isATFinished())
            return display;

        if (!this->isRebuildFinished())
            return display;

        /*重建任务完成，渲染任务等待，则显示等待页面*/
        if (this->getRenderStatus() == enRenderTaskWaitting) {
            display = enPageRenderWaiting;

            /*重建完成，但生产任务，则显示进度页面*/
        } else if (this->getRenderStatus() == enRenderTaskRendering) {
            display = enPageRenderProgress;

            /*生产任务停止，切换到停止页面*/
        } else if (this->getRenderStatus() == enRenderTaskStop
            || this->getRenderStatus() == enRenderUserStop
            || this->getRenderStatus() == enRenderTaskArrearageStop
            || this->getRenderStatus() == enRenderOverTimeStop) {

            display = enPageRenderStop;

            /*生产任务失败，切换到失败页面*/
        } else if (this->getRenderStatus() == enRenderTaskAbandon
            || this->getRenderStatus() == enRenderTaskFailed
            || this->getRenderStatus() == enRenderTaskFinishHasFailed) {

            display = enPageRenderFailed;

            /*空三完成，重建完成，渲染完成 => 重建完成页面*/
        }else if (this->getATStatus() == enRenderTaskFinished
            && this->getRebuildStatus() == enRenderTaskFinished
            && this->get3DTilesStatus() == enRenderTaskFinished
            && this->getRenderStatus() == enRenderTaskFinished) {
            display = enPageRenderFinished;

            /*提交中、提交失败、状态更新中、预处理中，不切换页面*/
        } else if (this->getRenderStatus() == enRenderTaskSubmitting
            || this->getRenderStatus() == enRenderTaskSubmitFailed
            || this->getRenderStatus() == enRenderStateUpdating) {

            display = enPageDisplayNone;
        } else {
            display = enPageDisplayNone;
        }

    }else if (this->getProjectType() == enBlockProject) {

        if (!this->isRebuildFinished())
            return display;

        if (this->getRenderStatus() == enRenderTaskWaitting) {

            /*生产任务等待，切换到等待页面*/
            display = enPageRenderWaiting;

        }else if (this->getRenderStatus() == enRenderTaskRendering) {

            /*生产任务或渲染任务都是渲染中，则显示进度页面*/
            display = enPageRenderProgress;

        }else if (this->getRenderStatus() == enRenderTaskStop
            || this->getRenderStatus() == enRenderUserStop
            || this->getRenderStatus() == enRenderTaskArrearageStop
            || this->getRenderStatus() == enRenderOverTimeStop) {

            /*生产停止失败，切换到停止页面*/
            display = enPageRenderStop;

        } else if (this->getRenderStatus() == enRenderTaskAbandon
            || this->getRenderStatus() == enRenderTaskFailed
            || this->getRenderStatus() == enRenderTaskFinishHasFailed) {
            /*生产任务失败，切换到失败页面*/
            display = enPageRenderFailed;

        }else if (this->getRenderStatus() == enRenderTaskFinished
            && this->get3DTilesStatus() == enRenderTaskFinished
            && this->getRebuildStatus() == enRenderTaskFinished ) {
            /*重建任务和渲染任务都完成，才算完成*/
            display = enPageRenderFinished;

        } else if (this->getRenderStatus() == enRenderTaskSubmitting
            || this->getRenderStatus() == enRenderTaskSubmitFailed
            || this->getRenderStatus() == enRenderStateUpdating) {

            display = enPageDisplayNone;

        } else {
            display = enPageDisplayNone;
        }
    }

    return display;
}

int JobState::get3DTilesJobDisplayState()
{
    int display = enPageDisplayNone;

    auto jobState = get3DTilesJob();
    if (jobState == nullptr) return display;
    if (jobState->m_munuTaskId.isEmpty())
        return enPageDisplayNone;

    if (this->getProjectType() == enImageProject) {

        if (!this->isATFinished())
            return display;

        if (!this->isRebuildFinished())
            return display;

        /*重建任务完成，渲染任务等待，则显示等待页面*/
        if (this->get3DTilesStatus() == enRenderTaskWaitting) {
            display = enPageRenderWaiting;

            /*重建完成，但生产任务，则显示进度页面*/
        } else if (this->get3DTilesStatus() == enRenderTaskRendering) {
            display = enPageRenderProgress;

            /*生产任务停止，切换到停止页面*/
        } else if (this->get3DTilesStatus() == enRenderTaskStop
            || this->get3DTilesStatus() == enRenderUserStop
            || this->get3DTilesStatus() == enRenderTaskArrearageStop
            || this->get3DTilesStatus() == enRenderOverTimeStop) {

            display = enPageRenderStop;

            /*生产任务失败，切换到失败页面*/
        } else if (this->get3DTilesStatus() == enRenderTaskAbandon
            || this->get3DTilesStatus() == enRenderTaskFailed
            || this->get3DTilesStatus() == enRenderTaskFinishHasFailed) {

            display = enPageRenderFailed;

            /*空三完成，重建完成，渲染完成 => 重建完成页面*/
        } else if (this->getATStatus() == enRenderTaskFinished
            && this->getRebuildStatus() == enRenderTaskFinished
            && this->get3DTilesStatus() == enRenderTaskFinished) {
            display = enPageRenderFinished;

            /*提交中、提交失败、状态更新中、预处理中，不切换页面*/
        } else if (this->get3DTilesStatus() == enRenderTaskSubmitting
            || this->get3DTilesStatus() == enRenderTaskSubmitFailed
            || this->get3DTilesStatus() == enRenderStateUpdating) {

            display = enPageDisplayNone;
        } else {
            display = enPageDisplayNone;
        }

    } else if (this->getProjectType() == enBlockProject) {

        if (!this->isRebuildFinished())
            return display;

        if (this->get3DTilesStatus() == enRenderTaskWaitting) {

            /*生产任务等待，切换到等待页面*/
            display = enPageRenderWaiting;

        } else if (this->get3DTilesStatus() == enRenderTaskRendering) {

            /*生产任务或渲染任务都是渲染中，则显示进度页面*/
            display = enPageRenderProgress;

        } else if (this->get3DTilesStatus() == enRenderTaskStop
            || this->get3DTilesStatus() == enRenderUserStop
            || this->get3DTilesStatus() == enRenderTaskArrearageStop
            || this->get3DTilesStatus() == enRenderOverTimeStop) {

            /*生产停止失败，切换到停止页面*/
            display = enPageRenderStop;

        } else if (this->get3DTilesStatus() == enRenderTaskAbandon
            || this->get3DTilesStatus() == enRenderTaskFailed
            || this->get3DTilesStatus() == enRenderTaskFinishHasFailed) {
            /*生产任务失败，切换到失败页面*/
            display = enPageRenderFailed;

        } else if (this->get3DTilesStatus() == enRenderTaskFinished
            && this->getRebuildStatus() == enRenderTaskFinished) {
            /*重建任务和渲染任务都完成，才算完成*/
            display = enPageRenderFinished;

        } else if (this->get3DTilesStatus() == enRenderTaskSubmitting
            || this->get3DTilesStatus() == enRenderTaskSubmitFailed
            || this->get3DTilesStatus() == enRenderStateUpdating) {

            display = enPageDisplayNone;

        } else {
            display = enPageDisplayNone;
        }
    }

    return display;
}

QColor JobState::getStateColor()
{
    QColor textColor(qRgb(0xff, 0xff, 0xff));

    if (this->isATFailed() || this->isRebuildFailed() || this-> isMajorFailed() || this->is3DTilesFailed()) {
        textColor = QColor(qRgb(0xe2, 0x5c, 0x59));
    }

    return textColor;
}

void JobState::setBlockExchangeInfo(BLOCK_TASK::BlockExchange* exchage)
{
    m_blockInfo = exchage;
}

BLOCK_TASK::BlockExchange* JobState::getBlockExchangeInfo()
{
    return m_blockInfo;
}

void JobState::addDownloadFile(const QString& outputPath)
{
    if (!m_downloadList.contains(outputPath)) {
        m_downloadList.append(outputPath);
    }
}

QStringList JobState::getDownloadFileList()
{
    return m_downloadList;
}

void JobState::setDownloadFiles(const QStringList& types)
{
    if (types.isEmpty())
        return;

    foreach(QString path, types) {
        this->addDownloadFile(path);
    }
}

void JobState::resetDownloadList()
{
    m_downloadList.clear();
}

bool JobState::isSubmitSuccess()
{
    if (m_jobType == RENDER_JOB)
        return true;
    return m_isSubmitSuccess;
}

void JobState::setIsSubmitSuccess(bool isSuccess)
{
    m_isSubmitSuccess = isSuccess;
}

qint32 JobState::GetExcuteCount()
{
    return m_dwExcuteCount;
}

qint32 JobState::GetDoneCount()
{
    return m_dwDoneCount;
}

qint32 JobState::GetFailedCount()
{
    return m_dwFailedCount;
}

qint32 JobState::GetAbortCount()
{
    return m_dwAbortCount;
}

qint32 JobState::GetWaitCount()
{
    return  m_dwWaitCount;
}

bool JobState::isCanShowDetailPage()
{
    if (m_jobType == UPLOAD_JOB)
        return false;

    if (this->getProjectType() == enImageProject) {
        if (this->isATFinished()) {
            JobState *rebuild = this->getRebuildJob();
            if (rebuild != nullptr && !rebuild->getMunuTaskId().isEmpty()) {
                return true;
            }
        }

    } else if (this->getProjectType() == enBlockProject) {
        JobState *rebuild = this->getRebuildJob();
        if (rebuild != nullptr && !rebuild->getMunuTaskId().isEmpty()) {
            return true;
        }
    }

    return false;
}

void JobState::setTotalPixel(const QString& pixel)
{
    m_totalPixel = pixel;
}

QString JobState::getTotalPixel()
{
    return m_totalPixel;
}

bool JobState::isCanJobOperable()
{
    bool isOperable = false;
    if (getJobOperable() == 0) {
        isOperable = false;
    } else if (getJobOperable() == 1) {
        isOperable = true;
    }
    return isOperable;
}

void JobState::statisticsUploadedCount()
{
    QMutexLocker locker(&m_mutexCount);
    static qint64 timeElapsed;
    if (GetTickCount() - timeElapsed > 10000) {
        m_perSecondUploadedFile = m_transFileCount * 1.0f / 10.0f;
        m_transFileCount = 0;
    }
    timeElapsed = GetTickCount();
    m_transFileCount++;
}

qint32 JobState::uploadRemainTime()
{
    QMutexLocker locker(&m_mutexCount);
    qint64 remainCount = m_totalCount - m_completedCount;
    qint32 rtime = remainCount * 10;
    if (m_perSecondUploadedFile > 0) {
        rtime = (float)remainCount / m_perSecondUploadedFile;
    }
    return rtime;
}