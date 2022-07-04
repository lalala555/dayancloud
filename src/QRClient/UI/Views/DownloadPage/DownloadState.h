/***********************************************************************
* Module:  DownloadState.h
* Author:  hqr
* Modified: 2016/11/14 17:24:17
* Purpose: Declaration of the class
***********************************************************************/
#pragma once
#ifndef DOWNLOADSTATE_H_
#define DOWNLOADSTATE_H_

#include <QMutex>
#include <QHash>
#include "kernel/macro.h"
#include "kernel/msg.h"
#include "Common/SyncObject.h"
class CDownloadState
{
public:
    explicit CDownloadState(qint32 jobId, CDownloadState* parentItem = 0);
    ~CDownloadState();

    enum HEADER_DOWNLOAD {
        COLUMN_DOWNLOAD_JOB_TYPE = 0,
        COLUMN_DOWNLOAD_STATUS,
        COLUMN_DOWNLOAD_LEVEL,
        COLUMN_DOWNLOAD_TASKID,
        COLUMN_DOWNLOAD_SCENE_NAME,
        COLUMN_DOWNLOAD_LAYER_NAME,
        COLUMN_DOWNLOAD_FRAME_NAME,
        COLUMN_DOWNLOAD_PROJECT,
        COLUMN_DOWNLOAD_PROGRESS,
        COLUMN_DOWNLOAD_SPEED,
        COLUMN_DOWNLOAD_FILE_SIZE,
        COLUMN_DOWNLOAD_STARTTIME,
        COLUMN_DOWNLOAD_ENDTIME,
        COLUMN_DOWNLOAD_FRAME_CHECK,

        COLUMN_DOWNLOAD_TOTAL
    };

    //检查坏帧状况
    enum enCheckingFrameStatus {
        enCheckingWaitting = 0,    //等待检测
        enCheckingSuccess,         //未检查出有错帧
        enCheckingFailed           //检查出有错帧
    };

    void SetTaskId(qint32 dwTaskId);
    void setScene(const QString& strScene);
    void setOrgScene(const QString& strScene);
    void SetLayerName(const QString& strLayer);
    void SetProgress(float dwProgress, bool bReset = false);
    void setStartDate(const QString& pszStartDate);
    void SetStopDate(const QString& pszStopDate);
    void setCompleteDate(const QString& pszCompleteDate);
    void SetStatus(int status);
    void setLocalSavePath(const QString& pszSavePath);
    void setTransState(int nTransState, bool invalidate = true);
    void SetProjectSymbol(const QString& strSymbol);
    void SetLocalSaveAsPath(const QString& pszSaveAsPath);
    void SetTaskOutputSize(qint64 size);

    qint32 getJobId();
    QString GetScene();
    QString GetOrgScene();
    QString GetLayerName();
    float GetProgress();
    QString GetStartDate();
    QString GetStopDate();
    QString GetCompleteDate();
    QString GetStatusString();
    int GetStatus();
    QString GetLocalSavePath();
    int  GetTransState();
    QString getProject();
    QString GetLocalSaveAsPath();
    qint64 GetTaskOutputSize();
    QString GetTaskOutputSizeStr();

    void SetDownloadFileName(const QString& strName);
    QString GetDownloadFileName();

    void SetSpeed(const QString& pszSpeed);
    void SetSpeed(float fSpeed);
    QString GetSpeed();
    float GetTransSpeed();

    void setArtist(const QString& pszArtist);
    QString GetArtist();

    QString getOutputPath(int ptid, int uid, int puid, const QString& projectSavePath);
    QString getOutputPath(const QString& projectSavePath);

    void SetCheckingStatus(int status, bool invalidate = true);
    int GetCheckingStatus();
    int GetCheckingStatusIcon();

    void setVisible(bool bVisible);
    bool getVisible(bool bVisible);
    bool IsTransform()
    {
        if(IsUploading() || IsDownloading()) {
            return true;
        }
        return false;
    }
    bool IsDownloading()
    {
        return m_nTransState == enTransDownloading;
    }
    bool IsUploading()
    {
        return m_nTransState == enTransUploading;
    }
    bool IsQueued()
    {
        return m_nTransState == enDownloadQueued;
    }
    bool IsWaitting()
    {
        return m_nTransState == enDownloadWaiting;
    }

