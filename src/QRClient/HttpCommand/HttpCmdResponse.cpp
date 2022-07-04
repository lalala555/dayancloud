#include "stdafx.h"
#include "HttpCommon.h"
#include "HttpCmdResponse.h"
#include "HttpCmdManager.h"
#include "HttpCmdRequest.h"
#include "HttpCmd/HttpSubTaskCmd.h"
#include "Views/NewJobs/JobStateMgr.h"
#include "Views/NewJobs/JobState.h"
#include "Kernel/TaskCenterManager.h"
#include "Common/LangTranslator.h"
#include "ThreadJob/SmallPicMgr.h"
#include <QtConcurrent/QtConcurrent>

HttpCmdResponse::HttpCmdResponse(QObject *parent)
    : QObject(parent)
{
}

HttpCmdResponse::~HttpCmdResponse()
{
}

void HttpCmdResponse::attach(QObject* observer)
{
    if(observer == Q_NULLPTR)
        return;

    m_observers.append(observer);
    connect(this, SIGNAL(sendRecvResponse(ResponseHead*)), observer, SLOT(onRecvResponse(QSharedPointer<ResponseHead>)));
}

void HttpCmdResponse::detach(QObject* observer)
{
    if(observer == Q_NULLPTR)
        return;

    m_observers.removeOne(observer);
    disconnect(this, SIGNAL(sendRecvResponse(ResponseHead*)), observer, SLOT(onRecvResponse(QSharedPointer<ResponseHead>)));
}

void HttpCmdResponse::notify(ResponseHead* data)
{
    if(m_observers.isEmpty()) {
        return;
    }

    foreach(QObject* observer, m_observers) {
        //observer->OnRecvResponse(data);
    }
}

void HttpCmdResponse::onRecvResponse(QSharedPointer<ResponseHead> data)
{
    if(data == Q_NULLPTR) {
        qWarning() << "Network error! cmd = " << data->cmdType << " " << queryCmdName(data->cmdType);
        return;
    }

#if 1
    int code = data->code;
    if (code != HTTP_ERROR_SUCCESS) {
        QString errorText = LangTranslator::RequstErrorI18n(data->msg);
#ifdef _DEBUG
        LOGFMTE("[HttpCmdResponse] response error msg, cmd = %d(%s), code = %d, msg = %s(%s)",
            data->cmdType, qPrintable(queryCmdName(data->cmdType)), code, qPrintable(data->msg), qPrintable(errorText));
#else
        LOGFMTE("[HttpCmdResponse] response error msg, cmd = %d(%s), code = %d, msg = %s(%s)",
                data->cmdType, qUtf8Printable(queryCmdName(data->cmdType)), code, qUtf8Printable(data->msg), qUtf8Printable(errorText));
#endif
        this->httpResponseException(data);
    }
    this->httpResponseSuccess(data);
#else
    QtConcurrent::run([this, data]() {
        int code = data->code;
        if (code != HTTP_ERROR_SUCCESS) {
            QString errorText = LangTranslator::RequstErrorI18n(data->msg);
            LOGFMTE("[HttpCmdResponse] response error msg, cmd = %d(%s), code = %d, msg = %s(%s)",
                data->cmdType, qUtf8Printable(queryCmdName(data->cmdType)), code, qUtf8Printable(data->msg), qUtf8Printable(errorText));
            this->httpResponseException(data);
        }
        this->httpResponseSuccess(data);    
    });
#endif

    HttpCmdRequest* req = (HttpCmdRequest*)sender();
    req->destroyRequst();
    this->detachRequestCmd(req);
    req->deleteLater();
}

void HttpCmdResponse::onUploadResponse(QSharedPointer<ResponseHead> data)
{
    if (data == Q_NULLPTR) {
        qWarning() << "Network error! cmd = " << data->cmdType << " " << queryCmdName(data->cmdType);
        return;
    }

    switch (data->cmdType)
    {
    case RESTAPI_UPLOAD_FILE:
        this->reportLogResponse(data);
        break;
    default:
        break;
    }

    HttpCmdRequest* req = (HttpCmdRequest*)sender();
    req->destroyRequst();
    this->detachRequestCmd(req);
    req->deleteLater();
}

void HttpCmdResponse::httpResponseException(QSharedPointer<ResponseHead> data)
{
    int code = data->code;
    switch(code) {
    case HTTP_ERROR_NEED_USER_LOGIN: {
        CGlobalHelp::Response(UM_NEED_USER_LOGIN);
        break;
    }
    case HTTP_ERROR_ACCOUNT_LOCKED:
    case HTTP_ERROR_ACCOUNT_USERNAME_PASSWORD_FAIL: {
        CGlobalHelp::Response(UM_USERNAME_PASSWORD_FAIL);
        break;
    }
    case HTTP_ERROR_TASK_EXPIRE_CANT_OPERATE:
        CGlobalHelp::Response(UM_MESSAGE_FLOAT, MSGBOX_TYPE_TASK_EXPIRE_CANT_OPERATE, 0);
        break;
    case HTTP_ERROR_RENDER_COMMIT_PASS_ONE_MONTH:
        CGlobalHelp::Response(UM_MESSAGE_FLOAT, MSGBOX_TYPE_RENDER_COMMIT_PASS_ONE_MONTH, 0);
        break;
    default:
        break;
    }

    int type = data->cmdType;
    switch(type) {
    case RESTAPI_PLATFORM_LIST:
        break;
    case RESTAPI_USER_LOGIN:
        break;
    case RESTAPI_USER_TRANS_BID:
        break;
    case RESTAPI_USER_INFO: {
        MSGBOXPARAM *param = new MSGBOXPARAM;
        param->content = data->msg;
        CGlobalHelp::Response(UM_MESSAGE_FLOAT, HTTP_ERROR_UNKNOWN, (intptr_t)(param));
    }
    break;
    case RESTAPI_JOB_LIST: {
        MSGBOXPARAM *param = new MSGBOXPARAM;
        param->content = data->msg;
        CGlobalHelp::Response(UM_MESSAGE_FLOAT, HTTP_ERROR_UNKNOWN, (intptr_t)(param));
    }
    break;
    case RESTAPI_USER_BALANCE: {
        MSGBOXPARAM *param = new MSGBOXPARAM;
        param->content = data->msg;
        CGlobalHelp::Response(UM_MESSAGE_FLOAT, HTTP_ERROR_UNKNOWN, (intptr_t)(param));
    }
    break;
    case RESTAPI_JOB_TASK_START:
    case RESTAPI_JOB_TASK_STOP:
    case RESTAPI_JOB_TASK_DELETE:
        break;
    case RESTAPI_JOB_TASK_RECOMMIT: {
        MSGBOXPARAM *param = new MSGBOXPARAM;
        param->content = data->msg;
        CGlobalHelp::Response(UM_MESSAGE_FLOAT, HTTP_ERROR_UNKNOWN, (intptr_t)(param));
    }
    break;
    case RESTAPI_JOB_CREATE:
        break;
    case RESTAPI_JOB_SUBMIT:
        break;
    case RESTAPI_JOB_UPDATE_TASK_REMARK:
    case RESTAPI_JOB_UPDATE_TASK_LEVEL:
    case RESTAPI_JOB_UPDATE_TASK_OVERTIME:
    case RESTAPI_JOB_UPDATE_TASK_TIMEOUT: {
        MSGBOXPARAM *param = new MSGBOXPARAM;
        param->code = data->code;
        param->content = data->msg;
        CGlobalHelp::Response(UM_MESSAGE_ERROR, HTTP_ERROR_UNKNOWN, (intptr_t)(param));
    }
    break;
    }
}

