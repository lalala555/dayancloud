/***********************************************************************
* Module:  JobState.h
* Author:  hqr
* Created: 2017/12/25 15:37:48
* Modifier: hqr
* Modified: 2017/12/25 15:37:48
* Purpose: Declaration of the class
***********************************************************************/
#pragma once
#ifndef JOBSTATE_H_
#define JOBSTATE_H_

#include <QMutex>
#include <QMap>
#include "UserConfig.h"
#include "kernel/msg.h"
#include "kernel/macro.h"
#include "kernel/render_struct.h"
#include "Common/Common/SyncObject.h"

class JobState
{
public:
    explicit JobState(quint64 taskId, JobState* parentItem = 0);
    ~JobState();

    struct DetailInfo {
        enum {
            task_frame_status_wait = 1,                    // "等待中",
            task_frame_status_rendering,                   // "渲染中",
            task_frame_status_stop,                        // "停止",
            task_frame_status_completed,                   // "已完成",
            task_frame_status_failed,                      // "失败",
            task_frame_status_wait_pre_finished,           // "等待预处理完成",
            task_frame_status_wait_photon_frame_finished,  // "等待光子帧渲染完成",
            task_frame_status_wait_priority_finished,      // "等待优先渲染完成",
            task_frame_status_wait_photon_finished,        // "等待光子作业渲染完成",
            task_frame_status_wait_resolve_finished,       // "等待解算作业渲染完成",
            task_frame_status_overtime_stop,               // "超时停止"
            task_frame_status_vice_frame_wait,             // "副帧等待中"
        };

        enum FrameType {
            pre_frame = 1,              // 预渲染(只有一帧,多相机情况也只有一帧)
            photon_frame = 2,           // 光子帧
            photon_compose_frame,       // 合成光子帧
            priority_frame,             // 优先帧
            major_frame,                // 渲染主图帧
            priority_compose_frame,     // maya/max优先渲染合成帧
            major_compose_frame,        // maya/max渲染主图合成帧
            houdini_balance_frame,      // houdini 解算帧
            max_channel_frame,          // max 通道帧
            houdini_distributed_normal, // houdini分布式结算普调帧
            houdini_distributed_master  // houdini分布式主控帧
        };

        bool checked;

        qint32 id;
        qint32 taskId;
        qint32 userId;

        qreal arrearsFee;
        qreal couponFee;
        qreal feeAmount;
        QString feeType;
        QString framePrice;

        qint32 frameType;
        QString gopName;

        QString isCopy;
        QString munuJobId;
        QString munuTaskId;
        QString platform;
        bool recommitFlag;
        bool frameBlock;

        QString frameIndex;         // 帧任务
        qint32 frameStatus;         // 状态
        QString frameStatusText;
        qint32 frameExecuteTime;    // 运行时间
        qint64 startTime;           // 开始时间
        qint64 endTime;             // 结束时间
        qint32 averageCpu;          // CPU平均使用率
        qint64 averageMemory;       // 内存平均使用率
        qint32 frameRam;            // 扣费内存
        qint32 frameRamRender;      // 渲染内存
        qint64 taskOverTime;        //
        qint64 serverTime;          // 服务器时间
        qint32 isOverTime;          // 是否超时
        qint32 openRenderRam;       // 是否显示节点机内存列
        QString frameProgress;      // 帧进度

		QString kmlName; //分块空三单块名称
		QString pixel; //像素值
		QString picCount; //照片数
        bool isFrameOverTime()
        {
            return isOverTime == 1 ? true : false;
        }
        bool isFrameCanStart() // 帧任务能否开始
        {
            return  isDistributedFrame() == false &&
                    isComposeFrame() == false &&
                    (frameStatus == task_frame_status_stop ||
                    frameStatus == task_frame_status_overtime_stop);
        }
        bool isFrameCanStop() // 帧任务能否停止
        {
            return isDistributedFrame() == false &&
                   isComposeFrame() == false &&
                   (frameStatus == task_frame_status_rendering ||
                   frameStatus == task_frame_status_wait);
        }
        bool isFrameCanResub() // 帧任务能否重提
        {
            return  isDistributedFrame() == false &&
                    isComposeFrame() == false &&
                    (/*frameStatus == task_frame_status_stop ||*/
                    frameStatus == task_frame_status_completed ||
                    frameStatus == task_frame_status_failed /*||
                    frameStatus == task_frame_status_overtime_stop*/);
        }
        bool isDistributedFrame() // 是否是分布式帧
        {
            return frameType == houdini_distributed_normal ||
                   frameType == houdini_distributed_master;
        }
        bool showMcNodeRam()
        {
            return openRenderRam == 1;
        }
        bool isComposeFrame()
        {
            return frameType == photon_compose_frame   ||
                   frameType == priority_compose_frame ||
                   frameType == major_compose_frame;
        }