    bool GetError()
    {
        return m_hasError;
    }
    void SetError(bool bError)
    {
        m_hasError = bError;
    }
    void setPriority(int priority)
    {
        m_priority = priority;
        switch(priority) {
        case enPriorityNormal:
            m_sPriority = "L";
            break;
        case enPriorityMiddle:
            m_sPriority = "M";
            break;
        case enPriorityHigh:
            m_sPriority = "H";
            break;
        default:
            break;
        }
    }
    int getPriority()
    {
        return m_priority;
    }
    QString getPriorityText()
    {
        return QString::number(getPriority());
        return m_sPriority;
    }
    void setPriorityTime(int priorityTime)
    {
        m_priorityTime = priorityTime;
    }
    int getPriorityTime()
    {
        return getStartDateTime(); // 开始时间为优先级
    }
    // 是否可以开始
    bool isCanStartDownload()
    {
        return  m_nTransState != enDownloadCompleted;
    }
    // 是否可以暂停
    bool isCanStopDownload()
    {
        return  (m_nTransState != enDownloadStop && m_nTransState != enDownloadCompleted);
    }
    // 是否完成
    bool isCompleted();

    QString getCurrProjectSavePath();
    CG_ID getCGId();

    // 子项目
    void addChildDownloadState(CDownloadState* child);
    QHash<qint64, CDownloadState*> getChildDownloadState();
    CDownloadState *child(int row);
    CDownloadState *getChildByTaskId(qint64 taskid);
    int childCount() const;
    int columnCount() const;
    int row();
    void clearAllChildren();
    QList<CDownloadState*> getChildren();
    qint32 getRowCount();
    QList<qint64> getChildrenIds();
    // 获取父节点
    void setParentItem(CDownloadState *parent);
    CDownloadState *getParentItem();
    // 是否是父节点
    void setIsParentItem(bool isParent);
    bool isParentItem();
    // 移除节点
    void removeChildState(CDownloadState* state);
    // 获取完成下载的子任务
    int getDownloadCompleteChildCount();
    // 获取传输进度
    QString getTransProgress();
    // 存储ID
    void setStorageId(qint32 bid);
    qint32 getStorageId();
    // ouputlabel
    void setOutputLabel(const QString& outputLabel);
    QString getOutputLabel();
    // 完成的任务情况
    void setTaskProgress(int excuteCount, int doneCount, int failedCount, int abortCount, int totalCount);
    // 是否删除
    void setIsDelete(bool isDelete);
    void setIsOpen(bool isOpen);
    bool isDelete();
    bool isOpen();
    // 任务号别名
    void setTaskAlias(const QString& alias);
    QString getJobIdAlias();
    // 是否是子任务
    bool isChildItem();
    // 获取本地下载路径全路径
    QString getDownloadFullPath(const QString& projSavePath);
    // 是否是光子任务
    bool isPhotonJob();
    // 任务类型
    QString getTaskType();
    void setTaskType(const QString& type);
    // 下载期限是否过期
    bool isDownloadExpired();
    // 任务开始渲染的时间
    void setStartRenderDate(qint64 startDate);
    QString getStartRenderDate();
    // 任务完成渲染的时间
    void setFinishedRenderDate(qint64 finishedDate);
    QString getFinishedRenderDate();
    // 壳作业outputLabel
    void setParentOutputLabel(const QString& outlabel);
    QString getParentOutputLabel();
    // 所在行
    void setRowNum(int rowNum);
    int getRowNum();
    // 刷新
    void InvalidateTask();
    // 壳任务ID
    void SetParentTaskId(t_taskid id);
    t_taskid GetParentTaskId();
    // 上报
    QString getLogFileState();
    QJsonObject dumpState(int errCode, const QString& localPath, const QString& remotePath);
    // 开始时间
    qint64 getStartDateTime();
    // 提交账号
    void SetCustomerId(qint64 customerId);
    qint64 GetCustomerId();
    bool isSubAccountJob();
    // 自定义优先
    void setProrityType(int prorityType);
    int  getProrityType();
    // 任务最后运行时间
    void updateTaskLastExecTime(bool reset = false);
    qint64 getTaskLastExecTime();
    // 数据是否是过时数据
    bool isObsoleteData();
    // 设置帧下载路径
    void setFrameOutput(const QStringList& frames);
    QStringList getFrameOutput();
    // 下载任务类型
    void setDownloadJobType(int jobType);
    int getDownloadJobType();
    bool isFrameDownloadJob();
    // 帧名
    void setFrameName(const QString& name);
    QString getFrameName();
    // 获取下载类型
    QString getJobOperateTypeString();
    // 是否是自动下载
    void setJobOperateType(int type);
    int getJobOperateType();
    // 所有帧是否下载完毕
    void setFrameFileDownloadState(const QString& remote, int status);
    bool isFrameDownloadFinished();
    // 下载文件的大小
    void setFileTotalSize(qint64 total);
    qint64 getFileTotalSize();
    // 已经传输的大小
    void setFileTransferedSize(qint64 total);
    qint64 getFileTransferedSize();
    // 项目名
    void setProjectName(const QString& name);
    QString getProjectName();
    // 完成次数
    void setFinishedTimes(int times);
    int getFinishedTimes();
    // 获取下载类型
    void setOutputType(const QStringList& type);
    QStringList getOutputType();
    // 需要下载的路径
    void addDownloadFile(const QString& outputPath);
    QStringList getDownloadFileList();
    void setDownloadFiles(const QStringList& types);
    // 已经下载过的文件
    void setDownloadedFile(const QString& filename);
    bool isDownloadedFileFinished(const QString& filename);
    int getDownloadedFileCount();
    void resetDownloadedInfo();
    bool isDownloadFinished();
    void resetDownloadList();
protected:
    bool m_bVisible;
    bool m_hasError;
    int  m_nStatus;
    int  m_nTransState;      // 上传 1,  下载 2
    int  m_nFrameStatus;     // 帧情况
    int  m_priority;
    int  m_priorityTime;
    qint32 m_dwTaskId;
    float   m_dwProgress;        // 上传用的进度
    QString m_strShortScene;    // 场景名
    QString m_strLayerName;     // 层名
    QString m_strOrgScene;
    QString m_strStartDate;
    QString m_strStopDate;
    QString m_strCompletedDate;
    QString m_strStatus;         // 状态
    QString m_artist;         // Owner
    QString m_strLocalSavePath;
    QString m_strProjectSymbol;  // 场景名
    QString m_outputPath;        // 下载保存路径
    QString m_strError;          // 错误提示
    QString m_strDownloadFileName; // 当前下载文件名 abc.txt
    QString m_strSpeed;          // 当前速度字符串
    QString m_sPriority;         // 优先级字符串
    QString m_sPathCmd;          // 生成下载路径的cmd命令
    QString m_currProjectSavePath;  // 记录下载任务开始时的本地路径
    QString m_outputLabel;
    QString m_taskAlias;         // 任务别名
    QString m_localSaveAsPath;   // 另存路径
    QString m_taskType;          // 任务类型
    QString m_frameName;         // 帧名
    QHash<qint64, CDownloadState*> m_children;   // 子条目
    CDownloadState *m_parentItem;        // 父节点
    QMutex          m_mutex;             // 用于防止 children多线程操作
    bool            m_isParentItem;
    qint32          m_storageId;