void HttpCmdResponse::httpResponseSuccess(QSharedPointer<ResponseHead> data)
{
    int type = data->cmdType;
    switch(type) {
    case SSOAPI_SIGNIN:
        this->ssoUserLoginResponse(data);
        break;
    case RESTAPI_PLATFORM_LIST:
        this->platformResponse(data);
        break;
    case RESTAPI_USER_LOGIN:
        this->userLoginResponse(data);
        break;
    case RESTAPI_USER_TRANS_BID:
        this->transEngineBidResponse(data);
        break;
    case RESTAPI_USER_TRANS_CONFIG:
        this->transEngineConfigResponse(data);
        break;
    case RESTAPI_USER_INFO:
        this->userInfoResponse(data);
        break;
    case RESTAPI_JOB_LIST:
        this->taskListResponse(data);
        //QtConcurrent::run(this, &HttpCmdResponse::taskListResponse, data);
        break;
	case RESTAPI_TASK_QUERYPUBLISH:
		this->publishListResponse(data);
		break;
    case RESTAPI_USER_BALANCE:
        this->userBalanceResponse(data);
        break;
    case RESTAPI_JOB_TASK_START:
    case RESTAPI_JOB_TASK_STOP:
    case RESTAPI_JOB_TASK_DELETE:
    case RESTAPI_JOB_TASK_FULL_SPEED:
    case RESTAPI_JOB_TASK_UPGRADE_TASK_RAM:
        this->taskOperatorResponse(data);
        break;
    case RESTAPI_JOB_TASK_FAILED_FRAME_RECOMMIT:
        this->taskResubFailFrameResponse(data);
        break;
    case RESTAPI_JOB_CREATE:
        this->createJobIdsResponse(data);
        break;
    case RESTAPI_JOB_SUBMIT:
        this->taskSubmitResponse(data);
        break;
    case RESTAPI_JOB_TASK_TOTAL_INFO:
        this->totalTaskFrameResponse(data);
        break;
    case RESTAPI_TASK_SUMMARY:
        this->getTaskStatusCountResponse(data);
        break;
    case RESTAPI_USER_RENDER_SETTING:
        this->userRenderSettingResponse(data);
        break;
    case RESTAPI_JOB_FRAME_RENDER_INFO:
        this->taskFrameRenderingInfoResponse(data);
        break;
    case RESTAPI_JOB_FRAME_RENDER_CONSUME:
        this->taskFrameRenderingConsumeResponse(data);
        break;
    case RESTAPI_JOB_LOAD_TASK_PARAMTER:
        this->loadinTaskParamterResponse(data);
        break;
    case RESTAPI_JOB_TASK_THUMBNAIL:
        QtConcurrent::run([this, data]() {
            this->loadTaskThumbnailResponse(data);
        });
        break;
    case RESTAPI_JOB_FRAME_RENDER_LOG:
        this->loadFrameRenderLogResponse(data);
        break;
    case RESTAPI_PROJECT_LIST:
        this->projectListResponse(data);
        break;
    case RESTAPI_USER_PLUGIN_CONFIG:
        this->pluginConfigResponse(data);
        break;
    case RESTAPI_JOB_FRAME_START:
    case RESTAPI_JOB_FRAME_STOP:
    case RESTAPI_JOB_FRAME_RECOMMIT:
        this->taskFrameOperatorResponse(data);
        break;
    case RESTAPI_LOADING_USER_NAME:
        this->loadUserNameResponse(data);
        break;
    case RESTAPI_ERROR_LIST_ALL:
        this->errorListResponse(data);
        break;
    case RESTAPI_ADVANCED_PARAM_SETTING:
        this->advancedParamSettingResponse(data);
        break;
    case RESTAPI_JOB_QUERY_TASK_INFO:
        this->taskListResponse(data);
        this->queryTaskInfoResponse(data);
        break;
    case RESTAPI_JOB_UPDATE_TASK_REMARK:
        this->updateTaskRemarkResponse(data);
        break;
    case RESTAPI_JOB_UPDATE_TASK_LEVEL:
        this->updateTaskLevelResponse(data);
        break;
    case RESTAPI_JOB_UPDATE_TASK_OVERTIME:
        this->updateTaskOvertimeResponse(data);
        break;
    case RESTAPI_JOB_UPDATE_TASK_TIMEOUT:
        this->updateTaskOvertimeStopResponse(data);
        break;
    case RESTAPI_JOB_LOAD_PRODUCER:
        this->queryJobProducerListResponse(data);
        break;
    case RESTAPI_TASK_PLATFORM_STATUS:
        this->platformStatusResponse(data);
        break;
    case RESTAPI_DOWNLOAD_START:
        this->downloadStartResponse(data);
        break;
    case RESTAPI_DOWNLOAD_COUNT:
        this->downloadCountResponse(data);
        break;
    case RESTAPI_LOADING_PROJECT_NAME:
        this->loadingAccountsProjNamesResponse(data);
        break;
    case RESTAPI_OUTPUT_GETSIZE:
        this->getTaskOutputSizeResponse(data);
        break;
    case RESTAPI_PAYMENT_USE_VOUCHER:
        this->useVoucherResponse(data);
        break;
    case RESTAPI_PAYMENT_QUERY_VOUCHER:
        this->queryVoucherResponse(data);
        break;
    case RESTAPI_QUERY_PICTURE_PRICE:
        this->queryPicturePriceResponse(data);
        break;
    case RESTAPI_JOB_RESUBMIT_CCAT:
        this->taskResubmitCCATResponse(data);
        break;
    case RESTAPI_SHARE_REBUILD_RESULT_LINK:
        this->queryShareResultLinkResponse(data);
        break;
    case RESTAPI_JOB_SUBMIT_CLONE_TASK:
        this->cloneTaskResponse(data);
        break;
    }
}

void HttpCmdResponse::ssoUserLoginResponse(QSharedPointer<ResponseHead> data)
{
    auto userlogin = qSharedPointerCast<SSOResponse>(data);
    MyConfig.userSet.userKey = userlogin->rsAuthToken;
}

void HttpCmdResponse::platformResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<PlatformResponse> platforms = qSharedPointerCast<PlatformResponse>(data);

}

void HttpCmdResponse::userLoginResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<UserLoginResponse> userlogin = qSharedPointerCast<UserLoginResponse>(data);
    if(userlogin->code == HTTP_ERROR_SUCCESS) {
        MyConfig.userSet.id             = userlogin->userId;
        MyConfig.userSet.userName       = userlogin->userName;
        MyConfig.userSet.channel        = userlogin->channel;
        MyConfig.userSet.platform       = userlogin->platform;
        MyConfig.userSet.signature      = userlogin->signature;
        MyConfig.userSet.userKey        = userlogin->userKey;
        MyConfig.userSet.version        = userlogin->version;
        MyConfig.userSet.raySyncUserKey = userlogin->raySyncUserKey;
        MyConfig.userSet.accountType    = userlogin->accountType;

        // 重新设置请求header
        HttpRequestHeader head = HttpCmdManager::getInstance()->getRequestHeader();
        head.userKey = userlogin->userKey;
        HttpCmdManager::getInstance()->setRequestHeader(head);

        MyConfig.accountSet.accountType      = userlogin->accountType;
        MyConfig.accountSet.shareMainCapital = userlogin->shareMainCapital;
        MyConfig.accountSet.phone            = userlogin->phone;
        // 重置登录次数
        MyConfig.accountSet.userName     = userlogin->userName;
        MyConfig.accountSet.reLoginTimes = 0;
        // 设置客户端内存临时信息
        MyConfig.client.agentName        = AGENT_NAME;
        MyConfig.client.userName         = userlogin->userName;
        MyConfig.client.password         = LocalSetting::getInstance()->getPassword();
        MyConfig.client.clientType       = CLIENT_ID;
        MyConfig.client.loginChannel     = CHANNEL_CLIENT;
        MyConfig.client.version          = STRFILEVERSION;
    } else if(userlogin->code == HTTP_ERROR_ACCOUNT_NOT_BINDING) {
        MyConfig.accountSet.userName     = userlogin->userName;
        MyConfig.accountSet.accessId     = userlogin->accessId;
        MyConfig.accountSet.phone        = userlogin->phone;
        CConfigDb::Inst()->setAccessId(userlogin->userName.toStdString(),
                                       userlogin->accessId.toStdString());
    }
}

void HttpCmdResponse::transEngineBidResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<TransBidResponse> transBid = qSharedPointerCast<TransBidResponse>(data);
    if(transBid->code == HTTP_ERROR_SUCCESS) {
        MyConfig.storageSet.commonBid   = transBid->config_bid.toInt();
        MyConfig.storageSet.uploadBid   = transBid->input_bid.toInt();
        MyConfig.storageSet.downloadBid = transBid->output_bid.toInt();
        MyConfig.storageSet.cloneBid    = transBid->config_bid.toInt();
        if(MyConfig.userSet.isChildAccount()) {
            if(MyConfig.accountSet.shareMainCapital) {
                MyConfig.storageSet.uploadBid = transBid->parent_input_bid.toInt();
            }
        }
    }
}

void HttpCmdResponse::transEngineConfigResponse(QSharedPointer<ResponseHead> data) {
    QSharedPointer<TransConfigResponse> configInfo = qSharedPointerCast<TransConfigResponse>(data);
    if (!TransConfig::getInstance()->setTransConfig(QJsonDocument(configInfo->trans_config))) {
        LOGFMTE("[HttpCmdResponse] Request TransEngine Config failed ...");
    }
}

