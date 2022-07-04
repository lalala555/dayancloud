#include "stdafx.h"
#include "JobListViewPage.h"
#include "JobListView.h"
#include "Views/NewJobs/JobStateMgr.h"
#include "Kernel/TaskCenterManager.h"
#include "HttpCommand/HttpCmdManager.h"
#include "Base/Controls/DirOpenMsgBox.h"
#include "ParameterCustom.h"
#include "DownloadPage/DownloadStateMgr.h"
#include "FloatLogPanel.h"
#include "TaskCloneWindow.h"

JobListViewPage::JobListViewPage(JobListView *listView, FloatLogPanel* floatPanel, QObject *parent)
    : QObject(parent)
    , m_listView(listView)
    , m_currentPage(TASK_PAGE_RENDER)
    , m_floatLogPanel(floatPanel)
{
    m_popJobMenu = new QMenu(listView);
    m_popUploadMenu = new QMenu(listView);
    m_downloadMenu = new QMenu(listView);
    m_extProcess = new QProcess(this);

    m_popJobMenu->setMinimumWidth(120);
    m_popUploadMenu->setMinimumWidth(120);

    // 上传菜单
    m_startUploadAction = new QAction(m_popUploadMenu);
    m_startUploadAction->setData(MENU_UPLOAD_START);
    m_popUploadMenu->addAction(m_startUploadAction);
    m_pauseUploadAction = new QAction(m_popUploadMenu);
    m_pauseUploadAction->setData(MENU_UPLOAD_CANCEL);
    m_popUploadMenu->addAction(m_pauseUploadAction);
    m_deleteUploadAction = new QAction(m_popUploadMenu);
    m_deleteUploadAction->setData(MENU_UPLOAD_DELETE);
    m_popUploadMenu->addAction(m_deleteUploadAction);
    m_restartUploadAction = new QAction(m_popUploadMenu);
    m_restartUploadAction->setData(MENU_UPLOAD_REUPLOAD);
#ifdef _DEBUG
    m_popUploadMenu->addAction(m_restartUploadAction);
#endif
    m_forceSubmitAction = new QAction(m_popUploadMenu);
    m_forceSubmitAction->setData(MENU_UPLOAD_FORCESUBMIT);
#ifdef _DEBUG
    m_popUploadMenu->addAction(m_forceSubmitAction);
#endif 
    m_forceSubmitAction->setVisible(false);

    // 渲染菜单
    m_refreshAction = new QAction(m_popJobMenu);
    m_refreshAction->setData(MENU_TASK_REFRESH);
    m_popJobMenu->addAction(m_refreshAction);
    m_popJobMenu->addSeparator();

    m_startAction = new QAction(m_popJobMenu);
    m_startAction->setData(MENU_TASK_START);
    // m_popJobMenu->addAction(m_startAction);
    m_pauseAction = new QAction(m_popJobMenu);
    m_pauseAction->setData(MENU_TASK_PAUSE);
    // m_popJobMenu->addAction(m_pauseAction);
    m_resubFailedAction = new QAction(m_popJobMenu);
    m_resubFailedAction->setData(MENU_TASK_RESUB);
    // m_popJobMenu->addAction(m_resubFailedAction);
    m_popJobMenu->addSeparator();

    m_locateAction = new QAction(m_popJobMenu);
    m_locateAction->setData(MENU_SHOW_LOCAL_PATH);
    // m_popJobMenu->addAction(m_locateAction);
    // m_downloadAction = new QAction(m_popJobMenu);
    // m_downloadAction->setData(MENU_TASK_DOWNLOAD);
    m_popJobMenu->addMenu(m_downloadMenu);
    m_stopDownloadAction = new QAction(m_popJobMenu);
    m_stopDownloadAction->setData(MENU_DOWNLOAD_CANCEL);
    // m_popJobMenu->addAction(m_stopDownloadAction);
    m_popJobMenu->addSeparator();

    m_deleteAction = new QAction(m_popJobMenu);
    m_deleteAction->setData(MENU_TASK_DELETE);
    m_popJobMenu->addAction(m_deleteAction);

    m_cloneTask = new QAction(m_popJobMenu);
    m_cloneTask->setData(MENU_TASK_CLONE);
    m_popJobMenu->addAction(m_cloneTask);

    m_popJobMenu->addSeparator();
    m_loadExistTask = new QAction(m_popJobMenu);
    m_loadExistTask->setData(MENU_TASK_LOAD_EXIST_TASK);
    m_popJobMenu->addAction(m_loadExistTask);

    m_popJobMenu->addSeparator();
    m_copyTaskId = new QAction(m_popJobMenu);
    m_copyTaskId->setData(MENU_TASK_COPY_TASK_ID);
    m_popJobMenu->addAction(m_copyTaskId);
    m_popUploadMenu->addAction(m_copyTaskId);

    m_popJobMenu->addSeparator();
    m_showTransLog = new QAction(m_popJobMenu);
    m_showTransLog->setData(MENU_SHOW_TRANS_LOG);
    m_popJobMenu->addAction(m_showTransLog);
    m_popUploadMenu->addAction(m_showTransLog);

    // 文件下载
    m_downloadMenu->setStyleSheet("QMenu::indicator:exclusive:checked{image: url(:/view/images/view/selected.png);}");
    connect(m_downloadMenu, SIGNAL(triggered(QAction*)), this, SLOT(onDownloadMenuTriggered(QAction*)));

    m_selOSGBAction = new QAction(m_downloadMenu);
    m_selObjAction = new QAction(m_downloadMenu);
    m_sel3DTilesAction = new QAction(m_downloadMenu);
    m_selLASAction = new QAction(m_downloadMenu);
    m_selTIFFAction = new QAction(m_downloadMenu);
    m_selEditableObjAction = new QAction(m_downloadMenu);
    m_sel3MXAction = new QAction(m_downloadMenu);
    m_selFBXAction = new QAction(m_downloadMenu);
    m_selS3CAction = new QAction(m_downloadMenu);
    m_selAllAction = new QAction(m_downloadMenu);

#ifdef FOXRENDERFARM
	m_selOSGBAction->setText(QObject::tr("OSGB"));
	m_selOSGBAction->setData(RBHelper::getOutputFileName(OUTPUT_TYPE_OSGB));
	m_outputActions.insert(OUTPUT_TYPE_OSGB, m_selOSGBAction);
	m_selObjAction->setText(QObject::tr("OBJ"));
	m_selObjAction->setData(RBHelper::getOutputFileName(OUTPUT_TYPE_OBJ));
	m_outputActions.insert(OUTPUT_TYPE_OBJ, m_selObjAction);
	m_sel3DTilesAction->setText(QObject::tr("Cesium 3D Files"));
	m_sel3DTilesAction->setData(RBHelper::getOutputFileName(OUTPUT_TYPE_3D_TILES));
	m_outputActions.insert(OUTPUT_TYPE_3D_TILES, m_sel3DTilesAction);
	m_selLASAction->setText(QObject::tr("LAS"));
	m_selLASAction->setData(RBHelper::getOutputFileName(OUTPUT_TYPE_LAS));
	m_outputActions.insert(OUTPUT_TYPE_LAS, m_selLASAction);
	m_selTIFFAction->setText(QObject::tr("Ortho TIFF File"));
	m_selTIFFAction->setData(RBHelper::getOutputFileName(OUTPUT_TYPE_TIFF));
	m_outputActions.insert(OUTPUT_TYPE_TIFF, m_selTIFFAction);
	m_selEditableObjAction->setText(QObject::tr("Editable OBJ File"));
	m_selEditableObjAction->setData(RBHelper::getOutputFileName(OUTPUT_TYPE_EDIT_OBJ));
	m_outputActions.insert(OUTPUT_TYPE_EDIT_OBJ, m_selEditableObjAction);
	m_sel3MXAction->setText(QObject::tr("3MX"));
	m_sel3MXAction->setData(RBHelper::getOutputFileName(OUTPUT_TYPE_3MX));
	m_outputActions.insert(OUTPUT_TYPE_3MX, m_sel3MXAction);
	m_selFBXAction->setText(QObject::tr("FBX"));
	m_selFBXAction->setData(RBHelper::getOutputFileName(OUTPUT_TYPE_FBX));
	m_outputActions.insert(OUTPUT_TYPE_FBX, m_selFBXAction);
	m_selS3CAction->setText(QObject::tr("S3C"));
	m_selS3CAction->setData(RBHelper::getOutputFileName(OUTPUT_TYPE_S3C));
	m_outputActions.insert(OUTPUT_TYPE_S3C, m_selS3CAction);
	m_selAllAction->setText(QObject::tr("Download All"));
	m_selAllAction->setData("");
#else
	m_selOSGBAction->setText(QObject::tr("OSGB文件"));
	m_selOSGBAction->setData(RBHelper::getOutputFileName(OUTPUT_TYPE_OSGB));
	m_outputActions.insert(OUTPUT_TYPE_OSGB, m_selOSGBAction);
	m_selObjAction->setText(QObject::tr("OBJ文件"));
	m_selObjAction->setData(RBHelper::getOutputFileName(OUTPUT_TYPE_OBJ));
	m_outputActions.insert(OUTPUT_TYPE_OBJ, m_selObjAction);
	m_sel3DTilesAction->setText(QObject::tr("Cesium 3D Tiles文件"));
	m_sel3DTilesAction->setData(RBHelper::getOutputFileName(OUTPUT_TYPE_3D_TILES));
	m_outputActions.insert(OUTPUT_TYPE_3D_TILES, m_sel3DTilesAction);
	m_selLASAction->setText(QObject::tr("LAS文件"));
	m_selLASAction->setData(RBHelper::getOutputFileName(OUTPUT_TYPE_LAS));
	m_outputActions.insert(OUTPUT_TYPE_LAS, m_selLASAction);
	m_selTIFFAction->setText(QObject::tr("正射TIFF文件"));
	m_selTIFFAction->setData(RBHelper::getOutputFileName(OUTPUT_TYPE_TIFF));
	m_outputActions.insert(OUTPUT_TYPE_TIFF, m_selTIFFAction);
	m_selEditableObjAction->setText(QObject::tr("可编辑的OBJ文件"));
	m_selEditableObjAction->setData(RBHelper::getOutputFileName(OUTPUT_TYPE_EDIT_OBJ));
	m_outputActions.insert(OUTPUT_TYPE_EDIT_OBJ, m_selEditableObjAction);
	m_sel3MXAction->setText(QObject::tr("3MX文件"));
	m_sel3MXAction->setData(RBHelper::getOutputFileName(OUTPUT_TYPE_3MX));
	m_outputActions.insert(OUTPUT_TYPE_3MX, m_sel3MXAction);
	m_selFBXAction->setText(QObject::tr("FBX文件"));
	m_selFBXAction->setData(RBHelper::getOutputFileName(OUTPUT_TYPE_FBX));
	m_outputActions.insert(OUTPUT_TYPE_FBX, m_selFBXAction);
	m_selS3CAction->setText(QObject::tr("S3C文件"));
	m_selS3CAction->setData(RBHelper::getOutputFileName(OUTPUT_TYPE_S3C));
	m_outputActions.insert(OUTPUT_TYPE_S3C, m_selS3CAction);
	m_selAllAction->setText(QObject::tr("下载全部"));
	m_selAllAction->setData("");
#endif

    m_downloadMenu->addAction(m_selOSGBAction);
    m_downloadMenu->addAction(m_selObjAction);
    m_downloadMenu->addAction(m_sel3DTilesAction);
    m_downloadMenu->addAction(m_selLASAction);
    m_downloadMenu->addAction(m_selTIFFAction);
    m_downloadMenu->addAction(m_selEditableObjAction);
    m_downloadMenu->addAction(m_sel3MXAction);
    m_downloadMenu->addAction(m_selFBXAction);
    m_downloadMenu->addAction(m_selS3CAction);
    m_downloadMenu->addAction(m_selAllAction);
    m_downloadMenu->setEnabled(false);

    connect(m_popJobMenu, SIGNAL(triggered(QAction *)), this, SLOT(onJobPopMenuSlot(QAction *)));
    connect(m_popUploadMenu, SIGNAL(triggered(QAction *)), this, SLOT(onUploadPopMenuSlot(QAction *)));
    connect(m_listView, SIGNAL(reachedBottom()), this, SLOT(onReachedBottom()));
    connect(m_listView, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(onDoubleClickedSlot(const QModelIndex &)));
    connect(m_listView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onCustomContextMenuRequested(const QPoint&)));
}

