#include "stdafx.h"
#include "UploadFileDatabase.h"

#define DBCONNECTNAME "UploadFile"

UploadFileDatabase::UploadFileDatabase(QObject *parent)
    : QObject(parent)
{
}

UploadFileDatabase::~UploadFileDatabase()
{
    m_db.close();
}

bool UploadFileDatabase::connectDatabase(const QString &path)
{
    //检查数据库文件是否存在
    QString dbPath = path;
    m_db = QSqlDatabase::addDatabase("QSQLITE", DBCONNECTNAME);
    m_db.setDatabaseName(dbPath);
    if(!m_db.open()) {
        qDebug() << "connect to db fail" << m_db.lastError().text();
        LOGFMTI("connect to db fail %s", qPrintable(m_db.lastError().text()));
        m_db.close();
        return false;
    }
    return true;
}

bool UploadFileDatabase::createTable(const QString &name)
{
    m_tableName = name;
    QString strSql = QString("CREATE TABLE IF NOT EXISTS %1("
                             "platform INT,"
                             "storageid INT,"
                             "useraccount VARCHAR(32),"
                             "localpath VARCHAR(512) NOT NULL,"
                             "remotepath VARCHAR(512) PRIMARY KEY,"
                             "filesize INT,"
                             "lasttime VARCHAR(32),"
                             "updatetime VARCHAR(32))").arg(m_tableName);

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

bool UploadFileDatabase::checkTableExist(const QString &tablename)
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

bool UploadFileDatabase::insert(const UploadFileInfo &info)
{
    QString tableName = m_tableName;
    QSqlQuery query(m_db);
    QString sql = QString("insert into %1 values(? ,? ,? ,? , ?, ?, ?, ?);").arg(tableName);
    query.prepare(sql);
    query.bindValue(0, info.platform);
    query.bindValue(1, info.storageID);
    query.bindValue(2, info.userAccount);
    query.bindValue(3, info.localPath);
    query.bindValue(4, info.remotePath);
    query.bindValue(5, info.fileSize);
    query.bindValue(6, info.lastTime);
    query.bindValue(7, info.updateTime);

    if(!query.exec()) {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.text() << "insert data failed";
        return false;
    }
    return true;
}

UploadFileInfo UploadFileDatabase::query(const QString& localPath)
{
    QString tableName = m_tableName;
    UploadFileInfo info;

    QSqlQuery query(m_db);
    QString sql = QString("select * from %1 where localpath='%2'").arg(tableName).arg(localPath);
    bool res = query.exec(sql);

    while(query.next()) {
        info.platform    = query.value(0).toInt();
        info.storageID   = query.value(1).toInt();
        info.userAccount = query.value(2).toString();
        info.localPath   = query.value(3).toString();
        info.remotePath  = query.value(4).toString();
        info.fileSize    = query.value(5).toInt();
        info.lastTime    = query.value(6).toString();
        info.updateTime  = query.value(7).toString();
    }
    return info;
}

UploadFileInfo UploadFileDatabase::query(const QString& localPath, const QString& remotePath)
{
    QString tableName = m_tableName;
    UploadFileInfo info;

    QSqlQuery query(m_db);
    QString sql = QString("select * from %1 where localpath='%2' and remotepath='%3'")
                  .arg(tableName).arg(localPath).arg(remotePath);
    bool res = query.exec(sql);

    while(query.next()) {
        info.platform    = query.value(0).toInt();
        info.storageID   = query.value(1).toInt();
        info.userAccount = query.value(2).toString();
        info.localPath   = query.value(3).toString();
        info.remotePath  = query.value(4).toString();
        info.fileSize    = query.value(5).toInt();
        info.lastTime    = query.value(6).toString();
        info.updateTime  = query.value(7).toString();
    }
    return info;
}

QList<UploadFileInfo> UploadFileDatabase::queryAll()
{
    QString tableName = m_tableName;
    QList<UploadFileInfo> infos;

    QSqlQuery query(m_db);
    bool res = query.exec("select * from " + tableName);
    qDebug() << "total record counts " << query.record().count();

    while(query.next()) {
        UploadFileInfo info;
        info.platform    = query.value(0).toInt();
        info.storageID   = query.value(1).toInt();
        info.userAccount = query.value(2).toString();
        info.localPath   = query.value(3).toString();
        info.remotePath  = query.value(4).toString();
        info.fileSize    = query.value(5).toInt();
        info.lastTime    = query.value(6).toString();
        info.updateTime  = query.value(7).toString();
        infos.append(info);
    }
    return infos;
}

bool UploadFileDatabase::deleteData(const QString& localPath, const QString& remotePath)
{
    QString tableName = m_tableName;
    QSqlQuery query(m_db);
    QString sql = QString("delete from %1 where localpath='%2' and remotepath='%3'")
                  .arg(tableName).arg(localPath).arg(remotePath);

    bool res = query.exec(sql);
    if(res) {
        qDebug() << "delete data success ";
        return true;
    } else {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.text() << "delete data failed";
        return false;
    }
}

bool UploadFileDatabase::deleteAll()
{
    QString tableName = m_tableName;
    QSqlQuery query(m_db);
    QString sql = QString("delete from %1").arg(tableName);
    bool res = query.exec(sql);
    if(res) {
        qDebug() << "delete all datas success ";
        return true;
    } else {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.text() << "delete all datas failed";
        return false;
    }
}

bool UploadFileDatabase::updateData(const UploadFileInfo &info)
{
    QString tableName = m_tableName;
    QSqlQuery query(m_db);
    QString sql = QString("update %1 set platform=:platform,storageid=:storageid,useraccount=:useraccount,"
                          "localpath=:localpath,remotepath=:remotepath,filesize=:filesize,lasttime=:lasttime,updatetime=:updatetime "
                          "where filepath=:filepath and remotepath=:remotepath").arg(tableName);

    query.prepare(sql);
    query.bindValue(":platform", info.platform);
    query.bindValue(":filesize", info.storageID);
    query.bindValue(":useraccount", info.userAccount);
    query.bindValue(":localpath", info.localPath);
    query.bindValue(":remotepath", info.remotePath);
    query.bindValue(":filesize", info.fileSize);
    query.bindValue(":lasttime", info.lastTime);
    query.bindValue(":updatetime", info.updateTime);
    query.bindValue(":localpath", info.localPath);
    query.bindValue(":remotepath", info.remotePath);

    if(!query.exec()) {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.text() << "update data failed";
        return false;
    }
    return true;
}

void UploadFileDatabase::insertBegin()
{
    m_db.transaction();
}

void UploadFileDatabase::insertCommit()
{
    m_db.commit();
}

qint64 UploadFileDatabase::getRecordCount()
{
    QString tableName = m_tableName;
    qint64 count;
    QSqlQuery query(m_db);
    QString sql = QString("SELECT count(*) FROM %1").arg(tableName);
    query.exec(sql);
    while(query.next()) {
        count = query.value(0).toInt();
    }
    return count;
}

bool UploadFileDatabase::clearOldestData(qint64 clearCount)
{
    QString tableName = m_tableName;
    QString tempTable = "temptable";
    QSqlQuery query(m_db);

    //创建一个临时表
    QString sql = QString("CREATE TEMPORARY TABLE %1("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "updatetime VARCHAR(32));").arg(tempTable);
    if(query.exec(sql)) {
        qDebug() << "create temp table success!";
    } else {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.text() << "create temp table failed!";
        return false;
    }

    // 将查询结果存入临时表中
    sql = QString("INSERT INTO %1(updatetime) SELECT updatetime FROM %2 order by updatetime ASC").arg(tempTable).arg(tableName);
    if(!query.exec(sql)) {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.text() << "insert data to temp table failed!";
        return false;
    }

    // 通过临时表中的index找到要删除的节点时间
    sql = QString("SELECT updatetime FROM %1 where id=%2").arg(tempTable).arg(clearCount);
    if(!query.exec(sql)) {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.text() << "qurey data from temp table failed!";
        return false;
    }

    QString lasttime;
    while(query.next()) {
        lasttime = query.value(0).toString();
    }

    // 使用找到的时间节点去删除主表中的数据
    sql = QString("delete from %1 where updatetime <= '%2'").arg(tableName).arg(lasttime);
    if(!query.exec(sql)) {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.text() << "delete data from temp table failed!";
        return false;
    }

    // 删除临时表
    sql = QString("drop table %1").arg(tempTable);
    if(!query.exec(sql)) {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.text() << "drop temp table failed!";
        return false;
    }

    return true;
}