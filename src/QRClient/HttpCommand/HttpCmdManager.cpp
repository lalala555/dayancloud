#include "stdafx.h"
#include "HttpCmdManager.h"
#include "HttpCmd/HttpPlatformCmd.h"
#include "HttpCmd/HttpUserLoginCmd.h"
#include "HttpCmd/HttpUserInfoCmd.h"
#include "HttpCmd/HttpGetTransBid.h"
#include "HttpCmd/HttpGetTransConfig.h"
#include "HttpCmd/HttpBalanceCmd.h"
#include "HttpCmd/HttpTaskListCmd.h"
#include "HttpCmd/HttpTaskResubFailFrameCmd.h"
#include "HttpCmd/HttpTaskOperaterCmd.h"
#include "HttpCmd/HttpSubTaskCmd.h"
#include "HttpCmd/HttpCreateJobId.h"
#include "HttpCmd/HttpFrameSummaryCmd.h"
#include "HttpCmd/HttpTaskSummaryCmd.h"
#include "HttpCmd/HttpRecentTaskCmd.h"
#include "HttpCmd/HttpProjectlistCmd.h"
#include "HttpCmd/HttpUpdateUserInfoCmd.h"
#include "HttpCmd/HttpUserStorageCmd.h"
#include "HttpCmd/HttpSearchCmd.h"
#include "HttpCmd/HttpUserRenderSettingCmd.h"
#include "HttpCmd/HttpUserLogoutCmd.h"
#include "HttpCmd/HttpTaskProcessImg.h"
#include "HttpCmd/HttpClientProNotice.h"
#include "HttpCmd/HttpClientSysNotice.h"
#include "HttpCmd/HttpLoadingTaskPatamter.h"
#include "HttpCmd/HttpTaskFrameRenderingInfo.h"
#include "HttpCmd/HttpLoadTaskThumbnail.h"
#include "HttpCmd/HttpRenderLog.h"
#include "HttpCmd/HttpOperateUserRenderPlugin.h"
#include "HttpCmd/HttpRenderSoftVersion.h"
#include "HttpCmd/HttpUpgradeTaskRam.h"
#include "HttpCmd/HttpUserPluginConfig.h"
#include "HttpCmd/HttpOperatorTaskFrame.h"
#include "HttpCmd/HttpLoadUserName.h"
#include "HttpCmd/HttpErrorListCmd.h"
#include "HttpCmd/HttpAdvSettingCmd.h"
#include "HttpCmd/HttpQueryTaskInfo.h"
#include "HttpCmd/HttpUserFeedbackCmd.h"
#include "HttpCmd/HttpUpdateTaskRemarkCmd.h"
#include "HttpCmd/HttpUpdateTaskLevelCmd.h"
#include "HttpCmd/HttpTaskOvertimeCmd.h"
#include "HttpCmd/HttpLoadProducerCmd.h"
#include "HttpCmd/HttpJobFrameGrab.h"
#include "HttpCmd/HttpQRCodeCmd.h"
#include "HttpCmd/HttpGetPlatformStatus.h"
#include "HttpCmd/HttpQueryBindWx.h"
#include "HttpCmd/HttpBindWxToUser.h"
#include "HttpCmd/HttpGetNodeDetailsCmd.h"
#include "HttpCmd/HttpDownloadCount.h"
#include "HttpCmd/HttpReportLogCmd.h"
#include "HttpCmd/HttpReportTransport.h"
#include "HttpCmd/HttpUpdateTaskLimitCmd.h"
#include "HttpCmd/HttpFrameRecommitInfoCmd.h"
#include "HttpCmd/HttpGetOutputSizeCmd.h"
#include "HttpCmd/HttpPaymentVoucherCmd.h"
#include "HttpCmd/HttpQueryPublishList.h"
#include "HttpCmd/HttpCreatePublish.h"
#include "HttpCmd/HttpDeletePublish.h"
#include "HttpCmd/HttpQueryPublishTask.h"
#include "HttpCmd/HttpUploadPic.h"
#include "HttpCmd\HttpGetPubById.h"
#include "HttpCmd\HttpUpdatePub.h"
#include "HttpCmd\HttpStopPublish.h"
#include "HttpCmd\HttpTaskNewList.h"
#include "HttpCmd\HttpSearchNew.h"
#include "HttpCmd\HttpCheckPublishDuplicate.h"
#include "SSOCmd/SSOUserLoginCmd.h"
#include "Common/SystemUtil.h"

HttpCmdManager::HttpCmdManager(QObject *parent)
    : QObject(parent)
{
    this->initRequestHeader();
    qRegisterMetaType<QSharedPointer<ResponseHead>>();
}

HttpCmdManager::~HttpCmdManager()
{
}

void HttpCmdManager::initRequestHeader()
{
    m_host = CConfigDb::Inst()->getHost();
    m_header.languageFlag = LocalSetting::getInstance()->getLanguageId();
    m_header.platform = LocalSetting::getInstance()->getDefaultsite();
    m_header.channel = CHANNEL_MODELLING;
    m_header.sTraceId = qApp->sessionId().replace("{", "").replace("}", "").toLower();
}

void HttpCmdManager::post(HttpCmdRequest* cmd, QObject* observer)
{
    connect(cmd, SIGNAL(sendResponseFormat(QSharedPointer<ResponseHead>)),
            &m_httpResponse, SLOT(onRecvResponse(QSharedPointer<ResponseHead>)));
    if(observer)
        connect(cmd, SIGNAL(sendResponseFormat(QSharedPointer<ResponseHead>)),
                observer, SLOT(onRecvResponse(QSharedPointer<ResponseHead>)));
    cmd->post(m_host, m_header);
    m_httpResponse.addRequestCmd(cmd);
}