void HttpCmdResponse::userInfoResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<UserInfoResponse> userInfo = qSharedPointerCast<UserInfoResponse>(data);
    if(userInfo->code == HTTP_ERROR_SUCCESS) {
        QDateTime timestamp;
        timestamp.setMSecsSinceEpoch(userInfo->serverTime);
        MyConfig.currentDateTime       = timestamp;
        MyConfig.accountSet.userId     = userInfo->userId;
        MyConfig.accountSet.mainUserId = userInfo->mainUserId;
        MyConfig.accountSet.zone       = userInfo->zone;
        MyConfig.accountSet.city       = userInfo->city;
        MyConfig.accountSet.country    = userInfo->country;
        MyConfig.accountSet.company    = userInfo->company;
        MyConfig.accountSet.enableNodeDetails = userInfo->enableNodeDetails;
        MyConfig.accountSet.taskNodeLimitPermission = userInfo->taskNodeLimitPermission;
        MyConfig.accountSet.groupTaskLevel = userInfo->groupTaskLevel;
        MyConfig.accountSet.openRenderRam  = userInfo->openRenderRam;
        MyConfig.accountSet.authStatus     = userInfo->authStatus;
        MyConfig.accountSet.delayAuthTime  = userInfo->delayAuthTime;
        MyConfig.accountSet.realnameStatus = userInfo->realnameStatus;

        MyConfig.userSet.id            = userInfo->userId;
        MyConfig.userSet.userName      = userInfo->userName;
        MyConfig.userSet.accountType   = userInfo->accountType;

        MyConfig.accountSet.hideBalance   = userInfo->hideBalance ? true : false;
        MyConfig.accountSet.hideJobCharge = userInfo->hideJobCharge ? true : false;
        MyConfig.accountSet.subDeleteTask = userInfo->subDeleteTask ? true : false;
        MyConfig.accountSet.gpuSingleDiscount = userInfo->gpuSingleDiscount;

        // balance
        MyConfig.accountSet.rmbbalance      = userInfo->balance.rmbbalance.toFloat();
        MyConfig.accountSet.usdbalance      = userInfo->balance.usdbalance.toFloat();
        MyConfig.accountSet.coupon          = userInfo->balance.coupon.toFloat();
        MyConfig.accountSet.downloadDisable = userInfo->balance.downloadDisable;
        MyConfig.accountSet.level           = userInfo->balance.vipLevel;
        MyConfig.accountSet.couponCount     = userInfo->balance.couponCount;
        MyConfig.accountSet.qyCoupon        = userInfo->balance.qyCoupon.toFloat();
        MyConfig.accountSet.qyCouponCount   = userInfo->balance.qyCouponCount;

        // pic
        MyConfig.accountSet.pictureLever = userInfo->pictureLever;
        MyConfig.accountSet.picMemberValidity = userInfo->picMemberValidity;

        // memberhistory
        MyConfig.accountSet.memberHistoryVOS = userInfo->memberHistoryVOS;

        MyConfig.accountSet.authStatus     = userInfo->authStatus;
        MyConfig.accountSet.realnameStatus = userInfo->realnameStatus;

        CGlobalHelp::Response(UM_UPDATE_MAIN_UI, PAGE_TASK, 0); // 更新主界面列信息
    }
}

void HttpCmdResponse::userRenderSettingResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<RenderSettingResponse> renderSetting = qSharedPointerCast<RenderSettingResponse>(data);
    if(renderSetting->code == HTTP_ERROR_SUCCESS) {
        MyConfig.renderSet.singleNodeRenderFrames    = renderSetting->singleNodeRenderFrames;
        MyConfig.renderSet.autoCommit                = renderSetting->autoCommit;
        MyConfig.renderSet.separateAccountFlag       = renderSetting->separateAccountFlag;
        MyConfig.renderSet.taskOverTimeSec           = renderSetting->taskOverTimeSec;
        MyConfig.renderSet.isVrayLicense             = renderSetting->isVrayLicense;

        // 公共设置
        MyConfig.renderSet.ignoreMapFlag             = renderSetting->ignoreMapFlag;
        MyConfig.renderSet.ignoreAnalyseWarn         = renderSetting->ignoreAnalyseWarn;
        MyConfig.renderSet.manuallyStartAnalysisFlag = renderSetting->manuallyStartAnalysisFlag;

        // Maya设置
        MyConfig.renderSet.miFileSwitchFlag          = renderSetting->miFileSwitchFlag;
        MyConfig.renderSet.assFileSwitchFlag         = renderSetting->assFileSwitchFlag;
        MyConfig.renderSet.forceAnalysisFlag         = renderSetting->mandatoryAnalyseAllAgent;
        MyConfig.renderSet.justUploadCgFlag          = renderSetting->justUploadCgFlag;

        // 3dsMax设置
        MyConfig.renderSet.maxIgnoreMapFlag          = renderSetting->maxIgnoreMapFlag;
        MyConfig.renderSet.justUploadConfigFlag      = renderSetting->justUploadConfigFlag;

        // 下载设置
        MyConfig.renderSet.downloadLimit             = renderSetting->downloadLimit;
        MyConfig.renderSet.downloadLimitDay          = renderSetting->downloadLimitDay;
        MyConfig.renderSet.cloneLimitDay             = renderSetting->cloneLimitDay;

        // 超时设置
        MyConfig.renderSet.jobTimeout                = CConfigDb::Inst()->getFrameTimeoutStopSec();

        // 禁止提交
        MyConfig.renderSet.submitDisable             = renderSetting->submitDisable;
    }
}

void HttpCmdResponse::userBalanceResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<BalanceResponse> balance = qSharedPointerCast<BalanceResponse>(data);
    if(balance->code == HTTP_ERROR_SUCCESS) {
        MyConfig.accountSet.rmbbalance      = balance->rmbbalance.toFloat();
        MyConfig.accountSet.usdbalance      = balance->usdbalance.toFloat();
        MyConfig.accountSet.coupon          = balance->coupon.toFloat();
        MyConfig.accountSet.downloadDisable = balance->downloadDisable;
        MyConfig.accountSet.level           = balance->vipLevel;
        MyConfig.accountSet.couponCount     = balance->couponCount;
        MyConfig.accountSet.qyCoupon        = balance->qyCoupon.toFloat();
        MyConfig.accountSet.qyCouponCount   = balance->qyCouponCount;
    }
}

void HttpCmdResponse::publishListResponse(QSharedPointer<ResponseHead> data)
{
	/*QSharedPointer<PublishItemsResponse> publishlist = qSharedPointerCast<PublishItemsResponse>(data);
	if (publishlist->code == HTTP_ERROR_SUCCESS)
	{

	}*/
}

