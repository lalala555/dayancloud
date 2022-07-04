#include "stdafx.h"
#include "UploadFileDBMgr.h"
#include "UploadFileDatabase.h"
#include "UserProfile/LocalSetting.h"
#include "UserProfile/LocalConfig.h"
#include "UncompleteUploadFileDB.h"
#include "TaskElement.h"
#include "TaskCenterManager.h"
#include "Common/SystemUtil.h"

UploadFileDBMgr::UploadFileDBMgr(QObject *parent)
    : QObject(parent)
    , m_uploadDb(new UploadFileDatabase())
    , m_unCompeleteUploadDb(new UncompleteUploadFileDB())
{
}

UploadFileDBMgr::~UploadFileDBMgr()
{
    delete m_uploadDb;
    delete m_unCompeleteUploadDb;
}

bool UploadFileDBMgr::init()
{
    // 获取存放数据库的路径

    int user_id = MyConfig.accountSet.userId;
    QString agent =  "";//CConfigDb::Inst()->GetSocialProxyName();
    QString filedb_path = QString("%1/%2/%3/profiles/users/%4/%5/%6")
        .arg(RBHelper::makePathFromConfigPath())
        .arg(LocalConfig::getInstance()->defaultProduct)
        .arg(LocalSetting::getInstance()->getPlatformIdForLocal())
        .arg(agent)
        .arg(MyConfig.accountSet.userId)
        .arg(System::getLoginDisplayName())
        .arg(UPLOADFILEDBNAME);
#if 0
    if(!m_uploadDb->connectDatabase(dbpath)) {
        LOGFMTE("[UploadFileDBMgr] connect to database failed!");
        return false;
    }
    if(!m_uploadDb->checkTableExist(UPLOADFILETABLENAME)) {
        if(!m_uploadDb->createTable(UPLOADFILETABLENAME)) {
            LOGFMTE("[UploadFileDBMgr] create table failed!");
            return false;
        }
    }
    if(!clearOldestData()) {
        LOGFMTE("[UploadFileDBMgr] clear oldest data failed!");
        return false;
    }
    // 打印当前数据库中记录的数据条数
    LOGFMTI("[UploadFileDBMgr] Current account recorded file count is %d", m_uploadDb->getRecordCount());
#endif
    if(!m_unCompeleteUploadDb->connectDatabase(filedb_path)) {
        LOGFMTE("[UploadFileDBMgr] connect to UncompleteUploadFileDB failed!");
        return false;
    }
    if(!m_unCompeleteUploadDb->checkTableExist(UNCOMPLETETABLENAME)) {
        if(!m_unCompeleteUploadDb->createTable(UNCOMPLETETABLENAME)) {
            LOGFMTE("[UploadFileDBMgr] create uncompletefiles table failed!");
            return false;
        }
    }
    return true;
}

FileRecordState UploadFileDBMgr::checkRecordState(const QString& localPath, const QString& remotePath) // 检查记录是否存在
{
    QMutexLocker lock(&m_mutex);
    QFileInfo file(localPath);

    //检查路径是文件还是文件夹
    if(file.isDir()) {
        DirFilesVec needUploadFile;
        // 进行文件夹检查
        qint64 fileCount = checkDirNeedUploadFile(localPath, remotePath, needUploadFile);

        if(fileCount == 0)
            return UPLOAD_DIR;

        // 如果发生改变的总数超过30%， 则直接上传文件夹
        if(needUploadFile.size() == 0)
            return DIR_NOT_MODIFY;
        else if(needUploadFile.size() / (fileCount * 1.0f) > 0.3f)
            return UPLOAD_DIR;
        else {
            m_dirNeedUploadFiles = needUploadFile;
            return DIR_FILE_MODIFY;
        }


    } else {
        // 该文件记录是否存在
        UploadFileInfo info = m_uploadDb->query(localPath, remotePath);

        // 如果查到的路径为空，或用户为空则表示不存在该条记录
        if(info.remotePath.isEmpty() && info.remotePath.isEmpty() && info.userAccount.isEmpty()) {
            qDebug() << "file not exist";
            return FILE_NOT_EXIST;
        }

        // 如果查询到了该记录，进行文件大小和最后修改时间的比对
        QFileInfo file(localPath);
        if(file.size() == info.fileSize &&
                file.lastModified().toTime_t() == info.lastTime.toUInt()) {
            qDebug() << "file exist";
            return FILE_EXIST;
        } else {
            qDebug() << "file modify";
            return FILE_MODIFY;
        }
    }

    return FILE_NOT_EXIST;
}