void HttpCmdManager::get(HttpCmdRequest* cmd, QObject* observer)
{
    connect(cmd, SIGNAL(sendResponseFormat(QSharedPointer<ResponseHead>)),
            &m_httpResponse, SLOT(onRecvResponse(QSharedPointer<ResponseHead>)));
    if(observer)
        connect(cmd, SIGNAL(sendResponseFormat(QSharedPointer<ResponseHead>)),
                observer, SLOT(onRecvResponse(QSharedPointer<ResponseHead>)));
    cmd->get(m_host, m_header);
    m_httpResponse.addRequestCmd(cmd);
}

void HttpCmdManager::upload(HttpCmdRequest* cmd, QObject* observer)
{
    connect(cmd, SIGNAL(sendResponseFormat(QSharedPointer<ResponseHead>)),
        &m_httpResponse, SLOT(onUploadResponse(QSharedPointer<ResponseHead>)));
    if (observer)
        connect(cmd, SIGNAL(sendResponseFormat(QSharedPointer<ResponseHead>)),
            observer, SLOT(onUploadResponse(QSharedPointer<ResponseHead>)));
    cmd->upload(m_host, m_header);
    m_httpResponse.addRequestCmd(cmd);
}

bool HttpCmdManager::isReSubmitTask(qint64 taskId)
{
    return m_httpResponse.isReSubmitTask(taskId);
}

void HttpCmdManager::exceptionResponse()
{
}

void HttpCmdManager::login(const QString& userKey, const QString& accessId, QObject* observer)
{
    qDebug() << RESTAPI_URI_USER_LOGIN;

    HttpSigninCmd *cmd = new HttpSigninCmd(userKey, accessId);
    this->post(cmd, observer);
}

void HttpCmdManager::platformList(QObject* observer)
{
    qDebug() << RESTAPI_URI_PLATFORM_LIST;

    HttpPlatformCmd *cmd = new HttpPlatformCmd();
    this->post(cmd, observer);
}

void HttpCmdManager::userInfo(QObject* observer)
{
    qDebug() << RESTAPI_URI_USER_INFO;

    HttpUserInfoCmd *cmd = new HttpUserInfoCmd();
    this->post(cmd, observer);
}

void HttpCmdManager::realnameAuthKey(QObject * observer)
{
    qDebug() << RESTAPI_URI_GENERATE_REALNAME_AUTH;

    HttpCreateAuthKeyCmd *cmd = new HttpCreateAuthKeyCmd();
    this->post(cmd, observer);
}

void HttpCmdManager::transformBid(QObject* observer)
{
    qDebug() << RESTAPI_URI_USER_TRANS_BID;

    HttpGetTransBid *cmd = new HttpGetTransBid();
    this->post(cmd, observer);
}

void HttpCmdManager::transformConfig(QObject * observer) {
    qDebug() << RESTAPI_URI_USER_TRANS_CONFIG;

    HttpGetTransConfig *cmd = new HttpGetTransConfig();
    this->post(cmd, observer);
}

void HttpCmdManager::userBalance(QObject* observer)
{
    qDebug() << RESTAPI_URI_USER_BALANCE;

    HttpBalanceCmd *cmd = new HttpBalanceCmd();
    this->post(cmd, observer);
}

void HttpCmdManager::renderList(const FilterContent& content, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_LIST;

    if(content.filterType == CONTENT_SEARCH) {
        HttpSearchCmd *cmd = new HttpSearchCmd(content.content, content.pageNum, content.pageSize);
        cmd->setRepeat(content.isRepeat);
        this->post(cmd, observer);

    } else if(content.filterType == CONTENT_FILTER) {
        HttpSearchCmd *cmd = new HttpSearchCmd(content);
        cmd->setRepeat(content.isRepeat);
        this->post(cmd, observer);

    } else {
        HttpTaskListCmd* cmd = new HttpTaskListCmd(content.content, content.showType, content.pageNum, content.pageSize);
        cmd->setRepeat(content.isRepeat);
        this->post(cmd, observer);
    }
    this->filterContent = content;
}

void HttpCmdManager::userLogout(QObject* observer)
{
    qDebug() << RESTAPI_URI_USER_LOGOUT;

    HttpUserLogoutCmd *cmd = new HttpUserLogoutCmd();
    this->post(cmd, observer);
}

void HttpCmdManager::taskOperatorStart(QList<qint64> taskIds, QObject* observer)
{
    HttpTaskOperaterCmd *cmd = new HttpTaskOperaterCmd(RESTAPI_JOB_TASK_START, taskIds);
    this->post(cmd, observer);
}

void HttpCmdManager::taskOperatorStart(qint64 taskId, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_TASK_START;

    HttpTaskOperaterCmd *cmd = new HttpTaskOperaterCmd(RESTAPI_JOB_TASK_START, taskId);
    this->post(cmd, observer);
}

void HttpCmdManager::taskOperatorStop(QList<qint64> taskIds, QObject* observer)
{
    HttpTaskOperaterCmd *cmd = new HttpTaskOperaterCmd(RESTAPI_JOB_TASK_STOP, taskIds);
    this->post(cmd, observer);
}

void HttpCmdManager::taskOperatorStop(qint64 taskId, const QString& option, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_TASK_STOP;

    HttpTaskOperaterCmd *cmd = new HttpTaskOperaterCmd(RESTAPI_JOB_TASK_STOP, taskId, option);
    this->post(cmd, observer);
}

void HttpCmdManager::taskOperatorResub(int resubStatus, qint64 taskId, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_FAILED_FRAME_RECOMMIT;

    HttpTaskResubFailFrameCmd *cmd = new HttpTaskResubFailFrameCmd(resubStatus, taskId);
    this->post(cmd, observer);
}


