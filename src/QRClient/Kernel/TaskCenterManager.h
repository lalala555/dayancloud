/***********************************************************************
* Module:  TaskCenterManager.h
* Author:  hqr
* Created: 2018/11/20 10:44:10
* Modifier: hqr
* Modified: 2018/11/20 10:44:10
* Purpose: Declaration of the class
***********************************************************************/
#pragma once
#ifndef TASKCENTER_MANAGER_H
#define TASKCENTER_MANAGER_H

#include "Common/MainFrame.h"
#include "Upload/UploadQueue.h"
#include "Download/DownloadQueue.h"
#include "TaskElement.h"
#include "LoadFileThread.h"

class JobState;
class JobState;
class CDownloadState;

class CTaskCenterManager : public IQueueSink<msg_callback_info_t>, public IResolveSink, public TSingleton<CTaskCenterManager>
{
public:
    CTaskCenterManager(void);
    virtual ~CTaskCenterManager(void);
protected:
    virtual void on_queue_data(msg_callback_info_t* pData);
    virtual void on_resolve(const resolve_callback_info_t & callback_info);
public:
    bool init();
    bool beepOn();
    void beepOff();
    void addTransferMsg(msg_callback_info_t* callBack);
    // 加载未完成的工作,每次程序启动都会去加载xml配置中的任务信息
    TaskElementPtr loadExistTask(t_taskid jobId);
    TaskElementPtr addNewTask(t_taskid jobId);
    void uploadTask(t_uid userId, t_taskid jobId, bool isClone = false);
    void loadFileFinished(t_taskid jobId);

    bool dumpParam(QSharedPointer<RenderParamConfig> param, QString& argumentFile);
    //====================上传相关函数======================
    void runAllUploadHistoryTask();
    bool startUpload(t_taskid jobId, bool bAuto = true);
    bool stopUploadTask(t_taskid jobId);
    bool removeUploadTask(t_taskid jobId);
    bool loadUploadState(JobState *pUploadState, TaskElementPtr task);
    bool loadUploadInfo(UploadInfo & info, TaskElementPtr task);
    bool updateUploadState(JobState *pUploadState, const QString& strInfo, enTransformStatus status);
    void addUploadTransMsg(msg_callback_info_t* callBack);
    void handleUploadTransMsg(msg_callback_info_t* callBack);
    bool createUploadState(t_taskid taskid);
    JobState* buildUploadState(TaskElementPtr task);
    //=====================下载相关函数=====================
    bool manualStartDownload(t_taskid jobId, bool bAuto = true);// 人工下载
    bool manualStopDownload(t_taskid jobId, bool bUpdateState = true); // 人工取消下载, 是否更改下载状态
    void runAllAutoDownload(const QList<qint64>& lists);//开启已经请求到数据的自动下载任务（定时触发）
    bool removeDownload(t_taskid jobId);
    bool needDownload(int taskStatus, int downloadStatus);
    bool isLastDownload(int taskStatus, int downloadStatus);
    void resubmitTask(t_taskid jobId);
    void submitTaskRet(t_taskid jobId, int errorCode); // 提交后的回调通知
    bool startDownloadHistoryTask(t_taskid taskId);
	bool downloadTaskJson(t_taskid jobId, t_taskid parentJobId, t_uid userId); // 下载配置文件 20220607

    void addDownloadHistory(t_taskid jobId, int status, CDownloadState *pTaskState, int checkingstatus = 0);
    void addDownloadHistory(t_taskid jobId, int status, JobState *pTaskState, int checkingstatus = 0);
    void getDownloadHistory(std::map<int, DownloadHistoryInfo>& downloadList, int filterDate);
    void removeDownloadHistory(t_taskid jobId);

    void addUploadHistory(t_taskid jobId, int status, JobState *pTaskState);
    void getUploadHistory(std::map<int, UploadHistoryInfo>& uploadList);
    void removeUploadHistory(t_taskid jobId);
    void addDownloadTransMsg(msg_callback_info_t* callBack);
    void handleDownloadTransMsg(msg_callback_info_t* callBack);

    bool checkDownloadFinished(t_taskid jobId); // 检查任务是否下载完成

    // =====================分析相关函数=====================
    void updateTaskTable(int page, const QVector<qint64>& table);
    void startInsertTask(int page);
    void insertTaskEnd(int page);

    ///执行下载之后检查帧情况
    bool isAllUploadCompleted();
    bool isAllDownloadCompleted();

    QString getProjectPath(t_taskid jobId);//获取工程配置文件路径

    //设置任务的优先渲染
    TaskElementPtr findTaskElement(t_taskid jobId);
    void resetTaskPacketBid();
    bool changeTransEngine(const QString &engine);
    bool changeTransServer(const QString &server);
    AbstractTransEngine* currentTransEngine();

    void rebuildUploadState(t_taskid jobId);
    void removeTaskElementList(t_taskid jobId);

    void limitTransSpeed(int type, int limitSpeed);

    void downloadPathChanged(const wchar_t * pLocalPath);//下载路径变化

    // 文件校验成功后的返回
    void checkUploadFileCompelete(TaskElementPtr task);

