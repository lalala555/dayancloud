/***********************************************************************
* Module:  UncompleteUploadFileDB.h
* Author:  yanglong
* Created: 2017/10/09 15:12:43
* Modifier: yanglong
* Modified: 2017/10/09 15:12:43
* Purpose: Declaration of the class
***********************************************************************/
#ifndef UNUPLOADCOMPLETEFILEDB_H
#define UNUPLOADCOMPLETEFILEDB_H

#include <QObject>
#include <QTextCodec>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTime>
#include <QSqlError>
#include <QtDebug>
#include <QSqlDriver>
#include <QSqlRecord>
#include <QList>

struct UncompleteFile {
    qint64 jobid;
    qint64 taskid;
    qint64 projectid;
    QString projectsymbol;
    QString framename;
    QString camera;
    QString scene;
    QString local_file;
    qint64 filesize;
    QString remote_file;
    QString rootid;
    QString updatetime;
    int filetype;
    int status;
    int loadtexturefile;
    QString pathMd5;

    UncompleteFile()
    {
        taskid          = 0;
        projectid       = 0;
        filesize        = 0;
        status          = -1;
        loadtexturefile = 0;
        projectsymbol   = "";
        framename       = "";
        camera          = "";
        scene           = "";
        local_file        = "";
        remote_file          = "";
        rootid          = "";
        updatetime      = "";
        filetype        = 0;
        pathMd5         = "";
     }

    bool operator==(const UncompleteFile& rhs){
        return (taskid == rhs.taskid) && (projectid == rhs.projectid) && (filesize == rhs.filesize)
            && (status == rhs.status) && (loadtexturefile == rhs.loadtexturefile) && (projectsymbol == rhs.projectsymbol)
            && (framename == rhs.framename) && (camera == rhs.camera) && (scene == rhs.scene) && (local_file == rhs.local_file)
            && (remote_file == rhs.remote_file) && (rootid == rhs.rootid) && (filetype == rhs.filetype) && (pathMd5 == rhs.pathMd5);
    }
};

class UncompleteUploadFileDB
{
public:
    UncompleteUploadFileDB(QObject *parent = 0);
    ~UncompleteUploadFileDB();

    void operationBegin();
    void operationCommit();
    void closeConnect();
    void setTableName(const QString& tabName);

    bool connectDatabase(const QString &path);
    bool createTable(const QString &name);
    bool checkTableExist(const QString &tablename);
    bool insert(const UncompleteFile &file, const QString &tablename);
    UncompleteFile* queryOneData(const QString& localName, const QString& rename, const QString &tablename);
    QList<UncompleteFile> queryAll(const QString &tablename, int states = -1);
    bool deleteOne(const qint64 taskid, const QString& rename, const QString &tablename);
    bool deleteByTaskId(const qint64 taskid, const QString &tablename);
    qint64 getRecordCount(const QString &tablename, int states = -1); //获取记录的数量
    bool updateData(const UncompleteFile &file);
    bool execSqlPrepare(const QString &tablename);
    int  getTableColCount(const QString &tablename); // 获取表的列数
    bool checkTableNeedUpdate(const QString &old_tablename, const QString &new_tablename);
    bool renameTableName(const QString &old_tablename, const QString &new_tablename);
    bool copyOldTableData(const QString &old_tablename, const QString &new_tablename);
    bool isUpdatedTabStruct();
    void setUpdateTabStruct(bool updated);
    bool updateDatasStatus(const QString &tablename, int factor, int updateStatus);
    bool updateDataStatus(const QString &tablename, const QString& pathMd5, int updateStatus);
    bool updateCheckSuccessStatus(const QString &tablename, const QString& pathMd5, int factor, int updateStatus);
    qint64 getTextureFileCount(const QString &tablename); //获取贴图文件的数量

    qint64 queryUploadCompletedCount(const QString &tablename);    // 已经上传完成的任务数量(上传和校验都ok)
    qint64 queryDataCountByStatus(const QString &tablename, int status); // 查询各个状态的任务数量
    QList<UncompleteFile> queryDatasByStatus(const QString &tablename, int status);
    UncompleteFile queryOneDataByStatus(const QString &tablename, const QString& pathMd5, int status);
    qint64 queryOneDataStatus(const QString &tablename, const QString& pathMd5);
    qint64 queryUnuploadDatas(const QString &tablename, QList<UncompleteFile>& unuploads);
    UncompleteFile queryOneDataByTaskId(const QString& tablename, t_taskid taskid);
    qint64 getFileSize(const QString &tablename, int status);
    qint64 queryUploadCompletedFileSize(const QString &tablename); // 已经完成的文件大小
    qint64 queryTotalUploadFileSize(const QString &tablename); // 所有文件的总大小
    qint64 queryFileSize(const QString &tablename, const QString& pathMd5);
    QList<UncompleteFile> queryUploadFinishedRecord(const QString &tablename);

    // 使用事务来更新表
    bool preUpdateDataStatus(const QString &tablename);
    bool preUpdateStatus(const QString& pathMd5, int updateStatus);

public:
    QSqlDatabase m_db;
    QString m_tableName;
    QSqlQuery *m_insertQuery;
    bool m_isUpdatedTabStruct;
    qint64 m_totalFileSize;
};
#endif // UNUPLOADCOMPLETEFILEDB_H