bool UploadFileDBMgr::insert(const int storageid, const QString& localPath, const QString& remotePath)
{
    QMutexLocker lock(&m_mutex);

    QFileInfo file(localPath);
    UploadFileInfo info;
    info.platform    = LocalSetting::getInstance()->getDefaultsite();
    info.storageID   = storageid;
    info.userAccount = QString::number(MyConfig.accountSet.userId);
    info.localPath   = localPath;
    info.remotePath  = remotePath;
    info.fileSize    = file.size();
    info.lastTime    = QString::number(file.lastModified().toTime_t());
    info.updateTime  = QString::number(QDateTime::currentDateTime().toTime_t());

    // 打印一条插入记录
    LOGFMTI("[UploadFileDBMgr] Insert file filepath=%s,platform=%d", localPath.toStdString().c_str(), info.platform);
    return  m_uploadDb->insert(info);
}

bool UploadFileDBMgr::update(const int storageid, const QString& localPath, const QString& remotePath)
{
    QMutexLocker lock(&m_mutex);

    QFileInfo file(localPath);
    UploadFileInfo info = m_uploadDb->query(localPath, remotePath);

    info.storageID = storageid;
    info.fileSize = file.size();
    info.remotePath = remotePath;
    info.lastTime = QString::number(file.lastModified().toTime_t());
    info.updateTime = QString::number(QDateTime::currentDateTime().toTime_t());

    // 打印一条更新记录
    LOGFMTI("[UploadFileDBMgr] Update file filepath=%s,platform=%d", localPath.toStdString().c_str(), info.platform);
    return m_uploadDb->updateData(info);
}

bool UploadFileDBMgr::deleteOneRecord(const QString& localPath, const QString& remotePath)
{
    QMutexLocker lock(&m_mutex);
    return m_uploadDb->deleteData(localPath, remotePath);
}

bool UploadFileDBMgr::deleteAll()
{
    QMutexLocker lock(&m_mutex);
    return m_uploadDb->deleteAll();
}

bool UploadFileDBMgr::clearOldestData()
{
    QMutexLocker lock(&m_mutex);
    QString maxCount = QString::fromStdString(0);
    if(maxCount.isEmpty()) maxCount = QString::number(MAX_RECORD_COUNT);
    bool isOk = true;
    if(m_uploadDb->getRecordCount() > maxCount.toUInt()) {
        qint64 clearCount = m_uploadDb->getRecordCount() / 10;
        isOk = m_uploadDb->clearOldestData(clearCount);
    }
    LOGFMTI("[UploadFileDBMgr] current record count is %lld", m_uploadDb->getRecordCount());
    return isOk;
}

qint64 UploadFileDBMgr::checkDirNeedUploadFile(const QString& localPath, const QString& remotePath, DirFilesVec &needUploadFile)
{
    QDir localDir(localPath);
    QStringList filters;
    QDirIterator dir_iterator(localPath,
                              filters,
                              QDir::Files | QDir::NoSymLinks,
                              QDirIterator::Subdirectories);

    qint64 fileCount = 0;
    while(dir_iterator.hasNext()) {
        dir_iterator.next();
        QFileInfo file_info = dir_iterator.fileInfo();
        QString absolute_file_path = file_info.absoluteFilePath();
        fileCount++;

        // 拼接远程目录
        QString rmotepath = remotePath + localDir.relativeFilePath(absolute_file_path);

        // 文件大小和最后修改时间未发生改变
        UploadFileInfo db_info = m_uploadDb->query(absolute_file_path, rmotepath);
        if(db_info.lastTime.toUInt() == file_info.lastModified().toTime_t() &&
                db_info.fileSize == file_info.size())
            continue;

        QPair<QString, QString> pair(absolute_file_path, rmotepath);
        needUploadFile.append(pair);
    }

    return fileCount;
}

DirFilesVec& UploadFileDBMgr::getDirNeedUploadFiles()
{
    return m_dirNeedUploadFiles;
}

