#include "stdafx.h"
#include "UncompleteUploadFileDB.h"
#include "Kernel/IPacket.h"
#include "Common/StringUtil.h"

#define DBCONNECTNAME "UnuploadCompleteFile"

UncompleteUploadFileDB::UncompleteUploadFileDB(QObject *parent)
    : m_insertQuery(0)
    , m_isUpdatedTabStruct(false)
    , m_totalFileSize(0)
{
}

UncompleteUploadFileDB::~UncompleteUploadFileDB()
{
}

bool UncompleteUploadFileDB::connectDatabase(const QString &path)
{
    //检查数据库文件是否存在
    QString dbPath = path;
    m_db = QSqlDatabase::addDatabase("QSQLITE", dbPath);
    m_db.setDatabaseName(dbPath);
    if(!m_db.open()) {
        LOGFMTI("connect to db fail %s", qPrintable(m_db.lastError().text()));
        m_db.close();
        return false;
    }
    m_insertQuery = new QSqlQuery(m_db);
    return true;
}

bool UncompleteUploadFileDB::createTable(const QString &name)
{
    m_tableName = name;
    QString strSql = QString("CREATE TABLE %1("
                             "taskid INT,"
                             "projectid INT,"
                             "projectsymbol VARCHAR(128),"
                             "framename VARCHAR(128),"
                             "filepath VARCHAR(512),"
                             "rename VARCHAR(512),"
                             "rootid VARCHAR(16),"
                             "camera VARCHAR(128),"
                             "scene VARCHAR(256),"
                             "filesize BIGINT,"
                             "status BIT(8),"
                             "filetype INT,"
                             "loadtexturefile INT,"
                             "updatetime VARCHAR(32),"
                             "pathmd5 VARCHAR(256) PRIMARY KEY NOT NULL)").arg(m_tableName);

    QSqlQuery query(m_db);
    if(query.exec(strSql)) {
        qDebug() << "create table success!";
        return true;
    } else {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.text() << "create table failed!";
        return false;
    }
}

bool UncompleteUploadFileDB::checkTableExist(const QString &tablename)
{
    m_tableName = tablename;
    QSqlQuery query(m_db);
    bool isExist = query.exec(QString("select count(*) from sqlite_master where type='table' and name='%1'").arg(tablename));
    if(!isExist) {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.text() << "checkTableExist failed!";
    } else {
        if(query.next()) {
            if(query.value(0).toInt() == 0) {
                isExist = false;
            } else {
                isExist = true;
            }
        }
    }
    return isExist;
}

bool UncompleteUploadFileDB::insert(const UncompleteFile &file, const QString &tablename)
{
    QString tableName = tablename;
    QSqlQuery query(m_db);
    QString sql = QString("replace into %1 values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);").arg(tableName);
    query.prepare(sql);
    query.bindValue(0, file.taskid);
    query.bindValue(1, file.projectid);
    query.bindValue(2, file.projectsymbol);
    query.bindValue(3, file.framename);
    query.bindValue(4, file.local_file);
    query.bindValue(5, file.remote_file);
    query.bindValue(6, file.rootid);
    query.bindValue(7, file.camera);
    query.bindValue(8, file.scene);
    query.bindValue(9, file.filesize);
    query.bindValue(10, file.status);
    query.bindValue(11, file.filetype);
    query.bindValue(12, file.loadtexturefile);
    query.bindValue(13, file.updatetime);
    query.bindValue(14, file.pathMd5);

    if(!query.exec()) {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.text() << "insert data failed";
        LOGFMTE("[UncompleteUploadFileDB] insert db failed, path=%s", qPrintable(file.local_file));
        return false;
    }
    return true;
}

UncompleteFile* UncompleteUploadFileDB::queryOneData(const QString& localName, const QString& rename, const QString &tablename)
{
    UncompleteFile* file = new UncompleteFile();
    QString tableName = tablename;
    QSqlQuery query(m_db);
    QString sql = QString("select * from %1 where filepath='%2' and rename='%3';").arg(tableName).arg(localName).arg(rename);
    bool res = query.prepare(sql);
    res = query.exec();
    if(!res) {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.text() << "query data failed";
        return NULL;
    }

    while(query.next()) {
        file->taskid = query.value(0).toInt();
        file->projectid = query.value(1).toInt();
        file->projectsymbol = query.value(2).toString();
        file->framename = query.value(3).toString();
        file->local_file = query.value(4).toString();
        file->remote_file = query.value(5).toString();
        file->rootid = query.value(6).toString();
        file->camera = query.value(7).toString();
        file->scene  = query.value(8).toString();
        file->filesize = query.value(9).toString().toLongLong();
        file->status  = query.value(10).toInt();
        file->filetype = query.value(11).toInt();
        file->loadtexturefile = query.value(12).toInt();
        file->updatetime = query.value(13).toString();
        file->pathMd5 = query.value(14).toString();
    }

    return file;
}

