#include "stdafx.h"
#include "UploadFileListByDb.h"
#include "Common/StringUtil.h"

UploadFileListByDb::UploadFileListByDb()
    : m_completedCount(0)
    , m_failedCount(0)
    , m_totalSize(0)
    , m_completedSize(0)
    , m_checkSuccessCount(0)
    , m_checkFailedCount(0)
    , m_uncheckedFileCount(0)
    , m_isDbInitOk(false)
    , m_need_check(false)
{
}

UploadFileListByDb::~UploadFileListByDb()
{
    // m_unfinishedDb.closeConnect();
}

void UploadFileListByDb::init()
{
    m_isDbInitOk = initDb();
}

bool UploadFileListByDb::initDb()
{
    QString filepath = QString("%1/%2").arg(m_dbPath).arg(UNCOMPLETEDBNAME);
    m_unfinishedDb.setTableName(UNCOMPLETETABLENAME);
    if(!m_unfinishedDb.connectDatabase(filepath)) {
        m_unfinishedDb.closeConnect();
        LOGFMTE("[UploadFileListByDb] connect to db failed, path=%s", qPrintable(filepath));
        return false;
    }
    if(!m_unfinishedDb.checkTableExist(UNCOMPLETETABLENAME)) {
        if(!m_unfinishedDb.createTable(UNCOMPLETETABLENAME)) {
            m_unfinishedDb.closeConnect();
            LOGFMTE("[UploadFileListByDb] create uncompletefiles table failed!");
            return false;
        }
    } else {  // 如果表已经存在 则检查表结构是否需要更新
        if(!updateTableStruct(m_unfinishedDb)) {
            return false;
        }
    }

    return true;
}

void UploadFileListByDb::clear()
{
}

void UploadFileListByDb::resetFileUploadFailed()
{
    m_failedCount = 0;

    CGuard guard(&m_lock);
    // 重置上传错误的状态
    if(m_isDbInitOk) {
        // 开启校验的情况下
        if(m_need_check) {
            // 上传失败，未校验 -- FILE_UPLOAD_FAILED + FILE_NOT_CHECK -> FILE_NOT_START + FILE_NOT_CHECK
            m_unfinishedDb.updateDatasStatus(UNCOMPLETETABLENAME, FILE_UPLOAD_FAILED + FILE_NOT_CHECK, FILE_NOT_START + FILE_NOT_CHECK);

            // 上传成功，校验失败 -- FILE_UPLOADED + FILE_CHECK_FAILED -> FILE_NOT_START + FILE_NOT_CHECK
            m_unfinishedDb.updateDatasStatus(UNCOMPLETETABLENAME, FILE_UPLOADED + FILE_CHECK_FAILED, FILE_NOT_START + FILE_NOT_CHECK);

            // 以前是关闭校验，现在打开校验 -- FILE_UPLOAD_FAILED + FILE_CHECK_SUCCESS -> FILE_NOT_START + FILE_NOT_CHECK
            m_unfinishedDb.updateDatasStatus(UNCOMPLETETABLENAME, FILE_UPLOAD_FAILED + FILE_CHECK_SUCCESS, FILE_NOT_START + FILE_NOT_CHECK);

            // 关闭校验的情况下
        } else {
            // 上传失败，校验成功(不开校验的情况下，校验状态是成功) -- FILE_UPLOAD_FAILED + FILE_CHECK_SUCCESS -> FILE_NOT_START + FILE_CHECK_SUCCESS
            m_unfinishedDb.updateDatasStatus(UNCOMPLETETABLENAME, FILE_UPLOAD_FAILED + FILE_CHECK_SUCCESS, FILE_NOT_START + FILE_CHECK_SUCCESS);

            // 上传失败，未校验 -- FILE_UPLOAD_FAILED + FILE_NOT_CHECK -> FILE_NOT_START + FILE_CHECK_SUCCESS
            m_unfinishedDb.updateDatasStatus(UNCOMPLETETABLENAME, FILE_UPLOAD_FAILED + FILE_NOT_CHECK, FILE_NOT_START + FILE_CHECK_SUCCESS);
        }
    }
    LOGFMTD("[UploadFileList] File upload reset failed , Fail count %d, Compeleted count %d, Total count %d!",
            getFailedCount(), getCompleteCount(), getTotalCount());
}

bool UploadFileListByDb::appendFile(const UncompleteFile &file)
{
    CGuard guard(&m_lock);

    // 采用数据库的方式填充数据
    if(m_isDbInitOk) {
        m_unfinishedDb.updateData(file);
    }

    return true;
}

bool UploadFileListByDb::insertOneRecord(const UncompleteFile &file)
{
    CGuard guard(&m_lock);

    if(m_isDbInitOk) {
        return m_unfinishedDb.insert(file, UNCOMPLETETABLENAME);
    }
    return false;
}

bool UploadFileListByDb::isUploadCompleted()
{
    CGuard guard(&m_lock);
    int total = getTotalCount();
    if(m_completedCount == total) {
        return true;
    }
    return false;
}