JobListViewPage::~JobListViewPage()
{
    delete m_popJobMenu;
    delete m_extProcess;
    delete m_popUploadMenu;
}

void JobListViewPage::retranslateUi()
{
    m_listView->setEmptyHint(":/view/images/view/jobempty.png");

    m_startUploadAction->setText(tr("开始上传"));
    m_pauseUploadAction->setText(tr("暂停上传"));
    m_deleteUploadAction->setText(tr("删除作业"));
    m_restartUploadAction->setText(tr("重新上传"));
    m_forceSubmitAction->setText(tr("跳过校验"));

    m_refreshAction->setText(tr("刷新作业"));
    m_startAction->setText(tr("开始作业"));
    m_pauseAction->setText(tr("停止作业"));
    m_resubFailedAction->setText(tr("重提"));
    m_locateAction->setText(tr("打开存储"));
    m_deleteAction->setText(tr("删除作业"));
    m_downloadMenu->setTitle(tr("下载作业"));
    m_stopDownloadAction->setText(tr("取消下载"));
    m_loadExistTask->setText(tr("提交历史作业"));
    m_cloneTask->setText(tr("克隆作业"));

    m_copyTaskId->setText(tr("复制作业ID"));

    m_showTransLog->setText(tr("运行日志"));
}

void JobListViewPage::initView()
{
    // 启动动画
    this->showLoading(true);
    JobStateMgr::Inst()->SetListView(this);
    JobStateMgr::Inst()->InitTaskState();
    JobStateMgr::Inst()->LoadLocalHistory();

}

void JobListViewPage::addTaskState(JobState * taskState)
{
    m_listView->addState(taskState);
}

void JobListViewPage::removeTaskState(int taskId)
{
    JobState* taskState = JobStateMgr::Inst()->LookupTaskstate(taskId);
    m_listView->removeState(taskState);
}

void JobListViewPage::removeRow(int row)
{
}