        DetailInfo()
        {
            arrearsFee = 0.0;
            feeType = "";
            framePrice = "";
            frameIndex = "";
            frameStatusText = "";
            isCopy = "";
            munuJobId = "";
            munuTaskId = "";
            platform = "";
            gopName = "";
            frameRam = 0;
            frameRamRender = 0;
        }
    };

    struct TaskFrameConsume {
        qreal couponConsume;
        qreal userAccountConsume;
        qint64 frameTimeConsumingAve;
        qint64 taskArrearage;
        qint64 taskTimeConsuming;
        qint64 totalFrames;
    };

    struct TaskFrameInfo {
        qint32  pageCount;
        qint32  pageNum;
        qint32  size;
        qint32  total;
        qint64  taskId;
        QString searchKey;
        QList<int> stateList;
        QMap<qint64, JobState::DetailInfo*> detailItems;
        QList<JobState::DetailInfo*> detailItemList;
        TaskFrameConsume jobConsume;
        QList<int> reqestPageNums; // 已经在请求的页面
        TaskFrameInfo()
        {
            searchKey = "";
        }
    };

    // 服务器任务状态
    enum enTaskStatus {
        enRenderTaskWaitting          = 0,  // 等待中//首先大任务的状态是start，没有running的任务即为等待
        enRenderTaskRendering         = 5,  // 渲染中//首先大任务的状态是start，只要有running的任务即为渲染中
        enRenderTaskPreRendering      = 8,  // 预处理//针对预处理任务，开始的预处理，状态为预处理中
        enRenderTaskStop              = 10, // 停止
        enRenderUserStop              = 15, // 用户停止
        enRenderTaskArrearageStop     = 20, // 欠费停止
        enRenderOverTimeStop          = 23, // 超时停止
        enRenderTaskFinished          = 25, // 已完成
        enRenderTaskFinishHasFailed   = 30, // 完成但有失败帧
        enRenderTaskAbandon           = 35, // 放弃
        enRenderTaskFinishTest        = 40, // 测试完成
        enRenderTaskFailed            = 45, // 失败
        enRenderTaskAnalyse           = 50, // 分析中
        enRenderTaskSubmitting        = 60, // 提交中
        enRenderTaskSubmitFailed      = 65, // 提交失败
        enRenderStateUpdating         = 100,// 状态更新中
        enRenderTaskSubFailed         = 0x80, // 提交失败
        enRenderTaskSubmit,
        enRenderTaskDelete,
        enRenderATWaiting,                  // 等待计算空三
        enRenderATStop,                     // 空三取消
        enRenderATFailed,                   // 空三失败
        enRenderATRendering,                // 空三计算中
        enRenderATFinished,                 // 空三计算完成
    };
    // 任务停止原因
    enum enTaskStopReason {
        enUnknownError    = 0, // 未知错误
        enUserStop        = 1, // 用户停止
        enAdminStop       = 2, // 管理员停止
        enOverduebillStop = 3, // 欠费停止
        enClientUserStop  = 4, // 客户端用户停止
        enApiUserStop     = 5, // api用户停止
        enFullspeedStop   = 6, // 全速停止
        enDeleteProjectStop = 7, // 删除项目停止
        enSubmitStop      = 9,   // 提交后停止
        enOvertimeStop    = 10   // 超时暂停
    };
    // 任务状态
    enum enTaskType {
        enGI = 1,           // 光子
        enGI_Mainmap,       // 光子+主图
        enMainmap,          // 主图
    };
    //
    enum enFrameStatus {
        enRenderFrameWaiting = 1,           // 等待中
        enRenderFrameRendering = 2,         // 渲染中
        enRenderFrameStopped = 3,           // 停止
        enRenderFrameDone = 4,              // 已完成
        enRenderFrameFailed = 5,            // 失败
        enRenderFrameWaitPreprocessing = 6, // 等待预处理完成
        enRenderFrameWaitPhotonFrame = 7,   // 等待光子帧渲染完成
        enRenderFrameWaitTestFrame = 8,     // 等待优先渲染完成
        enRenderFrameWaitPhotonJob = 9,     // 等待光子作业渲染完成
        enRenderFrameWaitResolve = 10,      // 等待解算作业渲染完成
    };

    struct ParamInfo {
        QString strName;
        QString strValue;
    };