    void restartUpload(t_taskid jobId);
    bool isSubmitJobDisable();
    // 传输相关
    void getTransHistory(std::map<int, TransHistoryInfo>& list);
    void getTransUploadHistory(std::map<int, TransHistoryInfo>& list);
    void getTransDownloadHistory(std::map<int, TransHistoryInfo>& list);
    void getDownloadHistory(std::map<int, DownloadHistoryInfo>& downloadList);
private:
    IQueue<msg_callback_info_t> m_msgQueue;
    UploadQueue m_uploadQueue;
    DownloadQueue m_downloadQueue;
    AbstractTransEngine* m_transEngine;
    QString m_currEngineName;
    QMap<t_taskid, TaskElementPtr> m_taskList;
    QVector<UploadInfo> m_waitingUploads;  //记录等待加载上传信息
    QSharedPointer<TaskCCWriter> m_taskWriter; // 使用taskInfoCC

	CloneQueue m_cloneQueue;   //20220607
private:
    void addTaskElement(TaskElementPtr task);

    void resetTaskState(TaskElementPtr task);
    void resetUploadState(TaskElementPtr task);

    void removeWaitUpload(t_taskid jobId);
    bool getWaitUploadIsExist(t_taskid jobId);

    bool isAutoDownloadTask(t_taskid jobId);      // 判断任务是否已经在下载队列中，如果在返回true
    bool updateDownloadState(t_taskid jobId, int status, const QString& promptMsg = "");
    bool updateUploadState(t_taskid jobId, int status, const QString& promptMsg = "");

    TaskElementPtr loadTaskFileList(const QString& filePath, t_taskid jobId);
    TaskElementPtr loadUncompleteFile(const QString& filePath, t_taskid jobId);
    TaskElementPtr createTaskElement(t_uid userId, t_taskid jobId);
    TaskElementPtr rebuildTask(t_taskid jobId, bool isClone = false);
    JobState* buildUploadState(JobState *jobState);
    CDownloadState* buildDownloadState(JobState *jobState, CDownloadState* parent = 0);

private:
    QString lefttimeDetail(quint64 size_transfered, quint64 size_total, float speed);
    void printLog(const char* szLog, int page = PAGE_ALL);
    void printTransLog(const transmit_callback_info_t& transInfo, int page);
    //===================上传回调==========================
    bool uploadMsgFilter(msg_callback_info_t *transferMsg);
    void uploadMsgCallback(msg_callback_info_t *transferMsg);
    void uploadEngineStartedCallback(msg_callback_info_t *transferMsg);
    void uploadEngineConnectingCallback(msg_callback_info_t *transferMsg);
    void uploadEngineConnectedCallback(msg_callback_info_t *transferMsg);
    void uploadEngineConnectFailedCallback(msg_callback_info_t *transferMsg);
    void uploadEngineBrokenCallback(msg_callback_info_t *transferMsg);
    void uploadFileStartCallback(msg_callback_info_t *transferMsg);
    void uploadFileProgressCallback(msg_callback_info_t *transferMsg);
    void uploadSameFileCallback(msg_callback_info_t *transferMsg);
    void uploadFileErrorCallback(msg_callback_info_t *transferMsg);
    void uploadFileCompleteCallback(msg_callback_info_t *transferMsg);
    void uploadCompleteCallback(msg_callback_info_t *transferMsg);

    //===================下载回调===========================
    bool downloadMsgFilter(msg_callback_info_t *transferMsg);
    void downloadMsgCallback(msg_callback_info_t *transferMsg);
    void downloadEngineStartedCallback(msg_callback_info_t *transferMsg);
    void downloadEngineConnectingCallback(msg_callback_info_t *transferMsg);
    void downloadEngineConnectedCallback(msg_callback_info_t *transferMsg);
    void downloadEngineStopCallback(msg_callback_info_t *transferMsg);
    void downloadEngineConnectFailedCallback(msg_callback_info_t *transferMsg);
    void downloadEngineBrokenCallback(msg_callback_info_t *transferMsg);
    void downloadFileStartCallback(msg_callback_info_t *transferMsg);
    void downloadFileProgressCallback(msg_callback_info_t *transferMsg);
    void downloadFileExceptionCallback(msg_callback_info_t *transferMsg);
    void downloadFileCompleteCallback(msg_callback_info_t *transferMsg);
    void downloadCompleteCallback(msg_callback_info_t *transferMsg);
    void downloadEngineHasfailureCallback(msg_callback_info_t *transferMsg);

	void downloadEngineCloneStopCallback(msg_callback_info_t *transferMsg);  //20220607
	void downloadEngineCloneBrokenCallback(msg_callback_info_t *transferMsg);//20220607
	void downloadEngineCloneDoneCallback(msg_callback_info_t *transferMsg);  //20220607

    //===================传输引擎会话解析回调=================
    void resolveCallback(msg_callback_info_t *transferMsg);
    void resolveTraceInfo(int error_type, int callback_type);
    void resolveConnectingCallback(msg_callback_info_t *transferMsg);
    void resolveConnectedCallback(msg_callback_info_t *transferMsg);
    void resolveConnectFailedCallback(msg_callback_info_t *transferMsg);
    void resolveDisconnectedCallback(msg_callback_info_t *transferMsg);
};

#endif // TASKCENTER_MANAGER_H
