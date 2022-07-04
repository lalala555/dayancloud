#ifndef UPLOAD_FILE_LISTDB_H
#define UPLOAD_FILE_LISTDB_H

#include "IPacket.h"
#include "Common/SyncObject.h"
#include "UploadFileDBMgr/UncompleteUploadFileDB.h"

/*====================================================================
**class UploadFileListByDb

**描述：上传文件列表，提供列表的统一控制（完成、失败等）接口
=======================================================================*/
class UploadFileListByDb
{
public:
    UploadFileListByDb();
    ~UploadFileListByDb();
    void init();
    void uninit();
    void clear();
    bool appendFile(const UncompleteFile &file);
    bool insertOneRecord(const UncompleteFile &file);
    // 判断是否所有文件都传输完成，是返回true，否返回false
    bool isUploadCompleted();

    // 文件状态改变处理函数：传输完成
    void setFileCompeleted(const QString& local_file, const QString& remote_file);
    // 文件状态改变处理函数：传输失败
    void setFileUploadFailed(const QString& local_file, const QString& remote_file);
    // 获取传输进度
    float getProgress();
    float getSizeProgress();

    // 已经传输成功的文件个数
    int getCompleteCount();
    // 任务的文件总个数
    int getTotalCount();
    // 传输失败的文件数目
    int getFailedCount();

    qint64 getTransformListCount(QList<UncompleteFile>& fileList);
    QList<UncompleteFile> getTransformLists();
    QList<UncompleteFile> getFinishedLists();

    void resetFileUploadFailed();

    int  getCheckSuccessCount();
    int  getCheckFailedCount();
    int  getUncheckedFileCount();
    bool initDb();
    bool updateTableStruct(UncompleteUploadFileDB& db);
    void setDbPath(const QString& path) { m_dbPath = path; }
    void operationBegin();
    void operationCommit();
    bool execSqlPrepare();
    UncompleteFile queryOneData(t_taskid taskid);
    qint32 getTextureFileCount(); // 获取贴图文件数量

    bool updateStatus(const QString& pathMd5, qint64 status);

    // 文件大小
    qint64 getCompleteFileSize();
    qint64 getTotalFileSize();
    qint64 getFileSize(const QString& local_file, const QString& remote_file);
private:
    mutable CLock m_lock;
    qint32 m_completedCount;     //传输成功文件数目
    qint32 m_failedCount;        //传输失败文件数目
    qint32 m_checkSuccessCount;  // 校验成功的个数
    qint32 m_checkFailedCount;   // 校验失败的个数
    qint32 m_uncheckedFileCount; // 未校验的文件个数
    qint64 m_totalSize;          // 传输文件总大小
    qint64 m_completedSize;      // 传输完成总大小
    qint64 m_totalFileCount;     // 传输文件总数

    QString m_dbPath;
    bool m_isDbInitOk;
    bool m_need_check;
    UncompleteUploadFileDB m_unfinishedDb; // 数据库
};

#endif // _UPLOAD_FILE_LISTDB_H