    bool IsDownloadable()
    {
        switch(m_renderState) {
        case enRenderTaskRendering:
        case enRenderTaskFinished:
        case enRenderTaskStop:
        case enRenderUserStop:
        case enRenderTaskArrearageStop:
        case enRenderOverTimeStop:
        case enRenderTaskAbandon:
        case enRenderTaskFailed:
        case enRenderTaskFinishHasFailed:
        case enRenderTaskFinishTest:
        case enRenderTaskWaitting: {
            if(m_jobPreState == enRenderTaskFinished)
                return true;
        }
        default:
            return false;
        }
        return false;
    }
    bool IsTransform()
    {
        if(IsUploading() || IsDownloading()) {
            return true;
        }
        return false;
    }
    bool IsStarted()
    {
        return m_renderState == enRenderTaskRendering;
    }
    bool IsDownloading()
    {
        return m_transState == enTransDownloading;
    }
    bool IsUploading()
    {
        return m_transState == enTransUploading;
    }
    bool TaskIsPrepareing()
    {
        return m_jobType == RENDER_JOB && m_jobPreState == enRenderTaskPreRendering;
    }
    bool IsPreTaskCanStop()
    {
        return m_jobType == RENDER_JOB && (m_jobPreState == enRenderTaskPreRendering /*|| m_jobPreState == enRenderTaskWaitting*/);
    }
    bool IsPreTaskCanStart()
    {
        return m_jobType == RENDER_JOB && (m_jobPreState == enRenderTaskStop);
    }
    bool IsPreTaskCanDelete()
    {
        return (m_jobPreState == enRenderTaskStop /*|| m_jobPreState == enRenderTaskWaitting || m_jobPreState == enRenderTaskFinished*/ ||
                m_jobPreState == enRenderTaskFinished /*|| m_jobPreState == enRenderTaskPreRendering*/);
    }
    bool IsPreTaskCanResub()
    {
        return m_jobType == RENDER_JOB && (m_jobPreState == enRenderTaskFailed);
    }
    bool IsPreTaskFinished()
    {
        return (m_jobPreState == enRenderTaskFinished);
    }
    bool TaskIsFinished()
    {
        return m_jobType == RENDER_JOB && m_renderState == enRenderTaskFinished /*|| m_renderState == enRenderTaskFinishHasFailed*/;
    }
    bool IsWaitting()
    {
        return m_transState == enRenderTaskWaitting;
    }
    bool TaskCanReSub()
    {
        // 效果图重提
        return (m_jobType == RENDER_JOB && (m_renderState == enRenderTaskFailed || m_renderState == enRenderTaskAbandon
            || m_renderState == enRenderTaskFinishHasFailed
            || m_renderState == enRenderTaskRendering || m_renderState == enRenderTaskStop)
            && IsPreTaskFinished());

        return ((m_renderState == enRenderTaskFailed || m_renderState == enRenderTaskFinishTest
            || m_renderState == enRenderTaskFinishHasFailed
            || m_renderState == enRenderTaskRendering || m_renderState == enRenderTaskStop
            || m_renderState == enRenderUserStop || m_renderState == enRenderTaskArrearageStop
            || m_renderState == enRenderOverTimeStop));
    }
    bool IsRenderCanStop()
    {
        return m_jobType == RENDER_JOB && (m_renderState == JobState::enRenderTaskRendering
                || m_renderState == JobState::enRenderTaskWaitting
                || m_renderState == JobState::enRenderTaskSubmitting
                || m_renderState == JobState::enRenderTaskFinishTest);
    }
    bool IsRenderCanStart()
    {
        return (m_jobType == RENDER_JOB && (m_renderState == JobState::enRenderTaskStop
                  || m_renderState == JobState::enRenderTaskArrearageStop
                  || m_renderState == JobState::enRenderOverTimeStop
                  || m_renderState == JobState::enRenderUserStop
                  || m_renderState == JobState::enRenderTaskSubmitFailed));
    }
    bool IsRenderCanDelete()
    {
        if(MyConfig.userSet.isChildAccount() && MyConfig.accountSet.subDeleteTask == false) {
            return false;
        }

        return  ((m_renderState == JobState::enRenderTaskStop ||
                 m_renderState == JobState::enRenderTaskFinished || m_renderState == JobState::enRenderTaskFailed ||
                 m_renderState == JobState::enRenderTaskArrearageStop || m_renderState == JobState::enRenderOverTimeStop ||
                 m_renderState == JobState::enRenderTaskFinishTest || m_renderState == JobState::enRenderTaskFinishHasFailed
                 || m_renderState == JobState::enRenderUserStop || m_renderState == JobState::enRenderTaskAbandon
                 || m_renderState == JobState::enRenderTaskSubmitFailed));
    }
    // 是否可以克隆
    bool IsRenderCanClone()
    {
        return ((m_renderState == JobState::enRenderTaskStop || m_renderState == JobState::enRenderTaskWaitting ||
                 m_renderState == JobState::enRenderTaskFinished || m_renderState == JobState::enRenderTaskFailed ||
                 m_renderState == JobState::enRenderTaskArrearageStop || m_renderState == JobState::enRenderOverTimeStop ||
                 m_renderState == JobState::enRenderTaskFinishTest || m_renderState == JobState::enRenderTaskFinishHasFailed ||
                 m_renderState == JobState::enRenderTaskRendering) && this->childCount() < 2
                && (this->getJobSubChannel() == CHANNEL_CLIENT) && this->isSupportScene());
    }
    // 是否可以全速
    bool IsRenderCanFullSpeed()
    {
        return ((m_renderState == JobState::enRenderTaskFinishTest) || (m_renderState == enRenderTaskRendering)) && (m_renderState != enRenderStateUpdating);
    }
    bool IsUploadCanStop()
    {
        return (m_jobType == UPLOAD_JOB) && (m_transState != enUploadStop);
    }
    bool IsUploadCanStart()
    {
        return (m_jobType == UPLOAD_JOB) && (m_transState == enUploadStop);
    }
    bool IsUploadCanDelete()
    {
        return (m_jobType == UPLOAD_JOB) && (m_transState == enUploadStop || m_transState == enUploadCompleted);
    }
    // 是否是超时停止
    bool IsRenderStop()
    {
        return m_renderState == enRenderTaskStop || m_renderState == enRenderUserStop ||
            m_renderState == enRenderTaskArrearageStop || m_renderState == enRenderOverTimeStop;
    }
    // 是否是超时停止
    bool IsTimeoutStop()
    {
        return m_renderState == enRenderOverTimeStop;
    }
    // 是否是父节点
    bool IsParentItem()
    {
        return m_isOpen;
    }
    // 是否是子节点,1.是不是有父节点，2.isopen是不是false
    bool IsChildItem()
    {
        return (getTaskParentId() > 0) || ((m_isOpen == false) && (m_parentItem != nullptr) && (m_parentItem->getTaskId() != -1));
    }
    // 重提完成帧
    bool IsCanResubFinishedTask()
    {
        return true;
        return (m_renderState == enRenderTaskFinishTest || m_renderState == enRenderTaskFinished);
    }
    // 重提失败帧
    bool IsCanResubFailedTask()
    {
        return true;
        return (m_renderState == enRenderTaskFailed || m_renderState == enRenderTaskFinishHasFailed);
    }
    // 重提超时停止帧
    bool IsCanResubTimeoutTask()
    {
        return true;
        return (m_renderState == enRenderOverTimeStop);
    }
    // 重提停止帧
    bool IsCanResubStopedTask()
    {
        return true;
        return m_renderState == enRenderUserStop || m_renderState == enRenderTaskStop || m_renderState == enRenderTaskArrearageStop;
    }
    // 是否能升级内存
    bool IsCanUpdateRam()
    {
        return m_renderState != enRenderStateUpdating;
    }
    // 是否可以修改节点机数量
    bool IsCanUpdateTaskLimit()
    {
        return m_renderState != enRenderStateUpdating;
    }
    // 是否可以修改任务等级
    bool IsCanUpdateTaskLevel()
    {
        return m_renderState != enRenderStateUpdating;
    }
    // 是否可以使用免单券
    bool IsCanUseVoucher()
    {
        return (m_jobType != UPLOAD_JOB) && (m_renderState != enRenderTaskRendering && m_renderState != enRenderStateUpdating && 
            m_renderState != enRenderTaskWaitting && m_renderState != enRenderTaskPreRendering);
    }
    // 是否正在渲染
    bool IsTaskRendering()
    {
        return m_renderState == enRenderTaskRendering;
    }
    // 是否显示耗时时间
    bool IsCanShowTaskCostTime()
    {
        return m_renderState == enRenderTaskFinished || m_renderState == enRenderTaskFinishHasFailed
            || m_renderState == enRenderTaskFinishTest || m_renderState == enRenderTaskStop
            || m_renderState == enRenderTaskArrearageStop || m_renderState == enRenderOverTimeStop;
    }
    // 是否下载完成
    bool IsDownloadFinished()
    {
        return m_transState == enDownloadCompleted;
    }
    // 可否停止下载
    bool IsCanStopDownload() 
    {
        return (m_jobType == DOWNLOAD_JOB) && (m_transState == enTransDownloading || m_transState == enDownloadWaiting
            || m_transState == enDownloadQueued || m_transState == enDownloadUnfinish);
    }
    // 是否上传完成(上传任务且完成，或 渲染任务   都可以任务文件已上传完成)
    bool IsUploadFinished()
    {
        return ((m_jobType == UPLOAD_JOB) && (m_transState == enUploadCompleted)) || m_jobType == RENDER_JOB;
    }
    bool TaskIsFailed()
    {
        return ((m_jobType != UPLOAD_JOB) && (m_renderState == enRenderTaskFailed));
    }
    bool IsRenderWaiting()
    {
        return m_renderState == enRenderTaskWaitting;
    }
    bool IsTaskSubmitFailed()
    {
        return m_renderState == enRenderTaskSubmitFailed;
    }
    bool IsTaskSubmitting()
    {
        return m_renderState == enRenderTaskSubmitting;
    }
    bool IsTaskStateUpdating()
    {
        return m_renderState == enRenderStateUpdating;
    }
    bool IsCloneTask()
    {
        return m_cloneOriginalId > 0 || m_cloneParentId > 0;
    }
    /******* 列表信息 ******/ 
    qint32 childCount() const;
    qint32 rowCount();
    qint32 row();
   
