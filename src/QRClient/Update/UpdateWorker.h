/***********************************************************************
* Module:  UpdateCheckWorker.h
* Author:  hqr
* Modified: 2016/08/29 16:44:48
* Purpose: Declaration of the class
***********************************************************************/
#pragma once
#ifndef UPDATECHECKWORKER_H_
#define UPDATECHECKWORKER_H_

#include <QThread>
#include "CoreBase/singleton.h"
#include "Process.h"
#include "RayUpdate/RayUpdate.h"

struct download_progress_message_t {
    unsigned long   total_file_count;
    unsigned long   current_file_index;         /* start from 1 */
    unsigned long   current_file_total_size;
    unsigned long   current_file_download_size;
    unsigned long   current_file_consume_time;  /* if 0 will rise to 1 */
    char            current_file_name[256];     /* file name is ansi-format */
};

struct UpdateTag {
    QString action;
    int     flag;
    QObject* reciver;
};

class UpdateCheckWorker : public QObject
{
    Q_OBJECT
public:
    UpdateCheckWorker(QString strUpdater, QString version);
    ~UpdateCheckWorker();
public slots:
    void doUpdateWork(void* parameter);
    void pauseUpdateWork();
private slots:
    void onDataArrived(const QString &data);
    void onUpdateTextArrived(const QMap<int, UPDATE_TEXT>& data, const QString& version);
signals:
    void resultReady(int errorCode);
    void dataArrived(const QString &);
    void cmdArrived(const QString &);
    void updateTextArrived(const QMap<int, UPDATE_TEXT>& data,  const QString& version);
private:
    QMutex mutex;
    QString m_version;
    QString m_strUpdater;
    RayUpdate* m_updater;
};

/*
typedef QSharedPointer<UpdateTag> UpdateTagPointer;
Q_DECLARE_METATYPE(UpdateTagPointer);
qRegisterMetaType<UpdateTagPointer>("UpdateTagPointer");
*/

#endif // UPDATECHECKWORKER_H_