void UploadFileListByDb::setFileCompeleted(const QString& local_file, const QString& remote_file)
{
    if(m_isDbInitOk) {
        QString pathMd5 = local_file + remote_file;
        pathMd5 = String::getStringMd5(pathMd5);
        // 直接更新数据库
        // 1. 不开启校验 -- 将状态更新为 FILE_UPLOADED + FILE_CHECK_SUCCESS
        // 2. 开启校验时 -- 将状态更新为 FILE_UPLOADED + FILE_NOT_CHECK
        int status = -1;
        if(m_need_check) {
            status = FILE_UPLOADED + FILE_NOT_CHECK;
        } else {
            status = FILE_UPLOADED + FILE_CHECK_SUCCESS;
        }
        this->updateStatus(pathMd5, status);
    }

    LOGFMTD("[UploadFileList] File upload Compeleted , Fail count %d, Compeleted count %d, Total count %d!",
            getFailedCount(), getCompleteCount(), getTotalCount());
}

void UploadFileListByDb::setFileUploadFailed(const QString& local_file, const QString& remote_file)
{
    if(m_isDbInitOk) {
        QString pathMd5 = local_file + remote_file;
        pathMd5 = String::getStringMd5(pathMd5);
        // 上传失败的更新到数据库
        // 1. 上传失败，未校验 -- FILE_UPLOAD_FAILED + FILE_NOT_CHECK
        // 2. 上传失败，校验成功（不开校验的情况） -- FILE_UPLOAD_FAILED + FILE_CHECK_SUCCESS
        int status = -1;
        if(m_need_check) {
            status = FILE_UPLOAD_FAILED + FILE_NOT_CHECK;
        } else {
            status = FILE_UPLOAD_FAILED + FILE_CHECK_SUCCESS;
        }
        this->updateStatus(pathMd5, status);
    }
}

float UploadFileListByDb::getProgress()
{
    int compelete = getCompleteCount();
    int total = getTotalCount();
    if(total > 0) {
        return (compelete * 1.0) / total;
    }
    return 0.0;
}

int UploadFileListByDb::getTotalCount()
{
    CGuard guard(&m_lock);

    // todo 查询总的文件数量
    qint64 total = m_unfinishedDb.getRecordCount(UNCOMPLETETABLENAME);

    return total;
}

int UploadFileListByDb::getFailedCount()
{
    CGuard guard(&m_lock);
    // todo 查询上传失败的文件数量
    int status = -1;
    if(m_need_check) {
        status = FILE_UPLOAD_FAILED + FILE_NOT_CHECK;
    } else {
        status = FILE_UPLOAD_FAILED + FILE_CHECK_SUCCESS;
    }

    m_failedCount = m_unfinishedDb.queryDataCountByStatus(UNCOMPLETETABLENAME, status);

    return m_failedCount;
}

int UploadFileListByDb::getCompleteCount()
{
    CGuard guard(&m_lock);

    // todo 查询完成的文件数量
    int status = status = FILE_UPLOADED + FILE_CHECK_SUCCESS;

    m_completedCount = m_unfinishedDb.queryDataCountByStatus(UNCOMPLETETABLENAME, status);

    return m_completedCount;
}

qint64 UploadFileListByDb::getTransformListCount(QList<UncompleteFile>& fileList)
{
    CGuard guard(&m_lock);
    // 获取数据库未上传的任务数量
    // 1. 未开启校验 -- (FILE_NOT_START || FILE_NOT_FOUND || FILE_UPLOAD_FAILED) + FILE_CHECK_SUCCESS -> status > 0 && status <= 3
    // 2. 开启校验 -- (FILE_NOT_START || FILE_NOT_FOUND || FILE_UPLOAD_FAILED) + FILE_NOT_CHECK -> status > 128
    if(m_isDbInitOk) {
        return m_unfinishedDb.queryUnuploadDatas(UNCOMPLETETABLENAME, fileList);
    }

    return 0;
}

// 使用文件大小度量
float UploadFileListByDb::getSizeProgress()
{
    if(m_totalSize == 0)
        return 0.0;

    return m_completedSize / m_totalSize;
}

QList<UncompleteFile> UploadFileListByDb::getTransformLists()
{
    CGuard guard(&m_lock);
    QList<UncompleteFile> list;
    this->getTransformListCount(list);
    return list;
}

int UploadFileListByDb::getCheckSuccessCount()
{
    CGuard guard(&m_lock);

    // todo 校验成功的数量
    if(m_isDbInitOk) {
        int status = FILE_UPLOADED + FILE_CHECK_SUCCESS;
        m_checkSuccessCount = m_unfinishedDb.queryDataCountByStatus(UNCOMPLETETABLENAME, status);
    }

    return m_checkSuccessCount;
}

int UploadFileListByDb::getCheckFailedCount()
{
    int failedCount = 0;
    CGuard guard(&m_lock);

    // todo 查询校验失败的文件数量
    // 1. 上传成功，但是校验失败 -- FILE_UPLOADED + FILE_CHECK_FAILED
    if(m_isDbInitOk) {
        int status = FILE_UPLOADED + FILE_CHECK_FAILED;
        failedCount = m_unfinishedDb.queryDataCountByStatus(UNCOMPLETETABLENAME, status);
    }

    return failedCount;
}

