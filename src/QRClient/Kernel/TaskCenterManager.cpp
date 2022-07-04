#include "stdafx.h"
#include "MainWindow.h"
#include "TaskCenterManager.h"
#include "NewJobs/JobStateMgr.h"
#include "MessagePool.h"
#include "TransEngine/TransfomEngineFactory.h"
#include "PluginConfig/ProjectMgr.h"
#include "PluginConfig/PluginMgr.h"
#include "Upload/UploadFileDBMgr/UploadFileDBMgr.h"
#include "HttpCommand/HttpCmdManager.h"
#include "Common/FileUtil.h"
#include "UI/Views/DownloadPage/DownloadStateMgr.h"

static QString pathFindFileName(const QString &fileName)
{
    QFileInfo fi(fileName);
    return fi.fileName();
}

CTaskCenterManager::CTaskCenterManager(void)
    : m_transEngine(nullptr)
{
    m_currEngineName = TransEngineFactory::getInstance()->defaultEngine();
}

CTaskCenterManager::~CTaskCenterManager(void)
{
    JobStateMgr::Destroy();
    JobStateMgr::Destroy();
    JobStateMgr::Destroy();
}

bool CTaskCenterManager::init()
{
    if(!m_downloadQueue.init()) {
        LOGFMTW("[TaskCenter] Download queue init failed!");
        return false;
    }
    if(!m_uploadQueue.init()) {
        LOGFMTW("[TaskCenter] Upload queue init failed!");
        return false;
    }

	if (!m_cloneQueue.init())
	{
		LOGFMTW("[TaskCenter] Clone Queue  init failed!");
		return false;
	}

    m_msgQueue.init(this);

    LOGFMTI("[TaskCenter] All queues init done!");

    return true;
}

bool CTaskCenterManager::beepOn()
{
    m_msgQueue.start();

    if(!HistoryDbMgr::getInstance()->beepOn()) {
        LOGFMTE("[TaskCenter] History manager start failed!");
        return false;
    }

    m_currEngineName = LocalSetting::getInstance()->getTransEngine();
    m_transEngine = TransEngineFactory::getInstance()->createEngine(m_currEngineName);
    if(m_transEngine == nullptr || !m_transEngine->init()) {
        LOGFMTE("[TaskCenter] transfer engine init failed!");
        return false;
    }

    this->limitTransSpeed(EN_LIMIT_UPLOAD_SPEED, LocalSetting::getInstance()->getLimitUpload());
    this->limitTransSpeed(EN_LIMIT_DOWNLOAD_SPEED, LocalSetting::getInstance()->getLimitDownload());
    if(!m_downloadQueue.start(m_transEngine)) {
        LOGFMTE("[TaskCenter] Download queue start failed!");
        return false;
    }
    if(!m_uploadQueue.start(m_transEngine)) {
        LOGFMTE("[TaskCenter] Upload queue start failed!");
        return false;
    }

	if (!m_cloneQueue.start(m_transEngine)) {
		LOGFMTE("[TaskCenter] Clone queue start failed!");
		return false;
	}

    LOGFMTI("[TaskCenter] All queues start done!");

    return true;
}

void CTaskCenterManager::beepOff()
{
    m_msgQueue.stop();
    LOGFMTI("[TaskCenter]Message queue exit.");

    if(m_transEngine != nullptr)
        m_transEngine->exit();
    LOGFMTI("[TaskCenter]Transfer engine exit.");

    m_downloadQueue.stop();
    LOGFMTI("[TaskCenter]Download quque exit.");
    m_uploadQueue.stop();
    LOGFMTI("[TaskCenter]Upload queue exit.");

    TransEngineFactory::destory();
    CMessagePool::Destroy();
    LOGFMTI("[TaskCenter]All queue exit done.");
}

bool CTaskCenterManager::changeTransEngine(const QString &engineName)
{
    AbstractTransEngine *engine = TransEngineFactory::getInstance()->createEngine(engineName);
    if(!engine) return false;

    bool bret = true;
    bret &= m_downloadQueue.switchEngine(engine);
    if(!bret)
        LOGFMTE("[TaskCenter]download engine change to [%s] failed.", qPrintable(engineName));
    bret &= m_uploadQueue.switchEngine(engine);
    if(!bret)
        LOGFMTE("[TaskCenter]upload engine change to [%s] failed.", qPrintable(engineName));

    if(bret) {
        m_transEngine    = engine;
        m_currEngineName = engineName;

        return true;
    }

    return false;
}

bool CTaskCenterManager::changeTransServer(const QString& server)
{
    if(m_transEngine)
        m_transEngine->switch_server(server.toStdString());

    LOGFMTI("[TaskCenter]engine server changed to [%s].", qUtf8Printable(server));

    return true;
}

AbstractTransEngine * CTaskCenterManager::currentTransEngine()
{
    return m_transEngine;
}

void CTaskCenterManager::addTaskElement(TaskElementPtr task)
{
    if(task.isNull())
        return;

    if(m_taskList.find(task->getJobId()) != m_taskList.end()) {
        return;
    }

    m_taskList[task->getJobId()] = task;
}

bool CTaskCenterManager::updateDownloadState(t_taskid jobId, int status, const QString& promptMsg)
{
    CDownloadState *downloadState = CDownloadStateMgr::Inst()->LookupTaskstate(jobId);
    if(downloadState == nullptr) {
        LOGFMTE("[TaskCenter] Download state not found, memory error!  prompt taskid=%d", jobId);
        return false;
    }

    if (!downloadState->isDownloadFinished()) {
        downloadState->SetDownloadFileName(promptMsg);
        downloadState->setTransState(status);
        downloadState->SetSpeed(0.0);
    }

    if(status == enDownloadCompleted) {
        QString stopTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        downloadState->SetStopDate(stopTime);
        downloadState->SetProgress(100);

        if(downloadState->isChildItem()) {
            CDownloadState *parentState = downloadState->getParentItem();
            if(parentState != nullptr) {
                if(parentState->getDownloadCompleteChildCount() == parentState->childCount()) {
                    parentState->SetStopDate(stopTime);
                }
            }
        }
    }

    return true;
}

bool CTaskCenterManager::updateUploadState(t_taskid jobId, int status, const QString& promptMsg)
{
    JobState *uploadState = JobStateMgr::Inst()->LookupTaskstate(jobId);
    if((uploadState == Q_NULLPTR) || (uploadState->getTransStatus() == enUploadStop))
        return false;

    if (status == enUploadCompleted) {
        uploadState->setUploadProgress(100.0);
        uploadState->setUploadedFileSize(uploadState->getTotalFileSize());;
    }
        
    uploadState->setUploadFileName(promptMsg);
    uploadState->setTransStatus(status);
    uploadState->setSpeedString(promptMsg);

    return true;
}

bool CTaskCenterManager::startUpload(t_taskid jobId, bool bAuto)
{
    TaskElementPtr task = findTaskElement(jobId);
    if(task.isNull()) {
        LOGFMTI("[TaskCenter] %d Task not found! Begin to load", jobId);
        task = loadExistTask(jobId);
        if(task.isNull())
            return false;
        resetTaskState(task);
    }

    JobState *jobState = JobStateMgr::Inst()->LookupAllUploadTaskstate(jobId);
    if(jobState == nullptr) {
        LOGFMTE("[TaskCenter] Upload job state not found, memory error!");
        return false;
    }

    JobState *uploadState = JobStateMgr::Inst()->LookupTaskstate(jobId);
    if(uploadState == nullptr) {
        LOGFMTE("[TaskCenter] Upload state not found, memory error!");
        return false;
    }
    jobState->setIsCloneTask(uploadState->isCloneTask());

    // 手动开始的任务要重置重试次数
    if(bAuto == false) task->setTaskRetryTimes(0);

    int retry_times = task->getTaskRetryTimes();
    int retry_times_max = LocalSetting::getInstance()->getMaxUploadRetryTimes();
    // 重试次数达到上限，直接返回
    if(retry_times > retry_times_max) {
        QString message = QObject::tr("上传出错，检查无误后可手动暂停再开始");
        updateUploadState(uploadState, message, enUploadError);
        LOGFMTE("[TaskCenter] Start task failed! Task used all retry times!taskid=%d", jobId);
        task->setTaskRetryTimes(retry_times + 1);
        return false;
    }

    if(uploadState->IsTransform()) {
        if(task->is_task_done()) {
            m_uploadQueue.freePacket(jobId);
            LOGFMTE("[TaskCenter] %d Task done!!", jobId);
        } else {
            LOGFMTE("[TaskCenter] %d Task transmitting or waiting! status:%d", jobId, uploadState->getTransStatus());
            return true;
        }
    }

    this->loadUploadState(uploadState, task);
    this->addUploadHistory(jobId, enUploadUnfinished, jobState);

    if(task->is_task_completed()) {
        checkUploadFileCompelete(task);
        return true;
    }

    task->start();

    UploadInfo info;
    this->loadUploadInfo(info, task);

    if(task->isTextureFileLoaded()) {
        if(m_uploadQueue.startTask(info, task->fileList())) {
            QString str = QObject::tr("排队等待上传...");
            updateUploadState(uploadState, str, enUploadWaiting);
        } else {
            return false;
        }
    } else {
        if(getWaitUploadIsExist(jobId)) {
            LOGFMTI("[TaskCenter] %d Wait for loading textures!", jobId);
            return true;
        }

        task->load_texture_file_list(); // 加载 texture 文件
        QString message = QObject::tr("加载上传文件");
        updateUploadState(uploadState, message, enUploadQueued);
        m_waitingUploads.push_back(info);
    }

    task->resetFailed();
    task->setSameFileError(false);

    LOGFMTI("[TaskCenter] startUpload task %d!", jobId);

    return true;
}

bool CTaskCenterManager::loadUploadInfo(UploadInfo & info, TaskElementPtr task)
{
    int ntaskid       = task->getJobId();
    info.bVip         = false;
    info.uid          = MyConfig.userSet.id;
    info.taskId       = task->getJobId();
    info.localPath    = getProjectPath(task->getJobId());
    info.scenePath    = task->getSceneFile();
    info.retryTimes   = task->getTaskRetryTimes();
    info.taskPriority = task->getTaskUploadPriority();
    return true;
}

bool CTaskCenterManager::updateUploadState(JobState *uploadState, const QString& strInfo, enTransformStatus status)
{
    uploadState->setSpeedString(strInfo);
    uploadState->setTransStatus(status);
    return true;
}

bool CTaskCenterManager::loadUploadState(JobState *uploadState, TaskElementPtr task)
{
    QString msg = QObject::tr("等待上传");
    uploadState->setJobType(UPLOAD_JOB);
    uploadState->setSpeedString(msg);
    uploadState->setTransStatus(enUploadQueued);
    uploadState->setUploadProgress(task->calcProgress());
    uploadState->setScene(task->getSceneName());
    uploadState->setProjectName(task->getProjectName());
    uploadState->setTaskIdAlias(JobStateMgr::Inst()->getTaskIdAlias(task->getJobId()));
    uploadState->setUploadedFileSize(task->getCompleteFileSize());
    uploadState->setPicCount(QString::number(task->getPictureCount()));
    uploadState->setCoordCount(QString::number(task->getCoordCount()));
    uploadState->setProgress(task->getCompeleteCount(), task->getTotalFileCount());
    return true;
}

// 停止上传任务
bool CTaskCenterManager::stopUploadTask(t_taskid jobId)
{
    JobState *pUploadState = JobStateMgr::Inst()->LookupTaskstate(jobId);
    if(pUploadState == nullptr) {
        LOGFMTW("[TaskCenter] Upload state not found, memory error! taskid=%d", jobId);
        return false;
    }

    updateUploadState(jobId, enUploadStop);

    TaskElementPtr task = findTaskElement(jobId);
    if(task.isNull()) {
        LOGFMTW("[TaskCenter] Task not found, memory error!");
        return false;
    }
    task->stop();

    if(!m_uploadQueue.stopTask(jobId)) {
        LOGFMTE("[TaskCenter] Stop upload failed, push to queue failed! taskid=%d", jobId);
        return false;
    }

    LOGFMTI("[TaskCenter] Stop upload taskid=%d", jobId);

    JobState *jobState = JobStateMgr::Inst()->LookupAllUploadTaskstate(jobId);
    if(jobState == nullptr) {
        return false;
    }
    addUploadHistory(jobId, enUploadStop, jobState);
    jobState->setSpeed(0);

    return true;
}

void CTaskCenterManager::removeWaitUpload(t_taskid jobId)
{
    auto it = m_waitingUploads.begin();
    while(it != m_waitingUploads.end()) {
        if(it->taskId == jobId) {
            m_waitingUploads.erase(it);
            break;
        }
        it++;
    }
}