QList<UncompleteFile> UncompleteUploadFileDB::queryAll(const QString &tablename, int states)
{
    QList<UncompleteFile> files;
    QString tableName = tablename;
    QSqlQuery query(m_db);
    QString sql = QString("select * from %1").arg(tableName);
    if(states != -1) {
        sql = QString("select * from %1 where status=%2").arg(tableName).arg(states);
    }
    bool res = query.exec(sql);
    if(!res) {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.text() << "queryAll data failed";
    }

    while(query.next()) {
        UncompleteFile file;
        file.taskid = query.value(0).toInt();
        file.projectid = query.value(1).toInt();
        file.projectsymbol = query.value(2).toString();
        file.framename = query.value(3).toString();
        file.local_file = query.value(4).toString();
        file.remote_file = query.value(5).toString();
        file.rootid = query.value(6).toString();
        file.camera = query.value(7).toString();
        file.scene  = query.value(8).toString();
        file.filesize = query.value(9).toString().toLongLong();
        file.status  = query.value(10).toInt();
        file.filetype = query.value(11).toInt();
        file.loadtexturefile = query.value(12).toInt();
        file.updatetime = query.value(13).toString();
        file.pathMd5 = query.value(14).toString();

        files.append(file);
    }

    return files;
}

bool UncompleteUploadFileDB::deleteOne(const qint64 taskid, const QString& rename, const QString &tablename)
{
    QString tableName = tablename;
    QSqlQuery query(m_db);
    QString sql = QString("delete from %1 where taskid=%2 and rename='%3';").arg(tableName).arg(taskid).arg(rename);

    if(!query.exec(sql)) {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.text() << "delete data failed";
        return false;
    }
    return true;
}

bool UncompleteUploadFileDB::deleteByTaskId(const qint64 taskid, const QString &tablename)
{
    QString tableName = tablename;
    QSqlQuery query(m_db);
    QString sql = QString("delete from %1 where taskid=%2;").arg(tableName).arg(taskid);

    if(!query.exec(sql)) {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.text() << "delete datas failed ! taskid = " << taskid  ;
        return false;
    }
    qDebug() << "delete datas success ! taskid = " << taskid  ;
    return true;
}

qint64 UncompleteUploadFileDB::getRecordCount(const QString &tablename, int states) //获取记录的数量
{
    QString tableName = tablename;
    qint64 count = 0;
    QSqlQuery query(m_db);
    QString sql = QString("SELECT count(*) FROM %1").arg(tableName);
    if(states != -1) {
        sql = QString("SELECT count(*) FROM %1 WHERE status=%2").arg(tableName).arg(states);
    }
    query.exec(sql);
    while(query.next()) {
        count = query.value(0).toInt();
    }
    return count;
}

bool UncompleteUploadFileDB::updateData(const UncompleteFile &file)
{
    if(m_insertQuery == nullptr) {
        m_insertQuery = new QSqlQuery(m_db);
    }

    m_insertQuery->bindValue(0, file.taskid);
    m_insertQuery->bindValue(1, file.projectid);
    m_insertQuery->bindValue(2, file.projectsymbol);
    m_insertQuery->bindValue(3, file.framename);
    m_insertQuery->bindValue(4, file.local_file);
    m_insertQuery->bindValue(5, file.remote_file);
    m_insertQuery->bindValue(6, file.rootid);
    m_insertQuery->bindValue(7, file.camera);
    m_insertQuery->bindValue(8, file.scene);
    m_insertQuery->bindValue(9, file.filesize);
    m_insertQuery->bindValue(10, file.status);
    m_insertQuery->bindValue(11, file.filetype);
    m_insertQuery->bindValue(12, file.loadtexturefile);
    m_insertQuery->bindValue(13, file.updatetime);
    m_insertQuery->bindValue(14, file.pathMd5);

    if(!m_insertQuery->exec()) {
        QSqlError lastError = m_insertQuery->lastError();
        qDebug() << lastError.text() << "update data failed ! rename = " << file.remote_file ;
        LOGFMTE("[UncompleteUploadFileDB] update db failed, path=%s", qPrintable(file.local_file));
        return false;
    }
    return true;
}

void UncompleteUploadFileDB::operationBegin()
{
    m_db.transaction();
}

void UncompleteUploadFileDB::operationCommit()
{
    m_db.commit();
}