int UploadFileListByDb::getUncheckedFileCount()
{
    int uncheckCount = 0;
    CGuard guard(&m_lock);

    // todo 获取未校验的文件数量
    // 1. 上传完成，未校验 -- FILE_UPLOADED + FILE_NOT_CHECK
    // 2. 未上传，未校验   -- FILE_NOT_START + FILE_NOT_CHECK
    // 3. 上传失败，未校验 -- FILE_UPLOAD_FAILED + FILE_NOT_CHECK
    if(m_isDbInitOk) {
        int status = FILE_UPLOADED + FILE_NOT_CHECK;
        uncheckCount += m_unfinishedDb.queryDataCountByStatus(UNCOMPLETETABLENAME, status);

        status = FILE_NOT_START + FILE_NOT_CHECK;
        uncheckCount += m_unfinishedDb.queryDataCountByStatus(UNCOMPLETETABLENAME, status);

        status = FILE_UPLOAD_FAILED + FILE_NOT_CHECK;
        uncheckCount += m_unfinishedDb.queryDataCountByStatus(UNCOMPLETETABLENAME, status);
    }

    return uncheckCount;
}

bool UploadFileListByDb::updateTableStruct(UncompleteUploadFileDB& db)
{
    // 1. 创建一个临时表，用于比较表中的字段是否发生改变
    // 2. 如果表被更新，则需要将旧表的数据拷贝到新表
    // 3. 拷贝完成后，删掉旧表，将临时表重命名

    if(!db.checkTableExist(TMPUNCOMPLETETABLENAME)) {
        if(!db.createTable(TMPUNCOMPLETETABLENAME)) {
            // db.closeConnect();
            LOGFMTE("[UploadFileListByDb] create TEMP uncompletefiles table failed!");
            return false;
        }
    }
    // 检查是否需要更新表
    if(!db.checkTableNeedUpdate(UNCOMPLETETABLENAME, TMPUNCOMPLETETABLENAME)) {
        LOGFMTI("[UploadFileListByDb] table need not update!");
        return true;
    }

    // 拷贝数据
    if(!db.copyOldTableData(UNCOMPLETETABLENAME, TMPUNCOMPLETETABLENAME)) {
        LOGFMTE("[UploadFileListByDb] copy old data from uncompletefiles table failed!");
        return false;
    }
    // 重命名表
    if(!db.renameTableName(UNCOMPLETETABLENAME, TMPUNCOMPLETETABLENAME)) {
        LOGFMTE("[UploadFileListByDb] rename table name failed!");
        return false;
    }
    return true;
}

bool UploadFileListByDb::execSqlPrepare()
{
    return m_unfinishedDb.execSqlPrepare(UNCOMPLETETABLENAME);
}

void UploadFileListByDb::operationBegin()
{
    if(m_isDbInitOk) {
        m_unfinishedDb.operationBegin();
    }
}

void UploadFileListByDb::operationCommit()
{
    if(m_isDbInitOk) {
        m_unfinishedDb.operationCommit();
    }
}

UncompleteFile UploadFileListByDb::queryOneData(t_taskid taskid)
{
    CGuard guard(&m_lock);

    UncompleteFile db;
    if(m_isDbInitOk) {
        db = m_unfinishedDb.queryOneDataByTaskId(UNCOMPLETETABLENAME, taskid);
    }

    return db;
}

bool UploadFileListByDb::updateStatus(const QString& pathMd5, qint64 status)
{
    CGuard guard(&m_lock);

    /*
    m_unfinishedDb.preUpdateDataStatus(UNCOMPLETETABLENAME);
    m_unfinishedDb.operationBegin();
    m_unfinishedDb.preUpdateStatus(pathMd5, status);
    m_unfinishedDb.operationCommit();
    */
    m_unfinishedDb.updateDataStatus(UNCOMPLETETABLENAME, pathMd5, status);

    return true;
}

qint32 UploadFileListByDb::getTextureFileCount()
{
    CGuard guard(&m_lock);

    return m_unfinishedDb.getTextureFileCount(UNCOMPLETETABLENAME);
}

qint64 UploadFileListByDb::getCompleteFileSize()
{
    CGuard guard(&m_lock);

    m_completedSize = m_unfinishedDb.queryUploadCompletedFileSize(UNCOMPLETETABLENAME);

    return m_completedSize;
}

qint64 UploadFileListByDb::getTotalFileSize()
{
    CGuard guard(&m_lock);
    m_totalSize = m_unfinishedDb.queryTotalUploadFileSize(UNCOMPLETETABLENAME);
    return m_totalSize;
}

qint64 UploadFileListByDb::getFileSize(const QString& local_file, const QString& remote_file)
{
    CGuard guard(&m_lock);
    QString pathMd5 = local_file + remote_file;
    pathMd5 = String::getStringMd5(pathMd5);

    return m_unfinishedDb.queryFileSize(UNCOMPLETETABLENAME, pathMd5);
}

QList<UncompleteFile> UploadFileListByDb::getFinishedLists()
{
    return m_unfinishedDb.queryUploadFinishedRecord(UNCOMPLETETABLENAME);
}