void HttpCmdManager::taskOperatorResubAtSurvey(int resubStatus, qint64 taskId, const QString& surveyFile, QObject* observer)
{
	qDebug() << "Aerial triple data re-extraction with control points,Recall interface: ";
	qDebug() << RESTAPI_URI_JOB_FAILED_FRAME_RECOMMIT; 

	HttpTaskResubFailFrameCmd *cmd = new HttpTaskResubFailFrameCmd(resubStatus, taskId);
	cmd->setSurveyPointOption(surveyFile);
	this->post(cmd, observer);

}

void HttpCmdManager::taskOperatorResub(QList<int> resubStatus, QList<qint64> taskIds, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_FAILED_FRAME_RECOMMIT;

    HttpTaskResubFailFrameCmd *cmd = new HttpTaskResubFailFrameCmd(resubStatus, taskIds);
    this->post(cmd, observer);
}

void HttpCmdManager::taskOperatorDelete(QList<qint64> taskIds, QObject* observer)
{
    HttpTaskOperaterCmd *cmd = new HttpTaskOperaterCmd(RESTAPI_JOB_TASK_DELETE, taskIds);
    this->post(cmd, observer);
}

void HttpCmdManager::taskOperatorDelete(qint64 taskId, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_TASK_DELETE;

    HttpTaskOperaterCmd *cmd = new HttpTaskOperaterCmd(RESTAPI_JOB_TASK_DELETE, taskId);
    this->post(cmd, observer);
}

void HttpCmdManager::taskOperatorFullSpeed(QList<qint64> taskIds, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_TASK_FULL_SPEED;

    HttpTaskOperaterCmd *cmd = new HttpTaskOperaterCmd(RESTAPI_JOB_TASK_FULL_SPEED, taskIds);
    this->post(cmd, observer);
}

void HttpCmdManager::taskOperatorFullSpeed(qint64 taskId, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_TASK_FULL_SPEED;

    HttpTaskOperaterCmd *cmd = new HttpTaskOperaterCmd(RESTAPI_JOB_TASK_FULL_SPEED, taskId);
    this->post(cmd, observer);
}

void HttpCmdManager::taskSubmit(qint64 jobId, const QString& sceneName, QObject* observer)
{
    if(isReSubmitTask(jobId)) {
        LOGFMTW("[HttpCmd] Task %lld is submitting...", jobId);
        return;
    }

    qDebug() << RESTAPI_URI_JOB_SUBMIT;

    QString artist = System::getLoginDisplayName();
    HttpSubTaskCmd *cmd = new HttpSubTaskCmd(jobId, artist, sceneName);
    this->post(cmd, observer);
}

void HttpCmdManager::taskSubmitAT(qint64 jobId, const QString& sceneName, int atType, QObject* observer)
{
    if(isReSubmitTask(jobId)) {
        LOGFMTW("[HttpCmd] AT Task %lld is submitting...", jobId);
        return;
    }

#if 0
    qDebug() << RESTAPI_URI_JOB_SUBMIT;

    QString artist = System::getLoginDisplayName();
    HttpSubTaskCmd *cmd = new HttpSubTaskCmd(jobId, artist, sceneName);
    cmd->setATOption(atType);
#else
    qDebug() << RESTAPI_URI_JOB_RESUBMIT_CCAT;

    HttpRecommitCCAT *cmd = new HttpRecommitCCAT(jobId, atType);
#endif

    this->post(cmd, observer);
}

void HttpCmdManager::taskSubmitRebuild(qint64 jobId, const QString& sceneName, const RebuildOptionParams& params, QObject* observer)
{
    if(isReSubmitTask(jobId)) {
        LOGFMTW("[HttpCmd] Rebuild Task %lld is submitting...", jobId);
        return;
    }

    qDebug() << RESTAPI_URI_JOB_SUBMIT;

    QString artist = System::getLoginDisplayName();
    HttpSubTaskCmd *cmd = new HttpSubTaskCmd(jobId, artist, sceneName);
    cmd->setRebuildOption(params);
    this->post(cmd, observer);
}

void HttpCmdManager::taskSubmitATSurvey(qint64 jobId, const QString& surveyFile, const QString& sceneName, QObject* observer)
{
    if (isReSubmitTask(jobId)) {
        LOGFMTW("[HttpCmd] Rebuild Task %lld is submitting...", jobId);
        return;
    }

    qDebug() << RESTAPI_URI_JOB_SUBMIT;
    QString artist = System::getLoginDisplayName();
    HttpSubTaskCmd *cmd = new HttpSubTaskCmd(jobId, artist, sceneName);
    cmd->setSurveyPointOption(surveyFile);
    this->post(cmd, observer);
}

void HttpCmdManager::cloneJobId(qint64 jobId, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_CREATE;

    HttpCreateJobId *cmd = new HttpCreateJobId(jobId);
    this->post(cmd, observer);
}

void HttpCmdManager::getPublishList(QString publishName, QString publishId,int status, QObject* observer)
{
    qDebug() << RESTAPI_URI_TASK_QUERYPUBLISH;

    HttpQueryPublishList *cmd = new HttpQueryPublishList(publishName, publishId,status);
    this->post(cmd, observer);
}

void HttpCmdManager::createPublish(const PublishItem &item, QObject * observer)
{
	qDebug() << RESTAPI_URI_TASK_CREATEPUBLISH;
	HttpCreatePublish *cmd = new HttpCreatePublish(item);
	this->post(cmd, observer);

}

void HttpCmdManager::deletePublish(int id, QObject * observer)
{
	qDebug() << RESTAPI_URI_TASK_DELETEPUBLISH;
	HttpDeletePublish *cmd = new HttpDeletePublish(id);
	this->post(cmd, observer);
}