void JobListViewPage::showLoading(bool bShow)
{
    m_listView->showLoading(bShow);

    if (!bShow)
        CGlobalHelp::Response(UM_SHOW_LOADING, 0, 0);
}

void JobListViewPage::deleteTaskUi(int taskId)
{
    JobState* job = JobStateMgr::Inst()->LookupTaskstate(taskId);
    if (job == NULL) return;

    if (job->getJobType() == UPLOAD_JOB) {
        CTaskCenterManager::Inst()->stopUploadTask(taskId);
        CTaskCenterManager::Inst()->removeUploadHistory(taskId);
    } else if (job->getJobType() == DOWNLOAD_JOB || job->getJobType() == RENDER_JOB) {
        CTaskCenterManager::Inst()->removeDownloadHistory(taskId);
    }
    JobStateMgr::Inst()->RemoveTaskstate(taskId);
    this->refresh();
}

void JobListViewPage::updateTasklistUi(bool bClean)
{
    if (bClean == true) {
        // 清理item
        m_listView->clearAllItems();

        //加载历史记录
        JobStateMgr::Inst()->LoadLocalHistory();

        JobState *rootItem = JobStateMgr::Inst()->resetTreeRootItem();
        int taskCount = JobStateMgr::Inst()->GetCurTaskCount();
        for (int i = 0; i < taskCount; i++) {

            int taskId = JobStateMgr::Inst()->GetTaskId(i);
            JobState *pTaskState = JobStateMgr::Inst()->LookupTaskstate(taskId);
            if (pTaskState) {
                pTaskState->setTransStatus(pTaskState->getTransStatus());
                addTaskState(pTaskState);
                pTaskState->setParentItem(rootItem);
                rootItem->addChildJobState(pTaskState);
            }
        }
        m_listView->setRootItem(rootItem);
    }

    // 刷新下载信息
    JobStateMgr::Inst()->UpdateDownloadHistroy();

    // 开启下载
    JobState *rootItem = JobStateMgr::Inst()->getTreeRootItem();
    CTaskCenterManager::Inst()->runAllAutoDownload(rootItem->getChildrenIds());

    // 刷新页面
    m_listView->updateView();

    // 取消动画
    this->showLoading(false);
}

void JobListViewPage::showTypeTask(int nType)
{
    this->showLoading(true);
    JobStateMgr::Inst()->SetClean(true);
    JobStateMgr::Inst()->m_vtPage.clear();

    SetShowType(nType);

    HttpCmdManager::getInstance()->filterContent.filterType = TIME_STATE_FILTER;
    HttpCmdManager::getInstance()->filterContent.showType = nType;

    JobStateMgr::Inst()->SendRefreshTasklist(-1, nType);
}

void JobListViewPage::SetShowType(int nType)
{
    MyConfig.client.showType = (SHOW_TASKTPE)nType;
    CConfigDb::Inst()->setShowTaskType(nType);
    m_listView->setProxyFilterKey(nType);
}

int JobListViewPage::GetShowType()
{
    return MyConfig.client.showType;
}

void JobListViewPage::refresh()
{
    m_listView->updateView();
}

void JobListViewPage::MenuStartUpload()
{
    LOGFMTI("[Upload]Menu::startUpload");

    QList<JobState*> states = m_listView->getSelectTaskInfos();
    foreach(auto pState, states) {
        if (pState == NULL || pState->IsUploading()) {
            LOGFMTE("[JobListViewPage] Start upload task failed, state not found!");
            continue;
        }
        CTaskCenterManager::Inst()->startUpload(pState->getTaskId());
    }
    // 执行完后需要刷新一下列表
    this->refresh();
}

void JobListViewPage::MenuStopUpload()
{
    LOGFMTI("[Upload]Menu::stopUpload");

    QList<JobState*> states = m_listView->getSelectTaskInfos();
    foreach(auto pState, states) {
        if (pState
            && pState->getTransStatus() != enUploadCompleted
            && pState->getTransStatus() != enUploadStop) {
            CTaskCenterManager::Inst()->stopUploadTask(pState->getTaskId());
        }
    }

    // 执行完后需要刷新一下列表
    this->refresh();

    CGlobalHelp::Response(UM_UPDATE_TRANS_SPEED, UM_UPDATE_TRANS_SPEED_UPLOAD, 0);
}

void JobListViewPage::MenuDeleteUpload()
{
    LOGFMTI("[Upload]Menu::deleteUpload");

    QList<JobState*> states = m_listView->getSelectTaskInfos();
    QListIterator<JobState*> it(states);
    while (it.hasNext()) {
        auto pState = it.next();
        int nTaskId = pState->getTaskId();
        if (pState) {
            if (pState->getTransStatus() != enUploadCompleted && pState->getTransStatus() != enUploadStop) {
                LOGFMTI("JobListViewPage::RunStop, taskid=%d, state=%d,", nTaskId, pState->getTransStatus());
                CTaskCenterManager::Inst()->stopUploadTask(nTaskId);
            } else {
                // do nothing
            }

            JobStateMgr::Inst()->RemoveTaskstate(nTaskId);
            CTaskCenterManager::Inst()->removeUploadTask(nTaskId);
            HttpCmdManager::getInstance()->taskOperatorDelete(nTaskId);
            QApplication::processEvents();
        }
    }
    this->refresh();

    CGlobalHelp::Response(UM_UPDATE_TRANS_SPEED, UM_UPDATE_TRANS_SPEED_UPLOAD, 0);
}

void JobListViewPage::MenuTaskReupload()
{
    LOGFMTI("[Upload]Menu::reupload");

    QList<JobState*> states = m_listView->getSelectTaskInfos();
    foreach(auto pState, states) {
        if (pState) {
            CTaskCenterManager::Inst()->rebuildUploadState(pState->getTaskId());
        }
    }
}

void JobListViewPage::MenuTaskStart()
{
    LOGFMTI("[Task]Menu::start");

    SelectTaskState(enBtnStart);
}

void JobListViewPage::MenuTaskFullspeed()
{
    LOGFMTI("[Task]Menu::fullspeed");

    SelectTaskState(enBtnFullspeed);
}

void JobListViewPage::MenuTaskStop()
{
    LOGFMTI("[Task]Menu::stop");
    if (Util::ShowMessageBoxQuesion(tr("确定停止当前作业吗？")) == QDialog::Rejected)
        return;

    SelectTaskState(enBtnStop);
}

void JobListViewPage::MenuTaskResub()
{
    LOGFMTI("[Task]Menu::resub");

    SelectTaskState(enBtnResub);
}

void JobListViewPage::MenuTaskAbort()
{
    LOGFMTI("[Task]Menu::abort");

    SelectTaskState(enBtnAbort);
}

void JobListViewPage::MenuTaskDelete()
{
    LOGFMTI("[Task]Menu::delete");

    QList<JobState*> states = m_listView->getSelectTaskInfos();
    if (states.size() > 0) {
        int ret = Util::ShowMessageBoxQuesion(QObject::tr("是否删除任务?"));
        if (ret == enIDOK) {
            SelectTaskState(enBtnDelete);
        }
    }
}