bool CTaskCenterManager::getWaitUploadIsExist(t_taskid jobId)
{
    auto it = m_waitingUploads.begin();
    while(it != m_waitingUploads.end()) {
        if(it->taskId == jobId) {
            return true;
        }
        it++;
    }
    return false;
}

//删除任务
bool CTaskCenterManager::removeUploadTask(t_taskid jobId)
{
    removeWaitUpload(jobId);

    LOGFMTI("[TaskCenter] Remove upload taskid=%d", jobId);

    this->removeUploadHistory(jobId);

    return true;
}

TaskElementPtr CTaskCenterManager::addNewTask(t_taskid jobId)
{
    TaskElementPtr task = createTaskElement(MyConfig.userSet.id, jobId);
    if(task->load_project_file()) {
        resetTaskState(task);
    } else {
        LOGFMTE("[TaskCenter] Add new task failed, load project failed! taskid=%d", jobId);
        task.reset();
    }

    return task;
}

void CTaskCenterManager::loadFileFinished(t_taskid jobId)
{
    TaskElementPtr task = findTaskElement(jobId);
    if(task.isNull())
        return;

    task->load_file_thread_finished();

    //将上传没有post文件，post
    this->startInsertTask(PAGE_UPLOAD);
    for(int i = 0; i < m_waitingUploads.size(); i++) {
        if(jobId == m_waitingUploads[i].taskId) {
            this->startUpload(jobId);
        }
    }
    this->insertTaskEnd(PAGE_UPLOAD);

    if(task->isStopped()) {
        m_uploadQueue.stopTask(jobId);
    }
}

bool CTaskCenterManager::dumpParam(QSharedPointer<RenderParamConfig> param, QString& argumentFile)
{
    m_taskWriter.reset(new TaskCCWriter(param->task));
    m_taskWriter->job_id = param->jobId;
    m_taskWriter->job_id_alias = param->jobIdAlias;
    m_taskWriter->cg_file = param->cgFile;
    m_taskWriter->ram = param->renderRAM;
    m_taskWriter->frameTimeoutRemind = param->frameTimeoutRemind;
    m_taskWriter->frameTimeoutStop = param->frameTimeoutStop;
    m_taskWriter->gpuCount = param->gpuCount;
    m_taskWriter->backup_and_remove_josn_file();
    m_taskWriter->init_project_path();
    m_taskWriter->dump_groupinfo_json(param);

    return m_taskWriter->dump_argument_json(param, argumentFile);
    // TaskInfoCC* task = (TaskInfoCC*)param->task.data();
    // if (m_taskWriter->dump_taskjson(param, 0)
    //     && m_taskWriter->dump_uploadjson(param)
    //     && m_taskWriter->dump_tipsjson(param)
    //     && m_taskWriter->dump_assetjson(param)) {
    //     return m_taskWriter->update_cc_task(*task);
    // }
    // return false;
}

void CTaskCenterManager::uploadTask(t_uid userId, t_taskid jobId, bool isClone)
{
    TaskElementPtr task = addNewTask(jobId);
    if(task.isNull()) {
        LOGFMTE("[TaskCenter] Upload task stopped as task json is invalid!");
        CGlobalHelp::Response(UM_MESSAGE_FLOAT, MSGBOX_TYPE_ADDTASK_FAILED, jobId);
        return;
    }

    JobState *jobState = JobStateMgr::Inst()->LookupTaskstate(jobId);
    if(jobState == nullptr) {
        LOGFMTE("[TaskCenter] task state not found, memory error!");
        return;
    }
    jobState->setPicCount(QString::number(task->getPictureCount()));
    jobState->setCoordCount(QString::number(task->getCoordCount()));

    //保存历史记录
    addUploadHistory(jobId, enUploadUnfinished, jobState);

    //开始上传
    startUpload(jobId);

    CGlobalHelp::Response(UM_START_UPLOAD_TASK, jobId, isClone);
}

// 创建新的上传任务，如果已经存在返回NULL
TaskElementPtr CTaskCenterManager::createTaskElement(t_uid userId, t_taskid jobId)
{
    LOGFMTI("[TaskCenter] create new task element, taskid=%d", jobId);

    TaskElementPtr task = findTaskElement(jobId);
    if(task.isNull()) {
        stTaskElement element{ userId, jobId };
        task = TaskElementPtr::create(element);
        addTaskElement(task);
    }
    task->m_job_id_alias = JobStateMgr::Inst()->getTaskIdAlias(jobId);

    return task;
}

//从配置中加载已有任务失败时需要重新创建任务
TaskElementPtr CTaskCenterManager::rebuildTask(t_taskid jobId, bool isClone)
{
    TaskElementPtr task = findTaskElement(jobId);
    if(task.isNull()) task = createTaskElement(MyConfig.userSet.id, jobId);
    if(task.isNull()) {
        LOGFMTE("[TaskCenter] Add new task failed, packet invalid! taskid=%d", jobId);
        return task;
    }
    //加载工程
    if(task->load_project_file(isClone)) {
        resetTaskState(task);
    } else {
        LOGFMTE("[TaskCenter] Add new task failed, load project failed! taskid=%d", jobId);
    }

    return task;
}

TaskElementPtr CTaskCenterManager::loadExistTask(t_taskid jobId)
{
    QString appConfigPath = RBHelper::getAppDataPath();
    appConfigPath.append("users");
    appConfigPath.append(QDir::separator());
    appConfigPath.append(QString::number(MyConfig.userSet.id));
    appConfigPath.append(QDir::separator());
    appConfigPath.append(QString::number(jobId));

    TaskElementPtr task = loadTaskFileList(appConfigPath, jobId);
    if(task && !task->isTextureFileLoaded()) {
        if(!task->load_project_file()) {
            LOGFMTE("[TaskCenter] Add new task failed, load project failed! taskid=%d", jobId);
        }
    }

    return task != NULL ? task : rebuildTask(jobId);
}

TaskElementPtr CTaskCenterManager::loadUncompleteFile(const QString& filePath, t_taskid jobId)
{
    LOGFMTI("[TaskCenter]==========Load uncompleted file in...=============");

    //创建packet,如果程序第一次启动，一定是新建，其余则是先查询是否已经有了，如果有就不创建，直接返回之前的那个内存数据
    TaskElementPtr task = createTaskElement(MyConfig.userSet.id, jobId);
    if(task.isNull()) {
        LOGFMTE("[TaskCenter] Load packet failed, packet invalid!");
        return task;
    }

    QString dbpath = filePath;
    task = UploadFileDBMgr::getInstance()->findUnfinished(dbpath, task);

    LOGFMTI("[TaskCenter]==========Load uncompleted file out...=============");
    return task;
}

TaskElementPtr CTaskCenterManager::loadTaskFileList(const QString& filePath, t_taskid jobId)
{
    TaskElementPtr task = loadUncompleteFile(filePath, jobId);
    return task;
}

void CTaskCenterManager::resetTaskState(TaskElementPtr task)
{
    if(task.isNull()) return;

    JobState* jobState = JobStateMgr::Inst()->LookupTaskstate(task->getJobId());
    if(jobState == nullptr) {
        jobState = new JobState(task->getJobId());
        JobStateMgr::Inst()->AddTaskstate(jobState);
    }
    QDateTime currDate = QDateTime::currentDateTime();
    QString time = currDate.toString("yyyy-MM-dd hh:mm:ss");

    jobState->setScene(task->getSceneName());
    jobState->setScenePath(task->getSceneFile());
    jobState->setTaskIdAlias(JobStateMgr::Inst()->getTaskIdAlias(task->getJobId()));
    jobState->setLayerName(task->getLayerName());
    jobState->setProjectName(task->getProjectName());
    jobState->setJobType(UPLOAD_JOB);
    jobState->setStartDateString(time);
    jobState->setPicCount(QString::number(task->getPictureCount()));
    jobState->setCoordCount(QString::number(task->getCoordCount()));
}

void CTaskCenterManager::resetUploadState(TaskElementPtr task)
{
    if(task.isNull()) return;

    JobState* uploadState = JobStateMgr::Inst()->LookupTaskstate(task->getJobId());
    if(uploadState == nullptr) return;
    QDateTime currDate = QDateTime::currentDateTime();
    QString time = currDate.toString("yyyy-MM-dd hh:mm:ss");

    uploadState->setJobType(UPLOAD_JOB);
    uploadState->setScene(task->getSceneName());
    uploadState->setLayerName(task->getLayerName());
    uploadState->setProjectName(task->getProjectName());
    uploadState->setUploadProgress(0, 0);
    uploadState->setStartDateString(time);
    uploadState->setTaskIdAlias(task->m_job_id_alias);
    uploadState->setUploadedFileSize(0);
    uploadState->setPicCount(QString::number(task->getPictureCount()));
    uploadState->setCoordCount(QString::number(task->getCoordCount()));
}

TaskElementPtr CTaskCenterManager::findTaskElement(t_taskid jobId)
{
    auto it = m_taskList.find(jobId);
    if(it != m_taskList.end()) {
        return it.value();
    }

    return TaskElementPtr(nullptr);
}

void CTaskCenterManager::resetTaskPacketBid()
{
    auto it = m_taskList.begin();
    for(; it != m_taskList.end(); ++it) {
        it.value()->rebuild_upload_root_map();
    }
}

void CTaskCenterManager::resubmitTask(t_taskid jobId)
{
#ifdef _DEBUG
#if 0
    LOGFMTW("[TaskCenter] [Debugging] task %d submit is blocked!", jobId);
    return;
#endif
#endif
    // 是否禁止提交
    if (isSubmitJobDisable()) {
        return;
    }

    TaskElementPtr task = CTaskCenterManager::Inst()->findTaskElement(jobId);
    if(task.isNull()) return ;

    LOGFMTI("[TaskCenter] SubmitTask start [%d]!", jobId);

    HttpCmdManager::getInstance()->taskSubmit(jobId, task->getSceneName());
}

void CTaskCenterManager::addUploadHistory(t_taskid jobId, int status, JobState *jobState)
{
    TransHistoryInfo trasnInfo;
    trasnInfo.taskId       = jobId;
    trasnInfo.jobType      = jobState->getJobType();
    trasnInfo.camera       = jobState->getCamera().toLocal8Bit();
    trasnInfo.scenePath    = jobState->getScenePath().toLocal8Bit();
    trasnInfo.sceneName    = jobState->getProjectName().toLocal8Bit();
    trasnInfo.downloadPath = jobState->getOutputLabel().toLocal8Bit();
    trasnInfo.artist       = jobState->getArtistName().toLocal8Bit();
    trasnInfo.startTime    = jobState->getStartDateString().toLocal8Bit();
    trasnInfo.endTime      = jobState->getCompelteDateString().toLocal8Bit();
    trasnInfo.transState   = status;
    trasnInfo.taskAlias    = jobState->getTaskIdAlias().toStdString();
    trasnInfo.pictureCount = jobState->getPicCount().toInt();
    trasnInfo.coordCount   = jobState->getCoordCount().toInt();
    HistoryDbMgr::getInstance()->addTransTask(jobId, trasnInfo);
}

void CTaskCenterManager::removeUploadHistory(t_taskid jobId)
{
    HistoryDbMgr::getInstance()->removeTransTask(jobId);
}