void HttpCmdManager::uploadPic(const QString& filePath, QObject* observer)
{
	qDebug() << RESTAPI_URI_UPLOAD_FILE;

	HttpUploadPic* cmd = new HttpUploadPic(filePath, observer);
	this->upload(cmd, observer);
}

void HttpCmdManager::getPublishTask(int id, QObject * observer)
{
	qDebug() << RESTAPI_URI_TASK_CREATEPUBLISH;
	HttpQueryPublishTask *cmd = new HttpQueryPublishTask(id);
	this->post(cmd, observer);
}

void HttpCmdManager::stopPublishTask(int id, QObject * observer)
{
	qDebug() << RESTAPI_URI_TASK_CREATEPUBLISH;
	HttpStopPublish *cmd = new HttpStopPublish(id);
	this->post(cmd, observer);
}

void HttpCmdManager::getPublishById(int id, QObject * observer)
{
	qDebug() << RESTAPI_URI_TASK_GETPUBLISHBYID;
	HttpGetPubById *cmd = new HttpGetPubById(id);
	this->post(cmd, observer);
}

void HttpCmdManager::updatePublish(const PublishItem & item, QObject * observer)
{
	qDebug() << RESTAPI_URI_TASK_UPDATEPUB;
	HttpUpdatePub *cmd = new HttpUpdatePub(item);
	this->post(cmd, observer);
}

void HttpCmdManager::renderListNew(const FilterContent & content, QObject * observer)
{
	qDebug() << RESTAPI_URI_JOB_LIST;

	if (content.filterType == CONTENT_SEARCH) {
		HttpSearchNew *cmd = new HttpSearchNew(content.content, content.pageNum, content.pageSize);
		cmd->setRepeat(content.isRepeat);
		this->post(cmd, observer);

	}
	else if (content.filterType == CONTENT_FILTER) {
		HttpSearchNew *cmd = new HttpSearchNew(content);
		cmd->setRepeat(content.isRepeat);
		this->post(cmd, observer);

	}
	else {
		HttpTaskNewList* cmd = new HttpTaskNewList(content.content, content.showType, content.pageNum, content.pageSize);
		cmd->setRepeat(content.isRepeat);
		this->post(cmd, observer);
	}
	this->filterContent = content;
}

void HttpCmdManager::checkId(int id, QObject * observer)
{
	qDebug() << RESTAPI_URI_TASK_CHECKID;
	HttpCheckPublishDuplicate *cmd = new HttpCheckPublishDuplicate(id);
	this->post(cmd, observer);
}

void HttpCmdManager::createJobIds(int createCount, int createType, QObject* observer)
{
	qDebug() << RESTAPI_URI_JOB_CREATE;

	HttpCreateJobId *cmd = new HttpCreateJobId(createCount, createType);
	this->post(cmd, observer);
}
void HttpCmdManager::createJobIds(socket_id sid, CommandServer* commandserver, int createCount, int createType, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_CREATE;

    HttpCreateJobId *cmd = new HttpCreateJobId(sid, commandserver, createCount, createType);
    this->post(cmd, observer);
}

void HttpCmdManager::taskTotalFrameInfo(QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_TOTAL_INFO;

    HttpFrameSummaryCmd* cmd = new HttpFrameSummaryCmd();
    this->post(cmd, observer);
}

void HttpCmdManager::updateUserInfo(qint32 taskOverTime, QObject* observer)
{
    qDebug() << RESTAPI_URI_UPDATE_USER_INFO;

    HttpUpdateUserInfoCmd* cmd = new HttpUpdateUserInfoCmd(taskOverTime, MyConfig.accountSet.userId);
    this->post(cmd, observer);
}

void HttpCmdManager::queryUserRenderSetting(QObject* observer)
{
    qDebug() << RESTAPI_URI_USER_RENDER_SETTING;

    HttpUserRenderSettingCmd* cmd = new HttpUserRenderSettingCmd();
    this->post(cmd, observer);
}

void HttpCmdManager::getTaskStatusCount(QObject* observer)
{
    qDebug() << RESTAPI_URI_TASK_SUMMARY;

    HttpTaskSummaryCmd* cmd = new HttpTaskSummaryCmd(MyConfig.accountSet.userId);
    this->post(cmd, observer);
}

void HttpCmdManager::getRecentTask(QObject* observer)
{
    qDebug() << RESTAPI_URI_RECENT_TASK;

    HttpRecentTaskCmd* cmd = new HttpRecentTaskCmd(MyConfig.accountSet.userId);
    this->post(cmd, observer);
}

void HttpCmdManager::getUserStorageInfo(QObject* observer)
{
    qDebug() << RESTAPI_URI_USER_STORAGE;

    HttpUserStorageCmd* cmd = new HttpUserStorageCmd();
    this->post(cmd, observer);
}

void HttpCmdManager::loadingProjectName(QObject* observer)
{
    qDebug() << RESTAPI_URI_PROJECT_LIST;

    //HttpProjectlistCmd* cmd = new HttpProjectlistCmd();
    //this->post(cmd, observer);
}

void HttpCmdManager::queryMajorProgress(qint64 taskid, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_TASK_PROCESS;

    HttpTaskProcessImg* cmd = new HttpTaskProcessImg(taskid, enRenderPhoton);
    this->post(cmd, observer);
}

void HttpCmdManager::queryPhotonProgress(qint64 taskid, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_TASK_PROCESS;

    HttpTaskProcessImg* cmd = new HttpTaskProcessImg(taskid, enRenderMainPhoto);
    this->post(cmd, observer);
}