void JobListViewPage::MenuTaskRefresh()
{
    if (m_currentPage != TASK_PAGE_RENDER)
        return;

    LOGFMTI("[Task]Menu::refresh");

    if (GetTickCount() - m_lastRefreshTime < 1000)
        return;

    m_lastRefreshTime = GetTickCount();
    JobStateMgr::Inst()->setSearchString();
    JobStateMgr::Inst()->SetClean(true);
    this->showLoading(true);

    // 刷新列表
    LOGFMTI("[JobViewPage] MenuTaskRefresh Manually refresh");
    HttpCmdManager::getInstance()->filterContent.pageNum = 1;
    JobStateMgr::Inst()->m_vtPage.clear();
    int ntype = GetShowType();
    JobStateMgr::Inst()->SendRefreshTasklist(-1, ntype);

    // 刷新余额
    HttpCmdManager::getInstance()->userBalance();
}

void JobListViewPage::MenuTaskDownload()
{
    LOGFMTI("[Task]Menu::download");

    QList<JobState*> states = m_listView->getSelectTaskInfos();
    if (states.isEmpty())
        return;

    // 寻找过期的任务并移除
    QList<JobState*> expiredJobs;
    foreach(auto state, states) {
        if (state->isDownloadExpired()) {
            expiredJobs.append(state);
            states.removeOne(state);
        }
    }

    if (!expiredJobs.isEmpty()) {
        QString jobs = "";
        foreach(JobState* job, expiredJobs) {
            jobs.append(job->getTaskIdAlias());
            jobs.append(", ");
        }
        QString msg = QObject::tr("[%1]输出文件已被清理，无法下载，如有疑问，请咨询24小时在线客服").arg(jobs);
        Util::ShowMessageBoxError(msg);
    }

    if (states.isEmpty()) return;

    foreach(auto state, states) {
        if (state->IsDownloadable()) {
            state->setJobType(DOWNLOAD_JOB);
            CTaskCenterManager::Inst()->manualStartDownload(state->getTaskId());
        }
    }
    this->refreshDownloadPage();
}

void JobListViewPage::MenuShowLocalSavePath(JobState * jobState)
{
    LOGFMTI("[Task]Menu::localDir");

    JobState *pState;
    if (jobState != NULL) {
        pState = jobState;
    } else {
        pState = (JobState *)m_listView->getSelectTaskInfo();
    }
    if (!pState) return;

    QString projectSavePath = pState->getAutoDownPath();
    QString outputPath = projectSavePath;
    if (!pState->isAutoDownload() || projectSavePath.isEmpty()) {
        projectSavePath = pState->getLocalSavePath();
    }
    if (!projectSavePath.contains(pState->getOutputLabel()))
        outputPath = pState->getOutputPath(projectSavePath);

    LOGFMTI("[Task]Menu::localDir, projectSavePath = %s", projectSavePath.toLocal8Bit().data());
    LOGFMTI("[Task]Menu::localDir, outputPath = %s", outputPath.toLocal8Bit().data());

    if (!outputPath.isEmpty()) {
        QDir dir(outputPath);
        if (!dir.exists()) {
            DirOpenMsgBox msgBox;
            msgBox.setMsgContent("要打开的路径不存在！请重新下载！");
            msgBox.exec();
            QString path = msgBox.getSavePath();
            if (path.isEmpty())return;

            // 重新下载
            if (!pState->isDownloadExpired()) {
                if (pState->IsDownloadable()) {
                    if (!path.isEmpty()) {
                        pState->setLocalSavePath(path);
                        pState->setDownloadProgress(0);
                        pState->setTransStatus(enDownloadQueued);
                        pState->setDownloadMode(LocalSetting::getInstance()->isDownloadMkSenceNameDir() ? 1 : 0);
                        pState->setDownloadPathCode(DOWNLOAD_USE_UNICODE);    
                        pState->setDownloadCompleted(false);
                    }
                    CTaskCenterManager::Inst()->manualStartDownload(pState->getTaskId(), false);
                    m_listView->updateView();
                    this->refreshDownloadPage();
                }
            } else {
                QString msg = QObject::tr("[%1]输出文件已被清理，无法下载，如有疑问，请咨询24小时在线客服").arg(pState->getTaskIdAlias());
                Util::ShowMessageBoxError(msg);
            }
            return;
        }
        try {
            outputPath = QDir::toNativeSeparators(outputPath);
            QDesktopServices::openUrl(QUrl::fromLocalFile(outputPath));
        } catch (std::exception &e) {

        }
    }
}

void JobListViewPage::MenuShowPreview()
{
    LOGFMTI("[Task]Menu::preview");

    JobState* state = m_listView->getSelectTaskInfo();
    if (!state) return;
}

void JobListViewPage::MenuCloneTask()
{
    LOGFMTI("[Task]Menu::clone");

    QList<JobState*> jobs = m_listView->getSelectTaskInfos();
    if (jobs.isEmpty())
        return;
    JobState* job = jobs.at(0);

    if (!job)  return;
    if (!job->isCanJobOperable()) {
        Util::ShowMessageBoxError(QObject::tr("当前任务不支持克隆！"));
        return;
    }

    QString projectName = QString("%1_copy").arg(job->getProjectName());
    // 项目名称超过200，则提示不予克隆
    if (projectName.length() > 200) {
        Util::ShowMessageBoxError(QObject::tr("名称过长，克隆失败！"));
        return;
    }
	//正在空三的任务，则提示不予克隆
	t_taskid at_taskid;
	QHash<qint64, JobState*> m_at_task = job->getChildJobState();
	foreach(JobState* job1, job->getChildJobState()) {
		if (job1->IsParentItem()) {
			foreach(JobState* subJob, job1->getChildJobState()) {
				if (!job1->m_taskType.compare("Triangulation"))
				{
					at_taskid = job1->m_taskId;
					Util::ShowMessageBoxError(QObject::tr("正在空三的任务不能克隆"));
					return;
				}
			}
		}
		else {
			if (!job1->m_taskType.compare("Triangulation"))
			{
				at_taskid = job1->m_taskId;
				Util::ShowMessageBoxError(QObject::tr("正在空三的任务不能克隆"));
				return;
			}
		}
	}
    RebuildOptionParams params;
    params.reset();
    HttpCmdManager::getInstance()->submitCloneTask(job->getTaskId(), projectName, params);

    // TaskCloneWindow cloneWindow(job->getTaskId());
    // cloneWindow.initUi();
    // if (cloneWindow.exec() == QDialog::Accepted) {
    //     MenuTaskRefresh();
    // }
}

void JobListViewPage::MenuTaskLevel(int nType)
{
    LOGFMTI("[Task]Menu::level");

    JobState *pState = m_listView->getSelectTaskInfo();
    if (!pState)  return;
}


void JobListViewPage::MenuFailFrameTask(bool bChecked)
{
}

