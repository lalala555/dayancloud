/***********************************************************************
* Module:  UploadFileDatabase.h
* Author:  yanglong
* Created: 2017/10/09 15:10:50
* Modifier: yanglong
* Modified: 2017/10/09 15:10:50
* Purpose: Declaration of the class
***********************************************************************/
#ifndef UPLOADFILEDATABASE_H
#define UPLOADFILEDATABASE_H

#include <QObject>
#include <QTextCodec>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTime>
#include <QSqlError>
#include <QtDebug>
#include <QSqlDriver>
#include <QSqlRecord>
#include <QFile>
#include <QDateTime>

struct UploadFileInfo {
    qint32  platform;
    qint32  storageID;
    qint64  fileSize;
    QString localPath;
    QString remotePath;
    QString userAccount;
    QString lastTime;
    QString updateTime;

    UploadFileInfo()
    {
        platform    = 0;
        fileSize    = 0;
        storageID   = 0;
        localPath   = "";
        remotePath  = "";
        userAccount = "";
        lastTime    = "";
        updateTime  = "";
    }
};

class UploadFileDatabase : public QObject
{
    Q_OBJECT

public:
    UploadFileDatabase(QObject *parent = 0);
    ~UploadFileDatabase();

    void insertBegin();
    void insertCommit();

    bool connectDatabase(const QString &path);
    bool createTable(const QString &name = "");
    bool checkTableExist(const QString &tablename);
    bool insert(const UploadFileInfo &info);
    UploadFileInfo query(const QString& localPath);
    UploadFileInfo query(const QString& localPath, const QString& remotePath);
    QList<UploadFileInfo> queryAll();
    bool deleteData(const QString& localPath, const QString& remotePath);
    bool deleteAll();
    bool updateData(const UploadFileInfo &info);
    qint64 getRecordCount(); //获取记录的数量
    bool clearOldestData(qint64 clearCount);// 删除老数据

private:
    QSqlDatabase m_db;
    QString m_tableName;
};



#endif // UPLOADFILEDATABASE_H