void HttpCmdManager::queryJobFrameGrab(qint32 frameId, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_FRAME_GRAB;

    HttpJobFrameGrab* cmd = new HttpJobFrameGrab(frameId);
    this->post(cmd, observer);
}

void HttpCmdManager::queryJobFrameRenderingTime(qint32 frameId, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_FRAME_RENDERING_TIME;

    HttpJobFrameRenderingTime* cmd = new HttpJobFrameRenderingTime(frameId);
    this->post(cmd, observer);
}

void HttpCmdManager::qureyClientProNotice(int channel, int lang, QObject* observer)
{
    qDebug() << RESTAPI_URI_USER_PRO_NOTICE;
    HttpClientProNotice* cmd = new HttpClientProNotice(channel, lang);
    this->post(cmd, observer);
}

void HttpCmdManager::qureyClientSysNotice(int channel, int lang, int noticeType, QObject* observer)
{
    qDebug() << RESTAPI_URI_USER_SYS_NOTICE;
    HttpClientSysNotice* cmd = new HttpClientSysNotice(channel, lang, noticeType);
    this->post(cmd, observer);
}

void HttpCmdManager::loadingTaskParamters(qint64 taskid, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_LOAD_TASK_PARAMTER;
    HttpLoadingTaskPatamter* cmd = new HttpLoadingTaskPatamter(taskid);
    this->post(cmd, observer);
}

void HttpCmdManager::qureyTaskFrameRenderingInfo(qint64 taskid, qint32 pageNum, qint32 pageSize, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_FRAME_RENDER_INFO;
    HttpTaskFrameRenderingInfo* cmd = new HttpTaskFrameRenderingInfo(taskid, pageNum, pageSize, "");
    this->post(cmd, observer);
}

void HttpCmdManager::loadTaskThumbnail(qint64 frameId, int frameStatus, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_THUMBNAIL;
    HttpLoadTaskThumbnail* cmd = new HttpLoadTaskThumbnail(frameId, frameStatus);
    this->post(cmd, observer);
}

void HttpCmdManager::loadFrameRenderLog(qint64 frameId, int pageNum, const QString& renderingType, qint32 pageSize, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_FRAME_RENDER_LOG;
    HttpRenderLog* cmd = new HttpRenderLog(frameId, pageNum, renderingType, pageSize);
    this->post(cmd, observer);
}

void HttpCmdManager::searchTaskFrameRenderingInfo(qint64 taskid, const QString& searchKey, const QList<int>& stateList, qint32 pageNum, qint32 pageSize, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_FRAME_RENDER_INFO;
    HttpTaskFrameRenderingInfo* cmd = new HttpTaskFrameRenderingInfo(taskid, pageNum, pageSize, searchKey, stateList);
    this->post(cmd, observer);
}

void HttpCmdManager::queryTaskRenderingConsume(qint64 taskid, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_FRAME_RENDER_CONSUME;
    HttpTaskRenderingConsume* cmd = new HttpTaskRenderingConsume(taskid);
    this->post(cmd, observer);
}

// 插件的操作和查询
void HttpCmdManager::queryRenderSoftVersion(qint64 cgId, const QString& os, QObject* observer) // 查询插件版本信息
{
    qDebug() << RESTAPI_URI_USER_RENDER_SOFT_VERSION;
    HttpRenderSoftVersion* cmd = new HttpRenderSoftVersion(cgId, os.toLower());
    this->post(cmd, observer);
}

void HttpCmdManager::queryUserPluginConfig(QList<qint64> cgId, QObject* observer) // 查询插件信息
{
    qDebug() << RESTAPI_URI_USER_PLUGIN_CONFIG;
    HttpUserPluginConfig* cmd = new HttpUserPluginConfig(cgId, OSTYPE_DEFAULT);// 默认获取windows系统下的插件
    this->post(cmd, observer);
}

void HttpCmdManager::addUserPluginConfig(OperateUserRenderPlugin* opt, QObject* observer) // 添加用户配置
{
    qDebug() << RESTAPI_URI_USER_ADD_RENDER_PLUGIN;
    HttpOperateUserRenderPlugin* cmd = new HttpOperateUserRenderPlugin(RESTAPI_USER_ADD_RENDER_PLUGIN, opt);
    this->post(cmd, observer);
}

void HttpCmdManager::removeUserPluginConfig(OperateUserRenderPlugin* opt, QObject* observer) // 删除用户配置
{
    qDebug() << RESTAPI_URI_USER_REMOVE_RENDER_PLUGIN;
    HttpOperateUserRenderPlugin* cmd = new HttpOperateUserRenderPlugin(RESTAPI_USER_REMOVE_RENDER_PLUGIN, opt);
    this->post(cmd, observer);
}

void HttpCmdManager::setDefaultUserPluginConfig(OperateUserRenderPlugin* opt, QObject* observer) // 用户配置设置默认
{
    qDebug() << RESTAPI_URI_USER_DEFAULT_RENDER_PLUGIN;
    HttpOperateUserRenderPlugin* cmd = new HttpOperateUserRenderPlugin(RESTAPI_USER_DEFAULT_RENDER_PLUGIN, opt);
    this->post(cmd, observer);
}

void HttpCmdManager::editUserPluginConfig(OperateUserRenderPlugin* opt, QObject* observer) // 编辑用户配置
{
    qDebug() << RESTAPI_URI_USER_EDIT_RENDER_PLUGIN;
    HttpOperateUserRenderPlugin* cmd = new HttpOperateUserRenderPlugin(RESTAPI_USER_EDIT_RENDER_PLUGIN, opt);
    this->post(cmd, observer);
}