void CTaskCenterManager::addDownloadHistory(t_taskid jobId, int status, CDownloadState *jobState, int checkingstatus)
{
    DownloadHistoryInfo historyInfo;
    historyInfo.project         = jobState->getProjectName().toStdString();
    historyInfo.status          = status;
    historyInfo.artist          = jobState->GetArtist().toStdString();
    historyInfo.sceneName       = jobState->GetScene().toStdString();
    historyInfo.layerName       = jobState->GetLayerName().toStdString();
    historyInfo.taskId          = jobId;
    historyInfo.ncheckingstatus = checkingstatus;
    historyInfo.storageId       = jobState->getStorageId();
    historyInfo.outputLabel     = jobState->getOutputLabel().toStdString();
    historyInfo.taskAlias       = jobState->getJobIdAlias().toStdString();
    historyInfo.savePath        = jobState->GetLocalSavePath().toStdString();
    historyInfo.saveAsPath      = jobState->GetLocalSaveAsPath().toStdString();
    JobState* pDownload = JobStateMgr::Inst()->LookupTaskstate(jobId);
    if(pDownload != nullptr) {
        historyInfo.startTime = pDownload->getStartDateString().toStdString();
        historyInfo.stopTime = pDownload->getDownloadStopDate().toStdString();
        historyInfo.outputSize = QString::number(pDownload->getTaskOutputSize()).toStdString();
    }
    QString outputTypes;
    foreach(QString output, jobState->getDownloadFileList()) {
        outputTypes.append(output);
        outputTypes.append("|");
    }
    historyInfo.downloadType = outputTypes.toStdString();

#if DOWNLOAD_PAGE_TREE_STRUCT
    if(jobState->getParentItem() != nullptr) {
        historyInfo.parentId = jobState->getParentItem()->GetTaskId();
        historyInfo.parentTaskAlias = jobState->getParentItem()->GetTaskIdAlias().toStdString();
    } else {
        historyInfo.parentId = -1;
    }
#else
    historyInfo.parentId = jobState->GetParentTaskId();
#endif
    HistoryDbMgr::getInstance()->addDownloadTask(jobId, historyInfo);

/*
    // trans 数据库
    TransHistoryInfo trasnInfo;
    trasnInfo.taskId    = jobId;
    trasnInfo.jobType   = jobState->getJobType();
    trasnInfo.camera    = jobState->getCamera().toLocal8Bit();
    trasnInfo.scenePath = jobState->getScenePath().toLocal8Bit();
    trasnInfo.sceneName = jobState->getScene().toLocal8Bit();
    trasnInfo.taskAlias = jobState->getTaskIdAlias().toLocal8Bit();

    QString projectSavePath = jobState->getAutoDownPath();
    if (jobState->getDownloadPathCode() == DOWNLOAD_USE_ANSI) {
        trasnInfo.downloadPath  = jobState->getAutoDownPath().toLocal8Bit();
        trasnInfo.loaclSavePath = jobState->getLocalSavePath().toLocal8Bit();
    }
    else if (jobState->getDownloadPathCode() == DOWNLOAD_USE_UNICODE) {
        trasnInfo.downloadPath  = jobState->getAutoDownPath().toStdString();
        trasnInfo.loaclSavePath = jobState->getLocalSavePath().toStdString();
    }
    trasnInfo.artist        = jobState->getArtistName().toLocal8Bit();
    trasnInfo.transState    = jobState->getTransStatus();
    trasnInfo.autoDownload  = jobState->isAutoDownload();

    QDateTime currDate         = QDateTime::currentDateTime();
    QString time               = currDate.toString("yyyy-MM-dd hh:mm:ss");
    trasnInfo.startTime        = time.toStdString();
    trasnInfo.endTime          = jobState->getCompelteDateString().toLocal8Bit();
    trasnInfo.transState       = status;
    trasnInfo.download_mode    = jobState->getDownloadMode();
    trasnInfo.unicode_download = jobState->getDownloadPathCode();
    trasnInfo.downloadType     = jobState->getDownloadType().toLocal8Bit();
    trasnInfo.pictureCount     = jobState->getPicCount().toInt();
    trasnInfo.coordCount       = jobState->getCoordCount().toInt();

    HistoryDbMgr::getInstance()->addTransTask(jobId, trasnInfo);
    */
}

void CTaskCenterManager::addDownloadHistory(t_taskid jobId, int status, JobState *jobState, int checkingstatus)
{
    DownloadHistoryInfo historyInfo;
    historyInfo.project = jobState->getProjectName().toStdString();
    historyInfo.status = status;
    historyInfo.artist = jobState->getArtistName().toStdString();
    historyInfo.sceneName = jobState->getScene().toStdString();
    historyInfo.layerName = jobState->getLayerName().toStdString();
    historyInfo.taskId = jobId;
    historyInfo.ncheckingstatus = checkingstatus;
    historyInfo.storageId = jobState->getOutputBid();
    historyInfo.outputLabel = jobState->getOutputLabel().toStdString();
    historyInfo.taskAlias = jobState->getTaskIdAlias().toStdString();
    historyInfo.savePath = jobState->getLocalSavePath().toStdString();
    historyInfo.saveAsPath = jobState->getLocalSavePath().toStdString();
    historyInfo.taskUserId = jobState->getSubJobUserId();
    CDownloadState* pDownload = CDownloadStateMgr::Inst()->LookupTaskstate(jobId);
    if (pDownload != nullptr) {
        historyInfo.startTime = pDownload->GetStartDate().toStdString();
        historyInfo.stopTime = pDownload->GetStopDate().toStdString();
        historyInfo.outputSize = QString::number(pDownload->GetTaskOutputSize()).toStdString();
        historyInfo.priorityType = pDownload->getProrityType();
        historyInfo.operateType = pDownload->getJobOperateType();
        historyInfo.downloadJobType = pDownload->getDownloadJobType();
    }

    QString outputTypes;
    foreach(QString output, jobState->getDownloadFileList()) {
        outputTypes.append(output);
        outputTypes.append("|");
    }
    historyInfo.downloadType = outputTypes.toStdString();

#if DOWNLOAD_PAGE_TREE_STRUCT
    if (jobState->getParentItem() != nullptr) {
        historyInfo.parentId = jobState->getParentItem()->GetTaskId();
        historyInfo.parentTaskAlias = jobState->getParentItem()->GetTaskIdAlias().toStdString();
    }
    else {
        historyInfo.parentId = -1;
    }
#else
    historyInfo.parentId = jobState->getTaskParentId();
#endif

    HistoryDbMgr::getInstance()->addDownloadTask(jobId, historyInfo);
}

void CTaskCenterManager::getDownloadHistory(std::map<int, DownloadHistoryInfo>& downloadList, int filterDate)
{
    HistoryDbMgr::getInstance()->getDownloadTaskList(downloadList, filterDate);
}

void CTaskCenterManager::removeDownloadHistory(t_taskid jobId)
{
    // HistoryDbMgr::getInstance()->removeTransTask(jobId);
    HistoryDbMgr::getInstance()->removeDownloadTask(jobId);
}

// 任务提交完成就添加到自动下载队列中
void CTaskCenterManager::submitTaskRet(t_taskid jobId, int code)
{
    if(code == SUBMIT_TASK_OK) {
        JobState* jobState = JobStateMgr::Inst()->LookupTaskstate(jobId);
        if (!jobState) {
            LOGFMTI("CTaskCenterManager::submitTaskSuccess pDownloadState not exist [%d]", jobId);
            return;
        }

        jobState->setIsSubmitSuccess(true);
        jobState->setJobType(RENDER_JOB);
        if (CConfigDb::Inst()->getAutoDownload()) {
            CDownloadState* downloadState = buildDownloadState(jobState);
            CDownloadStateMgr::Inst()->AddDownloadState(downloadState);
            addDownloadHistory(jobId, enDownloadWaiting, downloadState); // 添加一条下载记录到数据库，等待任务完成后下载
        }

        LOGFMTI("[TaskCenter] SubmitTask end [%d] OK", jobId);

        removeTaskElementList(jobId);
        removeUploadHistory(jobId);
    } else {
        JobState *uploadState = JobStateMgr::Inst()->LookupTaskstate(jobId);
        if(uploadState == Q_NULLPTR) return;

        // 有错误的任务，重新设置状态
        enTransformStatus uploadStatus = enNothing;
        QString errorMsg;
        switch(code) {
        case SUBMIT_TASK_FORMAT_ERROR:
            uploadStatus = enUploadWaiting;
            errorMsg = QObject::tr("提交作业失败, 建议删除从新提交");
            break;
        default:
            break;
        }
        if(errorMsg.isEmpty()) return;

        updateUploadState(uploadState, errorMsg, uploadStatus);
    }
}

QString CTaskCenterManager::getProjectPath(t_taskid jobId)
{
    QString projectPath = LocalSetting::getInstance()->getClientProjectPath();
    if(projectPath.isEmpty()) {
        LOGFMTI("[TaskCenter] %d>Project directory is not set!", jobId);
        return "";
    }

    return projectPath;
}

bool CTaskCenterManager::manualStartDownload(t_taskid jobId, bool bAuto)
{
    if (!JobStateMgr::Inst()->checkDownloadFlag(jobId)) {
        LOGFMTE("[TaskCenter] Start download is disabled, taskid=%d", jobId);
        return false;
    }

    JobState *jobState = JobStateMgr::Inst()->LookupAllDownloadTaskstate(jobId);
    if (jobState == nullptr) {
        LOGFMTE("[TaskCenter] Start download failed, task state not found! taskid=%d", jobId);
        return false;
    }
    // 任务不可下载、光子任务、解算任务，则不可下载(单独渲光子时，可以下载光子作业)
    if (!jobState->IsDownloadable() || jobState->isPhotonJob()/* || jobState->isGopRenderJob()*/) {
        LOGFMTE("[TaskCenter] Start download failed, task status is download disabled、photon job、goprender job ! taskid=%d ", jobId);
        return false;
    }
    JobStateMgr::Inst()->pushToAlUploadTaskstate(jobState->getTaskId(), jobState);

    // 1. 先确定这个任务是子任务还是父任务
    // 2. 如果是子任务先找到父任务号，然后找出父节点
    // 3. 若当前是父任务，则只放入管理，不添加至下载
    CDownloadState *downloadState = CDownloadStateMgr::Inst()->LookupTaskstate(jobId);
    if (downloadState == nullptr) {
#if DOWNLOAD_PAGE_TREE_STRUCT
        JobState *parentState = nullptr;
        if (jobState->IsChildItem()) {
            qint64 parentId = jobState->getParentItem()->GetTaskId();
            parentState = JobStateMgr::Inst()->LookupTaskstate(parentId);
            // 如果父节点找不到，先创建父节点
            if (parentState == nullptr) {
                parentState = buildDownloadState(jobState->getParentItem());
                JobStateMgr::Inst()->AddDownloadState(parentState);
            }
            // 父节点创建完成以后，初始化子节点
            JobState *childState = buildDownloadState(jobState, parentState);
            parentState->addChildDownloadState(childState);
            // 在列表中以父节点呈现
            JobStateMgr::Inst()->AddDownloadState(childState, 1);
            // 下载以子节点数据进行下载
            downloadState = childState;
            parentState->SetTransState(enTransDownloading);

        }
        else {
            // 不是子节点或者父节点 则创建后就添加
            downloadState = buildDownloadState(jobState);
            JobStateMgr::Inst()->AddDownloadState(downloadState);
        }

        if (downloadState->isParentItem()) {
            downloadState->SetTransState(enTransDownloading);
        }
#else
        // 不是子节点或者父节点 则创建后就添加
        downloadState = buildDownloadState(jobState);
        CDownloadStateMgr::Inst()->AddDownloadState(downloadState);
#endif
    }
    if (!jobState->getDownloadFileList().isEmpty()) {
        foreach(QString path, jobState->getDownloadFileList()) {
            downloadState->addDownloadFile(path);
        }
    } else {
        foreach(QString path, downloadState->getDownloadFileList()) {
            jobState->addDownloadFile(path);
        }
    }

    downloadState->setTransState(enDownloadQueued);
    downloadState->SetCustomerId(jobState->getSubJobUserId());
    downloadState->setJobOperateType(bAuto ? enAutoDownload : enManualDownload);
    if (downloadState->IsTransform()) {
        LOGFMTI("%d> Task transmitting!", jobId);
        return true;
    }
    downloadState->setLocalSavePath(jobState->getLocalSavePath());
    // 防止出现download页面的parentid为0（请求没返回?），但其实渲染页面的是有壳任务id的情况
    if (downloadState->GetParentTaskId() == 0 && jobState->getTaskParentId() > 0) {
        downloadState->SetParentTaskId(jobState->getTaskParentId());
    }

    downloadState->SetError(false);


    if (jobState->getStatus() == JobState::enRenderTaskFinished
        || jobState->getStatus() == JobState::enRenderTaskFinishTest) {
        addDownloadHistory(jobId, enDownloadLasttime, jobState, EN_CHECKFRAME_OK);
    } else {
        addDownloadHistory(jobId, enDownloadUnfinish, downloadState, EN_CHECKFRAME_OK);
    }

    // 如果是父节点，则不添加至传输引擎
    /*if(downloadState->IsParentItem()) {
        LOGFMTI("%d> Task is parent item!", jobId);
        return false;
    }*/
    // 效果图不需要此判断

    //下载任务已经入队列，等待下载

    QStringList remote_path_list;
    QString root_remote;
    if (downloadState->getOutputLabel().isEmpty()) {
        root_remote = QString("/%1").arg(QString::number(jobId));
    } else {
        root_remote = QString("/%1").arg(jobState->getOutputLabel());
        if (!downloadState->getDownloadFileList().isEmpty()) {
            remote_path_list.clear();
            foreach(QString outputType, downloadState->getDownloadFileList()) {
                remote_path_list.append(QString("%1/%2").arg(root_remote).arg(outputType));
            }           
        }
    }
        
    DownloadInfo info;
    info.isClone           = false;
    info.remotePath        = root_remote;
    info.taskId            = jobId;
    info.m_uid             = jobState->getSubJobUserId();
    info.storagetId        = jobState->getOutputBid();
    info.isVip             = false;
    info.isIgnoreUserId    = false;
    info.bAutoDownload     = bAuto;
    info.remotePathList    = remote_path_list;
    if(m_downloadQueue.startTask(info)) {
        updateDownloadState(jobId, enDownloadWaiting, QObject::tr("等待下载"));

        return true;
    }

    LOGFMTE("[TaskCenter] Start download startTask failed! taskid=%d ", jobId);
    return false;
}