void JobListViewPage::MenuSaveasDownload(const QStringList& outputList)
{
    LOGFMTI("[Task]Menu::SaveasDownload");

    QList<JobState*> pStates = m_listView->getSelectTaskInfos();
    if (pStates.isEmpty()) return;

    // 寻找过期的任务并移除
    QList<JobState*> expiredJobs;
    foreach(auto state, pStates) {
        if (state->isDownloadExpired()) {
            expiredJobs.append(state);
            pStates.removeOne(state);
        }
    }

    if (!expiredJobs.isEmpty()) {
        QString jobs = "";
        foreach(JobState* job, expiredJobs) {
            jobs.append(job->getTaskIdAlias());
            jobs.append(", ");
        }
        QString msg = QObject::tr("[%1]输出文件已被清理，无法下载，如有疑问，请咨询24小时在线客服").arg(jobs);
        Util::ShowMessageBoxError(msg);
    }

    if (pStates.isEmpty())
        return;

    QString strFolderName = "";
    QFileDialog filedialog;

    filedialog.setWindowTitle(tr("选择文件夹"));
    filedialog.setDirectory(".");
    filedialog.setOption(QFileDialog::ShowDirsOnly);
    filedialog.setFileMode(QFileDialog::Directory);
    if (filedialog.exec() != QDialog::Accepted) {
        return;
    }
    strFolderName = filedialog.selectedFiles()[0] + "/";

    foreach(auto state, pStates) {
        if (state->IsDownloadable()) {
            if (!strFolderName.isEmpty()) {
                state->setLocalSavePath(strFolderName);
                state->setDownloadProgress(0);
                state->setTransStatus(enDownloadQueued);
                state->setDownloadMode(LocalSetting::getInstance()->isDownloadMkSenceNameDir() ? 1 : 0);
                state->setDownloadPathCode(DOWNLOAD_USE_UNICODE);
                state->setDownloadFiles(outputList);
                state->setDownloadCompleted(false);
            }
            CTaskCenterManager::Inst()->manualStartDownload(state->getTaskId(), false);
        }
    }
    m_listView->updateView();
    this->refreshDownloadPage();
}

void JobListViewPage::RunTaskState(JobState * pState, enBtnCheckType nType)
{
    if (pState == NULL) {
        LOGFMTE("[JobViewPage]RunTaskState pState is Empty ,Type:%d", (int)nType);
        return;
    }

    switch (nType) {
    case enBtnStart: {
        if (pState->getJobType() == RENDER_JOB || pState->getJobType() == DOWNLOAD_JOB) {
            BtnTaskStart(pState);
        } else if (pState->getJobType() == UPLOAD_JOB) {
            MenuStartUpload();
        }
    }
        break;
    case enBtnFullspeed: {
        BtnTaskFullspeed(pState);
    }
        break;
    case enBtnStop: {
        if (pState->getJobType() == RENDER_JOB || pState->getJobType() == DOWNLOAD_JOB) {
            BtnTaskStop(pState);
        } else if (pState->getJobType() == UPLOAD_JOB) {
            MenuStopUpload();
        }
    }
        break;
    case enBtnResub:
        BtnTaskResub(pState);
        break;
    case enBtnDelete: {
        if (pState->getJobType() == RENDER_JOB || pState->getJobType() == DOWNLOAD_JOB) {
            BtnTaskDelete(pState);
        } else if (pState->getJobType() == UPLOAD_JOB) {
            this->MenuDeleteUpload();
        }
    }
        break;
    case enBtnAbort:
        BtnTaskAbort(pState);
        break;
    }
}

void JobListViewPage::SelectTaskState(enBtnCheckType nType)
{
    QList<JobState*> states = m_listView->getSelectTaskInfos();

    QListIterator<JobState*> it(states);
    while (it.hasNext()) {
        auto state = it.next();
        if (state) {
            RunTaskState(state, nType);
        }
    }
    this->refresh();
}

int JobListViewPage::GetSelectCount()
{
    return 0;
}

JobState * JobListViewPage::GetSelectState()
{
    return m_listView->getSelectTaskInfo();
}

QList<JobState*> JobListViewPage::GetSelectStates()
{
    return m_listView->getSelectTaskInfos();
}

void JobListViewPage::BtnTaskStart(JobState * pState)
{
    if (pState == NULL) {
        LOGFMTE("[JobViewPage]RunStart failed: input invalid!");
        return;
    }
    LOGFMTD("Task::RunStart, taskid=%I64d, state=%d,", pState->getTaskId(), pState->getStatus());

    switch (pState->getStatus()) {
    case JobState::enRenderTaskPreRendering:
        pState->setJobType(RENDER_JOB);
        CTaskCenterManager::Inst()->startUpload(pState->getTaskId());
        break;
    case JobState::enRenderTaskSubmit:
        pState->setJobType(RENDER_JOB);
        CTaskCenterManager::Inst()->resubmitTask(pState->getTaskId());
        break;
    default:
        if (pState->getStatus() == JobState::enRenderTaskStop
            && pState->isDependency()
            && !pState->getDependentTask().isEmpty()
            && pState->getDependentTask() != "0"
            ) {
            int ret = Util::ShowMessageBoxQuesion(QObject::tr("您设置的父任务正在渲染，您确定现在启动子任务?"));
            if (ret != enIDOK) {
                break;
            }
        }
        pState->setJobType(RENDER_JOB);
        HttpCmdManager::getInstance()->taskOperatorStart(pState->getTaskId());
        break;
    }
}

void JobListViewPage::BtnTaskFullspeed(JobState * pState)
{
}

void JobListViewPage::BtnTaskStop(JobState * pState)
{
    t_taskid taskId = pState->getTaskId();
    LOGFMTI("[Task]Menu::stop task %d", taskId);

    if (pState->IsRenderCanStop() || pState->IsPreTaskCanStop()) {
        pState->setJobType(RENDER_JOB);
        HttpCmdManager::getInstance()->taskOperatorStop(pState->getTaskId());
    }
    if (pState->IsUploading()) {
        CTaskCenterManager::Inst()->stopUploadTask(taskId);
    }
    if (pState->IsDownloading()) {
        CTaskCenterManager::Inst()->manualStopDownload(taskId);
    }
}

void JobListViewPage::BtnTaskResub(JobState * pState)
{
    QString strCompleteDate = pState->getStartDateString().split('.')[0];
    QDateTime completeDate = QDateTime::fromString(strCompleteDate, "yyyy-MM-dd HH:mm:ss");
    QDateTime currentDate = MyConfig.currentDateTime;
    qint64 days = completeDate.daysTo(currentDate);
    if (days > 30) {
        Util::ShowMessageBoxError(QObject::tr("该任务已超过1个月，不能重提失败帧!"));
        return;
    }

    if (pState->IsParentItem()) {
        QList<qint64> taskIds = pState->getCanResubChildTaskList();
        taskIds.append(pState->getTaskId());
        QList<int> status;
        status << enReSubFailedTask;
        HttpCmdManager::getInstance()->taskOperatorResub(status, taskIds);   
    } else {
        HttpCmdManager::getInstance()->taskOperatorResub(enReSubFailedTask, pState->getTaskId());
    }
    pState->setJobType(RENDER_JOB);
}

void JobListViewPage::BtnTaskDelete(JobState * pState)
{
    if (pState->getStatus() == JobState::enRenderTaskRendering) {
        Util::ShowMessageBoxError(QObject::tr("任务尚在运行，请首先停止任务！(ID:%1)").arg(pState->getTaskId()));
        return;
    }

    // 发送运行操作
    JobState *pDownState = JobStateMgr::Inst()->LookupTaskstate(pState->getTaskId());
    if (pDownState && (pDownState->IsDownloading() || pDownState->IsWaitting())) {
        CTaskCenterManager::Inst()->manualStopDownload(pState->getTaskId());
    }
    HttpCmdManager::getInstance()->taskOperatorDelete(pState->getTaskId());

    CGlobalHelp::Response(UM_UPDATE_TRANS_SPEED, UM_UPDATE_TRANS_SPEED_DOWNLOAD, 0);
}