    int             m_totalFrames;
    int             m_abortFrames;
    int             m_executingFrames;
    int             m_doneFrames;
    int             m_failedFrames;
    int             m_waitFrames;
    bool            m_isOpen;
    bool            m_isDelete;
    bool            m_isInDb;
    bool            m_isAutoDownload;  // 是否是自动下载
    qint64          m_startRenderDate;
    qint64          m_finishedRenderDate;
    QString         m_startRenderDateStr;
    QString         m_finishedRenderDateStr;
    float           m_transSpeed;
    QString         m_parentOutputLabel;
    int             m_rowNum;
    t_taskid        m_parentId;
    QString         m_logFileState; // DownloadState详情文件
    CLock           m_lock;
    CLock           m_lockSpeed;
    qint64          m_taskFileSize;
    qint64          m_nCustomerId;
    int             m_prorityType; // 自定义优先
    qint64          m_taskLastExecTime; // 任务最后执行时间
    qint64          m_timeElapsed; // 数据更新时间间隔
    qint64          m_frameId;
    int             m_jobType;     // 下载镜头、下载帧
    int             m_jobOperateType; // 自动、手动
    QStringList     m_frameOutput; // 输出帧路径
    QMap<QString, int> m_framesDownloadState;

    qint64 m_fileTotalSize;
    qint64 m_fileTransferSize;

    QString m_projectName;
    int m_downloadFinishTimes; // 下载完成次数

    QStringList m_outputType;
    QStringList m_downloadList;
    QStringList m_downloadedFiles;
};
Q_DECLARE_METATYPE(CDownloadState*)

#endif // DOWNLOADSTATE_H_