void HttpCmdResponse::taskListResponse(QSharedPointer<ResponseHead> data)
{
    qint64 preTaskId = -1;
    QSharedPointer<MaxTaskItemsResponse> tasklist = qSharedPointerCast<MaxTaskItemsResponse>(data);
    // JobStateMgr::Inst()->m_vtPage.clear();
    JobStateMgr::Inst()->setPageSummury(tasklist->total, tasklist->pageCount, tasklist->pageNum, tasklist->size);
    JobStateMgr::Inst()->m_dwCurPage = tasklist->pageNum;
    if(tasklist->code == HTTP_ERROR_SUCCESS) {
        QDateTime timestamp;
        timestamp.setMSecsSinceEpoch(tasklist->serverTime);
        MyConfig.currentDateTime = timestamp;
        HttpCmdManager::getInstance()->filterContent.pageCount = tasklist->pageCount;

        for (int i =0; i < tasklist->items.count(); i++) {
            JobState *jobState = nullptr;
            const MaxTaskItem* info = tasklist->items[i];
            // if(info->cgId != CG_ID_CONTEXTCAPTURE) continue; // 只显示CC任务
            // if(info->id != 8382805) continue;
            jobState = JobStateMgr::Inst()->LookupTaskstate(info->id);

            if(jobState == nullptr) {
                jobState = new JobState(info->id);
                jobState->setJobType(RENDER_JOB);
                JobStateMgr::Inst()->AddTaskstate(jobState);
            }
            JobStateMgr::Inst()->m_vtPage.push_back(info->id);
            jobState->setSubJobUserId(info->userId);
            jobState->setTaskLevel(info->taskLevel);
            jobState->setTaskUserLevel(info->taskUserLevel);
            jobState->setIsDelete(info->isDelete == 1 ? true : false);
            jobState->setShareMainCapital(info->shareMainCapital);
            jobState->setCgId(info->cgId);
            jobState->setCloneOriginalId(info->cloneOriginalId);
            jobState->setCloneParentId(info->cloneParentId);
            jobState->setLayerParentId(info->layerParentId);

            jobState->setScene(info->sceneName);
            jobState->setProjectName(info->projectName);
            jobState->setLayerName(info->layerName);
            jobState->setCamera(info->renderCamera);
            jobState->setOutputLabel(info->outputFileName);		
            jobState->setArtistName(info->producer);
            jobState->setAutoDownPath(info->locationOutput);
            // 自动下载路径由本地设置路径决定
            // jobState->setAutoDownPath(ProjectMgr::getInstance()->getProjectOutput(jobState->GetProjectName())); //todo
            jobState->setJobSubmitter(info->userName);

            jobState->setTaskIdAlias(info->taskAlias);
            jobState->setPreTaskStatus(info->preTaskStatus);
            jobState->setPreTaskStatusText(info->preStatusText);
            jobState->setTaskType(info->taskType);
            jobState->setTaskLimit(info->taskLimit);
            jobState->setMemorySize(info->taskRam);

            jobState->setFrameScope(info->framesRange);
            jobState->setStartDate(info->startTime);
            jobState->setCompelteDate(info->completedDate);
            jobState->setSubmitDate(info->submitDate);
            jobState->setRenderTimeCost(info->renderDuration);
            jobState->setOverTimeRemind(info->taskOverTime);
            jobState->setIsOverTime(info->isOverTime == 1 ? true : false);
            jobState->setOverTimeStop(info->overTimeStop);

            // jobState->SetFeemap(QString::number(info->renderConsume));
            jobState->setTaskRenderConsume(info->renderConsume);
            jobState->setUserAccountConsume(info->userAccountConsume);
            jobState->setCouponConsume(info->couponConsume);
            jobState->setQYCouponConsume(info->qyCouponConsume);
            jobState->setTaskArrears(info->taskArrears);
            jobState->setTaskRenderConsume(info->renderConsume);
            jobState->setOutputBid(MyConfig.storageSet.downloadBid);
            jobState->setJobSubChannel(info->channel);
            jobState->setRemark(info->remark);
            jobState->setStopType((JobState::enTaskStopReason)info->stopType);
            jobState->setJobOperable(info->operable);
            jobState->setRenderTiles(info->tiles);
            jobState->setCoordCount(info->ctrlPointCount);
            jobState->setPicCount(info->picCount);
            jobState->setProjectType(info->jobType);
            jobState->setProjectOutputType(info->outputType);
            jobState->setProgressPercent(info->progress);
            jobState->setMunuTaskId(info->munuTaskId);
            jobState->setTotalPixel(info->totalPixel);
            // jobState->setDiffOfRenderConsume(info->differenceOfRenderConsume);

            //jobState->SetTag(QString::fromLocal8Bit(info->tag.c_str()));
            //jobState->setDependentTask(QString::fromLocal8Bit(info->sonId.c_str()));
            //jobState->setDependency(info->dependency);
            //jobState->setRenderTiles(QString::fromLocal8Bit(info->tiles.c_str()));

            if(info->isOpen && info->respRenderingTaskList.count()) {
                foreach(MaxTaskItem* child, info->respRenderingTaskList) {
                    jobState->addChildJobState(addTaskChildJobState(child, jobState));
                }
            }
            jobState->setIsOpen(info->isOpen == 1 ? true : false);
            jobState->setStatus(info->taskStatus);
            jobState->updateTaskType();
            // jobState->setStatus(info->statusText);
            jobState->setRenderProgress(info->executingFrames, info->doneFrames, info->failedFrames, info->abortFrames, info->totalFrames);
            preTaskId = info->id;
        }
    }

    if (!tasklist->isRepeat) {// 非循环请求才刷新界面
        JobStateMgr::Inst()->SetClean(true);
        JobStateMgr::Inst()->TaskListEventRet(JobStateMgr::Inst()->m_dwCurPage);
    }
}

JobState* HttpCmdResponse::addTaskChildJobState(const MaxTaskItem* child, JobState* parentItem)
{
    if(child == nullptr)
        return nullptr;

    JobState *jobState = nullptr;
#if ANIMATION
    jobState = JobStateMgr::Inst()->LookupTaskstate(child->id);
    if (jobState == nullptr) {
        jobState = new JobState(child->id, parentItem);
        JobStateMgr::Inst()->AddTaskstate(jobState);
    } else {
        jobState->setParentItem(parentItem);
    }
    if (parentItem != NULL) jobState->setParentItem(parentItem); // 供下载页面使用
#else
    jobState = JobStateMgr::Inst()->LookupTaskstate(child->id);
    if (jobState == nullptr) {
        jobState = new JobState(child->id, parentItem);
        JobStateMgr::Inst()->AddTaskstate(jobState);
    } 
#endif
      
    jobState->setJobType(RENDER_JOB);
    jobState->setSubJobUserId(child->userId);
    jobState->setTaskLevel(child->taskLevel);
    jobState->setTaskUserLevel(child->taskUserLevel);
    jobState->setIsDelete(child->isDelete == 1 ? true : false);
    jobState->setShareMainCapital(child->shareMainCapital);
    jobState->setCgId(child->cgId);
    jobState->setCloneOriginalId(child->cloneOriginalId);
    jobState->setCloneParentId(child->cloneParentId);
    jobState->setLayerParentId(child->layerParentId);

    jobState->setScene(child->sceneName);
    jobState->setProjectName(child->projectName);
    jobState->setLayerName(child->layerName);
    jobState->setCamera(child->renderCamera);
    jobState->setOutputLabel(child->outputFileName);
    jobState->setArtistName(child->producer);
    jobState->setAutoDownPath(child->locationOutput);
    // 自动下载路径由本地设置路径决定
    //jobState->setAutoDownPath(ProjectMgr::getInstance()->getProjectOutput(jobState->GetProjectName())); //todo
    jobState->setJobSubmitter(child->userName);

    jobState->setTaskIdAlias(child->taskAlias);
    jobState->setPreTaskStatus(child->preTaskStatus);
    jobState->setPreTaskStatusText(child->preStatusText);
    jobState->setTaskType(child->taskType);
    jobState->setMemorySize(child->taskRam);
    jobState->setTaskLimit(child->taskLimit);

    jobState->setFrameScope(child->framesRange);
    jobState->setStartDate(child->startTime);
    jobState->setCompelteDate(child->completedDate);
    jobState->setSubmitDate(child->submitDate);
    jobState->setRenderTimeCost(child->renderDuration);
    jobState->setOverTimeRemind(child->taskOverTime);
    jobState->setIsOverTime(child->isOverTime == 1 ? true : false);
    jobState->setOverTimeStop(child->overTimeStop);
    //jobState->setFeemap(QString::number(child->renderConsume));
    jobState->setTaskRenderConsume(child->renderConsume);
    jobState->setUserAccountConsume(child->userAccountConsume);
    jobState->setCouponConsume(child->couponConsume);
    jobState->setQYCouponConsume(child->qyCouponConsume);
    jobState->setTaskArrears(child->taskArrears);
    jobState->setTaskRenderConsume(child->renderConsume);
    jobState->setOutputBid(MyConfig.storageSet.downloadBid);
    jobState->setJobSubChannel(child->channel);
    jobState->setRemark(child->remark);
    jobState->setStopType((JobState::enTaskStopReason)child->stopType);
    jobState->setJobOperable(child->operable);
    jobState->setRenderTiles(child->tiles);
    jobState->setCoordCount(child->ctrlPointCount);
    jobState->setPicCount(child->picCount);
    jobState->setProjectType(child->jobType);
    jobState->setProjectOutputType(child->outputType);
    jobState->setProgressPercent(child->progress);
    jobState->setMunuTaskId(child->munuTaskId);
    jobState->setTotalPixel(child->totalPixel);
    if (parentItem != NULL) jobState->setTaskParentId(parentItem->getTaskId());

    if(child->isOpen && child->respRenderingTaskList.count()) {
        foreach(MaxTaskItem* chld, child->respRenderingTaskList) {
            jobState->addChildJobState(addTaskChildJobState(chld, jobState));
        }
    }

    jobState->setIsOpen(child->isOpen == 1 ? true : false);
    jobState->setStatus(child->taskStatus);
    jobState->updateTaskType();
    // jobState->setStatusText(child->statusText); //todo
    jobState->setRenderProgress(child->executingFrames, child->doneFrames, child->failedFrames, child->abortFrames, child->totalFrames);

    return jobState;
}