void UncompleteUploadFileDB::closeConnect()
{
    if(m_db.isOpen())
        m_db.close();
}

bool UncompleteUploadFileDB::execSqlPrepare(const QString &tablename)
{
    QString tableName = tablename;
    QString sql = QString("replace into %1 values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);").arg(tableName);

    if(m_insertQuery == nullptr) {
        m_insertQuery = new QSqlQuery(m_db);
    }

    if(!m_insertQuery->prepare(sql)) {
        QSqlError lastError = m_insertQuery->lastError();
        qDebug() << lastError.text() << "update data failed !";
        LOGFMTE("[UncompleteUploadFileDB] update prepare db failed, error msg is %s", qPrintable(lastError.text()));
        return false;
    }
    return true;
}

int UncompleteUploadFileDB::getTableColCount(const QString &tablename)
{
    QString tableName = tablename;
    qint64 count;
    QSqlQuery query(m_db);
    QString sql = QString("PRAGMA table_info(\"%1\")").arg(tableName);
    query.exec(sql);
    while(query.next()) {
        count = query.value(0).toInt();
    }
    return count;
}

bool UncompleteUploadFileDB::checkTableNeedUpdate(const QString &old_tablename, const QString &new_tablename)
{
    // 获取当前表的字段数
    int old_table = getTableColCount(old_tablename);
    int new_table = getTableColCount(new_tablename);

    return old_table != new_table;
}

bool UncompleteUploadFileDB::renameTableName(const QString &old_tablename, const QString &new_tablename)
{
    // 删除表
    QSqlQuery query(m_db);
    QString sql = QString("drop table %1").arg(old_tablename);
    if(!query.exec(sql)) {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.text() << "drop " <<  old_tablename << " table failed!";
        return false;
    }

    // 重命名表名
    sql = QString("ALTER TABLE %1 RENAME TO %2").arg(new_tablename).arg(old_tablename);
    if(!query.exec(sql)) {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.text() << "rename " <<  old_tablename << " table failed!";
        return false;
    }

    return true;
}

bool UncompleteUploadFileDB::copyOldTableData(const QString &old_tablename, const QString &new_tablename)
{
    // sql语句预处理
    if(!execSqlPrepare(new_tablename))
        return false;
    // 开启事物操作 提升插入效率
    operationBegin();

    QList<UncompleteFile> fileList = queryAll(old_tablename);
    if(fileList.isEmpty()) return false;

    foreach(UncompleteFile file, fileList) {
        file.pathMd5 = String::getStringMd5(file.local_file + file.remote_file);
        updateData(file);
    }

    operationCommit();

    return true;
}

qint64 UncompleteUploadFileDB::queryUploadCompletedCount(const QString &tablename)
{
    return this->getRecordCount(tablename, FILE_UPLOADED & FILE_CHECK_SUCCESS);
}

bool UncompleteUploadFileDB::updateDatasStatus(const QString &tablename, int factor, int updateStatus)
{
    QSqlQuery query(m_db);
    QString sql = QString("update %1 set status=:updateStatus where status=:factor").arg(tablename);

    query.prepare(sql);
    query.bindValue(":updateStatus", updateStatus);
    query.bindValue(":factor", factor);

    if(!query.exec()) {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.text() << "update data failed";
        return false;
    }
    return true;
}

qint64 UncompleteUploadFileDB::queryDataCountByStatus(const QString &tablename, int status)
{
    return this->getRecordCount(tablename, status);
}

bool UncompleteUploadFileDB::updateDataStatus(const QString &tablename, const QString& pathMd5, int updateStatus)
{
    QSqlQuery query(m_db);
    QString sql = QString("update %1 set status=:updateStatus where pathmd5=:pathMd5").arg(tablename);

    query.prepare(sql);
    query.bindValue(":updateStatus", updateStatus);
    query.bindValue(":pathMd5", pathMd5);

    if(!query.exec()) {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.text() << "update data failed";
        return false;
    }
    return true;
}

bool UncompleteUploadFileDB::updateCheckSuccessStatus(const QString &tablename, const QString& pathMd5, int factor, int updateStatus)
{
    QSqlQuery query(m_db);
    QString sql = QString("update %1 set status=:updateStatus where status!=:factor and pathmd5=:pathMd5").arg(tablename);

    query.prepare(sql);
    query.bindValue(":updateStatus", updateStatus);
    query.bindValue(":factor", factor);
    query.bindValue(":pathMd5", pathMd5);

    if(!query.exec()) {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.text() << "update data failed";
        return false;
    }
    return true;
}

QList<UncompleteFile> UncompleteUploadFileDB::queryDatasByStatus(const QString &tablename, int status)
{
    return this->queryAll(tablename, status);
}