// 用户项目管理
void HttpCmdManager::queryUserProjectList(QObject* observer)
{
    qDebug() << RESTAPI_URI_PROJECT_LIST;
    HttpProjectlistCmd* cmd = new HttpProjectlistCmd(QUERY_PROJECT);
    this->post(cmd, observer);
}

void HttpCmdManager::addUserProject(const QString& projName, QObject* observer)
{
    qDebug() << RESTAPI_URI_PROJECT_LIST;
    HttpProjectlistCmd* cmd = new HttpProjectlistCmd(ADD_PROJECT, projName);
    this->post(cmd, observer);
}

void HttpCmdManager::deleteUserProject(const QString& projName, QObject* observer)
{
    qDebug() << RESTAPI_URI_PROJECT_LIST;
    HttpProjectlistCmd* cmd = new HttpProjectlistCmd(DELETE_PROJECT, projName);
    this->post(cmd, observer);
}

// 帧操作 开始
void HttpCmdManager::frameOperatorStart(qint64 taskId, QList<qint64> frameIds, int selectAll, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_FRAME_START;

    HttpOperatorTaskFrame* cmd = new HttpOperatorTaskFrame(taskId, frameIds, selectAll, RESTAPI_JOB_FRAME_START);
    this->post(cmd, observer);
}

void HttpCmdManager::frameOperatorStart(qint64 taskId, qint64 frameId, int selectAll,QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_FRAME_START;

    HttpOperatorTaskFrame* cmd = new HttpOperatorTaskFrame(taskId, frameId, selectAll, RESTAPI_JOB_FRAME_START);
    this->post(cmd, observer);
}

// 帧操作 重提
void HttpCmdManager::frameOperatorRecommit(qint64 taskId, QList<qint64> frameIds, int selectAll, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_FRAME_RECOMMIT;

    HttpOperatorTaskFrame* cmd = new HttpOperatorTaskFrame(taskId, frameIds, selectAll, RESTAPI_JOB_FRAME_RECOMMIT);
    this->post(cmd, observer);
}

void HttpCmdManager::frameOperatorRecommit(qint64 taskId, qint64 frameId, int selectAll,QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_FRAME_RECOMMIT;

    HttpOperatorTaskFrame* cmd = new HttpOperatorTaskFrame(taskId, frameId, selectAll, RESTAPI_JOB_FRAME_RECOMMIT);
    this->post(cmd, observer);
}

// 帧操作 停止
void HttpCmdManager::frameOperatorStop(qint64 taskId, QList<qint64> frameIds, int selectAll, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_FRAME_STOP;

    HttpOperatorTaskFrame* cmd = new HttpOperatorTaskFrame(taskId, frameIds, selectAll, RESTAPI_JOB_FRAME_STOP);
    this->post(cmd, observer);
}

void HttpCmdManager::frameOperatorStop(qint64 taskId, qint64 frameId, int selectAll,QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_FRAME_STOP;

    HttpOperatorTaskFrame* cmd = new HttpOperatorTaskFrame(taskId, frameId, selectAll, RESTAPI_JOB_FRAME_STOP);
    this->post(cmd, observer);
}

void HttpCmdManager::querySubAccountNameList(QObject* observer)
{
    qDebug() << RESTAPI_URI_LOADING_USER_NAME;

    HttpLoadUserName* cmd = new HttpLoadUserName();
    this->post(cmd, observer);
}

void HttpCmdManager::queryAllErrorCodes(QObject* observer)
{
    qDebug() << RESTAPI_URI_ERROR_LIST_ALL;

    int lang = LocalSetting::getInstance()->getLanguageId();
    HttpErrorListCmd* cmd = new HttpErrorListCmd(lang);
    this->post(cmd, observer);
}

void HttpCmdManager::queryAdvancedParamSetting(QObject* observer)
{
    qDebug() << RESTAPI_URI_ADVANCED_PARAM_SETTING;

    // 填1就对了
    HttpAdvSettingCmd* cmd = new HttpAdvSettingCmd(1);
    this->post(cmd, observer);
}

bool HttpCmdManager::queryTaskInfo(QList<qint64> taskIds, bool freshView, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_QUERY_TASK_INFO;
    if(taskIds.isEmpty())
        return false;

    int requestTimes = (taskIds.count() / 100) + 1;
    int pos = 0;
    for(int i = 0; i < requestTimes; i++){
        QList<qint64> taskid = taskIds.mid(pos, 100);
        HttpQueryTaskInfo* cmd = new HttpQueryTaskInfo(taskid, i == (requestTimes - 1), freshView);
        this->post(cmd, observer);
        pos += 100;
    }
    return true;
}

void HttpCmdManager::userFeedback(const QString& userName, const QString& detail, const QString& contactWay, int type, QObject* observer)
{
    qDebug() << RESTAPI_URI_USER_FEEDBACK;

    HttpUserFeedbackCmd* cmd = new HttpUserFeedbackCmd(userName, detail, contactWay, type, CLIENT_ANIMATE, STRFILEVERSION);
    this->post(cmd, observer);
}

void HttpCmdManager::taskUpgradeRam(QList<qint64> taskIds, int ram, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_TASK_UPGRADE_TASK_RAM;

    HttpUpgradeTaskRam* cmd = new HttpUpgradeTaskRam(taskIds, ram);
    this->post(cmd, observer);
}

void HttpCmdManager::taskUpgradeRam(qint64 taskId, int ram, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_TASK_UPGRADE_TASK_RAM;

    HttpUpgradeTaskRam* cmd = new HttpUpgradeTaskRam(taskId, ram);
    this->post(cmd, observer);
}

void HttpCmdManager::updateTaskRemark(qint64 taskId, const QString& remark, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_UPDATE_TASK_REMARK;

    HttpUpdateTaskRemarkCmd* cmd = new HttpUpdateTaskRemarkCmd(taskId, remark);
    this->post(cmd, observer);
}