void HttpCmdResponse::taskOperatorResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<TaskOperateResponse> operate = qSharedPointerCast<TaskOperateResponse>(data);
    if(operate->code == HTTP_ERROR_SUCCESS) {
        if(operate->cmdType == RESTAPI_JOB_TASK_DELETE) {
            foreach(qint64 taskId, operate->taskIds) {
                JobStateMgr::Inst()->DeleteTaskRet(taskId);
            }
        } else {
            JobStateMgr::Inst()->TaskOperatorRet();
        }
    } else {
        if(operate->code == HTTP_ERROR_USER_BALANCE_NOT_ENOUGH) {
            CGlobalHelp::Response(UM_MESSAGE_FLOAT, MSGBOX_TYPE_START_TASK_FAILED, 0);
        } else if(operate->code == HTTP_ERROR_DO_NOT_HAVE_ANY_MORE_RECORD) {
            CGlobalHelp::Response(UM_MESSAGE_FLOAT, HTTP_ERROR_DO_NOT_HAVE_ANY_MORE_RECORD, 0);
        } else if(operate->code == HTTP_ERROE_RENDER_WAIT_TASK_COMPLETE) {
            CGlobalHelp::Response(UM_MESSAGE_FLOAT, HTTP_ERROE_RENDER_WAIT_TASK_COMPLETE, 0);
        } else if(operate->code == HTTP_ERROR_FORBIDDEN) {
            CGlobalHelp::Response(UM_MESSAGE_FLOAT, MSGBOX_TYPE_DELETETASK_FORBIDDEN, 0);
        } else if (operate->code == HTTP_ERROR_STOP_TASK_ALL_COPY_PICTURE) {
            CGlobalHelp::Response(UM_MESSAGE_FLOAT, MSGBOX_TYPE_STOP_TASK_ALL_COPY_PICTURE, 0);
        } else if (operate->code == HTTP_ERROR_VINSUFFICIENT_PERMISSIONS) {
            CGlobalHelp::Response(UM_MESSAGE_FLOAT, MSGBOX_TYPE_VINSUFFICIENT_PERMISSIONS, 0);
        } else {
            MSGBOXPARAM *param = new MSGBOXPARAM;
            param->content = operate->msg;
            CGlobalHelp::Response(UM_MESSAGE_FLOAT, HTTP_ERROR_UNKNOWN, (intptr_t)(param));
        }
    }
}

void HttpCmdResponse::taskResubFailFrameResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<FailFrameResubOperateResponse> resub = qSharedPointerCast<FailFrameResubOperateResponse>(data);
    if(resub->code == HTTP_ERROR_SUCCESS) {
        JobStateMgr::Inst()->TaskOperatorRet();
        // 重提后任务状态有延时，需要缓一缓再刷新
        QTimer::singleShot(5000, this, [resub]() {
            JobStateMgr::Inst()->TaskOperatorRet();   
        });
        // 界面通知
        if (resub->resubStatus.contains(enReSubFinishedTask) && !resub->taskIds.isEmpty()) {
            qint64 taskId = resub->taskIds.at(0);
            MSGBOXPARAM *param = new MSGBOXPARAM;
            CGlobalHelp::Response(UM_RESUBMIT_TASK_FREAME_RET, taskId, (intptr_t)param);
        }

    } else {
        if(resub->code == HTTP_ERROR_USER_BALANCE_NOT_ENOUGH) {
            CGlobalHelp::Response(UM_MESSAGE_FLOAT, MSGBOX_TYPE_START_TASK_FAILED, 0);
        } else if(resub->code == HTTP_ERROR_DO_NOT_HAVE_ANY_MORE_RECORD) {
            CGlobalHelp::Response(UM_MESSAGE_FLOAT, HTTP_ERROR_DO_NOT_HAVE_ANY_MORE_RECORD, 0);
        } else if(resub->code == HTTP_ERROE_RENDER_WAIT_TASK_COMPLETE) {
            CGlobalHelp::Response(UM_MESSAGE_FLOAT, HTTP_ERROE_RENDER_WAIT_TASK_COMPLETE, 0);
        } else {
            MSGBOXPARAM *param = new MSGBOXPARAM;
            param->content = resub->msg;
            CGlobalHelp::Response(UM_MESSAGE_FLOAT, HTTP_ERROR_UNKNOWN, (intptr_t)param);
        }
    }
}

void HttpCmdResponse::createJobIdsResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<JobCreateResponse> jobs = qSharedPointerCast<JobCreateResponse>(data);
    if(jobs->code == HTTP_ERROR_SUCCESS) {
#if 0
        std::stringstream ss;
        if(jobs->taskids.isEmpty()) {
            ss << "-1" << ",";
        } else {
            matchTaskIdAlias(jobs->taskids, jobs->taskIdAlias);

            for(int i = 0; i < jobs->taskIdAlias.count(); i++) {
                ss << jobs->taskIdAlias[i].toStdString() << ",";
            }
        }

        std::string ids = ss.str();
        ids.erase(ids.find_last_not_of(',') + 1);

        if(jobs->createType == REND_CMD_TASK_IDS) {
            sendTaskIdsToRenderCmd(jobs->sid, jobs->commandServer, ids);
        }
#endif
    }
}

void HttpCmdResponse::taskSubmitResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<TaskSubmitResponse> subtask = qSharedPointerCast<TaskSubmitResponse>(data);

    MSGBOXPARAM *param = new MSGBOXPARAM;
    param->code    = data->code;
    param->content = data->msg;
    param->action  = subtask->option;

    JobStateMgr::Inst()->SubmitEventRet(subtask->taskId, (intptr_t)param);
}

void HttpCmdResponse::totalTaskFrameResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<TotalTaskFrameInfoResponse> total = qSharedPointerCast<TotalTaskFrameInfoResponse>(data);
    if(total->code == HTTP_ERROR_SUCCESS) {
        MyConfig.renderStatusSet.fs.waiting = total->waitingFramesTotal;
        MyConfig.renderStatusSet.fs.execute = total->executingFramesTotal;
        MyConfig.renderStatusSet.fs.done    = total->doneFramesTotal;
        MyConfig.renderStatusSet.fs.failed  = total->failedFramesTotal;
        MyConfig.renderStatusSet.fs.total   = total->totalFrames;
        MyConfig.renderStatusSet.fs.abandon = total->totalFrames - (total->waitingFramesTotal + total->executingFramesTotal + total->doneFramesTotal + total->failedFramesTotal);
        MyConfig.renderStatusSet.fs.abandon = MyConfig.renderStatusSet.fs.abandon < 0 ? 0 : MyConfig.renderStatusSet.fs.abandon;
    }
}

void HttpCmdResponse::getTaskStatusCountResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<TaskSummaryResponse> summary = qSharedPointerCast<TaskSummaryResponse>(data);
    if(summary->code == HTTP_ERROR_SUCCESS) {
        MyConfig.renderStatusSet.ts.waiting = summary->waitingTask;
        MyConfig.renderStatusSet.ts.execute = summary->renderingTask;
        MyConfig.renderStatusSet.ts.done    = summary->doneTask;
        MyConfig.renderStatusSet.ts.failed  = summary->failureTask;
        MyConfig.renderStatusSet.ts.total   = summary->waitingTask + summary->renderingTask + summary->doneTask + summary->failureTask;
    }
}

void HttpCmdResponse::taskFrameRenderingInfoResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<TaskFrameRenderingInfoResponse> frameInfo = qSharedPointerCast<TaskFrameRenderingInfoResponse>(data);
    if(frameInfo->code == HTTP_ERROR_SUCCESS) {
        JobState* jobState = JobStateMgr::Inst()->LookupTaskstate(frameInfo->taskId);
        if(jobState == nullptr)
            return;

        // 如果是第一页则清空数据
        if(frameInfo->pageNum == 1) {
            jobState->m_frameDetails.detailItemList.clear();
        }
        jobState->m_frameDetails.pageCount = frameInfo->pageCount;
        jobState->m_frameDetails.pageNum   = frameInfo->pageNum;
        jobState->m_frameDetails.size      = frameInfo->size;
        jobState->m_frameDetails.taskId    = frameInfo->taskId;
        jobState->m_frameDetails.total     = frameInfo->total;
        jobState->m_frameDetails.searchKey = frameInfo->searchKey;
        jobState->m_frameDetails.stateList = frameInfo->stateList;
        foreach(JobState::DetailInfo* detail, frameInfo->detailItems) {
            /*auto it = job->m_frameDetails.detailItems.find(detail->id);
            JobState::DetailInfo* info = nullptr;
            if(it == job->m_frameDetails.detailItems.end()) {
                info = new JobState::DetailInfo;
            } else {
                info = job->m_frameDetails.detailItems.value(detail->id);
            }*/
            JobState::DetailInfo* info = new JobState::DetailInfo;

            info->id                = detail->id;
            info->taskId            = detail->taskId;
            info->userId            = detail->userId;

            info->arrearsFee        = detail->arrearsFee;
            info->couponFee         = detail->couponFee;
            info->feeAmount         = detail->feeAmount;
            info->feeType           = detail->feeType;

            info->frameBlock        = detail->frameBlock;
            info->frameExecuteTime  = detail->frameExecuteTime;
            info->frameIndex        = detail->frameIndex;
            info->framePrice        = detail->framePrice;
            info->frameRam          = detail->frameRam;
            info->frameRamRender    = detail->frameRamRender;
            info->frameStatus       = detail->frameStatus;
            info->frameStatusText   = detail->frameStatusText;
            info->frameType         = detail->frameType;

            info->gopName           = detail->gopName;
            info->isCopy            = detail->isCopy;
            info->munuJobId         = detail->munuJobId;
            info->munuTaskId        = detail->munuTaskId;
            info->recommitFlag      = detail->recommitFlag;

            info->startTime         = detail->startTime;
            info->endTime           = detail->endTime;
            info->averageCpu        = detail->averageCpu;
            info->averageMemory     = detail->averageMemory;
            info->taskOverTime      = detail->taskOverTime;
            info->serverTime        = frameInfo->serverTime;
            info->isOverTime        = detail->isOverTime;
            info->frameProgress     = detail->frameProgress;

            //job->m_frameDetails.detailItems[info->id] = info;
            jobState->m_frameDetails.detailItemList.append(info);
        }
        JobStateMgr::Inst()->DetailInfoEventRet(frameInfo->taskId, enDetailsUi);
    }
}