    void addChildJobState(JobState* child); // 添加子项目
    void setParentItem(JobState* parent);
    void removeChildState(JobState* job);// 移除子节点
    void clearAllChildren();
   
    JobState *childItem(qint32 row);
    JobState *getChild(qint64 taskid);
    JobState *getParentItem();// 获取父节点

    QList<JobState*> getChildren();
    QList<qint64> getChildrenIds();  
    QHash<qint64, JobState*> getChildJobState();

    QList<JobState*> getPhotonJobs();
    QList<JobState*> getMajorJobs();
    /******* 任务信息 ******/
    void setTaskId(qint64 taskId);
    void setTaskParentId(qint64 pId);
    void setSubJobUserId(qint64 uId);
    void setCloneOriginalId(qint64 orgCloneId);
    void setCloneParentId(qint64 cloneParentId);
    void setLayerParentId(qint64 layerPid);
    void setStatus(qint32 status);
    void setTransStatus(qint32 status);
    void setCheckedState(qint32 status);

    void setScene(const QString& strScene);
    void setProjectName(const QString& projectName);
    void setTaskIdAlias(const QString& alias);
    void setCamera(const QString& camara);
    void setJobSubmitter(const QString& submitter);
    void setLayerName(const QString& layerName);
    void setArtistName(const QString& artistName);
    void setRemark(const QString& remark);
    void setFrameScope(const QString& frameScope);
    void setTaskType(const QString& taskType);
    void setJobPreStateText(const QString& preState);