//手动取消下载，并清理自动下载
bool CTaskCenterManager::manualStopDownload(t_taskid jobId, bool bUpdateState)
{
    LOGFMTI("[TaskCenter] Manual stop download! taskid=%d", jobId);
    CDownloadState *downloadState = CDownloadStateMgr::Inst()->LookupTaskstate(jobId);
    if(downloadState == nullptr) {
        LOGFMTE("[TaskCenter] Manual stop task failed! Task state not found, memory error! taskid=%d", jobId);
        return false;
    }

    if (bUpdateState == true) {
        if (!m_downloadQueue.stopTask(jobId)) {
            LOGFMTE("[TaskCenter] Manual stop task failed! taskid=%d", jobId);
            return false;
        }

        updateDownloadState(jobId, enDownloadStop);
        this->addDownloadHistory(jobId, enDownloadStop, downloadState, EN_CHECKFRAME_OK);
    } 

    if (!m_downloadQueue.stopTask(jobId)) {
        LOGFMTE("[TaskCenter] Manual stop task failed! taskid=%d", jobId);
        return false;
    }
    
    return true;
}

bool CTaskCenterManager::needDownload(int taskStatus, int downloadStatus)
{
    // 只有任务完成时才会进行下载
    if (taskStatus == JobState::enRenderTaskFinished) {
        if (downloadStatus != enDownloadStop) {
            return true;
        }
    }

    return false;
}

//如果任务是done状态，但是下载队列中还是等待下载时，也要下载（最后下载一次）
bool CTaskCenterManager::isLastDownload(int taskStatus, int downloadStatus)
{
    if(taskStatus == JobState::enRenderTaskFinished && downloadStatus == enDownloadUnfinish) {
        return true;
    }
    return false;
}

void CTaskCenterManager::runAllAutoDownload(const QList<qint64>& lists)
{
    std::map<int, DownloadHistoryInfo> downloadList;  
    this->getDownloadHistory(downloadList);
    CTaskCenterManager::Inst()->startInsertTask(PAGE_DOWNLOAD);

    auto it = downloadList.begin();
    while(it != downloadList.end()) {
        t_taskid jobId = it->first;
        if (!lists.contains(jobId)) {
            ++it;
            continue; 
        }

        CDownloadState *downloadState = CDownloadStateMgr::Inst()->LookupTaskstate(jobId);
        if(downloadState == NULL) {
            ++it;
            continue;
        }

        if (needDownload(downloadState->GetTransState(), it->second.status)) {
            manualStartDownload(jobId);
        }

        ++it;
    }

    CTaskCenterManager::Inst()->insertTaskEnd(PAGE_DOWNLOAD);
    downloadList.clear();
}

void CTaskCenterManager::runAllUploadHistoryTask()
{
    std::map<int, UploadHistoryInfo> uploadList;
    // HistoryDbMgr::getInstance()->getUploadTaskList(uploadList);
    this->getUploadHistory(uploadList);
    auto it = uploadList.begin();
    int maxUploadCount = LocalSetting::getInstance()->getUploadThreadCount();
    int count = 0;
    this->startInsertTask(PAGE_UPLOAD);
    while(it != uploadList.end()) {
        /*if(count > maxUploadCount)
            break;*/

        t_taskid jobId = it->first;

        if(it->second.status != enUploadStop) {
            if(startUpload(jobId)) {
                count++;
                /*LOGFMTE("[TaskCenter] %d>Start task failed!", taskId);*/
            }
        }

        ++it;
    }
    this->insertTaskEnd(PAGE_UPLOAD);
}

bool CTaskCenterManager::removeDownload(t_taskid jobId)
{
    manualStopDownload(jobId);
    removeDownloadHistory(jobId);

    // 被删掉的任务，任务列表的进度条要还原
    CDownloadState* job = CDownloadStateMgr::Inst()->LookupTaskstate(jobId);
    if(job != nullptr) {
        if(job->isParentItem()) {
            foreach(CDownloadState* child, job->getChildren()) {
                job->setCompleteDate(false);
                job->setLocalSavePath("");
                job->resetDownloadList();
            }
        } else {
            job->setCompleteDate(false);
            job->setLocalSavePath("");
            job->resetDownloadList();
        }
    }

    // 清理掉渲染列表中任务的下载列表
    JobState *jobState = JobStateMgr::Inst()->LookupAllDownloadTaskstate(jobId);
    if (jobState != nullptr) {
        jobState->resetDownloadList();
    }

    return false;
}

JobState * CTaskCenterManager::buildUploadState(JobState *jobState)
{
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    JobState *uploadState = new JobState(jobState->getTaskId());
    uploadState->setTaskIdAlias(JobStateMgr::Inst()->getTaskIdAlias(jobState->getTaskId()));
    uploadState->setProjectName(jobState->getProjectName());
    uploadState->setScene(jobState->getScene());
    uploadState->setScenePath(jobState->getScenePath());
    uploadState->setArtistName(jobState->getArtistName());
    uploadState->setLayerName(jobState->getLayerName());
    uploadState->setStartDateString(time);
    uploadState->setJobType(UPLOAD_JOB);
    uploadState->setPicCount(jobState->getPicCount());
    uploadState->setCoordCount(jobState->getCoordCount());
    return uploadState;
}

CDownloadState* CTaskCenterManager::buildDownloadState(JobState* jobState, CDownloadState* parent)
{
    if(jobState == nullptr) return parent;

    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    CDownloadState *downloadState = new CDownloadState(jobState->getTaskId(), parent);

    // downloadState->setJobType(DOWNLOAD_JOB);
    downloadState->SetStatus(jobState->getStatus());
    downloadState->setProjectName(jobState->getProjectName());
    downloadState->setScene(jobState->getScene());
    downloadState->SetLayerName(jobState->getLayerName());
    //downloadState->SetArtistName(jobState->getArtistName());
    downloadState->setStartDate(time);
    downloadState->setLocalSavePath(jobState->getLocalSavePath());
    downloadState->SetLocalSaveAsPath(jobState->getLocalSavePath());
    downloadState->setTransState(enDownloadWaiting);
    downloadState->setStorageId(jobState->getOutputBid());
    downloadState->setOutputLabel(jobState->getOutputLabel());
    downloadState->setTaskAlias(jobState->getTaskIdAlias());
    downloadState->setOrgScene(jobState->getScene());
    downloadState->setIsOpen(jobState->isOpen());
    downloadState->SetParentTaskId(jobState->getTaskParentId());
    downloadState->SetTaskOutputSize(jobState->getTaskOutputSize());
    downloadState->setOutputType(jobState->getProjectOutputType());
    downloadState->SetCustomerId(jobState->getSubJobUserId());
    downloadState->setDownloadFiles(jobState->getDownloadFileList());
    return downloadState;
}

void CTaskCenterManager::rebuildUploadState(t_taskid jobId)
{
    JobState *uploadState = JobStateMgr::Inst()->LookupTaskstate(jobId);
    if(uploadState != nullptr) {
        removeTaskElementList(jobId);
        TaskElementPtr task = rebuildTask(jobId);
        resetUploadState(task);

        {
            removeWaitUpload(jobId);
        }
        CGlobalHelp::Response(UM_START_UPLOAD_TASK, jobId, 0);

        JobState *pTaskState = JobStateMgr::Inst()->LookupTaskstate(jobId);
        if(pTaskState)
            addUploadHistory(jobId, enUploadUnfinished, pTaskState);
    }
}

void CTaskCenterManager::removeTaskElementList(t_taskid jobId)
{
    auto it = m_taskList.find(jobId);
    if(it != m_taskList.end()) {
        m_taskList.erase(it);
    }
}

void CTaskCenterManager::limitTransSpeed(int type, int limitSpeed)
{
    if(!m_transEngine) return;

    switch(type) {
    case EN_LIMIT_UPLOAD_SPEED: {
        float speed = limitSpeed * LocalSetting::getInstance()->getUploadSpeedFactor();
        m_transEngine->set_max_upload_speed(qCeil(speed));
    }     
        break;
    case EN_LIMIT_DOWNLOAD_SPEED: {
        float speed = limitSpeed * LocalSetting::getInstance()->getDownloadSpeedFactor();
        m_transEngine->set_max_download_speed(qCeil(speed));
    }    
        break;
    case EN_LIMIT_UPLOAD_COUNT:
        // m_transEngine->set_max_upload_count(limitSpeed);
        break;
    case EN_LIMIT_DOWNLOAD_COUNT:
        // m_transEngine->set_max_download_count(limitSpeed);
        break;
    }
}

void CTaskCenterManager::on_resolve(const resolve_callback_info_t & callback_info)
{
    msg_callback_info_t *callback = CMessagePool::Inst()->Pop();
    callback->resolve_info.callback_type = callback_info.callback_type;

    switch(callback_info.message_type) {
    case resolve_callback_message_type_t::callback_send_connect_request:
        callback->resolve_info.message_type = RESOLVE_CONNECTING;
        break;
    case resolve_callback_message_type_t::callback_send_disconnect_request:
        break;
    case resolve_callback_message_type_t::callback_recv_disconnect_response:
        callback->resolve_info.message_type = RESOLVE_DISCONNECTED;
        break;
    case resolve_callback_message_type_t::callback_recv_connect_response:
        callback->resolve_info.message_type = RESOLVE_CONNECTED;
        break;
    case resolve_callback_message_type_t::callback_connect_failure:
    case resolve_callback_message_type_t::callback_connection_exception:
        callback->resolve_info.message_type = RESOLVE_CONNECT_FAILED;
        break;
    case resolve_callback_message_type_t::callback_message_response_exception:
        callback->resolve_info.message_type = RESOLVE_LB_NULL;
        break;
    case resolve_callback_message_type_t::callback_message_response_wait_timeout:
        callback->resolve_info.message_type = RESOLVE_CONNECT_FAILED;
        break;
    case resolve_callback_message_type_t::callback_message_response_success:
        callback->resolve_info.message_type = RESOLVE_LB_SUCCESS;
        break;
    case resolve_callback_message_type_t::callback_trace_info_message:
        callback->resolve_info.message_type = RESOLVE_TRACE;
        callback->resolve_info.error_type = callback_info.error_type;
        break;
    default:
        break;
    }
    callback->msg_type = msg_callback_info_t::TYPE_RESOLVE;
    CTaskCenterManager::Inst()->addTransferMsg(callback);
}
////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CTaskCenterManager::on_queue_data(msg_callback_info_t* pData)
{
    if(pData == nullptr)
        return;

    msg_callback_info_t* tranMsg = pData;
    switch(tranMsg->msg_type) {
    case msg_callback_info_t::TYPE_RESOLVE:
        resolveCallback(tranMsg);
        break;
    case msg_callback_info_t::TYPE_DOWNLOAD:
        downloadMsgCallback(tranMsg);
        break;
    case msg_callback_info_t::TYPE_UPLOAD:
        uploadMsgCallback(tranMsg);
        break;
    case msg_callback_info_t::TYPE_CHECK:
        break;
    }

    CMessagePool::Inst()->Push(tranMsg);
}

void CTaskCenterManager::addTransferMsg(msg_callback_info_t* callBack)
{
    m_msgQueue.post(callBack);
}

QString CTaskCenterManager::lefttimeDetail(quint64 size_transfered, quint64 size_total, float speed)
{
    size_t size_left = size_total - size_transfered;
    int time_left = size_left / (speed + 1.0f);

    if(time_left == 0 || (time_left/3600) > 24)
        return QString();

    QString infoMsg = QObject::tr("(剩余时间:%1)").arg(RBHelper::timeCostToString(time_left));
    return infoMsg;
}

void CTaskCenterManager::printLog(const char* szLogMsg, int page)
{
    if(strlen(szLogMsg) > 0) {
        std::unique_ptr<MsgInfo> pMsg(new MsgInfo(szLogMsg));
        if(CGlobalHelp::Response(UM_PRINT_LOGINFO, page, (intptr_t)pMsg.get()))
            pMsg.release();
    }
}

void CTaskCenterManager::printTransLog(const transmit_callback_info_t& transInfo, int page)
{
#ifdef _DEBUG
    CGlobalHelp::Response(UM_PRINT_TRANSLOG, page, (intptr_t)&transInfo);
#endif
}