void HttpCmdManager::updateTaskOverTime(qint64 taskId, int overtime, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_UPDATE_TASK_OVERTIME;

    HttpTaskOvertimeCmd* cmd = new HttpTaskOvertimeCmd(taskId, overtime);
    this->post(cmd, observer);
}

void HttpCmdManager::updateTaskOverTimeStop(qint64 taskId, int overtime, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_UPDATE_TASK_TIMEOUT;

    HttpTaskOvertimeStopCmd* cmd = new HttpTaskOvertimeStopCmd(taskId, overtime);
    this->post(cmd, observer);
}

void HttpCmdManager::updateTaskLevel(qint64 taskId, const QString& level, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_UPDATE_TASK_LEVEL;

    HttpUpdateTaskLevelCmd* cmd = new HttpUpdateTaskLevelCmd(taskId, level);
    this->post(cmd, observer);
}

void HttpCmdManager::queryJobProducerList(QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_LOAD_PRODUCER;

    HttpLoadProducerCmd* cmd = new HttpLoadProducerCmd();
    this->post(cmd, observer);
}

void HttpCmdManager::queryQRCode(QObject* observer)
{
    qDebug() << RESTAPI_URI_ATTENTION_QRCODE;

    HttpQRCodeCmd* cmd = new HttpQRCodeCmd();
    this->post(cmd, observer);
}

// 获取绑定信息
void HttpCmdManager::queryBindWx(QObject* observer)
{
    qDebug() << RESTAPI_URI_QUERY_BINDING_WX;

    HttpQueryBindWx *cmd = new HttpQueryBindWx();
    this->post(cmd, observer);
}

// 是否同意绑定
void HttpCmdManager::bindWxToUser(qint64 id, qint32 bindtype, QObject* observer)
{
    qDebug() << RESTAPI_URI_BIND_WX_TO_USER;

    HttpBindWxToUser *cmd = new HttpBindWxToUser(id, bindtype);
    this->post(cmd, observer);
}

void HttpCmdManager::queryPlatformStatus(int zone, QObject* observer)
{
    qDebug() << RESTAPI_URI_TASK_PLATFORM_STATUS;

    HttpGetPlatformStatus* cmd = new HttpGetPlatformStatus(zone);
    this->post(cmd, observer);
}

void HttpCmdManager::startDownload(qint64 taskId, QObject* observer)
{
    qDebug() << RESTAPI_URI_DOWNLOAD_START;

    HttpDownloadStartCmd* cmd = new HttpDownloadStartCmd(taskId);
    this->post(cmd, observer);
}

void HttpCmdManager::downloadComplete(qint64 taskId, const QString& downloadRequestId, QObject* observer)
{
    qDebug() << RESTAPI_URI_DOWNLOAD_COMPLETE;

    HttpDownloadCompleteCmd* cmd = new HttpDownloadCompleteCmd(taskId, downloadRequestId);
    this->post(cmd, observer);
}

void HttpCmdManager::queryTaskDownloadCount(QList<qint64> taskIds, QObject* observer)
{
    qDebug() << RESTAPI_URI_DOWNLOAD_COUNT;

    int requestTimes = (taskIds.count() / 100) + 1;
    int pos = 0;
    for (int i = 0; i < requestTimes; i++) {
        QList<qint64> taskid = taskIds.mid(pos, 100);
        HttpDownloadCountCmd* cmd = new HttpDownloadCountCmd(taskid);
        this->post(cmd, observer);
        pos += 100;
    }
}

void HttpCmdManager::getTaskDownloadInfo(qint64 taskId, QObject* observer)
{
    qDebug() << RESTAPI_URI_DOWNLOAD_INFO;

    HttpDownloadInfoCmd* cmd = new HttpDownloadInfoCmd(taskId);
    this->post(cmd, observer);
}

void HttpCmdManager::uploadReport(qint64 taskId, const QString& filePath, QObject * observer)
{
    qDebug() << RESTAPI_URI_UPLOAD_FILE;

    HttpReportLogCmd* cmd = new HttpReportLogCmd(taskId, filePath);
    this->upload(cmd, observer);
}

void HttpCmdManager::uploadReport(qint64 taskId, const QJsonObject & jsonObject, QObject * observer)
{
    qDebug() << RESTAPI_URI_TRANSPORTINFO;

    HttpReportTransport* cmd = new HttpReportTransport(taskId, jsonObject);
    this->post(cmd, observer);
}

void HttpCmdManager::queryNodeDetails(qint64 frameId, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_NODE_DETAILS;

    HttpGetNodeDetailsCmd* cmd = new HttpGetNodeDetailsCmd(frameId);
    this->post(cmd, observer);
}

void HttpCmdManager::loadingAccountsProjNames(QObject* observer)
{
    qDebug() << RESTAPI_URI_LOADING_PROJECT_NAME;

    HttpProjectNamesCmd* cmd = new HttpProjectNamesCmd();
    this->post(cmd, observer);
}

void HttpCmdManager::updateTaskLimit(qint64 taskId, const QString& count, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_UPDATE_TASK_LIMIT;

    HttpUpdateTaskLimitCmd* cmd = new HttpUpdateTaskLimitCmd(taskId, count);
    this->post(cmd, observer);
}

void HttpCmdManager::queryFrameRecommitInfo(qint64 frameId, int pageNum, qint64 pageSize, QObject* observer)
{
    qDebug() << RESTAPI_URI_FRAME_RECOMMIT_INFO;

    HttpFrameRecommitInfoCmd* cmd = new HttpFrameRecommitInfoCmd(frameId, pageNum, pageSize);
    this->post(cmd, observer);
}