void HttpCmdResponse::taskFrameRenderingConsumeResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<JobConsumeResponse> consumeInfo = qSharedPointerCast<JobConsumeResponse>(data);
    if(consumeInfo->code == HTTP_ERROR_SUCCESS) {
        JobState* jobState = JobStateMgr::Inst()->LookupTaskstate(consumeInfo->taskId);
        if(jobState == nullptr)
            return;

        JobState::TaskFrameConsume jobConsume;
        jobConsume.couponConsume         = consumeInfo->couponConsume;
        jobConsume.userAccountConsume    = consumeInfo->userAccountConsume;
        jobConsume.frameTimeConsumingAve = consumeInfo->frameTimeConsumingAve;
        jobConsume.taskArrearage         = consumeInfo->taskArrearage;
        jobConsume.taskTimeConsuming     = consumeInfo->taskTimeConsuming;
        jobConsume.totalFrames           = consumeInfo->totalFrames;
        jobState->m_frameDetails.jobConsume = jobConsume;

        JobStateMgr::Inst()->DetailInfoEventRet(consumeInfo->taskId, enTaskConsume);
    }
}

void HttpCmdResponse::loadinTaskParamterResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<TaskParamterResponse> params = qSharedPointerCast<TaskParamterResponse>(data);
    if(params->code == HTTP_ERROR_SUCCESS) {
        JobState* jobState = JobStateMgr::Inst()->LookupTaskstate(params->taskId);
        if(jobState == nullptr)
            return;

        jobState->setScenePath(params->cgFilePath);
        if(!params->taskParam.isEmpty()) {
            QStringList pList;
            QString key = "";
            foreach(QString pstr, params->taskParam) {
                /*QString valueStr = "";
                pList = pstr.split("|");
                if(pList.count() == 2) {
                    key = LangTranslator::RequstErrorI18n(pList.at(0));
                    if(key.isEmpty()) continue;
                    valueStr += key;
                    valueStr += "\t";
                    valueStr += pList.at(1);
                }*/
                jobState->m_taskParam.append(pstr);
            }
            JobStateMgr::Inst()->DetailInfoEventRet(params->taskId, enTaskParamters);
        }
    }
}

void HttpCmdResponse::loadTaskThumbnailResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<TaskThumbnailResponse> params = qSharedPointerCast<TaskThumbnailResponse>(data);
    CSmallpicMgr::Inst()->clearAllContainer(params->frameId);
    if(params->code == HTTP_ERROR_SUCCESS) {
        if(params->thumbnails.isEmpty()) {
            CSmallpicMgr::Inst()->AddUrlPath(params->frameId, ":/images/preview.png", false);
        } else {
            foreach(QString url, params->thumbnails) {
                QString path = CConfigDb::Inst()->getHost();
                path = QString("%1/%2").arg(path).arg(url);
                CSmallpicMgr::Inst()->AddUrlPath(params->frameId, path, true);
            }
        }
    } else {
        CSmallpicMgr::Inst()->AddUrlPath(params->frameId, ":/images/preview.png", false);
    }
}

void HttpCmdResponse::loadFrameRenderLogResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<TaskRenderLogResponse> logs = qSharedPointerCast<TaskRenderLogResponse>(data);
    if(logs->code == HTTP_ERROR_SUCCESS) {

    }
}

void HttpCmdResponse::projectListResponse(QSharedPointer<ResponseHead> data)
{
    // todo
    /*QSharedPointer<ProjectListResponse> projectList = qSharedPointerCast<ProjectListResponse>(data);
    if(projectList->code == HTTP_ERROR_SUCCESS) {
        if(projectList->optType == QUERY_PROJECT) {
            ProjectMgr::getInstance()->clear();
            ProjectMgr::getInstance()->isInitialized = true;
            foreach (auto project, projectList->projectList) {
                ProjectMgr::getInstance()->addProject(project->projectId, project->projectName);
            }
            CGlobalHelp::Response(UM_UPDATE_PROJECT_LIST, 0, 0);
        }
    }*/
}
void HttpCmdResponse::pluginConfigResponse(QSharedPointer<ResponseHead> data)
{
    // todo
    /*QSharedPointer<PluginConfigResponse> pluginList = qSharedPointerCast<PluginConfigResponse>(data);
    if(pluginList->code == HTTP_ERROR_SUCCESS) {
        foreach(qint64 cgId, pluginList->cgIds) {
            PluginMgr::getInstance()->clearSoftConfig(cgId);
        }
        PluginMgr::getInstance()->isInitialized = true;
        foreach (auto plugin, pluginList->pluginConfigs) {
            PluginMgr::getInstance()->addPluginConfig(*plugin);
        }
        CGlobalHelp::Response(UM_UPDATE_PLUGINLIST, 0, 0);
    }*/
}

void HttpCmdResponse::taskFrameOperatorResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<TaskFrameOperateResponse> opt = qSharedPointerCast<TaskFrameOperateResponse>(data);
    if(opt->code == HTTP_ERROR_SUCCESS) {
        JobStateMgr::Inst()->DetailInfoEventRet(opt->taskId, enDetailOperate);
    } else {
        if(opt->code == HTTP_ERROR_USER_BALANCE_NOT_ENOUGH) {
            CGlobalHelp::Response(UM_MESSAGE_FLOAT, MSGBOX_TYPE_START_TASK_FAILED, 0);
        } else if(opt->code == HTTP_ERROR_DO_NOT_HAVE_ANY_MORE_RECORD) {
            CGlobalHelp::Response(UM_MESSAGE_FLOAT, HTTP_ERROR_DO_NOT_HAVE_ANY_MORE_RECORD, 0);
        } else if(opt->code == HTTP_ERROE_RENDER_WAIT_TASK_COMPLETE) {
            CGlobalHelp::Response(UM_MESSAGE_FLOAT, HTTP_ERROE_RENDER_WAIT_TASK_COMPLETE, 0);
        } else {
            MSGBOXPARAM *param = new MSGBOXPARAM;
            param->content = opt->msg;
            CGlobalHelp::Response(UM_MESSAGE_FLOAT, HTTP_ERROR_UNKNOWN, (intptr_t)(param));
        }
    }
}

void HttpCmdResponse::loadUserNameResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<LoadUserNameResponse> subAccounts = qSharedPointerCast<LoadUserNameResponse>(data);
    if(subAccounts->code == HTTP_ERROR_SUCCESS) {
        JobStateMgr::Inst()->clearSubAccountNameList();
        foreach(UserName* subaccount, subAccounts->userNames) {
            UserName* name = JobStateMgr::Inst()->getSubAccountName(subaccount->userId);
            if(name == nullptr) {
                name = new UserName;
            }
            name->userId   = subaccount->userId;
            name->userName = subaccount->userName;
            JobStateMgr::Inst()->addSubAccountName(name);
        }
        CGlobalHelp::Response(UM_UPDATE_USERNAME_LIST, 0, 0);
    }
}