bool CTaskCenterManager::uploadMsgFilter(msg_callback_info_t* transferMsg)
{
    char szLogMsg[MAX_PATH_LEN] = {0};
    char szInfoMsg[MAX_PATH_LEN] = {0};
    char szSpeed[20] = {0};

    std::string strErrorDesc = RBHelper::getTransmitErrorDisplay(transferMsg->trans_info.error_type).toStdString();
    std::string strErrorUrl  = RBHelper::getTransmitErrorUrl(transferMsg->trans_info.error_type).toStdString();

    int type = transferMsg->trans_info.connect_type;
    char protoType = type == enConnectType::udp ? 'U' : 'T';

    switch(transferMsg->trans_info.message_type) {
    case UPLOAD_FILE_START: {
        sprintf(szLogMsg,
                  "[%c][%d] %s[start]",
                  protoType,
                  transferMsg->trans_info.task_id,
                  transferMsg->trans_info.local_pathname);
    }
    break;

    case UPLOAD_FILE_COMPELETE: {
        sprintf(szLogMsg,
                  "[%c][%d] %s[complete]",
                  protoType,
                  transferMsg->trans_info.task_id,
                  transferMsg->trans_info.local_pathname);
    }
    break;

    case UPLOAD_FILE_INTERRUPT:
    case UPLOAD_FILE_FAIL_SAME:
    case UPLOAD_FILE_FAIL: {
        if(transferMsg->trans_info.error_type != transmit_callback_error_type_t::success) {
            sprintf(szInfoMsg,
                      "[%c][%d] '%s' -> '%s' [error:%d(%s),storage:%d]",
                      protoType,
                      transferMsg->trans_info.task_id,
                      transferMsg->trans_info.local_pathname,
                      transferMsg->trans_info.remote_pathname,
                      transferMsg->trans_info.error_type,
                      strErrorDesc.c_str(),
                      MyConfig.storageSet.uploadBid);

            QString infoMsg = QString::fromStdString(szInfoMsg);
            std::string htmlInfoMsg = infoMsg.toHtmlEscaped().toStdString();
            sprintf(szLogMsg,
                      "%s %s",
                      htmlInfoMsg.c_str(),
                      strErrorUrl.c_str());
        }
    }
    break;

    case UPLOAD_FILE_PROGRESS: {
        size_t size_sent = transferMsg->trans_info.file_transfer_size;
        size_t size_total = transferMsg->trans_info.file_total_size;

        if(RBHelper::isFloatValid(transferMsg->trans_info.speed)) {
            QString qspeed = RBHelper::convertTransSpeedToStr(transferMsg->trans_info.speed, DISPLAY_UNIT_BIT);
            sprintf(szInfoMsg,
                    "[%c][%d] %s(%.2f%%)[%s]",
                    protoType,
                    transferMsg->trans_info.task_id,
                    transferMsg->trans_info.local_pathname,
                    transferMsg->trans_info.progress,
                    qPrintable(qspeed));

            size_t size_transfered = transferMsg->trans_info.file_transfer_size;
            size_t size_total = transferMsg->trans_info.file_total_size;
            QString infoMsg = lefttimeDetail(size_transfered, size_total, transferMsg->trans_info.speed);

            sprintf(szLogMsg,
                    "%s %s",
                    szInfoMsg,
                    infoMsg.toStdString().c_str());
        } else {
            if(RBHelper::isFloatValid(transferMsg->trans_info.progress)) {
                sprintf(szLogMsg,
                          "[%c][%d] %s(%.2f%%)",
                          protoType,
                          transferMsg->trans_info.task_id,
                          transferMsg->trans_info.local_pathname,
                          transferMsg->trans_info.progress);
            }
        }
    }
    break;
    default:

        break;
    }

    printLog(szLogMsg, PAGE_UPLOAD);
    printTransLog(transferMsg->trans_info, PAGE_UPLOAD);

    return false;
}

void CTaskCenterManager::uploadMsgCallback(msg_callback_info_t* transferMsg)
{
    if(transferMsg == NULL || uploadMsgFilter(transferMsg)) {
        return;
    }

    switch(transferMsg->trans_info.message_type) {

    case UPLOAD_SESSION_START:
        uploadEngineStartedCallback(transferMsg);
        break;
    case UPLOAD_SESSION_STOP:
        uploadCompleteCallback(transferMsg);
        break;
    case UPLOAD_SESSION_ERROR:
        uploadEngineBrokenCallback(transferMsg);
        break;
    case UPLOAD_SESSION_CONNECTING:
        uploadEngineConnectingCallback(transferMsg);
        break;
    case UPLOAD_SESSION_CONNECTED:
        uploadEngineConnectedCallback(transferMsg);
        break;
    case UPLOAD_SESSION_CONNECTFAILED:
        uploadEngineConnectFailedCallback(transferMsg);
        break;
    case UPLOAD_FILE_START:
        uploadFileStartCallback(transferMsg);
        break;
    case UPLOAD_FILE_COMPELETE:
        uploadFileCompleteCallback(transferMsg);
        break;
    case UPLOAD_FILE_FAIL_SAME:
        uploadSameFileCallback(transferMsg);
        break;
    case UPLOAD_FILE_FAIL:
        uploadFileErrorCallback(transferMsg);
        break;
    case UPLOAD_FILE_PROGRESS:
        uploadFileProgressCallback(transferMsg);
        break;
    default:
        break;
    }

    // 过滤秒传信息(速度为0，但是状态是完成)
    int msg = transferMsg->trans_info.message_type;
    float speed = transferMsg->trans_info.speed;    
    bool isFastTrans = (speed == 0 && msg == UPLOAD_FILE_COMPELETE);
    if (!isFastTrans) {
        CGlobalHelp::Response(UM_UPDATE_TRANS_SPEED, UM_UPDATE_TRANS_SPEED_UPLOAD, REFRESH_DISPLAY_SPEED_TIME);
    }

    // 记录回调时间
    t_taskid jobId = transferMsg->trans_info.task_id;
    TaskElementPtr task = findTaskElement(jobId);
    if(!task.isNull()) {
        task->setTaskRecvMsgTime(); // 引擎回调时间
    }
}

bool CTaskCenterManager::downloadMsgFilter(msg_callback_info_t* transferMsg)
{
    char szLogMsg[MAX_PATH_LEN] = {0};
    char szInfoMsg[MAX_PATH_LEN] = {0};
    char szSpeed[20] = {0};

    std::string strErrorDesc = RBHelper::getTransmitErrorDisplay(transferMsg->trans_info.error_type).toStdString();
    std::string strErrorUrl  = RBHelper::getTransmitErrorUrl(transferMsg->trans_info.error_type).toStdString();

    int type = transferMsg->trans_info.connect_type;
    char protoType = type == enConnectType::udp ? 'U' : 'T';

    switch(transferMsg->trans_info.message_type) {
    case DOWNLOAD_SESSION_START:
    case DOWNLOAD_FILE_START: {
        sprintf(szLogMsg,
                "[%c][%d] %s[start]",
                protoType,
                transferMsg->trans_info.task_id,
                transferMsg->trans_info.remote_pathname);
    }
    break;

    case DOWNLOAD_SESSION_STOP:
    case DOWNLOAD_FILE_STOP:
    case DOWNLOAD_FILE_COMPLETED: {
        sprintf(szLogMsg,
                "[%c][%d] %s[complete]",
                protoType,
                transferMsg->trans_info.task_id,
                transferMsg->trans_info.remote_pathname);
    }
    break;

    case DOWNLOAD_SESSION_ERROR:
    case DOWNLOAD_FILE_ERROR:
    case DOWNLOAD_ALLFILE_HASFAILURE: {
        sprintf(szInfoMsg,
                "[%c][%d] '%s' <- '%s' [error:%d(%s),storage:%d]",
                protoType,
                transferMsg->trans_info.task_id,
                transferMsg->trans_info.local_pathname,
                transferMsg->trans_info.remote_pathname,
                transferMsg->trans_info.error_type,
                strErrorDesc.c_str(),
                transferMsg->trans_info.storage_id == 0 
                ? MyConfig.storageSet.downloadBid : transferMsg->trans_info.storage_id);

        QString infoMsg = QString::fromStdString(szInfoMsg);
        std::string htmlInfoMsg = infoMsg.toHtmlEscaped().toStdString();
        sprintf(szLogMsg,
                  "%s %s",
                  htmlInfoMsg.c_str(),
                  strErrorUrl.c_str());
    }
    break;

    case DOWNLOAD_FILE_PROGRESS: {
        if(RBHelper::isFloatValid(transferMsg->trans_info.speed)) {
            QString qspeed = RBHelper::convertTransSpeedToStr(transferMsg->trans_info.speed, DISPLAY_UNIT_BIT);
            sprintf(szInfoMsg,
                    "[%c][%d] %s(%.2f%%)[%s]",
                    protoType,
                    transferMsg->trans_info.task_id,
                    transferMsg->trans_info.remote_pathname,
                    transferMsg->trans_info.progress,
                    qPrintable(qspeed));

            size_t size_transfered = transferMsg->trans_info.file_transfer_size;
            size_t size_total = transferMsg->trans_info.file_total_size;
            QString infoMsg = lefttimeDetail(size_transfered, size_total, transferMsg->trans_info.speed);

            sprintf(szLogMsg,
                    "%s %s",
                    szInfoMsg,
                    infoMsg.toStdString().c_str());
        } else {
            if(RBHelper::isFloatValid(transferMsg->trans_info.progress)) {
                sprintf(szLogMsg,
                        "[%c][%d] %s(%.2f%%)",
                        protoType,
                        transferMsg->trans_info.task_id,
                        transferMsg->trans_info.remote_pathname,
                        transferMsg->trans_info.progress);
            }
        }
    }
    break;

    default:
        break;
    }

    printLog(szLogMsg, PAGE_DOWNLOAD);
    printTransLog(transferMsg->trans_info, PAGE_DOWNLOAD);

    return false;
}

void CTaskCenterManager::downloadMsgCallback(msg_callback_info_t* transferMsg)
{
    if(transferMsg == NULL || downloadMsgFilter(transferMsg)) {
        return;
    }

    switch(transferMsg->trans_info.message_type) {
    case DOWNLOAD_SESSION_START:
        downloadEngineStartedCallback(transferMsg);
        break;
    case DOWNLOAD_SESSION_STOP:
        downloadEngineStopCallback(transferMsg);
        break;
    case DOWNLOAD_SESSION_ERROR:
        downloadEngineBrokenCallback(transferMsg);
        break;
    case DOWNLOAD_SESSION_CONNECTING:
        downloadEngineConnectingCallback(transferMsg);
        break;
    case DOWNLOAD_SESSION_CONNECTED:
        downloadEngineConnectedCallback(transferMsg);
        break;
    case DOWNLOAD_SESSION_CONNECTFAILED:
        downloadEngineConnectFailedCallback(transferMsg);
        break;
    case DOWNLOAD_FILE_START:
        downloadFileStartCallback(transferMsg);
        break;
    case DOWNLOAD_FILE_STOP:
    case DOWNLOAD_FILE_COMPLETED:
        break;
    case DOWNLOAD_FILE_ERROR:
        downloadFileExceptionCallback(transferMsg);
        break;
    case DOWNLOAD_FILE_PROGRESS:
        downloadFileProgressCallback(transferMsg);
        break;
    case DOWNLOAD_ALLFILE_COMPLETED:
        downloadCompleteCallback(transferMsg);
        break;
    case DOWNLOAD_ALLFILE_HASFAILURE:
        downloadEngineHasfailureCallback(transferMsg);
        break;

	case DOWNLOAD_CLONE_COMPLETED:
		downloadEngineCloneStopCallback(transferMsg);
		break;
	case DOWNLOAD_CLONE_SESSION_ERROR:
		downloadEngineCloneBrokenCallback(transferMsg);
		break;
	case DOWNLOAD_CLONE_SESSION_STOP:
		downloadEngineCloneDoneCallback(transferMsg);
		break;

    default:
        break;
    }

    CGlobalHelp::Response(UM_UPDATE_TRANS_SPEED, UM_UPDATE_TRANS_SPEED_DOWNLOAD, REFRESH_DISPLAY_SPEED_TIME);
}