    void setErrorString(const QString& errorString);
    void setSpeedString(const QString& speed);
    void setDependentTask(const QString& dependTask);

    void setRenderTiles(const QString& tiles);

    /******* 路径相关 ******/
    void setScenePath(const QString& path);
    void setOutputLabel(const QString& outputLabel);
    void setLocalSavePath(const QString& saveAsPath);
    void setAutoDownPath(const QString& downPath);
    void setUploadFileName(const QString& name);
    void setDownloadFileName(const QString& name);

    /******* 时间相关 ******/
    void setStartDate(qint64 startDate);
    void setCompelteDate(qint64 endDate);
    void setSubmitDate(qint64 submitDate);
    void setRenderTimeCost(qint64 cost);
    void setAvgTimeCost(qint64 cost);
    void setOverTimeRemind(qint64 remind);
    void setOverTimeStop(qint64 timeStop);

    void setStartDateString(const QString& startDate);
    void setCompelteDateString(const QString& endDate);
    void setSubmitDateString(const QString& submitDate);
    void setDownloadStopDate(const QString& stopDate);

    /******* 进度相关 ******/
    void setUploadProgress(float upload, bool bReset = false);
    void setDownloadProgress(qint64 download, bool bReset = false);
    void setRenderProgress(int excuteCount, int doneCount, int failedCount, int abortCount, int totalCount);
    void setProgress(qint32 completed, qint32 total);
    void setProgressByFileSize(qint64 completed, qint64 total);
    void setUploadedFileSize(qint64 uploaded);

    /******* 数量相关 ******/
    void setGroup(qint32 group);
    void setDownloadCount(qint32 count);
    void setMemorySize(qint32 size);
    void setTaskLimit(qint32 limit);
    void setCgId(qint32 cgId);
    void setTaskOutputSize(qint64 size);

    void setTaskLevel(qint32 level);
    void setTaskUserLevel(qint32 userLevel);
    void setShareMainCapital(qint32 share);
    void setJobOperable(qint32 operable);
    void setOutputBid(qint32 bid);
    void setJobSubChannel(qint32 channel);
    void setJobType(qint32 jobType);

    void setUserAccountConsume(float accountConsume);
    void setCouponConsume(float couponConsume);
    void setQYCouponConsume(float couponConsume);
    void setTaskArrears(float taskArrears);
    void setTaskRenderConsume(float renderConsume); // 任务总消费
    void setSpeed(float fSpeed);

    void setDownloadMode(qint32 mode);
    void setDownloadPathCode(qint32 code);

    /******* 标志符 ******/
    void setIsOpen(bool isOpen);
    void setIsDelete(bool isDelete);
    void setIsCloneTask(bool isClone);
    void setDownloadError(bool error);
    void setAutoDownload(bool isAuto);
    void setDownloadCompleted(bool compelete);
    void setIsOverTime(bool overTime);
    void setDependency(bool isDepend);

    /******* 获取任务信息 ******/
    qint64 getTaskId();
    qint64 getTaskParentId();
    qint64 getSubJobUserId();
    qint64 getCloneOriginalId();
    qint64 getCloneParentId();
    qint64 getLayerParentId();
    qint32 getStatus();
    qint32 getTransStatus();
    qint32 getCheckedState();