void HttpCmdResponse::errorListResponse(QSharedPointer<ResponseHead> data)
{
    // todo
    /*QSharedPointer<ErrorListResponse> errorList = qSharedPointerCast<ErrorListResponse>(data);
    if(errorList->code == HTTP_ERROR_SUCCESS) {
        CBatchStateMgr::Inst()->clearErrorCode();
        CBatchStateMgr::Inst()->isInitialized = true;
        foreach(const ErrorCodeItem& error, errorList->errorCodes) {
            CBatchStateMgr::Inst()->addErrorCode(error);
        }
    }*/
}

void HttpCmdResponse::advancedParamSettingResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<AdvancedParamSetResponse> advancedParamSetting = qSharedPointerCast<AdvancedParamSetResponse>(data);
    if(advancedParamSetting->code == HTTP_ERROR_SUCCESS) {
        // 全部以秒计算
        MyConfig.paramSet.overtime.defaultTimeOut = advancedParamSetting->timeout.defaultSecond;
        MyConfig.paramSet.overtime.maxTimeOut = advancedParamSetting->timeout.max;
        MyConfig.paramSet.overtime.minTimeOut = advancedParamSetting->timeout.min;
        // 超时停止
        MyConfig.paramSet.timeoutstop.minTimeoutStop = advancedParamSetting->timeoutstop.min;
        MyConfig.paramSet.timeoutstop.maxTimeoutStop = advancedParamSetting->timeoutstop.max;
        MyConfig.paramSet.timeoutstop.defaultTimeoutStop = advancedParamSetting->timeoutstop.defaultSecond;
        // 一机多帧
        MyConfig.paramSet.rendernum.minRenderNum = advancedParamSetting->rendernum.minRenderNum;
        MyConfig.paramSet.rendernum.maxRenderNum = advancedParamSetting->rendernum.maxRenderNum;
        MyConfig.paramSet.rendernum.defaultRenderNum = advancedParamSetting->rendernum.defaultRenderNum;
        // 分块
        MyConfig.paramSet.renderblocks.maxBlocks = advancedParamSetting->renderblocks.maxBlocks;
        MyConfig.paramSet.renderblocks.defaultBlocks = advancedParamSetting->renderblocks.defaultBlocks;
        // ram
        MyConfig.paramSet.rams.clear();
        foreach (auto & r, advancedParamSetting->rams) {
            ParamSet::Ram ram;
            ram.isDefault = r.isDefault;
            ram.name      = r.name;
            ram.rate      = r.rate;
            ram.value     = r.value;
            MyConfig.paramSet.rams.append(ram);
        }
        // gpu
        MyConfig.paramSet.gpus.clear();
        foreach(auto & g, advancedParamSetting->gpus) {
            ParamSet::Gpu gpu;
            gpu.isDefault = g.isDefault;
            gpu.name = g.name;
            gpu.rate = g.rate;
            gpu.value = g.value;
            MyConfig.paramSet.gpus.append(gpu);
        }
    }
}

QString getParentOutLabel(const MaxTaskItem* info)
{
    if (info == NULL || info->layerParentId <= 0)
        return "";

    qint64 pId = info->layerParentId;
    QString sence = QFileInfo(info->sceneName).baseName();

    QString output = QString("%1_%2").arg(pId).arg(sence);
    return output;
}

void HttpCmdResponse::queryTaskInfoResponse(QSharedPointer<ResponseHead> data)
{
    return;
}

JobState* HttpCmdResponse::addTaskChildDownloadState(MaxTaskItem* child, JobState* parentItem)
{
    if(child == nullptr)
        return nullptr;

    JobState *downloadState = nullptr;
    downloadState = JobStateMgr::Inst()->LookupTaskstate(child->id);
    if(downloadState == nullptr) {
        downloadState = new JobState(child->id, parentItem);
        downloadState->setJobType(DOWNLOAD_JOB);
#if DOWNLOAD_PAGE_TREE_STRUCT
        CDownloadStateMgr::Inst()->AddDownloadState(downloadState, 1);
#endif
    }

    downloadState->setSubJobUserId(child->userId);
    downloadState->setScene(child->sceneName);
    downloadState->setLayerName(child->layerName);
    downloadState->setArtistName(child->producer);
    downloadState->setScene(child->sceneName);
    downloadState->setProjectName(child->projectName);
    downloadState->setOutputLabel(child->outputFileName);
    downloadState->setRenderProgress(child->executingFrames, child->doneFrames, child->failedFrames, child->abortFrames, child->totalFrames);
    downloadState->setIsDelete(child->isDelete);
    downloadState->setIsOpen(child->isOpen);
    downloadState->setTaskIdAlias(child->taskAlias);
    downloadState->setStatus(child->taskStatus);
    downloadState->setOutputBid(MyConfig.storageSet.downloadBid);
    downloadState->setTaskType(child->taskType);
    downloadState->setStartDate(child->startTime);
    downloadState->setCompelteDate(child->completedDate);
    //downloadState->setParentOutputLabel(getParentOutLabel(child)); //toto
    //downloadState->setParentTaskId(child->layerParentId);
    // downloadState->SetLocalSavePath(child->locationOutput);
    // 未设置路径时，设置路径
    // todo
    /*if (downloadState->getLocalSavePath().isEmpty()) {
        downloadState->setLocalSavePath(ProjectMgr::getInstance()->getProjectOutput(downloadState->getProject()));
    }

    if(child->isOpen && child->respRenderingTaskList.count()) {
        foreach(MaxTaskItem* chld, child->respRenderingTaskList) {
            downloadState->addChildJobState(addTaskChildJobState(chld, downloadState));
        }
    }
    downloadState->SetStatus(child->taskStatus);*/

    return downloadState;
}

void HttpCmdResponse::updateTaskRemarkResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<UpdateTaskRemarkResponse> opt = qSharedPointerCast<UpdateTaskRemarkResponse>(data);
    if(opt->code == HTTP_ERROR_SUCCESS) {

    } else {

    }
}

void HttpCmdResponse::updateTaskLevelResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<UpdateTaskLevelResponse> opt = qSharedPointerCast<UpdateTaskLevelResponse>(data);
    if(opt->code == HTTP_ERROR_SUCCESS) {

    } else {

    }
}

void HttpCmdResponse::updateTaskOvertimeResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<UpdateTaskOvertimeResponse> opt = qSharedPointerCast<UpdateTaskOvertimeResponse>(data);
    if(opt->code == HTTP_ERROR_SUCCESS) {

    } else {

    }
}

void HttpCmdResponse::updateTaskOvertimeStopResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<UpdateTaskOvertimeResponse> opt = qSharedPointerCast<UpdateTaskOvertimeResponse>(data);
    if (opt->code == HTTP_ERROR_SUCCESS) {

    }
}

void HttpCmdResponse::queryJobProducerListResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<LoadProducerResponse> opt = qSharedPointerCast<LoadProducerResponse>(data);
    if(opt->code == HTTP_ERROR_SUCCESS) {
        JobStateMgr::Inst()->setProducerList(opt->producers);
        CGlobalHelp::Response(UM_UPDATE_USERNAME_LIST, 0, 0);
    }
}

void HttpCmdResponse::downloadStartResponse(QSharedPointer<ResponseHead> data)
{
    // todo
    QSharedPointer<DownloadStartResponse> downloadResp = qSharedPointerCast<DownloadStartResponse>(data);
    if (downloadResp->code == HTTP_ERROR_SUCCESS) {
        // CDownloadStateMgr::Inst()->setDownloadRequestId(downloadResp->taskId, downloadResp->downloadRequestId);
    }
}

void HttpCmdResponse::downloadCountResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<DownloadCountResponse> downloadResp = qSharedPointerCast<DownloadCountResponse>(data);
    if (downloadResp->code == HTTP_ERROR_SUCCESS) {
        QList<DownloadCountItem> items = downloadResp->items;
        foreach (auto item, items) {
            JobState* jobState = JobStateMgr::Inst()->LookupTaskstate(item.taskId);
            if (jobState) {
                jobState->setDownloadCount(item.downloadCount);
                // jobState->setDownloadStatus(item.downloading); todo
            }
        }       
    }
}

void HttpCmdResponse::platformStatusResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<PlatformStatusResponse> opt = qSharedPointerCast<PlatformStatusResponse>(data);
    if (opt->code == HTTP_ERROR_SUCCESS) {
        if (opt->platforms.isEmpty()) return;
        foreach (PlatformStatus*status, opt->platforms){
            if(status->platform == MyConfig.client.platform){
                MyConfig.client.platformStatus = status->status;
                break;
            }
        }
        CGlobalHelp::Response(UM_UPDATE_PLATFORM_STATUS, MyConfig.client.platformStatus, 0);

    } else {

    }
}

