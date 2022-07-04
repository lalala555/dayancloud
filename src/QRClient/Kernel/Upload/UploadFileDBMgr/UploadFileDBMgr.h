/***********************************************************************
* Module:  UploadFileDBMgr.h
* Author:  yanglong
* Created: 2017/10/09 15:10:38
* Modifier: yanglong
* Modified: 2017/10/09 15:10:38
* Purpose: Declaration of the class
***********************************************************************/
#pragma once

#include <QObject>
#include <QTextCodec>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTime>
#include <QSqlError>
#include <QtDebug>
#include <QSqlDriver>
#include <QSqlRecord>
#include <QMutex>
#include <QMutexLocker>
#include <QVector>
#include <QPair>

#include "Kernel/TaskElement.h"
class UncompleteUploadFileDB;
class UploadFileDatabase;
typedef QVector<QPair<QString, QString>> DirFilesVec;

enum FileRecordState {
    FILE_NOT_EXIST = 0,
    FILE_EXIST     = 1,
    FILE_MODIFY    = 2,
    UPLOAD_DIR     = 3, // �ϴ������ļ���
    DIR_FILE_MODIFY = 4,
    DIR_NOT_MODIFY  = 5,
};

class UploadFileDBMgr : public QObject
{
    Q_OBJECT

public:
    ~UploadFileDBMgr();
    static UploadFileDBMgr* getInstance()
    {
        static UploadFileDBMgr instance;
        return &instance;
    }

    bool init();

    // UploadFileDatabase��صĲ���
    bool insert(const int storageid, const QString& localPath, const QString& remotePath);
    bool update(const int storageid, const QString& localPath, const QString& remotePath);
    bool deleteOneRecord(const QString& localPath, const QString& remotePath);
    bool deleteAll();
    FileRecordState checkRecordState(const QString& localPath, const QString& remotePath); // ����¼�Ƿ����
    bool clearOldestData();// ������ϵ�����
    DirFilesVec& getDirNeedUploadFiles();

    // UnuploadCompleteFileDB ��ز���
    bool insertTaskRecord(const QString& dbpath, TaskElementPtr task);
    bool updateTaskRecord(const QString& dbpath, TaskElementPtr task);
    TaskElementPtr findUnfinished(const QString& dbpath, TaskElementPtr pTaskElement);
    bool deleteOneTaskRecord(const QString& dbpath, const qint64 taskid, const QString& rename);
    bool deleteAllTaskRecord(const QString& dbpath, const qint64 taskid);
    bool updateTableStruct(UncompleteUploadFileDB& db);//���±�ṹ
private:
    UploadFileDBMgr(QObject *parent = 0);
    // ����ļ�������Ҫ�����ϴ����ļ�
    qint64 checkDirNeedUploadFile(const QString& localPath, const QString& remotePath, DirFilesVec &needUploadFile);

private:
    QMutex m_mutex;
    UploadFileDatabase *m_uploadDb;
    UncompleteUploadFileDB *m_unCompeleteUploadDb;
    DirFilesVec m_dirNeedUploadFiles;
};