bool UploadFileDBMgr::insertTaskRecord(const QString& dbpath, TaskElementPtr task)
{
    if(dbpath.isEmpty() || task.isNull())
        return false;

    QMutexLocker lock(&m_mutex);
    QString filepath = QString("%1/%2").arg(dbpath).arg(UNCOMPLETEDBNAME);
    UncompleteUploadFileDB unfinishedDb;
    unfinishedDb.setTableName(UNCOMPLETETABLENAME);
    if(!unfinishedDb.connectDatabase(filepath)) {
        unfinishedDb.closeConnect();
        LOGFMTE("[UploadFileDBMgr] connect to db failed, path=%s", filepath.toStdString().c_str());
        return false;
    }
    if(!unfinishedDb.checkTableExist(UNCOMPLETETABLENAME)) {
        if(!unfinishedDb.createTable(UNCOMPLETETABLENAME)) {
            unfinishedDb.closeConnect();
            LOGFMTE("[UploadFileDBMgr] create uncompletefiles table failed!");
            return false;
        }
    } else {  // 如果表已经存在 则检查表结构是否需要更新
        if(!updateTableStruct(unfinishedDb)) {
            return false;
        }
    }

    bool res = true;

    // sql语句预处理
    if(!unfinishedDb.execSqlPrepare(UNCOMPLETETABLENAME))
        return false;
    // 开启事物操作 提升插入效率
    unfinishedDb.operationBegin();

    auto fileElems = task->getTransformLists();
    auto it = fileElems.begin();
    for(; it != fileElems.end(); ++it) {
        UncompleteFile unFile;
        // 公有属性
        unFile.taskid = task->getJobId();
        unFile.projectid = task->getProjectId();
        unFile.projectsymbol = task->getProjectName();
        unFile.loadtexturefile = task->isTextureFileLoaded() == true ? 1 : 0;
        unFile.framename = task->getSceneName();
        unFile.scene = task->getSceneFile();
        unFile.updatetime = QString::number(QDateTime::currentDateTime().toTime_t());

        // 插入或更新
        unfinishedDb.updateData(unFile);

        // 防止界面卡顿
        qApp->processEvents();
    }
    unfinishedDb.operationCommit();
    unfinishedDb.closeConnect();
    return res;
}

bool UploadFileDBMgr::updateTaskRecord(const QString& dbpath, TaskElementPtr task)
{
    return false;
}

TaskElementPtr UploadFileDBMgr::findUnfinished(const QString& dbpath, TaskElementPtr taskElement)
{
    if(taskElement.isNull())
        return taskElement;

    taskElement->initDataFromLocalDb();

    return taskElement;
}

bool UploadFileDBMgr::deleteOneTaskRecord(const QString& dbpath, const qint64 taskid, const QString& rename)
{
    QMutexLocker lock(&m_mutex);
    QString filepath = QString("%1/%2").arg(dbpath).arg(UNCOMPLETEDBNAME);
    UncompleteUploadFileDB unfinishedDb;
    unfinishedDb.setTableName(UNCOMPLETETABLENAME);
    if(!unfinishedDb.connectDatabase(filepath)) {
        LOGFMTI("[UploadFileDBMgr] connect to db failed, path=%s", filepath.toStdString().c_str());
        return false;
    }
    return unfinishedDb.deleteOne(taskid, rename, UNCOMPLETETABLENAME);
}

bool UploadFileDBMgr::deleteAllTaskRecord(const QString& dbpath, const qint64 taskid)
{
    QMutexLocker lock(&m_mutex);
    QString filepath = QString("%1/%2").arg(dbpath).arg(UNCOMPLETEDBNAME);
    UncompleteUploadFileDB unfinishedDb;
    unfinishedDb.setTableName(UNCOMPLETETABLENAME);
    if(!unfinishedDb.connectDatabase(filepath)) {
        LOGFMTI("[UploadFileDBMgr] connect to db failed, path=%s", filepath.toStdString().c_str());
        return false;
    }
    return unfinishedDb.deleteByTaskId(taskid, UNCOMPLETETABLENAME);
}

bool UploadFileDBMgr::updateTableStruct(UncompleteUploadFileDB& db)
{
    // 1. 创建一个临时表，用于比较表中的字段是否发生改变
    // 2. 如果表被更新，则需要将旧表的数据拷贝到新表
    // 3. 拷贝完成后，删掉旧表，将临时表重命名

    if(!db.checkTableExist(TMPUNCOMPLETETABLENAME)) {
        if(!db.createTable(TMPUNCOMPLETETABLENAME)) {
            // db.closeConnect();
            LOGFMTE("[UploadFileDBMgr] create TEMP uncompletefiles table failed!");
            return false;
        }
    }
    // 检查是否需要更新表
    if(!db.checkTableNeedUpdate(UNCOMPLETETABLENAME, TMPUNCOMPLETETABLENAME)) {
        LOGFMTI("[UploadFileDBMgr] table need not update!");
        return true;
    }

    // 拷贝数据
    if(!db.copyOldTableData(UNCOMPLETETABLENAME, TMPUNCOMPLETETABLENAME)) {
        LOGFMTE("[UploadFileDBMgr] copy old data from uncompletefiles table failed!");
        return false;
    }
    // 重命名表
    if(!db.renameTableName(UNCOMPLETETABLENAME, TMPUNCOMPLETETABLENAME)) {
        LOGFMTE("[UploadFileDBMgr] rename table name failed!");
        return false;
    }
    return true;
}