void JobListViewPage::BtnTaskAbort(JobState * pState)
{
}

void JobListViewPage::onCustomContextMenuRequested(QPoint pos)
{
    // 隐藏下载列表
    emit hideDownloadView();

    QModelIndex index = m_listView->indexAt(pos);
    if (index.isValid()) {
        JobState *state = GetSelectState();
        if (!state) return;

        // 有条件的显示 跳过校验菜单
        if (state->getTransStatus() == enUploadCompleted) {
            m_forceSubmitAction->setVisible(true);
        } else {
            m_forceSubmitAction->setVisible(false);
        }

        // 有条件的显示加载任务
        m_loadExistTask->setVisible(RBHelper::isDeveloper());

        int jobType = state->getJobType();
        if (jobType == UPLOAD_JOB) { //上传菜单
            QList<QAction*> actions = m_popUploadMenu->actions();
            foreach(QAction* action, actions) {
                switch (action->data().toInt()) {
                case MENU_UPLOAD_START:
                case MENU_UPLOAD_REUPLOAD:
                    if (state->IsUploadCanStart()) {
                        action->setEnabled(true);
                    }
                    else {
                        action->setEnabled(false);
                    }
                    break;
                case MENU_UPLOAD_CANCEL:
                    if (state->IsUploadCanStop()) {
                        action->setEnabled(true);
                    }
                    else {
                        action->setEnabled(false);
                    }
                    break;
                case MENU_UPLOAD_DELETE:
                    if (!state->IsUploadCanDelete())
                        action->setEnabled(false);
                    else
                        action->setEnabled(true);
                    break;
                case MENU_UPLOAD_FORCESUBMIT:
                    if (state->getTransStatus() == enUploadCompleted)
                        action->setEnabled(true);
                    else
                        action->setEnabled(false);
                    break;
                default:
                    action->setEnabled(true);
                    break;
                }
            }

            m_popUploadMenu->exec(QCursor::pos());
            return;

        } else if (jobType == RENDER_JOB || jobType == DOWNLOAD_JOB) {
            QList<QAction*> actions = m_popJobMenu->actions();
            bool bDelOrPrepare = (state->getStatus() == JobState::enRenderTaskDelete)
                || (state->getStatus() == JobState::enRenderTaskPreRendering);

            if (!state->IsParentTaskCanDownload() || bDelOrPrepare 
                || GetSelectCount() > 1 
                || (state->getRenderJob() != nullptr && state->getRenderJob()->getProjectOutputType().isEmpty())) {
                m_downloadMenu->setEnabled(false);
            } else {
                m_downloadMenu->setEnabled(true);
                downloadMenuDisplay(state);
            }

            foreach(QAction* action, actions) {
                switch (action->data().toInt()) {
                case MENU_TASK_START: {
                    // 开始菜单可用控制
                    int isCanStart = 0;
                    QList<JobState*> selectSts = GetSelectStates();
                    foreach(JobState* sts, selectSts) {
                        if (sts->IsParentTaskCanStart() || sts->IsPreTaskCanStart()) {
                            ++isCanStart;
                        }
                    }
                    if (isCanStart > 0) {
                        action->setEnabled(true);
                    } else {
                        action->setEnabled(false);
                    }
                }
                    break;
                case MENU_TASK_PAUSE: {
                    // 预渲任务可用菜单
                    if (state->IsParentTaskCanStop() && state->IsPreTaskCanStop()) {
                        action->setEnabled(true);
                    } else {
                        action->setEnabled(false);
                    }
                }
                    break;
                case MENU_TASK_RESUB: {
                    // 重提菜单可用控制
                    if ((!state->IsParentTaskCanResubmit())/* && !state->IsPreTaskCanResub()*/) {
                        action->setEnabled(false);
                    } else {
                        action->setEnabled(true);
                    }
                    /*{
                    int totalCount = state->GetExcuteCount() + state->GetFailedCount()
                    + state->GetDoneCount() + state->GetWaitCount()
                    + state->GetAbortCount();

                    if(totalCount == 0) {
                    action->setEnabled(true);
                    } else if(totalCount == state->GetDoneCount()) {
                    action->setEnabled(false);
                    }
                    }*/
                }
                    break;
                case MENU_SHOW_LOCAL_PATH: {
                    if (!state->IsParentTaskCanDownload() || bDelOrPrepare || GetSelectCount() > 1) {
                        action->setEnabled(false);
                    } else {
                        action->setEnabled(true);
                    }
                }
                    break;
                case MENU_TASK_DELETE: {
                    // Delete和Prepare状态 禁止删除
                    if (!state->IsParentTaskCanDelete() && !state->IsPreTaskCanDelete()) {
                        action->setEnabled(false);
                    } else {
                        action->setEnabled(true);
                    }
                }
                    break;
                case MENU_TASK_DOWNLOAD: {
                    if (!state->IsParentTaskCanDownload()) {
                        action->setEnabled(false);
                    } else {
                        action->setEnabled(true);
                    }
                }
                    break;
                case MENU_TASK_CLONE: {
                    if (state->getJobType() != UPLOAD_JOB) {
                        action->setEnabled(true);
                    } else {
                        action->setEnabled(false);
                    }
                }
                    break;
                case MENU_DOWNLOAD_CANCEL:
                    if (state->IsCanStopDownload()) {
                        action->setEnabled(true);
                    } else {
                        action->setEnabled(false);
                    }
                    break;
                default:
                    action->setEnabled(true);
                    break;
                }
            }
            m_popJobMenu->exec(QCursor::pos());
            return;
        }

    } else {
        QList<QAction*> actions = m_popJobMenu->actions();
        foreach(QAction* action, actions) {
            switch (action->data().toInt()) {
            case MENU_TASK_START:
            case MENU_SHOW_PREVIEW:
            case MENU_TASK_CLONE:
            case MENU_SHOW_LOCAL_PATH:
            case MENU_TASK_DELETE:
            case MENU_TASK_DOWNLOAD:
            case MENU_TASK_RESUB:
            case MENU_TASK_PAUSE:
            case MENU_TASK_DEPEND:
            case MENU_CAMERA_CREATE:
            case MENU_TASK_FULLSPEED:
            case MENU_TASK_USE_VOUCHER:
            case MENU_DOWNLOAD_CANCEL:
                action->setEnabled(false);
                break;
            default:
                action->setEnabled(true);
                break;
            }
        }
    }

    m_popJobMenu->exec(QCursor::pos());
}