void CTaskCenterManager::resolveCallback(msg_callback_info_t* transferMsg)
{
    std::string strMessage;
    static bool bHasBaolongNoticed = false;
    if(transferMsg == nullptr) return;

    char szLogMsg[512] = {0};
    switch(transferMsg->resolve_info.message_type) {
    case RESOLVE_CONNECTING:
        strMessage = QObject::tr("正在接入传输服务器...").toStdString();
        sprintf(szLogMsg, strMessage.c_str());
        break;
    case RESOLVE_CONNECTED:
        bHasBaolongNoticed = false;
        strMessage = QObject::tr("成功接入传输服务器...").toStdString();
        sprintf(szLogMsg, strMessage.c_str());
        break;
    case RESOLVE_DISCONNECTED:
        CGlobalHelp::Response(UM_UPLOAD_DOWNLOAD_ERROR, DISCONNECT_SERVER, 0);
        strMessage = QObject::tr("与传输服务器断开连接...").toStdString();
        sprintf(szLogMsg, strMessage.c_str());
        break;
    case RESOLVE_CONNECT_FAILED:
        CGlobalHelp::Response(UM_UPLOAD_DOWNLOAD_ERROR, FAILED_CONNECT_SERVER, 0);
        strMessage = QObject::tr("与传输接入服务器建立连接失败...").toStdString();
        sprintf(szLogMsg, "[ERROR:%d] %s", transferMsg->resolve_info.error_type, strMessage.c_str());
        break;
    case RESOLVE_LB_SUCCESS:
        if(bHasBaolongNoticed) return;
        bHasBaolongNoticed = true;
        strMessage = QObject::tr("成功获取传输服务器地址...").toStdString();
        sprintf(szLogMsg, strMessage.c_str());
        break;
    case RESOLVE_LB_NULL:
        CGlobalHelp::Response(UM_UPLOAD_DOWNLOAD_ERROR, UNABLE_OBTAIN_SERVER_ADDR, 0);
        bHasBaolongNoticed = false;
        strMessage = QObject::tr("无法获取传输服务器地址...").toStdString();
        sprintf(szLogMsg, "[ERROR:%d] %s", transferMsg->resolve_info.error_type, strMessage.c_str());
        break;
    case RESOLVE_TRACE:
        resolveTraceInfo(transferMsg->resolve_info.error_type, transferMsg->resolve_info.callback_type);
        break;
    default:
        break;
    }

    int page = PAGE_ALL;
    if(transferMsg->resolve_info.callback_type == enTransmitCallBackType::upload)
        page = PAGE_UPLOAD;
    else if (transferMsg->resolve_info.callback_type == enTransmitCallBackType::download)
        page = PAGE_DOWNLOAD;

    printLog(szLogMsg, page);
}

void CTaskCenterManager::resolveTraceInfo(int error_type, int callback_type)
{
   QString strMessage;

    switch (error_type) {
    case resolve_callback_message_type_t::info_send_connect:
        strMessage = QObject::tr("正在连接到服务器...");
        break;
    case resolve_callback_message_type_t::info_recv_connect:
        strMessage = QObject::tr("连接服务器成功...");
        break;
    case resolve_callback_message_type_t::info_connect_fail:
        strMessage = QObject::tr("连接服务器失败...");
        break;
    case resolve_callback_message_type_t::info_send_userinfo:
        strMessage = QObject::tr("同步传输数据...");
        break;
    case resolve_callback_message_type_t::info_recv_userinfo:
        strMessage = QObject::tr("同步传输数据成功...");
        break;
    case resolve_callback_message_type_t::info_send_userinfo_timeout:
        strMessage = QObject::tr("同步传输数据超时...");
        break;
    case resolve_callback_message_type_t::info_create_dir:
        strMessage = QObject::tr("正在创建本地目录...");
        break;
    case resolve_callback_message_type_t::info_create_dir_done:
        strMessage = QObject::tr("创建本地目录完成...");
        break;
    case resolve_callback_message_type_t::info_wait_done_timeout:
        strMessage = QObject::tr("服务器无响应,正在重试...");
        break;
    default:
        break;
    }

    int page = PAGE_ALL;
    if(callback_type == enTransmitCallBackType::upload)
        page = PAGE_UPLOAD;
    else if(callback_type == enTransmitCallBackType::download)
        page = PAGE_DOWNLOAD;

    printLog(strMessage.toStdString().c_str(), page);
}
//===================上传====================================

// 上传传输引擎已经连接上
void CTaskCenterManager::uploadEngineStartedCallback(msg_callback_info_t* transferMsg)
{
    QString message = QObject::tr("上传中...");
    updateUploadState(transferMsg->trans_info.task_id, enTransUploading, message);
}

// 正在创建连接
void CTaskCenterManager::uploadEngineConnectingCallback(msg_callback_info_t* transferMsg)
{
    QString message = QObject::tr("正在创建连接...");
    updateUploadState(transferMsg->trans_info.task_id, enTransUploading, message);
}

void CTaskCenterManager::uploadEngineConnectedCallback(msg_callback_info_t* transferMsg)
{
    QString message = QObject::tr("与传输服务器确立连接...");
    updateUploadState(transferMsg->trans_info.task_id, enTransUploading, message);
}

void CTaskCenterManager::uploadEngineConnectFailedCallback(msg_callback_info_t* transferMsg)
{
    QString message = QObject::tr("与传输接入服务器建立连接失败");
    updateUploadState(transferMsg->trans_info.task_id, enUploadQueued, message);
}

void CTaskCenterManager::uploadEngineBrokenCallback(msg_callback_info_t *transferMsg)
{
    LOGFMTE("[TaskCenter] Upload connect broken! taskid=%d", transferMsg->trans_info.task_id);
    CGlobalHelp::Response(UM_MESSAGE_FLOAT, MSGBOX_TYPE_ENGINE_BROKEN, PAGE_UPLOAD);
}

// 单个文件上传开始回调
void CTaskCenterManager::uploadFileStartCallback(msg_callback_info_t* transferMsg)
{
    JobState *uploadState = JobStateMgr::Inst()->LookupTaskstate(transferMsg->trans_info.task_id);
    if((uploadState != nullptr) && (uploadState->getTransStatus() != enUploadStop)) {
        QString speedMsg = QString("%1(%2)")
                           .arg(QString::fromUtf8(transferMsg->trans_info.local_pathname))
                           .arg(QObject::tr("开始"));
        uploadState->setUploadFileName(QString::fromUtf8(transferMsg->trans_info.local_pathname));
        uploadState->setTransStatus(enTransUploading);
        uploadState->setSpeedString(speedMsg);

        TaskElementPtr task = findTaskElement(transferMsg->trans_info.task_id);
        if (task == nullptr)
            return;
        uploadState->setProgressByFileSize(task->getUploadedFileSize(), task->getTotalFileSize());
        uploadState->setProgress(task->getCompeleteCount(), task->getTotalFileCount());
    }
}

// 文件上传进度
void CTaskCenterManager::uploadFileProgressCallback(msg_callback_info_t* transferMsg)
{
    JobState *uploadState = JobStateMgr::Inst()->LookupTaskstate(transferMsg->trans_info.task_id);
    if(uploadState == nullptr)
        return;

    QString local_pathname = QString::fromUtf8(transferMsg->trans_info.local_pathname);
    QString remote_pathname = QString::fromUtf8(transferMsg->trans_info.remote_pathname);

    if(uploadState->getTransStatus() != enUploadStop) {
        QString fileName = pathFindFileName(local_pathname);
        QString progress = QString::number(transferMsg->trans_info.progress, 'f', 2);
        QString speedMsg;
        if(RBHelper::isFloatValid(transferMsg->trans_info.speed)) {
            QString qspeed = RBHelper::convertTransSpeedToStr(transferMsg->trans_info.speed, DISPLAY_UNIT_BIT);

            speedMsg = QString("%1(%2%)[%3]")
                       .arg(fileName)
                       .arg(progress)
                       .arg(qspeed);

            uploadState->setSpeed(transferMsg->trans_info.speed);// 速度（数字）           
        } else {
            speedMsg = QString("%1(%2%)[0Kbps]")
                       .arg(fileName)
                       .arg(progress);

            uploadState->setSpeed(0.0);// 速度（数字）
        }

        uploadState->setUploadFileName(local_pathname);
        uploadState->setTransStatus(enTransUploading);
        uploadState->setSpeedString(speedMsg);

        TaskElementPtr task = findTaskElement(transferMsg->trans_info.task_id);
        if (task == nullptr)
            return;
        task->setUploadFileSize(local_pathname, remote_pathname, transferMsg->trans_info.file_transfer_size, transferMsg->trans_info.file_total_size);
        uploadState->setUploadedFileSize(task->getUploadedFileSize());
        uploadState->setProgressByFileSize(task->getUploadedFileSize(), task->getTotalFileSize());
        uploadState->setProgress(task->getCompeleteCount(), task->getTotalFileCount());
    }
}

void CTaskCenterManager::uploadSameFileCallback(msg_callback_info_t* transferMsg)
{
    this->uploadFileErrorCallback(transferMsg);

    t_taskid jobId = transferMsg->trans_info.task_id;

    TaskElementPtr task = findTaskElement(jobId);
    if(task.isNull()) {
        LOGFMTE("[TaskCenter] Upload file complete failed, packet not found! taskid=%d", jobId);
        return;
    }
    LOGFMTW("[TaskCenter] uploadSameFileCallback (%d)! file = %s", jobId, transferMsg->trans_info.local_pathname);
    task->setSameFileError(true);
}

// 文件上传失败回调
void CTaskCenterManager::uploadFileErrorCallback(msg_callback_info_t* transferMsg)
{
    t_taskid jobId = transferMsg->trans_info.task_id;

    TaskElementPtr task = findTaskElement(jobId);
    if(task.isNull()) {
        LOGFMTE("[TaskCenter] Upload file complete failed, packet not found! taskid=%d", jobId);
        return;
    }

    QString local_pathname = QString::fromUtf8(transferMsg->trans_info.local_pathname);
    QString remote_pathname = QString::fromUtf8(transferMsg->trans_info.remote_pathname);

    int ec = transferMsg->trans_info.error_type;
    LOGFMTI("[TaskCenter] uploadFileErrorCallback (%d)! file = %s(%s)", jobId,
            transferMsg->trans_info.local_pathname,
            transferMsg->trans_info.error_description);
    task->failedFile(local_pathname, remote_pathname);
    task->setSameFileError(false);

    JobState *uploadState = JobStateMgr::Inst()->LookupTaskstate(jobId);
    if((uploadState != nullptr) && (uploadState->getTransStatus() != enUploadStop)) {
        QString fileName = pathFindFileName(local_pathname);
        QString speedMsg = QString("%1[%2:%3(%4)]").arg(fileName)
                           .arg(QObject::tr("错误类型"))
                           .arg(ec)
                           .arg(RBHelper::getTransmitErrorDisplay(ec));

        uploadState->setUploadFileName(local_pathname);
        uploadState->setTransStatus(enTransUploading);
        uploadState->setProgress(task->getCompeleteCount(), task->getTotalFileCount());
        uploadState->setUploadProgress(task->calcProgress());
        uploadState->setSpeedString(speedMsg);     
        uploadState->setUploadedFileSize(task->getUploadedFileSize());
        uploadState->setProgressByFileSize(task->getUploadedFileSize(), task->getTotalFileSize());
    }
}

//文件上传完成回调
void CTaskCenterManager::uploadFileCompleteCallback(msg_callback_info_t* transferMsg)
{
    t_taskid jobId = transferMsg->trans_info.task_id;

    TaskElementPtr task = findTaskElement(jobId);
    if(task.isNull() || task->isStopped()) {
        LOGFMTE("[TaskCenter] Upload file complete failed, packet not found or stop! taskid=%d", jobId);
        return;
    }

    QString local_pathname  = QString::fromUtf8(transferMsg->trans_info.local_pathname);
    QString remote_pathname = QString::fromUtf8(transferMsg->trans_info.remote_pathname);

    task->completeFile(local_pathname, remote_pathname);

    JobState *uploadState = JobStateMgr::Inst()->LookupTaskstate(jobId);
    if((uploadState != nullptr) && (uploadState->getTransStatus() != enUploadStop)) {
        QString fileName = pathFindFileName(local_pathname);
        QString speedMsg = QString("%1(%2)")
                           .arg(fileName)
                           .arg(QObject::tr("完成"));

        uploadState->setUploadFileName(local_pathname);
        uploadState->setTransStatus(enTransUploading);
        uploadState->setProgress(task->getCompeleteCount(), task->getTotalFileCount());
        uploadState->setUploadProgress(task->calcProgress());
        uploadState->setSpeedString(speedMsg);      
        uploadState->statisticsUploadedCount();

        qint64 fileSize = task->getFileSize(local_pathname, remote_pathname);
        task->setUploadFileSize(local_pathname, remote_pathname, fileSize, fileSize);
        uploadState->setUploadedFileSize(task->getUploadedFileSize());
        uploadState->setProgressByFileSize(task->getUploadedFileSize(), task->getTotalFileSize());
    }
}