    QString getScene();
    QString getProjectName();
    QString getTaskIdAlias();
    QString getCamera();
    QString getJobSubmitter();
    QString getLayerName();
    QString getArtistName();
    QString getRemark();
    QString getFrameScope();
    QString getTaskType();
    QString getJobPreStateText();

    QString getErrorString();
    QString getSpeedString();
    QString getDependentTask();

    QString getRenderTiles();
    QString getConsumpFee();
    float   getConsumFeeNum();
   
    /******* 路径相关 ******/
    QString getScenePath();
    QString getOutputLabel();
    QString getLocalSavePath();
    QString getAutoDownPath();
    QString getOutputPath(const QString& projectSavePath);
    QString getUploadFileName();
    QString getDownloadFileName();
    QString getDownloadFullPath(const QString& projSavePath);

    /******* 时间相关 ******/
    qint64 getStartDate();
    qint64 getCompelteDate();
    qint64 getSubmitDate();
    qint64 getRenderTimeCost();
    qint64 getAvgTimeCost();
    qint64 getOverTimeRemind();
    qint64 getOverTimeStop();

    QString getStartDateString(const QString& format = "yyyy-MM-dd hh:mm:ss");
    QString getCompelteDateString(const QString& format = "yyyy-MM-dd hh:mm:ss");
    QString getSubmitDateString();
    QString getDownloadStopDate();
    QString getRenderTimeCostString();

    /******* 进度相关 ******/
    float  getUploadProgress();
    qint64 getDownloadProgress();
    double getRenderProgress();
    qint32 getCompleteFileCount();
    qint32 getTotalFileCount();
    float  getUploadProgressByFileSize();
    qint64 getTotalFileSize();
    qint64 getCompleteFileSize();
    qint32 getUploadRemainTime();


    /******* 数量相关 ******/
    qint32 getGroup();
    qint32 getDownloadCount();
    qint32 getMemorySize();
    qint32 getTaskLimit();
    qint32 getCgId();
    qint64 getTaskOutputSize();

    qint32 getTaskLevel();
    qint32 getTaskUserLevel();
    qint32 getShareMainCapital();
    qint32 getJobOperable();
    qint32 getOutputBid();
    qint32 getJobSubChannel();
    qint32 getJobType();
    qint32 getSceneType();

    qint32 getDownloadMode();
    qint32 getDownloadPathCode();
    qint32 getDownloadCompleteChildCount();

    float getUserAccountConsume();
    float getCouponConsume();
    float getQYCouponConsume();
    float getTaskArrears();
    float getTaskRenderConsume();
    float getTotalConsume();
    float getTransSpeed();

    /******* 标志符 ******/
    bool isOpen();
    bool isDelete();
    bool isCloneTask();
    bool getDownloadError();
    bool isAutoDownload();
    bool isDownloadCompleted();
    bool isOverTime();
    bool isDependency();
    bool isSupportScene();
    bool isChecked();
    bool hasPhotonJob();
    bool isPhotonJob();
    bool isContainsMajor();
    bool isPhotonJobProccessEnable();
    bool isMajorJobProccessEnable();
    bool showPreState();
    bool isPhotonJobRendering();
    bool isContainsRebuildJob();
    bool isContains3DTilesJob();
    bool isContainsRenderJob();

    /******* 业务相关 ******/
    void invalidateTask();    
    QVariant getJobFeeTips(); // 获取费用的tooltips
    
    // 下载期限是否过期
    bool isDownloadExpired();
    // 寻找未过期的子任务
    QList<JobState*> getCanDownloadChildJob();
    QList<JobState*> getExpiredChildJob();
    QList<JobState*> getSubTasks();

    // 获取状态文字
    QString getStatusText();
    // 任务预处理状态
    void setPreTaskStatus(int preState);
    int  getPreTaskStatus();
    void setPreTaskStatusText(const QString& text);
    QString getPreTaskStatusText();

    // 是否是子账号的任务
    bool isSubAccountJob();
    // 行颜色
    QColor getRowColor();
    // 停止类型
    void setStopType(JobState::enTaskStopReason type);
    JobState::enTaskStopReason getStopType();
    QString getStopReasonText();

    // 可以重提的子任务
    QList<qint64> getCanResubChildTaskList();

    // 上报
    QString getLogFileState();
    QJsonObject dumpState(int errCode, const QString& localPath, const QString& remotePath);

    // 所在行
    void setRowNum(qint32 row);
    qint32 getRowNum();

    // 完成次数
    void setFinishedTimes(int times);
    int getFinishedTimes();

    // 是否是会员任务
    bool isMemberTask();
    // 更新任务类型
    void updateTaskType();

    void setCoordCount(const QString& count);
    QString getCoordCount();

    void setPicCount(const QString& count);
    QString getPicCount();

    void setMunuTaskId(const QString& munuId);
    QString getMunuTaskId();

