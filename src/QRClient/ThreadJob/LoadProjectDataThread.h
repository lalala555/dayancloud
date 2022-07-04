#pragma once

#include <QThread>

class LoadProjectDataThread : public QThread
{
    Q_OBJECT

public:
    LoadProjectDataThread(qint64 taskId, BaseTaskInfo* baseTaskInfo, QObject *parent = 0);
    ~LoadProjectDataThread();
    qint64 getTaskId();
    BaseTaskInfo* getBaseTaskInfo();
    int loadProjectData();
    void setLocatePage(int page) { m_page = page; }
    int getLocatePage() { return m_page; }

protected:
    virtual void run();

private:
    QString getProjectFilePath();
    bool loadPictureGroupInfo(const QJsonObject& task_json);
    bool loadCtrlPointInfo(const QJsonObject& task_json);
    bool loadPositionInfo(const QJsonObject& task_json);
    bool loadBlockInfo(const QJsonObject& task_json);

signals:
    void loadProjectDataFinished(int code);

private:
    qint64 m_taskId;
    BaseTaskInfo* m_baseTaskInfo;
    int m_page;
};