void CTaskCenterManager::uploadCompleteCallback(msg_callback_info_t* transferMsg)
{
    t_taskid jobId = transferMsg->trans_info.task_id;
    TaskElementPtr task = findTaskElement(jobId);
    if(task.isNull() || task->isStopped()) {
        LOGFMTE("[TaskCenter] Complete task failed, upload packet not found or is stopped! taskid=%u", jobId);
        return;
    }

    JobState *uploadState = JobStateMgr::Inst()->LookupTaskstate(jobId);
    if(uploadState == nullptr) {
        LOGFMTE("[TaskCenter] Complete task failed, upload state not found or is stopped! taskid=%u", jobId);
        return;
    }

    // 所有文件传输完全成功
    if(task->is_task_completed()) {
        addUploadHistory(jobId, enUploadCompleted, uploadState);

        m_uploadQueue.freePacket(jobId);

        checkUploadFileCompelete(task);
    }
    // 所有文件传输完成，但是出现错误
    else if(task->is_task_done()) {
        m_uploadQueue.freePacket(jobId);

        QString message = QObject::tr("传输未完成，稍后重试...");
        if(task->hasSameFileError())
            message = QObject::tr("文件被其他线程占用，稍等...");

        updateUploadState(jobId, enUploadQueued, message);
        CGlobalHelp::Response(UM_UPLOAD_DOWNLOAD_ERROR, UPLOAD_TASK_FILE_ERROR, jobId);

        LOGFMTI("[TaskCenter] Upload completed with failed files. taskid=%d", jobId);
    }
    // 文件传输个数不匹配BUG
    else {
        QString message = QObject::tr("传输中 ...");
        updateUploadState(jobId, enTransUploading, message);
        LOGFMTI("[TaskCenter] Upload completed with exception, continue... taskid=%d", jobId);
    }
}

//===================下载====================================

void CTaskCenterManager::downloadEngineStartedCallback(msg_callback_info_t *transferMsg)
{
    QString message = QObject::tr("下载中...");
    updateDownloadState(transferMsg->trans_info.task_id, enTransDownloading, message);
}

// 传输引擎正在连接服务器
void CTaskCenterManager::downloadEngineConnectingCallback(msg_callback_info_t* transferMsg)
{
    QString message = QObject::tr("正在创建连接...");
    updateDownloadState(transferMsg->trans_info.task_id, enTransDownloading, message);
}

// 传输引擎已经连接上
void CTaskCenterManager::downloadEngineConnectedCallback(msg_callback_info_t* transferMsg)
{
    QString message = QObject::tr("与传输服务器确立连接...");
    updateDownloadState(transferMsg->trans_info.task_id, enTransDownloading, message);
}

void CTaskCenterManager::downloadEngineConnectFailedCallback(msg_callback_info_t* transferMsg)
{
    QString message = QObject::tr("与传输接入服务器建立连接失败");
    updateDownloadState(transferMsg->trans_info.task_id, enDownloadQueued, message);
}

void CTaskCenterManager::downloadEngineBrokenCallback(msg_callback_info_t *transferMsg)
{
    LOGFMTE("[TaskCenter] Download connect broken! taskid=%d", transferMsg->trans_info.task_id);
    CGlobalHelp::Response(UM_MESSAGE_FLOAT, MSGBOX_TYPE_ENGINE_BROKEN, PAGE_DOWNLOAD);
}

// 下载失败，添加到自动下载队列中
void CTaskCenterManager::downloadEngineStopCallback(msg_callback_info_t* transferMsg)
{
    t_taskid jobId = transferMsg->trans_info.task_id;

    LOGFMTI("[TaskCenter] Download engine stopped. taskid=%d", jobId);
    CDownloadPacket* packet = (CDownloadPacket*)transferMsg->trans_info.user_data;
    // m_downloadQueue.freePacket(packet);

    JobState* jobState = JobStateMgr::Inst()->LookupAllDownloadTaskstate(jobId);
    if(jobState == nullptr) {
        updateDownloadState(jobId, enDownloadQueued);  // 相关的数据回不来，直接提示错误
        LOGFMTI("[TaskCenter] Download engine state nil. taskid=%d", jobId);
        m_downloadQueue.freePacket(packet);
        return;
    }

    CDownloadState *downloadState = CDownloadStateMgr::Inst()->LookupTaskstate(jobId);
    if((downloadState == nullptr) || (downloadState->GetTransState() == enDownloadStop)) {
        LOGFMTE("[TaskCenter] Task state not found, memory error! taskid=%d", jobId);
        m_downloadQueue.freePacket(packet);
        return ;
    }

    if(!downloadState->GetError()
        && downloadState->isCompleted()
        && downloadState->isDownloadFinished()) {

        // 任务列表的任务在下载完成后进度条变灰
        JobState* job = JobStateMgr::Inst()->LookupTaskstate(jobId);
        if(job != nullptr) {
            job->setDownloadCompleted(downloadState->isDownloadFinished());
        }

        // 检查文件是否有缺帧，坏帧等相关的错误
        QString projectSavePath = ProjectMgr::getInstance()->getProjectOutput(downloadState->getProjectName());
        projectSavePath = downloadState->getDownloadFullPath(projectSavePath);
        QString outputPath = RBHelper::getOutputPath(downloadState->getJobId(),
                                                     downloadState->GetParentTaskId(),
                                                     downloadState->GetScene(),
                                                     projectSavePath);

        LOGFMTI("[TaskCenter] Download engine complete. taskid=%d", jobId);
        updateDownloadState(jobId, enDownloadCompleted);
        addDownloadHistory(jobId, enDownloadCompleted, downloadState, EN_CHECKFRAME_OK);
        m_downloadQueue.freePacket(packet);

        CGlobalHelp::Response(UM_EVENT_COMPLETED, UM_DOWNLOAD_COMPLETED, jobId);
    } else {
        LOGFMTI("[TaskCenter] Download engine not complete. taskid=%d,error=%d,isCompelte=%d",
                jobId,
                downloadState->GetError(),
                downloadState->isDownloadFinished());
        updateDownloadState(jobId, enDownloadQueued);
    }
}

void CTaskCenterManager::downloadFileStartCallback(msg_callback_info_t* transferMsg)
{
    CDownloadState *downloadState = CDownloadStateMgr::Inst()->LookupTaskstate(transferMsg->trans_info.task_id);
    if((downloadState != nullptr) && (downloadState->GetTransState() != enDownloadStop)) {
        downloadState->SetDownloadFileName(QString::fromUtf8(transferMsg->trans_info.remote_pathname));
        downloadState->setTransState(enTransDownloading);
        downloadState->SetError(false);
    }
}

void CTaskCenterManager::downloadFileProgressCallback(msg_callback_info_t* transferMsg)
{
    CDownloadState *downloadState = CDownloadStateMgr::Inst()->LookupTaskstate(transferMsg->trans_info.task_id);
    if((downloadState != nullptr) && (downloadState->GetTransState() != enDownloadStop)) {
        QString fileName = QString::fromUtf8(transferMsg->trans_info.remote_pathname);
        QString progress = QString::number(transferMsg->trans_info.progress, 'f', 2);
        QString strProgress = QString("%1(%2%)").arg(fileName).arg(progress);

        downloadState->SetDownloadFileName(strProgress);
        downloadState->setTransState(enTransDownloading);
        downloadState->SetSpeed(transferMsg->trans_info.speed);
        downloadState->SetProgress(progress.toFloat());
    }
}

void CTaskCenterManager::downloadFileExceptionCallback(msg_callback_info_t* transferMsg)
{
    CDownloadState *downloadState = CDownloadStateMgr::Inst()->LookupTaskstate(transferMsg->trans_info.task_id);
    if((downloadState != nullptr) && (downloadState->GetTransState() != enDownloadStop)) {
        downloadState->SetDownloadFileName(QString::fromUtf8(transferMsg->trans_info.remote_pathname));
        downloadState->setTransState(enDownloadQueued);
        downloadState->SetError(true);
    }
}

//下载完成，停止已完成的任务,并从自动下载列表中清除
//只有任务没有被强制停止才会走这里来
void CTaskCenterManager::downloadCompleteCallback(msg_callback_info_t* transferMsg)
{
    CDownloadState *downloadState = CDownloadStateMgr::Inst()->LookupTaskstate(transferMsg->trans_info.task_id);
    if(downloadState == nullptr)
        return;
    if ((downloadState != nullptr) && (downloadState->GetTransState() != enDownloadStop)) {
        QString name = QString::fromUtf8(transferMsg->trans_info.local_pathname);
        downloadState->setDownloadedFile(name);
        if (downloadState->isDownloadFinished()) {
            downloadState->setTransState(enDownloadCompleted);
        }      
        downloadState->SetError(false);
        downloadState->InvalidateTask();
    }

    if(transferMsg->trans_info.error_type == transmit_callback_error_type_t::success) {
        LOGFMTI("[TaskCenter] Download engine complete. taskid=%d", transferMsg->trans_info.task_id);
        downloadState->SetError(false);
    } else {
        LOGFMTI("[TaskCenter] Download engine complete with failure. taskid=%d, status=%s",
                transferMsg->trans_info.task_id,
                transferMsg->trans_info.error_description);
        downloadState->SetError(true);
    }
}

void CTaskCenterManager::downloadEngineHasfailureCallback(msg_callback_info_t *transferMsg)
{
    CDownloadState *downloadState = CDownloadStateMgr::Inst()->LookupTaskstate(transferMsg->trans_info.task_id);
    if((downloadState != nullptr) && (downloadState->GetTransState() != enDownloadStop)) {
        downloadState->setTransState(enDownloadQueued);
        downloadState->SetError(true);
        downloadState->InvalidateTask();
    }
}

bool CTaskCenterManager::isAllUploadCompleted()
{
    return JobStateMgr::Inst()->isAllCompleted();
}

bool CTaskCenterManager::isAllDownloadCompleted()
{
    return JobStateMgr::Inst()->isAllCompleted();
}

void CTaskCenterManager::downloadPathChanged(const wchar_t * pLocalPath)
{
    QString strOldPath = QString::fromWCharArray(pLocalPath);
    auto tasklist = CDownloadStateMgr::Inst()->getAllDownloadTask();
    auto it = tasklist.begin();
    //遍历下载列表
    while(it != tasklist.end()) {

        CDownloadState *downloadState = it->second;
        if(downloadState != nullptr && (downloadState->GetTransState() == enTransDownloading)) {
            /////对比下载本地路径（如不相符即暂停任务）
            // QString strlocalpath = ProjectMgr::getInstance()->getProjectOutput(downloadState->getCGId());
            QString strlocalpath = ProjectMgr::getInstance()->getProjectOutput(downloadState->getProjectName());
            if(strOldPath != strlocalpath) {
                manualStopDownload(downloadState->getJobId());
                manualStartDownload(downloadState->getJobId(), false);
            }
        }
        it++;
    }

    tasklist.clear();
    delete pLocalPath;
}

void CTaskCenterManager::checkUploadFileCompelete(TaskElementPtr task)
{
    t_taskid jobId = task->getJobId();

    QString message = QObject::tr("上传完成，正在提交...");
    updateUploadState(jobId, enUploadCompleted, message);

    m_uploadQueue.freePacket(jobId);

    CGlobalHelp::Response(UM_EVENT_COMPLETED, UM_UPLOAD_COMPLETED, jobId);

    LOGFMTI("[TaskCenter] Upload completed, submit task. taskid=%d", jobId);
}