    // 任务信息
    void updataBaseTaskInfo(BaseTaskInfo* taskInfo);
    BaseTaskInfo* getBaseTaskInfo();
    void setSubmitCtrlPoint(bool isSubmit);
    bool isSubmitCtrlPoint();
    void setSubmitPosInfo(bool isSubmit);
    bool isSubmitPosInfo();
    void setProjectType(int type); // 项目数据类型
    int getProjectType();
    void setProjectOutputType(const QStringList& types);
    QStringList getProjectOutputType();
    void setProgressPercent(int progress);
    int getProgressPercent();

    // 空三、重建相关
    QList<JobState*> getATJobs();
    QList<JobState*> getRebuildJobs();
    QList<JobState*> getRenderJobs();
    QList<JobState*> get3DTilesJobs();
    JobState* getATJob();
    JobState* getRebuildJob();
    JobState* getRenderJob();
    JobState* get3DTilesJob();
    bool isATJob();
    bool isRebuildJob();
    bool isRenderJob();
    bool is3DTilesJob();

    bool isATJobRendering();
    bool isRebuildJobRendering();
    bool isMajorJobRendering();
    bool is3DTilesJobRendering();

    bool isATFinished();
    bool isRebuildFinished();
    bool isTaskFinished();
    bool is3DTilesFinished();
    bool isProductionFinished();

    bool isMajorJob();
    bool isMajorJobFinished();
    
    bool isATStopped();
    bool isRebuildStopped();
    bool is3DTilesStopped();

    bool isATFailed();
    bool isRebuildFailed();
    bool isMajorFailed(); 
    bool is3DTilesFailed();

    // 状态
    int getATStatus();
    int getRebuildStatus();
    int getRenderStatus();
    int get3DTilesStatus();
    int getProductionStatus(); // 3dtiles + render的 状态
    int getTaskStatus();

    QString getATStateText();
    QString getRebuildStateText();
    QString getProductionStateText();

    // 页面
    int pageDisplayByState();
    int getATJobDisplayState();
    int getRebuildJobDisplayState();
    int getRenderJobDisplayState();
    int get3DTilesJobDisplayState();

    // 右键菜单
    bool IsParentTaskCanStart();
    bool IsParentTaskCanStop();
    bool IsParentTaskCanDelete();
    bool IsParentTaskCanResubmit();
    bool IsParentTaskCanDownload();
    bool IsParentTaskSubmitFailed();
    bool IsParentTaskSubmitting();
    bool IsParentTaskStateUpdating();
    // 状态文字颜色
    QColor getStateColor();
    // 区块文件内容
    void setBlockExchangeInfo(BLOCK_TASK::BlockExchange* exchage);
    BLOCK_TASK::BlockExchange* getBlockExchangeInfo();
    // 需要下载的路径
    void setDownloadFiles(const QStringList& types);
    void addDownloadFile(const QString& outputPath);
    QStringList getDownloadFileList();
    void resetDownloadList();
    // 是否提交成功
    bool isSubmitSuccess();
    void setIsSubmitSuccess(bool isSuccess);
    // 帧信息
    qint32 GetExcuteCount();   // 正在执行中的
    qint32 GetDoneCount();     // 完成的
    qint32 GetFailedCount();   // 失败的
    qint32 GetAbortCount();    // 放弃的
    qint32 GetWaitCount();     // 等待中的
    // 是否显示详情页
    bool isCanShowDetailPage();
    // 总像素
    void setTotalPixel(const QString& pixel);
    QString getTotalPixel();
    // 是否可以操作
    bool isCanJobOperable();
    // 统计上传的数量（10秒内的）
    void statisticsUploadedCount();
    qint32 uploadRemainTime();
public:
    //int  m_renderState;
    //int  m_transState;           // 上传 1,  下载 2

    bool m_isOpen;                // 是否包含子任务   
    bool m_isDelete;              // 是否删除
    bool m_isChecked;             // 是否选中
    
    bool m_isClone;
    bool m_downloading;
    bool m_downloadError;         // 下载出错
    bool m_isAutoDownload;        // 是否是自动下载任务
    bool m_isDownloadCompleted;

    bool m_bDependency;           // 是否依赖标识
    bool m_bOverTime;             // 任务是否已超时
 
    qint64 m_taskId;
    qint64 m_parentId;
    qint64 m_subJobUserId;        // 任务提交ID
    qint64 m_cloneOriginalId;
    qint64 m_cloneParentId;       
    qint64 m_layerParentId;       // 层的父ID号
                                  
    qint64 m_startDate;           // 开始时间
    qint64 m_completedDate;       // 完成时间
    qint64 m_submitDate;          // 提交时间
    qint64 m_renderTimeCost;      // 渲染时长
    qint64 m_avgTimeCost;         // 平均耗时
    qint64 m_overTimeRemind;      // 超时提醒
    qint64 m_overTimeStop;        // 超时停止
    qint64 m_taskOutputSize;      // 任务输出大小
                                  
