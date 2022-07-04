#pragma once

#include <QtCore>
#include "TransEngine/TransformEngine.h"

struct DownloadInfo {
    QString localPath;       //下载文件保存的本地路径(只能在下载queue中获取)
    QString remotePath;      //下载文件保存的本地路径
    t_taskid taskId;         //任务id（用户选择的下载的任务id）
    t_taskid parentTaskId;
    t_uid m_uid;               //userid（从db配置中获取）
    int storagetId;         // 需要下载的projectId定位
    bool isVip;
    bool isIgnoreUserId;     //忽略userid下载，可下载任意文件
    bool isClone;
    int  priority;
    int  priorityTime;
    bool bAutoDownload;
    int index;
    QStringList localPathList;
    QStringList remotePathList;
    DownloadInfo()
    {
        taskId = 0;
        priority = 0;
        priorityTime = 0;
        bAutoDownload = false;
        index = 0;
    }
};

class CDownloadPacket : public IPacket, public IDownloadSink
{
public:
    CDownloadPacket();
    ~CDownloadPacket(void);
public:
    virtual void on_download(const download_callback_info_t & callback_info) override;
    int translateRayvisionCallback(const download_callback_info_t & callback_info);
    bool init(const DownloadInfo* info, AbstractTransEngine *engine);
    bool start()
    {
        if(m_runStatus == PACKET_STOP /*|| m_runStatus == PACKET_RUNNING*/)
            return false;
        m_runStatus = PACKET_RUNNING;
        return true;
    }
    void stop()
    {
        m_runStatus = PACKET_STOP;
    }
    bool isRunning()
    {
        return m_runStatus == PACKET_RUNNING;
    }
    t_taskid getTaskId()
    {
        return m_taskId;
    }

    t_taskid getParentId()
    {
        return m_parentId;
    }
    t_uid getUserId()
    {
        return m_uid;
    }
    QString getLocalPath()
    {
        return m_localPath;
    }
    void clearPacket();
    int getStorageId()
    {
        return m_projectId;
    }
    QString getRemotePath()
    {
        return m_remotePath;
    }
    bool getVip()
    {
        return m_isVip;
    }
    bool getIgnoreUserId()
    {
        return m_isIgnoreUserId;
    }
    void setLocalPath(QString strPath)
    {
        m_localPath = strPath;
    }
    bool getClone()
    {
        return m_isClone;
    }
    void setPriority(int priority)
    {
        m_priority = priority;
    }
    int getPriority()
    {
        return m_priority;
    }
    void setPriorityTime(int priorityTime)
    {
        m_priorityTime = priorityTime;
    }
    int getPriorityTime()
    {
        return m_priorityTime;
    }
    bool isAutoDownload()
    {
        return m_bAutoDownload;
    }
    void startTask();
    void stopTask();
    void updatePriority(int priority);
    void updateTimePriority(unsigned int timePriority);
    void setPacketIndex(int index) { _packetIndex = index; }
    void setTransEngine(AbstractTransEngine *engine) { m_engine = engine; }
    void setLocalPathList(const QStringList& pathList)
    {
        m_localPathList = pathList;
    }
    QStringList getLocalPathList()
    {
        return m_localPathList;
    }
    void setRemotePathList(const QStringList& pathList)
    {
        m_remotePathList = pathList;
    }
    QStringList getRemotePathList()
    {
        return m_remotePathList;
    }
public:
    QString m_localPath;  // 下载任务按目录为单位
    QString m_remotePath;
    t_taskid m_taskId;
    t_taskid m_parentId;
    t_uid m_uid;
    int m_projectId;
    bool m_isVip;
    bool m_isIgnoreUserId;
    bool m_isClone;
    int  m_priority;        // 下载优先级0普通，1为高
    int  m_priorityTime;
    bool m_bAutoDownload;   // 区分手动下载还是自动下载
    AbstractTransEngine *m_engine;
    QStringList m_localPathList;
    QStringList m_remotePathList;
};