bool CTaskCenterManager::startDownloadHistoryTask(t_taskid jobId)
{
    if(!JobStateMgr::Inst()->checkDownloadFlag(jobId)) {
        LOGFMTE("[TaskCenter] Start download is disabled, taskid=%d", jobId);
        return false;
    }

    CDownloadState* downloadState = CDownloadStateMgr::Inst()->LookupTaskstate(jobId);
    if(downloadState == nullptr) {
        LOGFMTE("[TaskCenter] Start download failed, task state not found! taskid=%d", jobId);
        return false;
    }
    downloadState->setTransState(enDownloadQueued);

    // 验证jobstate的完整性
    // JobState* job = JobStateMgr::Inst()->LookupAllDownloadTaskstate(jobId);
    // if (job == nullptr) {
    //     updateDownloadState(jobId, enDownloadQueued);
    //     LOGFMTE("[TaskCenter] Start download failed, job state not found, server not response jobinfo! taskid=%d", jobId);
    //     return false;
    // }

    // 如果是父节点，则不添加至传输引擎
    if(downloadState->isParentItem()) {
        downloadState->setTransState(enTransDownloading);
        LOGFMTI("[TaskCenter] %d Task is parent item!", jobId);
        return false;
    }

    // if(downloadState->IsTransform()) {
    //     LOGFMTI("[TaskCenter] %d Task transmitting!", jobId);
    //     return true;
    // }

    if(downloadState->GetTransState() == JobState::enRenderTaskFinished || downloadState->GetTransState() == JobState::enRenderTaskStop) {
        addDownloadHistory(jobId, enDownloadLasttime, downloadState, EN_CHECKFRAME_OK);
    } else {
        addDownloadHistory(jobId, enDownloadUnfinish, downloadState, EN_CHECKFRAME_OK);
    }

    downloadState->SetError(false);
    //下载任务已经入队列，等待下载
    QStringList remote_path_list;
    QString root_remote;
    if (downloadState->getOutputLabel().isEmpty()) {
        root_remote = QString("/%1").arg(QString::number(jobId));
    }
    else {
        root_remote = QString("/%1").arg(downloadState->getOutputLabel());
        if (!downloadState->getDownloadFileList().isEmpty()) {
            remote_path_list.clear();
            foreach(QString outputType, downloadState->getDownloadFileList()) {
                remote_path_list.append(QString("%1/%2").arg(root_remote).arg(outputType));
            }
        }
    }

    DownloadInfo info;
    info.isClone           = false;
    info.remotePath        = root_remote;
    info.taskId            = jobId;
    info.m_uid             = downloadState->GetCustomerId();
    info.storagetId        = downloadState->getStorageId();
    info.isVip             = false;
    info.isIgnoreUserId    = false;
    info.bAutoDownload     = true;
    info.remotePathList    = remote_path_list;
    if(m_downloadQueue.startTask(info)) {
        updateDownloadState(jobId, enDownloadWaiting, QObject::tr("等待下载"));

        return true;
    }

    return false;
}

void CTaskCenterManager::restartUpload(t_taskid jobId)
{
    JobState *pUploadState = JobStateMgr::Inst()->LookupTaskstate(jobId);
    if(pUploadState == nullptr) {
        LOGFMTW("[TaskCenter::restartUpload] Upload state not found, memory error! taskid=%d", jobId);
        return;
    }

    updateUploadState(jobId, enUploadQueued);

    TaskElementPtr task = findTaskElement(jobId);
    if(task.isNull()) {
        LOGFMTW("[TaskCenter::restartUpload] Task not found, memory error!");
        return;
    }
    task->stop();

    if(!m_uploadQueue.stopTask(jobId)) {
        LOGFMTE("[TaskCenter::restartUpload] Stop upload failed, push to queue failed! taskid=%d", jobId);
    }
}

void CTaskCenterManager::addUploadTransMsg(msg_callback_info_t* callBack)
{
    m_uploadQueue.addUploadRunningMsg(callBack);
}

void CTaskCenterManager::handleUploadTransMsg(msg_callback_info_t* callBack)
{
    if (callBack == nullptr)
        return;

    msg_callback_info_t* tranMsg = callBack;
    switch (tranMsg->msg_type) {
    case msg_callback_info_t::TYPE_UPLOAD:
        uploadMsgCallback(tranMsg);
        break;
    }

    CMessagePool::Inst()->Push(tranMsg);
}

void CTaskCenterManager::updateTaskTable(int page, const QVector<qint64>& table)
{
    switch (page)
    {
    case PAGE_BATCH:
        break;
    case PAGE_UPLOAD:
        m_uploadQueue.updateTaskTable(table);
        break;
    case PAGE_DOWNLOAD:
        m_downloadQueue.updateTaskTable(table);
        break;
    default:
        break;
    } 
}

void CTaskCenterManager::startInsertTask(int page)
{
    switch (page)
    {
    case PAGE_BATCH:
        break;
    case PAGE_UPLOAD:
        m_uploadQueue.startAddTask();
        break;
    case PAGE_DOWNLOAD:
        m_downloadQueue.startAddTask();
        break;
    default:
        break;
    }
}

void CTaskCenterManager::insertTaskEnd(int page)
{
    switch (page)
    {
    case PAGE_BATCH:
        break;
    case PAGE_UPLOAD:
        m_uploadQueue.addTaskEnd();
        break;
    case PAGE_DOWNLOAD:
        m_downloadQueue.addTaskEnd();
        break;
    default:
        break;
    }  
}

void CTaskCenterManager::addDownloadTransMsg(msg_callback_info_t* callBack)
{
    m_downloadQueue.addUploadRunningMsg(callBack);
}

void CTaskCenterManager::handleDownloadTransMsg(msg_callback_info_t* callBack)
{
    if (callBack == nullptr)
        return;

    msg_callback_info_t* tranMsg = callBack;
    switch (tranMsg->msg_type) {
    case msg_callback_info_t::TYPE_DOWNLOAD:
        downloadMsgCallback(tranMsg);
        break;
    }

    CMessagePool::Inst()->Push(tranMsg);
}

bool CTaskCenterManager::isSubmitJobDisable()
{
    // 禁止提交
    if (MyConfig.renderSet.submitDisable == 1) {
        CGlobalHelp::Response(UM_MESSAGE_FLOAT, MSGBOX_TYPE_SUBMIT_DISABLE, 0);
        return true;
    }
    return false;
}

void CTaskCenterManager::getTransHistory(std::map<int, TransHistoryInfo>& list)
{
    HistoryDbMgr::getInstance()->getAllTransTaskHistory(list);
}

void CTaskCenterManager::getTransUploadHistory(std::map<int, TransHistoryInfo>& list)
{
    HistoryDbMgr::getInstance()->getUnfinishedUploadTransTaskList(list);
}

void CTaskCenterManager::getTransDownloadHistory(std::map<int, TransHistoryInfo>& list)
{
    HistoryDbMgr::getInstance()->getTransDownloadHistroy(list);
}

void CTaskCenterManager::getDownloadHistory(std::map<int, DownloadHistoryInfo>& downloadList)
{
    std::map<int, TransHistoryInfo> downlist;
    HistoryDbMgr::getInstance()->getUnfinishedDownloadTransTaskList(downlist);
    auto it = downlist.begin();
    while (it != downlist.end()) {
        DownloadHistoryInfo downInfo;
        downInfo.sceneName = it->second.sceneName;
        downInfo.savePath = it->second.downloadPath;
        downInfo.artist = it->second.artist;
        downInfo.taskId = it->second.taskId;
        downInfo.startTime = it->second.startTime;
        downInfo.stopTime = it->second.endTime;
        downInfo.status = it->second.transState;

        downloadList.insert(std::make_pair(it->first, downInfo));
        ++it;
    }
}

void CTaskCenterManager::getUploadHistory(std::map<int, UploadHistoryInfo>& uploadList)
{
    std::map<int, TransHistoryInfo> downlist;
    HistoryDbMgr::getInstance()->getUnfinishedUploadTransTaskList(downlist);
    auto it = downlist.begin();
    while (it != downlist.end()) {
        UploadHistoryInfo upInfo;
        upInfo.sceneName = it->second.sceneName;
        upInfo.scenePath = it->second.scenePath;
        upInfo.vip = it->second.vipTask;
        upInfo.taskId = it->second.taskId;
        upInfo.startTime = it->second.startTime;
        upInfo.stopTime = it->second.endTime;
        upInfo.status = it->second.transState;


        uploadList.insert(std::make_pair(it->first, upInfo));
        ++it;
    }
}

bool CTaskCenterManager::createUploadState(t_taskid taskid)
{
    TaskElementPtr task = findTaskElement(taskid);
    if (task == nullptr) {
        LOGFMTE("[TaskCenter] task state not found, memory error!");
        return false;
    }

    JobState *uploadState = JobStateMgr::Inst()->LookupTaskstate(taskid);
    if (uploadState == nullptr) {
        JobStateMgr::Inst()->AddTaskstate(uploadState);
    }
    uploadState = buildUploadState(task);

    // 保存历史记录
    addUploadHistory(taskid, enUploadUnfinished, uploadState);

    return true;
}

JobState* CTaskCenterManager::buildUploadState(TaskElementPtr task)
{
    QDateTime currDate = QDateTime::currentDateTime();
    QString time = currDate.toString("yyyy-MM-dd hh:mm:ss");

    JobState *uploadState = new JobState(task->getJobId());
    uploadState->setTaskIdAlias(JobStateMgr::Inst()->getTaskIdAlias(task->getCgId()));
    uploadState->setProjectName(task->getProjectName());
    uploadState->setScene(task->getSceneName());
    uploadState->setScenePath(task->getSceneFile());
    uploadState->setStartDateString(time);
    uploadState->setJobType(UPLOAD_JOB);
    uploadState->setPicCount(QString::number(task->getPictureCount()));
    uploadState->setCoordCount(QString::number(task->getCoordCount()));
    return uploadState;
}

/************************************************************************/
/* 检查任务是否下载完成。存在下载完成，但是缺帧的情况。
原因：下载在某个时刻回调stop时，渲染状态更新为完成。从而导致 假的 下载完成。
暂定解决方案：
根据引擎的下载规则，客户端每次push任务到引擎时，传输引擎会更新下载列表，根据这个规则
在下载进入完成判断时，不设置完成状态，直到这个判断超过3次后设置为完成（次数可以配置）
/************************************************************************/
bool CTaskCenterManager::checkDownloadFinished(t_taskid jobId)
{
    CDownloadState *downloadState = CDownloadStateMgr::Inst()->LookupTaskstate(jobId);
    if (downloadState == NULL) return true;

    // 任务完成时间超过一定天数的(默认7天)直接返回true，避免过度等待
    QString time = downloadState->GetCompleteDate();
    QDate finishedDate = QDateTime::fromString(time, "yyyy-MM-dd hh:mm:ss").date();
    QDate currDate = MyConfig.currentDateTime.date();
    int days = finishedDate.daysTo(currDate);
    if (days >= LocalSetting::getInstance()->getCheckIsDownloadFinishedTimesSkipDays()) {
        return true;
    }

    int times = downloadState->getFinishedTimes();
    int checkTimes = LocalSetting::getInstance()->getCheckIsDownloadFinishedTimes();

    if (times >= checkTimes) {
        downloadState->setFinishedTimes(0);
        return true;
    }
    downloadState->setFinishedTimes(times + 1);
    return false;
}

bool CTaskCenterManager::downloadTaskJson(t_taskid jobId, t_taskid parentJobId, t_uid userId)
{
	JobState *pTaskState = JobStateMgr::Inst()->LookupAllDownloadTaskstate(jobId);
	if (pTaskState == nullptr) {
		LOGFMTE("[TaskCenter] Start clone download failed, task state not found! taskid=%d", jobId);
		return false;
	}

	QString remote_pathname = QString("/%1/cfg").arg(jobId);
#ifdef FOXRENDERFARM
	QString local_pathname = QString("%1/%2").arg("C:/AvicaCloud/Project").arg(jobId);
#else
	QString local_pathname = QString("%1/%2").arg("C:/DayanCloud/Project").arg(jobId);
#endif


	DownloadInfo info;
	info.isClone = true;
	info.localPath = local_pathname;
	info.remotePath.append(remote_pathname);
	info.taskId = jobId;
	info.parentTaskId = parentJobId;
	info.m_uid = userId;    //uid -> m_uid
	info.storagetId = MyConfig.storageSet.cloneBid;
	info.isVip = true;
	info.isIgnoreUserId = false;

	if (!m_cloneQueue.startTask(info)) {
		LOGFMTE("[TaskCenter] Push to queue failed. taskid=%d", jobId);
		return false;
	}

	return true;
}

void CTaskCenterManager::downloadEngineCloneStopCallback(msg_callback_info_t* transferMsg)
{
	LOGFMTI("[TaskCenter] clone success. taskid=%d", transferMsg->trans_info.task_id);
	CDownloadPacket* packet = (CDownloadPacket*)transferMsg->trans_info.user_data;
	CGlobalHelp::Response(UM_CLONE_CFG_DOWNLOADED, packet->getTaskId(), packet->getParentId());
}

void CTaskCenterManager::downloadEngineCloneBrokenCallback(msg_callback_info_t* transferMsg)
{
	LOGFMTI("[TaskCenter] clone failed. taskid=%d", transferMsg->trans_info.task_id);
	CDownloadPacket* packet = (CDownloadPacket*)transferMsg->trans_info.user_data;
	CGlobalHelp::Response(UM_CLONE_CFG_DOWNLOADED, packet->getTaskId(), 0);
}

void CTaskCenterManager::downloadEngineCloneDoneCallback(msg_callback_info_t *transferMsg)
{
	LOGFMTI("[TaskCenter] clone done. taskid=%d", transferMsg->trans_info.task_id);
	m_cloneQueue.freePacket((CDownloadPacket*)transferMsg->trans_info.user_data);
}
