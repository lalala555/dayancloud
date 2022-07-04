#ifndef HTTPCMDRESPONSE_H
#define HTTPCMDRESPONSE_H

#include <QObject>
#include "kernel/macro.h"

#define MAX_SUBMIT_TIMES 3

class  JobState;
class  ResponseHead;
struct MaxTaskItem;
class  HttpCmdRequest;
class  CDownloadState;

class HttpCmdResponse : public QObject
{
    Q_OBJECT

public:
    HttpCmdResponse(QObject *parent = 0);
    ~HttpCmdResponse();

    // 观察者模式
    virtual void attach(QObject* observer);
    virtual void detach(QObject* observer);
    virtual void notify(ResponseHead* data);

    void addRequestCmd(HttpCmdRequest* cmd);
    void detachRequestCmd(HttpCmdRequest* cmd);
    bool isReSubmitTask(qint64 taskid);

private slots:
    void onRecvResponse(QSharedPointer<ResponseHead> data);
    void onUploadResponse(QSharedPointer<ResponseHead> data);
signals:
    void sendRecvResponse(ResponseHead*);

private:
    void ssoUserLoginResponse(QSharedPointer<ResponseHead> data);
    void platformResponse(QSharedPointer<ResponseHead> data);
    void userLoginResponse(QSharedPointer<ResponseHead> data);
    void transEngineBidResponse(QSharedPointer<ResponseHead> data);
    void transEngineConfigResponse(QSharedPointer<ResponseHead> data);
    void userInfoResponse(QSharedPointer<ResponseHead> data);
    void userRenderSettingResponse(QSharedPointer<ResponseHead> data);
    void userBalanceResponse(QSharedPointer<ResponseHead> data);
    void taskListResponse(QSharedPointer<ResponseHead> data);
     //发布列表响应
	void publishListResponse(QSharedPointer<ResponseHead> data);
    JobState* addTaskChildJobState(const MaxTaskItem* child, JobState* parentItem = 0);
    void taskOperatorResponse(QSharedPointer<ResponseHead> data);
    void taskResubFailFrameResponse(QSharedPointer<ResponseHead> data);
    void createJobIdsResponse(QSharedPointer<ResponseHead> data);
    void taskSubmitResponse(QSharedPointer<ResponseHead> data);
    void httpResponseException(QSharedPointer<ResponseHead> data);
    void httpResponseSuccess(QSharedPointer<ResponseHead> data);
    void totalTaskFrameResponse(QSharedPointer<ResponseHead> data);
    void getTaskStatusCountResponse(QSharedPointer<ResponseHead> data);
    void matchTaskIdAlias(QList<qint64>& taskids, QList<QString>& taskAlias);
    void taskFrameRenderingInfoResponse(QSharedPointer<ResponseHead> data);
    void taskFrameRenderingConsumeResponse(QSharedPointer<ResponseHead> data);
    void loadinTaskParamterResponse(QSharedPointer<ResponseHead> data);
    void loadTaskThumbnailResponse(QSharedPointer<ResponseHead> data);
    void loadFrameRenderLogResponse(QSharedPointer<ResponseHead> data);
    void projectListResponse(QSharedPointer<ResponseHead> data);
    void pluginConfigResponse(QSharedPointer<ResponseHead> data);
    void taskFrameOperatorResponse(QSharedPointer<ResponseHead> data);
    void loadUserNameResponse(QSharedPointer<ResponseHead> data);
    void errorListResponse(QSharedPointer<ResponseHead> data);
    void advancedParamSettingResponse(QSharedPointer<ResponseHead> data);
    void queryTaskInfoResponse(QSharedPointer<ResponseHead> data);
    JobState* addTaskChildDownloadState(MaxTaskItem* child, JobState* parentItem = 0);
    void updateTaskRemarkResponse(QSharedPointer<ResponseHead> data);
    void updateTaskLevelResponse(QSharedPointer<ResponseHead> data);
    void updateTaskOvertimeResponse(QSharedPointer<ResponseHead> data); 
    void updateTaskOvertimeStopResponse(QSharedPointer<ResponseHead> data);
    void queryJobProducerListResponse(QSharedPointer<ResponseHead> data);
    void platformStatusResponse(QSharedPointer<ResponseHead> data);
    void downloadStartResponse(QSharedPointer<ResponseHead> data);
    void downloadCountResponse(QSharedPointer<ResponseHead> data);
    void reportLogResponse(QSharedPointer<ResponseHead> data);
    void loadingAccountsProjNamesResponse(QSharedPointer<ResponseHead> data);
    void updateTaskLimitResponse(QSharedPointer<ResponseHead> data);
    void getTaskOutputSizeResponse(QSharedPointer<ResponseHead> data);
    void taskPageOutputSizeResponse(QSharedPointer<ResponseHead> data);
    void downloadPageOutputSizeResponse(QSharedPointer<ResponseHead> data);
    void useVoucherResponse(QSharedPointer<ResponseHead> data);
    void queryVoucherResponse(QSharedPointer<ResponseHead> data);
    void queryPicturePriceResponse(QSharedPointer<ResponseHead> data);
    void taskResubmitCCATResponse(QSharedPointer<ResponseHead> data);
    void queryShareResultLinkResponse(QSharedPointer<ResponseHead> data);
    void cloneTaskResponse(QSharedPointer<ResponseHead> data);
private:
    QList<QObject*> m_observers;
    QList<HttpCmdRequest*> m_request;
};

#endif // HTTPCMDRESPONSE_H