qint64 UncompleteUploadFileDB::queryOneDataStatus(const QString &tablename, const QString& pathMd5)
{
    int status = -1;
    QString tableName = tablename;
    QSqlQuery query(m_db);
    QString sql = QString("select status from %1 where pathmd5='%2';").arg(tableName).arg(pathMd5);
    bool res = query.prepare(sql);
    res = query.exec();
    if(!res) {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.text() << "query data failed";
        return status;
    }

    while(query.next()) {
        status = query.value(0).toInt();
    }

    return status;
}

qint64 UncompleteUploadFileDB::queryUnuploadDatas(const QString &tablename, QList<UncompleteFile>& unuploads)
{
    QSqlQuery query(m_db);
    QString sql = QString("select *,rowid from %1 where status>0").arg(tablename);
    bool res = query.exec(sql);
    if(!res) {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.text() << "queryAll data failed";
        return 0;
    }

    while(query.next()) {
        UncompleteFile file;
        file.taskid = query.value(0).toInt();
        file.projectid = query.value(1).toInt();
        file.projectsymbol = query.value(2).toString();
        file.framename = query.value(3).toString();
        file.local_file = query.value(4).toString();
        file.remote_file = query.value(5).toString();
        file.rootid = query.value(6).toString();
        file.camera = query.value(7).toString();
        file.scene  = query.value(8).toString();
        file.filesize = query.value(9).toString().toLongLong();
        file.status  = query.value(10).toInt();
        file.filetype = query.value(11).toInt();
        file.loadtexturefile = query.value(12).toInt();
        file.updatetime = query.value(13).toString();
        file.pathMd5 = query.value(14).toString();
        file.jobid = query.value(15).toInt();

        unuploads.append(file);
    }

    return unuploads.count();
}

UncompleteFile UncompleteUploadFileDB::queryOneDataByStatus(const QString &tablename, const QString& pathMd5, int status)
{
    UncompleteFile file;
    QSqlQuery query(m_db);
    QString sql = QString("select *,rowid from %1 where pathmd5='%2' and status=%3;").arg(tablename).arg(pathMd5).arg(status);
    bool res = query.prepare(sql);
    res = query.exec();
    if(!res) {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.text() << "query data failed";
        return file;
    }

    while(query.next()) {
        file.taskid = query.value(0).toInt();
        file.projectid = query.value(1).toInt();
        file.projectsymbol = query.value(2).toString();
        file.framename = query.value(3).toString();
        file.local_file = query.value(4).toString();
        file.remote_file = query.value(5).toString();
        file.rootid = query.value(6).toString();
        file.camera = query.value(7).toString();
        file.scene  = query.value(8).toString();
        file.filesize = query.value(9).toString().toLongLong();
        file.status  = query.value(10).toInt();
        file.filetype = query.value(11).toInt();
        file.loadtexturefile = query.value(12).toInt();
        file.updatetime = query.value(13).toString();
        file.pathMd5 = query.value(14).toString();
        file.jobid = query.value(15).toInt();
    }

    return file;
}

UncompleteFile UncompleteUploadFileDB::queryOneDataByTaskId(const QString& tablename, t_taskid taskid)
{
    UncompleteFile file;

    QSqlQuery query(m_db);
    QString sql = QString("select *,rowid from %1 where taskid=%2;").arg(tablename).arg(taskid);
    bool res = query.prepare(sql);
    res = query.exec();
    if(!res) {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.text() << "query data failed";
        return file;
    }

    while(query.next()) {
        file.taskid = query.value(0).toInt();
        file.projectid = query.value(1).toInt();
        file.projectsymbol = query.value(2).toString();
        file.framename = query.value(3).toString();
        file.local_file = query.value(4).toString();
        file.remote_file = query.value(5).toString();
        file.rootid = query.value(6).toString();
        file.camera = query.value(7).toString();
        file.scene  = query.value(8).toString();
        file.filesize = query.value(9).toString().toLongLong();
        file.status  = query.value(10).toInt();
        file.filetype = query.value(11).toInt();
        file.loadtexturefile = query.value(12).toInt();
        file.updatetime = query.value(13).toString();
        file.pathMd5 = query.value(14).toString();
        file.jobid = query.value(15).toInt();
        break;
    }

    return file;
}

bool UncompleteUploadFileDB::isUpdatedTabStruct()
{
    return m_isUpdatedTabStruct;
}

void UncompleteUploadFileDB::setUpdateTabStruct(bool updated)
{
    m_isUpdatedTabStruct = updated;
}

