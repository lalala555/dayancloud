/***********************************************************************
* Module:  UpdateManager.h
* Author:  hqr
* Modified: 2016/08/29 16:44:48
* Purpose: Declaration of the class
***********************************************************************/
#pragma once
#ifndef UPDATEMANAGER_H_
#define UPDATEMANAGER_H_

#include <QThread>
#include "UpdateWorker.h"

class UpdateManager : public QObject
{
    Q_OBJECT

public:
    UpdateManager(QObject* parent = 0);
    ~UpdateManager();

    void init(const QString & strUpdater, const QString & version);
    void checkUpdate(int flag, QObject * sink = 0);
    void performUpdate(int flag, QObject * sink = 0);

    void cancelUpdate();

    void waitUpdate();
    int exitCode();

    QString getVersionText(QString& strVersionInfo, int langId);
    QString getAnnounceText(int langId);

signals:
    void pauseWork();
    void operate(void *);
    void resultReady(int errorCode);
    void cmdArrived(const QString &);

private:
    void checkAction(UpdateTag * ut);
    void downloadAction(UpdateTag * ut);
public slots:
    void handleResults(int errorCode);
    void handleCmdArrived(const QString& data);
    void handleUpdateText(const QMap<int, UPDATE_TEXT>& data, const QString& version);

private:
    int m_exitCode;
    QThread m_workerThread;
    QObject* m_sink;
    QString m_strUpdater;
    QString m_version;
    QString m_new_version;
    QMap<int, UPDATE_TEXT> m_langid_text;
    UpdateCheckWorker* m_update_work;
};

#define UpdateManagerInstance QSingleton<UpdateManager>::instance()


#endif // UPDATEMANAGER_H_