void HttpCmdManager::getTaskOutputSize(qint64 taskId, int fromPage, QObject* observer)
{
    qDebug() << RESTAPI_URI_OUTPUT_GETSIZE;

    HttpGetOutputSizeCmd* cmd = new HttpGetOutputSizeCmd(taskId, fromPage);
    this->post(cmd, observer);
}

void HttpCmdManager::getTasksOutputSize(const QList<qint64>& taskIds, int fromPage, QObject* observer)
{
    qDebug() << RESTAPI_URI_OUTPUT_GETSIZE;

    if (taskIds.isEmpty())
        return;

    HttpGetOutputSizeCmd* cmd = new HttpGetOutputSizeCmd(taskIds, fromPage);
    this->post(cmd, observer);
}

void HttpCmdManager::paymentVoucher(qint64 taskId, QObject* observer)
{
    qDebug() << RESTAPI_URI_PAYMENT_USE_VOUCHER;

    HttpPaymentVoucherCmd *cmd = new HttpPaymentVoucherCmd(taskId);
    this->post(cmd, observer);
}

void HttpCmdManager::queryVoucher(QObject* observer)
{
    qDebug() << RESTAPI_URI_PAYMENT_QUERY_VOUCHER;

    HttpPaymentQueryVoucherCmd *cmd = new HttpPaymentQueryVoucherCmd();
    this->post(cmd, observer);
}

void HttpCmdManager::ssoLogin(const QString & username, const QString & passwd, QObject * observer)
{
    qDebug() << SSOAPI_URI_SIGNIN;

    SSOUserLoginCmd *cmd = new SSOUserLoginCmd(username, passwd);
    this->post(cmd, observer);
}

void HttpCmdManager::thirdLogin(const QString & userKey, int type, QObject * observer)
{
    qDebug() << RESTAPI_URI_THIRD_USER_LOGIN;

    HttpThirdSigninCmd *cmd = new HttpThirdSigninCmd(userKey, type);
    this->post(cmd, observer);
}

void HttpCmdManager::queryPicturePrice(QObject* observer)
{
    qDebug() << RESTAPI_URI_PICTURE_PRICE;

    HttpQueryPicturePrice *cmd = new HttpQueryPicturePrice();
    this->post(cmd, observer);
}

void HttpCmdManager::queryAtReportInfo(qint64 userId, qint64 taskId,  QObject* observer)
{
    qDebug() << RESTAPI_URI_AT_REPORT_DATA_URL;

    HttpAtReportUrl *cmd = new HttpAtReportUrl(userId, taskId);
    this->post(cmd, observer);
}

void HttpCmdManager::queryAtCloudPointInfo(qint64 userId, qint64 taskId, QObject* observer)
{
	qDebug() << RESTAPI_URI_AT_POINTCLOUD_DATA_URL;

	HttpAtPointCloudDataUrl *cmd = new HttpAtPointCloudDataUrl(userId, taskId);
	this->post(cmd, observer);
}

void HttpCmdManager::submitCloneTask(qint64 taskId, const QString& projectName, const RebuildOptionParams& params, QObject* observer)
{
    qDebug() << RESTAPI_URI_JOB_SUBMIT_CLONE_TASK;

    QString artist = System::getLoginDisplayName();
    HttpCloneTask *cmd = new HttpCloneTask(taskId, artist, projectName, params);
    this->post(cmd, observer);
}

void HttpCmdManager::queryShareResultLink(qint64 taskId, int days , QObject* observer)
{
    qDebug() << RESTAPI_URI_SHARE_REBUILD_RESULT_LINK;

    HttpShareResultLink* cmd = new HttpShareResultLink(taskId, days);
    this->post(cmd, observer);
}

void HttpCmdManager::uploadTaskSurveyFile(qint64 taskId, const QString& filePath, QObject* observer)
{
    qDebug() << RESTAPI_URI_UPLOAD_JSON_FILE_URL;

    HttpUploadJsonFileCmd* cmd = new HttpUploadJsonFileCmd(taskId, filePath);
	
    this->post(cmd, observer);
}

void HttpCmdManager::checkImagePosJsonMD5(qint64 userId, qint64 taskId, QString md5, QObject* observer)
{
	qDebug() << RESTAPI_URI_CHECK_IMAGE_POS_MD5;

	HttpCheckImagePosFileMd5* cmd = new HttpCheckImagePosFileMd5(userId, taskId, md5);
	this->post(cmd, observer);
}

void HttpCmdManager::updateFilterTime()
{
    if(filterContent.filterType != TIME_STATE_FILTER)
        return;

    QString content = filterContent.content;
    QDateTime currTime = QDateTime::currentDateTime();

    QStringList timeRound = content.split("::");
    QDateTime endDate = QDateTime::fromString(timeRound.at(timeRound.size() - 1), "yyyy-MM-dd hh:mm:ss");

    // 如果时间已经翻越，则需要更新时间
    if(endDate.toTime_t() < currTime.toTime_t()) {

        QDate currDate = QDate::currentDate();
        QDate start;
        QDate end = currDate;
        switch(m_curTimeFilter) {
        case enFilterAll:
            start = currDate.addYears(-5);
            break;
        case enFilter1Year:
            start = currDate.addYears(-1);
            break;
        case enFilter3Month:
            start = currDate.addMonths(-3);
            break;
        case enFilter1Month:
            start = currDate.addMonths(-1);
            break;
        case enFilter1Week:
            start = currDate.addDays(-7);
            break;
        default:
            break;
        }

        filterContent.content = start.toString("yyyy-MM-dd") + " 00:00:00" + "::" + end.toString("yyyy-MM-dd") + " 23:59:59";
    }
}