void HttpCmdResponse::reportLogResponse(QSharedPointer<ResponseHead> data)
{
}

void HttpCmdResponse::loadingAccountsProjNamesResponse(QSharedPointer<ResponseHead> data)
{
    // todo
    /*QSharedPointer<ProjectListResponse> projectList = qSharedPointerCast<ProjectListResponse>(data);
    if (projectList->code == HTTP_ERROR_SUCCESS) {
        ProjectMgr::getInstance()->allAccountProjNames.clear();
        foreach(auto project, projectList->projectList) {
            ProjectMgr::getInstance()->addProject(project);
        }
        CGlobalHelp::Response(UM_LOADING_PROJECT_NAME, 0, 0);
    }*/
}

void HttpCmdResponse::updateTaskLimitResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<UpdateTaskLimitResponse> opt = qSharedPointerCast<UpdateTaskLimitResponse>(data);
    if (opt->code == HTTP_ERROR_SUCCESS) {
        LOGFMTI("[HttpCmdResponse] upate task %lld limit[%s] success", opt->taskId, qPrintable(opt->limitCount));
    } else {
        LOGFMTE("[HttpCmdResponse] upate task %lld limit[%s] failed", opt->taskId, qPrintable(opt->limitCount));
    }
}

void HttpCmdResponse::getTaskOutputSizeResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<TaskOutputSizeResponse> output = qSharedPointerCast<TaskOutputSizeResponse>(data);
    switch (output->fromPage)
    {
    case PAGE_TASK:
        this->taskPageOutputSizeResponse(data);
        break;
    case PAGE_DOWNLOAD:
        this->downloadPageOutputSizeResponse(data);
        break;
    }
}

void HttpCmdResponse::taskPageOutputSizeResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<TaskOutputSizeResponse> output = qSharedPointerCast<TaskOutputSizeResponse>(data);
    if (output->code == HTTP_ERROR_SUCCESS) {
        QList<qint64> taskIds;
        auto it = output->tasksSize.begin();
        while (it != output->tasksSize.end()) {

            JobState *jobstate = nullptr;
            const TaskOutputSize taskSize = it.value();
            jobstate = JobStateMgr::Inst()->LookupTaskstate(it.key());

            if (jobstate != NULL) {
                jobstate->setTaskOutputSize(taskSize.outputSize);
                taskIds.append(it.key());
            }

            it++;
        }

    } else {
        LOGFMTE("[HttpCmdResponse] Task page get tasks output size failed!");
    }
}

void HttpCmdResponse::downloadPageOutputSizeResponse(QSharedPointer<ResponseHead> data)
{
    // todo
    /*QSharedPointer<TaskOutputSizeResponse> output = qSharedPointerCast<TaskOutputSizeResponse>(data);
    if (output->code == HTTP_ERROR_SUCCESS) {
        QList<qint64> taskIds;
        auto it = output->tasksSize.begin();
        while (it != output->tasksSize.end()){

            CDownloadState *downloadState = nullptr;
            const TaskOutputSize taskSize = it.value();
            downloadState = CDownloadStateMgr::Inst()->LookupTaskstate(it.key());

            if (downloadState != NULL) {
                downloadState->SetTaskOutputSize(taskSize.outputSize);
                taskIds.append(it.key());
            }

            it++;
        }

        // 刷新页面
        CDownloadStateMgr::Inst()->taskListEventRet(true);
        // 更新数据到数据库
        CDownloadStateMgr::Inst()->updateTaskOutputSize(taskIds);

    } else {
        LOGFMTE("[HttpCmdResponse] Download page get tasks output size failed!");
    }*/
}

void HttpCmdResponse::useVoucherResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<PaymentUseVoucherResponse> opt = qSharedPointerCast<PaymentUseVoucherResponse>(data);
    if (opt->code == HTTP_ERROR_SUCCESS) {
        MSGBOXPARAM *param = new MSGBOXPARAM;
        param->code = 1;
        param->content = QObject::tr("[%1]任务免单成功！").arg(opt->taskId);
        CGlobalHelp::Response(UM_MESSAGE_FLOAT, MSGBOX_TYPE_PAYMENT_USE_VOUCHER, (intptr_t)(param));

    } else if (opt->code == HTTP_ERROR_TASK_DEDUCT_ILLEGAL) {
        MSGBOXPARAM *param = new MSGBOXPARAM;
        param->code = 0;
        param->content = QObject::tr("[%1]任务不符合免单券使用要求！").arg(opt->taskId);
        CGlobalHelp::Response(UM_MESSAGE_FLOAT, MSGBOX_TYPE_PAYMENT_USE_VOUCHER, (intptr_t)(param));

    } else {
        MSGBOXPARAM *param = new MSGBOXPARAM;
        param->code = 0;
        param->content = QObject::tr("[%1]该作业不符合要求，具体免单券规则参考《效果图会员协议》！").arg(opt->taskId);
        CGlobalHelp::Response(UM_MESSAGE_FLOAT, MSGBOX_TYPE_PAYMENT_USE_VOUCHER, (intptr_t)(param));
    }

    // 每次执行抵扣，需要重新查询一次免单券
    HttpCmdManager::getInstance()->queryVoucher();
}

void HttpCmdResponse::queryVoucherResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<PaymentQueryVoucherResponse> opt = qSharedPointerCast<PaymentQueryVoucherResponse>(data);
    if (opt->code == HTTP_ERROR_SUCCESS) {
        MyConfig.voucherSet.effectiveVouchers = opt->effectiveVouchers;
        MyConfig.voucherSet.invalidVouchers   = opt->invalidVouchers;
    } else {
        LOGFMTE("[HttpCmdResponse] Query voucher failed!");
    }
}

void HttpCmdResponse::queryPicturePriceResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<PicturePriceResponse> opt = qSharedPointerCast<PicturePriceResponse>(data);
    if (opt->code == HTTP_ERROR_SUCCESS) {
        MyConfig.picPrice.commonRate = opt->commonRate;
        MyConfig.picPrice.memberRate = opt->memberRate;
        MyConfig.picPrice.maxCommonPrice = opt->maxCommonPrice;
        MyConfig.picPrice.maxMemberPrice = opt->maxMemberPrice;      
    } else {
        LOGFMTE("[HttpCmdResponse] Query picture price failed!");
    }
}

void HttpCmdResponse::taskResubmitCCATResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<TaskSubmitCCATResponse> subtask = qSharedPointerCast<TaskSubmitCCATResponse>(data);

    MSGBOXPARAM *param = new MSGBOXPARAM;
    param->code = data->code;
    param->content = data->msg;

    JobStateMgr::Inst()->SubmitEventRet(subtask->taskId, (intptr_t)param);
}

void HttpCmdResponse::queryShareResultLinkResponse(QSharedPointer<ResponseHead> data)
{
}

void HttpCmdResponse::cloneTaskResponse(QSharedPointer<ResponseHead> data)
{
    QSharedPointer<TaskCloneResponse> subtask = qSharedPointerCast<TaskCloneResponse>(data);

    MSGBOXPARAM *param = new MSGBOXPARAM;
    param->code = data->code;
    param->content = data->msg;
    param->action = "clone";

    JobStateMgr::Inst()->SubmitEventRet(subtask->taskId, (intptr_t)param);
}

void HttpCmdResponse::addRequestCmd(HttpCmdRequest* cmd)
{
    m_request.append(cmd);
}

void HttpCmdResponse::detachRequestCmd(HttpCmdRequest* cmd)
{
    if(m_request.isEmpty())
        return;

    if(!m_request.removeOne(cmd)) {
        qDebug() << "[HttpCmdResponse] Detach RequestCmd failed";
    }
}

bool HttpCmdResponse::isReSubmitTask(qint64 taskid)
{
    if(m_request.isEmpty())
        return false;

    foreach(HttpCmdRequest* cmd, m_request) {
        if(cmd->getCmdType() == RESTAPI_JOB_SUBMIT) {
            HttpSubTaskCmd* subtask = qobject_cast<HttpSubTaskCmd*>(cmd);
            if(subtask && subtask->getTaskId() == taskid) {
                if(subtask->isTimeout(5))
                    return false;
                return true;
            }
        }
    }
    return false;
}

void HttpCmdResponse::matchTaskIdAlias(QList<qint64>& taskids, QList<QString>& taskAlias)
{
    if(taskids.isEmpty() || taskAlias.isEmpty())
        return;

    foreach(qint64 taskid, taskids) {
        foreach(QString alias, taskAlias) {
            if(alias.contains(QString::number(taskid))) {
                JobStateMgr::Inst()->insertTaskIdAndAlias(taskid, alias);
            }
        }
    }
}