void JobListViewPage::onJobPopMenuSlot(QAction * action)
{
    if (!action) return;

    switch (action->data().toInt()) {
    case MENU_TASK_START:
        MenuTaskStart();
        break;
    case MENU_TASK_FULLSPEED:
        MenuTaskFullspeed();
        break;
    case MENU_TASK_PAUSE:
        MenuTaskStop();
        break;
    case MENU_TASK_RESUB:
        MenuTaskResub();
        break;
    case MENU_TASK_ABORT:
        MenuTaskAbort();
        break;
    case MENU_TASK_DELETE:
        MenuTaskDelete();
        break;
    case MENU_TASK_REFRESH:
        MenuTaskRefresh();
        break;
    case MENU_TASK_DOWNLOAD:
        //MenuTaskDownload();
        MenuSaveasDownload(QStringList());
        this->refresh();
        break;
    case MENU_SHOW_LOCAL_PATH:
        MenuShowLocalSavePath();
        break;
    case MENU_SHOW_PREVIEW:
        MenuShowPreview();
        break;
    case MENU_TASK_CLONE:
        MenuCloneTask();
        break;
    case MENU_LEVEL_C:
        MenuTaskLevel(TPYE_LEVEL_TASK_P);
        break;
    case MENU_LEVEL_H:
        MenuTaskLevel(TPYE_LEVEL_TASK_H);
        break;
    case MENU_TASK_FAILFRAME:
        MenuFailFrameTask(action->isChecked());
        break;
    case MENU_TASK_USE_VOUCHER:
        MenuUseVoucher();
        break;
    case MENU_DOWNLOAD_CANCEL:
        MenuStopDownload();
        break;
    case MENU_TASK_LOAD_EXIST_TASK:
        MenuLoadExistTask();
        break;
    case MENU_TASK_COPY_TASK_ID:
        MenuCopyTaskId();
        break;
    case MENU_SHOW_TRANS_LOG:
        MenuShowTransLog();
        break;
    default:
        break;
    }
}

void JobListViewPage::onUploadPopMenuSlot(QAction * action)
{
    switch (action->data().toInt()) {
    case MENU_UPLOAD_START:
        MenuStartUpload();
        break;
    case MENU_UPLOAD_CANCEL:
        MenuStopUpload();
        break;
    case MENU_UPLOAD_DELETE:
        MenuDeleteUpload();
        break;
    case MENU_UPLOAD_REUPLOAD:
        MenuTaskReupload();
        break;
    case MENU_TASK_COPY_TASK_ID:
        MenuCopyTaskId();
        break;
    case MENU_SHOW_TRANS_LOG:
        MenuShowTransLog();
        break;
    default:
        break;
    }
}

void JobListViewPage::onEnteredSlot(const QModelIndex & index)
{
}

void JobListViewPage::onClickedSlot(const QModelIndex & index)
{
}
void JobListViewPage::onDoubleClickedSlot(const QModelIndex &index)
{
	JobState *jobState = m_listView->getCurrentTaskInfo(index);
	qint64 taskId = jobState->getTaskId();
	if (!index.isValid())
		return;
	
	if (jobState == nullptr)
		return;
	if (m_floatLogPanel != nullptr)
	{
		m_floatLogPanel->hide();
	}
	emit taskItemDoubleClicked(taskId);
}
//void JobListViewPage::onDoubleClickedSlot(const QModelIndex & index)
//{
//    if (!index.isValid())
//        return;
//
//    JobState *jobState = m_listView->getCurrentTaskInfo(index);
//    if (jobState == nullptr)
//        return;
//
//    if (m_floatLogPanel != nullptr) {
//        m_floatLogPanel->hide();
//    }
//
//    qint64 taskId = jobState->getTaskId();
//    emit taskItemDoubleClicked(taskId);
//}

void JobListViewPage::onReachedBottom()
{
    this->showLoading(true);
    FilterContent &filterContent = HttpCmdManager::getInstance()->filterContent;
	//新增，每次刷新会加入对关键词的清空
    filterContent.pageNum = filterContent.pageNum + 1;
    if (filterContent.pageNum > filterContent.pageCount)
        filterContent.pageNum = filterContent.pageCount; 
    HttpCmdManager::getInstance()->renderList(filterContent);
}

void JobListViewPage::onHeaderChanged()
{
}

bool JobListViewPage::UseVoucher()
{
    QList<JobState*> jobs = m_listView->getSelectTaskInfos();
    if (jobs.isEmpty())
        return false;
    JobState* job = jobs.at(0);

    HttpCmdManager::getInstance()->paymentVoucher(job->getTaskId());
    return true;
}

bool JobListViewPage::isCanUseVoucher()
{
    QList<JobState*> jobs = m_listView->getSelectTaskInfos();
    if (jobs.isEmpty())
        return false;
    JobState* job = jobs.at(0);

    int effectiveCount = 0;
    qint64 submitTime = job->getSubmitDate();
    for (int i = 0; i < MyConfig.voucherSet.effectiveVouchers.size(); i++) {
        VoucherInfo voucher = MyConfig.voucherSet.effectiveVouchers.at(i);
        if (submitTime > voucher.effectiveDate && submitTime < voucher.expireDate) {
            effectiveCount++;
        }
    }

    return effectiveCount > 0;
}

void JobListViewPage::onDownloadClicked(JobState* job, const QStringList& outputList)
{
    if (job == NULL) return;

    if (job->isDownloadExpired()) {
        QString msg = QObject::tr("[%1]输出文件已被清理，无法下载，如有疑问，请咨询24小时在线客服").arg(job->getTaskIdAlias());
        Util::ShowMessageBoxError(msg);
        return;
    }
    
    QString strFolderName = "";
    QFileDialog filedialog;

    filedialog.setWindowTitle(tr("选择文件夹"));
    filedialog.setDirectory(".");
    filedialog.setOption(QFileDialog::ShowDirsOnly);
    filedialog.setFileMode(QFileDialog::Directory);
    if (filedialog.exec() != QDialog::Accepted) {
        return;
    }
    strFolderName = filedialog.selectedFiles()[0] + "/";

    if (job->IsDownloadable()) {
        if (!strFolderName.isEmpty()) {
            job->setJobType(DOWNLOAD_JOB);
            job->setLocalSavePath(strFolderName);
            job->setDownloadProgress(0);
            job->setTransStatus(enDownloadQueued);
            job->setDownloadMode(LocalSetting::getInstance()->isDownloadMkSenceNameDir() ? 1 : 0);
            job->setDownloadPathCode(DOWNLOAD_USE_UNICODE);
        }
        job->setDownloadFiles(outputList);
        CTaskCenterManager::Inst()->manualStartDownload(job->getTaskId(), false);
    }
    m_listView->updateView();
}

void JobListViewPage::onOpenFileDlgClicked(JobState* job)
{
    this->MenuShowLocalSavePath(job);
}

void JobListViewPage::appendLog(const QString &AppendLog, int page)
{
    if (m_floatLogPanel != nullptr) {
        m_floatLogPanel->appendLog(AppendLog, page);
    }  
}

void JobListViewPage::setProxySortKey(int key, int sortType)
{
    m_listView->setProxySortKey(key, sortType);
}

void JobListViewPage::setProxyFilterKey(int key)
{
    m_listView->setProxyFilterKey(key);
}

void JobListViewPage::MenuUseVoucher()
{
    if (!this->isCanUseVoucher()) {
        Util::ShowMessageBoxInfo(tr("该作业不符合要求，只能抵扣在券有效期内提交的任务！"), tr("使用免单券"));
        return;
    }

    int code = Util::ShowMessageBoxQuesion(tr("是否使用免单券？一旦确认无法撤回！"), tr("使用免单券"));
    if (code == QDialog::Accepted) {
        this->UseVoucher();
    }
}

// 上传速度
QString JobListViewPage::getUploadSpeedText()
{
    return JobStateMgr::Inst()->uploadJosSpeedStr();
}

float JobListViewPage::getUploadSpeed()
{
    return JobStateMgr::Inst()->uploadJobsTotalSpeed();
}