    qint32 m_renderState;         
    qint32 m_transState;          
    qint32 m_checkedState;        
    qint32 m_jobPreState;         // 预处理状态

    qint32 m_dwGroup;             
    qint32 m_downloadCount;       // 下载统计
    qint32 m_memorySize;          // 内存
    qint32 m_renderingCount;      // 渲染中
    qint32 m_taskLimit;           
    qint32 m_cgId;
                                  
    qint32 m_taskLevel;           // 任务级别
    qint32 m_taskUserLevel;       // 任务用户级别
    qint32 m_shareMainCapital;    // 是否开启主子账号共享
    qint32 m_jobOperable;         // 任务是否可操作
    qint32 m_outputBid;           // 输出BID
    qint32 m_jobSubChannel;       // 任务提交来源
    qint32 m_jobType;             // job类型 上传，渲染，下载
                                      
    float  m_uploadProgress;      // 上传用的进度
    qint32 m_downloadProgress;    // 下载用的进度
    double m_renderProgress;      // 渲染进度(只能通过 0|0|0|0|0 状态设置)
    qint32 m_completedCount;
    qint32 m_totalCount;
    qint32 m_dwExcuteCount;       // 正在执行中的
    qint32 m_dwDoneCount;         // 完成的
    qint32 m_dwFailedCount;       // 失败的
    qint32 m_dwAbortCount;        // 丢弃的
    qint32 m_dwWaitCount;         // 等待中的

    qint32 m_waitJobCounts;       // 等待的任务数
    qint32 m_downloadMode;        // 下载模式
    qint32 m_downloadCode;        // 下载编码
                                  
    float  m_accountConsume;      // 用户余额花费
    float  m_couponConsume;       // 优惠券花费
    float  m_qyCouponConsume;     // 青云券
    float  m_taskArrears;         // 任务欠费
    float  m_renderConsume;       // 任务总消费
    float  m_transSpeed;          // 传输速度
                                  
    QString m_taskAlias;          // 任务别名
    QString m_shortScene;         // 场景名
    QString m_orgScenePath;       // 场景路径
    QString m_camera;             // 相机名
    QString m_jobSubmitter;       // 提交账号
    QString m_layerName;          // 层名
    QString m_artistName;         // 制作人
    QString m_projectName;        // 项目名
    QString m_remark;             // 备注
    QString m_frameScope;         // 帧范围
    QString m_taskType;
    QString m_jobPreStateText;    // 预处理状态
    QString m_tiles;              // 块数

    QString m_sceneLocalPath;     // 场景文件路径
    QString m_strLocalSavePath;   // 用户另存为路径
    QString m_autoDownPath;       // 自动下载保存路径
    QString m_strOutputLabel;     // 服务器存储后缀
    QString m_uploadFileName;
    QString m_logFileState;

    QString m_strError;           // 错误提示
    QString m_strSpeed;           // 当前速度字符串
    QString m_strDependentTask;   // 依赖任务

    QString m_strStartDate;       // 开始时间字符串
    QString m_strCompletedDate;   // 结束时间字符串
    QString m_strSubmitDate;      // 提交时间字符串
    QString m_strStopDate;

    QStringList   m_taskParam;
    TaskFrameInfo m_frameDetails;

    JobState *m_parentItem;                  // 父节点
    enTaskStopReason m_stopReason;           // 任务停止类型
    QHash<qint64, JobState*> m_children;     // 子条目
    QMap<QString, JobState*> m_subTaskState; // 子任务的状态
    QMutex m_mutex;    // 用于防止 children
	CLock  m_lockSpeed;
    QList<TaskGrabInfo*> m_grabInfo; // 进度图
    qint32 m_rowNum;
    int m_downloadFinishTimes; // 下载完成次数
    QString m_diffOfRenderConsume; // 费用差额
    qint64 m_completedFileSize;
    qint64 m_totalFileSize;
    qint64 m_uploadedFileSize;
    qint64 m_uploadedSizePerSecond; // 每秒传输多少

    QString m_ctrlPointCount; // 控制点数量
    QString m_picCount; // 照片数量
    QString m_totalPixel;// 总像素

    QString m_downloadType; // osgb / obj

    BaseTaskInfo* m_baseTaskInfo; // 任务基本信息
    BLOCK_TASK::BlockExchange* m_blockInfo;

    qint32 m_projectType;
    QStringList m_outputType;
    int m_progressPercent;
    QStringList m_downloadList;
    QString m_munuTaskId;

    bool m_isSubmitSuccess;

    QMutex m_mutexCount;   
    int m_transFileCount;
    float m_perSecondUploadedFile;
};
Q_DECLARE_METATYPE(JobState::DetailInfo*)
Q_DECLARE_METATYPE(JobState*)

#endif // TASKSTATE_H_