void UncompleteUploadFileDB::setTableName(const QString& tabName)
{
    m_tableName = tabName;
}

bool UncompleteUploadFileDB::preUpdateDataStatus(const QString &tablename)
{
    QString tableName = tablename;
    QString sql = QString("update %1 set status=:updateStatus where pathmd5=:pathMd5;").arg(tableName);

    if(m_insertQuery == nullptr) {
        m_insertQuery = new QSqlQuery(m_db);
    }

    if(!m_insertQuery->prepare(sql)) {
        QSqlError lastError = m_insertQuery->lastError();
        qDebug() << lastError.text() << "update data failed !";
        LOGFMTE("[UncompleteUploadFileDB] update prepare db failed");
        return false;
    }
    return true;
}

bool UncompleteUploadFileDB::preUpdateStatus(const QString& pathMd5, int updateStatus)
{
    if(m_insertQuery == nullptr)
        return false;

    m_insertQuery->bindValue(":updateStatus", updateStatus);
    m_insertQuery->bindValue(":pathMd5", pathMd5);

    if(!m_insertQuery->exec()) {
        QSqlError lastError = m_insertQuery->lastError();
        qDebug() << lastError.text() << "update data failed ! rename = " << pathMd5 ;
        LOGFMTE("[UncompleteUploadFileDB] update db failed, path=%s", qPrintable(pathMd5));
        return false;
    }
    return true;
}

qint64 UncompleteUploadFileDB::getTextureFileCount(const QString &tablename)
{
    QString tableName = tablename;
    qint64 count = 0;
    QSqlQuery query(m_db);
    QString sql = QString("SELECT count(*) FROM %1 WHERE filetype=%2").arg(tableName).arg(TYPE_SCENE);

    query.exec(sql);
    while (query.next()) {
        count = query.value(0).toInt();
    }
    return count;
}

qint64 UncompleteUploadFileDB::getFileSize(const QString &tablename, int status)
{
    QString tableName = tablename;
    qint64 count = 0;
    QSqlQuery query(m_db);
    QString sql = QString("SELECT sum(filesize) FROM %1").arg(tableName);
    if (status != -1) {
        sql = QString("SELECT sum(filesize) FROM %1 WHERE status=%2").arg(tableName).arg(status);
    }
    query.exec(sql);
    while (query.next()) {
        count = query.value(0).toString().toLongLong();
    }
    return count;
}

qint64 UncompleteUploadFileDB::queryUploadCompletedFileSize(const QString &tablename) // 已经完成的文件大小
{
    return getFileSize(tablename, FILE_UPLOADED & FILE_CHECK_SUCCESS);
}

qint64 UncompleteUploadFileDB::queryTotalUploadFileSize(const QString &tablename) // 所有文件的总大小
{
    if (m_totalFileSize <= 0) {
        m_totalFileSize = getFileSize(tablename, -1);
    }
    return m_totalFileSize;
}

qint64 UncompleteUploadFileDB::queryFileSize(const QString &tablename, const QString& pathMd5)
{
    qint64 filesize = 0;
    QString tableName = tablename;
    QSqlQuery query(m_db);
    QString sql = QString("select filesize from %1 where pathmd5='%2';").arg(tableName).arg(pathMd5);
    bool res = query.prepare(sql);
    res = query.exec();
    if (!res) {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.text() << "query data failed";
        return filesize;
    }

    while (query.next()) {
        filesize = query.value(0).toInt();
    }

    return filesize;
}

QList<UncompleteFile> UncompleteUploadFileDB::queryUploadFinishedRecord(const QString &tablename)
{
    QList<UncompleteFile> items;
    QSqlQuery query(m_db);
    QString sql = QString("select * from %1 where status=0").arg(tablename);
    bool res = query.exec(sql);
    if (!res) {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.text() << "queryAll data failed";
        return items;
    }

    while (query.next()) {
        UncompleteFile file;
        file.taskid = query.value(0).toInt();
        file.projectid = query.value(1).toInt();
        file.projectsymbol = query.value(2).toString();
        file.framename = query.value(3).toString();
        file.local_file = query.value(4).toString();
        file.remote_file = query.value(5).toString();
        file.rootid = query.value(6).toString();
        file.camera = query.value(7).toString();
        file.scene = query.value(8).toString();
        file.filesize = query.value(9).toString().toLongLong();
        file.status = query.value(10).toInt();
        file.filetype = query.value(11).toInt();
        file.loadtexturefile = query.value(12).toInt();
        file.updatetime = query.value(13).toString();
        file.pathMd5 = query.value(14).toString();
        file.jobid = query.value(15).toInt();

        items.append(file);
    }

    return items;
}