bool JobListViewPage::isLowUploadSpeed()
{
    return JobStateMgr::Inst()->isLowUploadSpeed();
}

// 下载速度
QString JobListViewPage::getDownloadSpeedText()
{
    return JobStateMgr::Inst()->downloadJosSpeedStr();
}

float JobListViewPage::getDownloadSpeed()
{
    return JobStateMgr::Inst()->downloadingJobsTotalSpeed();
}

void JobListViewPage::cancelAllDownloads()
{
#if 0
    auto states = JobStateMgr::Inst()->getDownloadingJobs();
    foreach(auto state, states) {
        if (state
            && state->getTransStatus() != enDownloadCompleted
            && state->getTransStatus() != enDownloadStop) {
            CTaskCenterManager::Inst()->manualStopDownload(state->getTaskId());
        }
        else {
            LOGFMTW("[JobListViewPage] Delete auto download task failed! taskid=%d", state->getTaskId());
        }
    }
#endif // 
}

void JobListViewPage::MenuStopDownload()
{
    QList<JobState*> states = m_listView->getSelectTaskInfos();
    if (states.isEmpty())
        return;

    foreach(auto state, states) {
        if (state
            && state->getTransStatus() != enDownloadCompleted
            && state->getTransStatus() != enDownloadStop) {
            CTaskCenterManager::Inst()->manualStopDownload(state->getTaskId());
        } else {
            LOGFMTW("[JobListViewPage] Stop download task failed! taskid=%I64d", state->getTaskId());
        }
    }
    CGlobalHelp::Response(UM_UPDATE_TRANS_SPEED, UM_UPDATE_TRANS_SPEED_DOWNLOAD, 0);
}

void JobListViewPage::updateRow(qint32 row)
{
    m_listView->updateRow(row);
}

void JobListViewPage::setCurrentPage(int page)
{
    m_currentPage = page;
}

void JobListViewPage::onDownloadMenuTriggered(QAction *action)
{
    if (action == NULL) return;
    QList<JobState*> pStates = m_listView->getSelectTaskInfos();
    if (pStates.isEmpty()) return;
    JobState* curJob = pStates.at(0);
    if (curJob == nullptr) return;
    curJob = curJob->getRenderJob();
    if (curJob == nullptr) return;

    QString type = action->data().toString();
    QStringList outputList;

    if (type.isEmpty()) {
        if (curJob->getProjectOutputType().isEmpty()) {
            outputList.append(RBHelper::getOutputFileName(OUTPUT_TYPE_OSGB));
        } else {
            outputList = curJob->getProjectOutputType();
        }
    } else {
        outputList.append(type);
    }

    CDownloadState* downloadState = CDownloadStateMgr::Inst()->LookupTaskstate(curJob->getTaskId());
    if (downloadState != nullptr && !downloadState->getDownloadFileList().isEmpty()) {
        foreach(QString path, downloadState->getDownloadFileList()) {
            if (!outputList.contains(path)) {
                outputList.append(path);
            }
        }
    }
    //开始前先停止
    CTaskCenterManager::Inst()->manualStopDownload(curJob->getTaskId());
    downloadRenderOutputFiles(curJob->getTaskId(), outputList);
    this->refresh();
}

void JobListViewPage::MenuLoadExistTask()
{
    // 0. 填写历史任务号
    // 1. 先获取新的任务号
    // 2. 复制历史作业
    // 3. 将复制过来的作业中的任务号修改为新的作业号
    // 4. 提交任务
    QScopedPointer<ParameterCustom> dialog(new ParameterCustom());
    dialog->exec();

}

void JobListViewPage::MenuCopyTaskId()
{
    QList<JobState*> jobs = m_listView->getSelectTaskInfos();
    if (jobs.isEmpty())
        return;
    JobState* job = jobs.at(0);

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(job->getTaskIdAlias());

    new ToastWidget(m_listView, QObject::tr("复制成功!"), QColor("#675ffe"));
}

void JobListViewPage::refreshDownloadPage()
{
    // 刷新下载界面
    // 刷新界面
    CGlobalHelp::Response(UM_CHANGE_STATE_VIEW, VIEW_REFRESH_DOWNLOAD_PAGE_ALL, 0);
}

void JobListViewPage::downloadRenderOutputFiles(qint64 taskId, const QStringList& files)
{
    LOGFMTI("[Task]Menu::Download render output");

    JobState* pStates = JobStateMgr::Inst()->LookupTaskstate(taskId);
    if (pStates == nullptr) return;

    // 寻找过期的任务并移除
    QList<JobState*> expiredJobs;
    if (pStates->isDownloadExpired()) {
        QString msg = QObject::tr("[%1]输出文件已被清理，无法下载，如有疑问，请咨询24小时在线客服").arg(pStates->getTaskIdAlias());
        Util::ShowMessageBoxError(msg);
        return;
    }
	QString tIdAlias = QString::number(pStates->getTaskParentId());

    QString strFolderName = "";
    QFileDialog filedialog;

    filedialog.setWindowTitle(tr("选择文件夹"));
    filedialog.setDirectory(".");
    filedialog.setOption(QFileDialog::ShowDirsOnly);
    filedialog.setFileMode(QFileDialog::Directory);
    if (filedialog.exec() != QDialog::Accepted) {
        return;
    }
    strFolderName = filedialog.selectedFiles()[0] + "/";
//	strFolderName += tIdAlias;
	strFolderName += "/";

    if (pStates->IsDownloadable()) {
        if (!strFolderName.isEmpty()) {
            pStates->setLocalSavePath(strFolderName);
            pStates->setDownloadProgress(0);
            pStates->setTransStatus(enDownloadQueued);
            pStates->setDownloadMode(LocalSetting::getInstance()->isDownloadMkSenceNameDir() ? 1 : 0);
            pStates->setDownloadPathCode(DOWNLOAD_USE_UNICODE);
            pStates->setDownloadFiles(files);
            pStates->setDownloadCompleted(false);
        }
        CTaskCenterManager::Inst()->manualStartDownload(pStates->getTaskId(), false);
    }

    m_listView->updateView();
    this->refreshDownloadPage();
}

void JobListViewPage::downloadMenuDisplay(JobState* jobState)
{
    if (jobState == nullptr)
        return;
    JobState* renderJob = jobState->getRenderJob();
    if (renderJob == nullptr)
       return;

    foreach(QAction *act, m_outputActions.values()) {
        if (act != nullptr) {
            m_downloadMenu->removeAction(act);
        }
    }
    m_downloadMenu->removeAction(m_selAllAction);
	

    QStringList outputType = jobState->getProjectOutputType();
    for (int i = 0; i < OUTPUT_TYPE_TOTAL; i++) {
        QString output = RBHelper::getOutputFileName((OutputType)i);
        if (outputType.contains(output)) {
            QAction* act = m_outputActions.value(i);
            if (act != nullptr) {
                m_downloadMenu->addAction(act);
            }
        }
    }
    m_downloadMenu->addAction(m_selAllAction);
}

void JobListViewPage::MenuShowTransLog()
{
    if (m_floatLogPanel != nullptr) {
        m_floatLogPanel->show();
    }
}

void JobListViewPage::itemSelected(qint64 taskId)
{
    JobState* job = JobStateMgr::Inst()->LookupTaskstate(taskId);
    if (job == nullptr) return;

    m_listView->setItemSelected(job);
}