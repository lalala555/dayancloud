/***********************************************************************
* Module:  TaskElement.h
* Author:  hqr
* Created: 2018/11/08 18:08:15
* Modifier: hqr
* Modified: 2018/11/08 18:08:15
* Purpose: Declaration of the class
***********************************************************************/
#ifndef TASK_ELEMENT_H
#define TASK_ELEMENT_H

#include "kernel/msg.h"
#include "kernel/render_struct.h"
#include "Upload/UploadFileListByDb.h"
#include "Common/RBHelper.h"
#include "TaskWriter.h"

class LoadFileThread;

class TaskElement
{
public:
    TaskElement(const stTaskElement& element);
    ~TaskElement();

    void init();

    t_taskid getJobId()
    {
        return m_job_id;
    }
    float calcProgress()
    {
        return m_fileList.getProgress();
    }
    int getProjectId()
    {
        return m_project_id;
    }
    int getCgId()
    {
        return RBHelper::cgidOfScene(m_input_cg_file);
    }
    QString getSceneName()const
    {
        return m_cg_file_name;
    }
    QString getSceneFile()
    {
        return m_input_cg_file;
    }
    QString getProjectName()
    {
        return m_project_name;
    }
    QString getLayerName()
    {
        return "-";
    }
    int getRenderRam()
    {
        return m_render_ram;
    }

    //////////////////////////////////////////////////////////////////////////////////////
    bool parse_software_type(const QJsonObject& json);
    bool parse_task_json_cc(const QJsonObject& json);
    // 读取工程文件，得到需要上传的文件列表
    // 每次创建新的任务时需要调用load_project
    bool load_project_file(bool is_clone = false);
    // 添加工程的配置文件到上传容器中,因为只有在新建的时候才会传工程配置文件，所以一定是notcomplete
    bool add_config_to_filelist(const std::wstring& projectPath);
    // 从工程配置文件中加载需要上传到的text文件列表，并添加到传输队列中
    bool load_texture_file_list();
    bool add_file_to_filelist(const QString& localPath, const QString& remotePath, enAssetType fileType);
    //////////////////////////////////////////////////////////////////////////////////////
    bool update_cc_task(const TaskInfoCC& taskInfo);
    bool update_task_json_cc(QJsonObject& task_json, const TaskInfoCC& taskInfo);
    //////////////////////////////////////////////////////////////////////////////////////
    //任务是否成功，所有文件都传成功返回true，否则返回false
    bool is_task_completed();
    //任务是否完成，包括成功和失败的
    bool is_task_done();
    
    void completeFile(const QString& local_file, const QString& remote_file)
    {
        m_fileList.setFileCompeleted(local_file, remote_file);
    }
    void resetFailed()
    {
        m_fileList.resetFileUploadFailed();
    }
    void failedFile(const QString& local_file, const QString& remote_file)
    {
        m_fileList.setFileUploadFailed(local_file, remote_file);
    }
    int getFailedCount()
    {
        return m_fileList.getFailedCount();
    }
    int getCompeleteCount()
    {
        return m_fileList.getCompleteCount();
    }
    int getTotalFileCount()
    {
        return m_fileList.getTotalCount();
    }
    UploadFileListByDb* fileList()
    {
        return &m_fileList;
    }
    void stop()
    {
        m_is_stopped = true;
    }
    void start()
    {
        m_is_stopped = false;
    }
    bool isStopped()
    {
        return m_is_stopped;
    }
    QList<UncompleteFile> getTransformLists()
    {
        return m_fileList.getTransformLists();
    }
    // 文件大小
    qint64 getCompleteFileSize()
    {
        return m_fileList.getCompleteFileSize();
    }
    qint64 getTotalFileSize()
    {
        return m_fileList.getTotalFileSize();
    }
    qint64 getFileSize(const QString& local_file, const QString& remote_file)
    {
        return m_fileList.getFileSize(local_file, remote_file);
    }

    bool load_common_file_list();
    bool load_upload_file_list();
    bool build_upload_root_map();
    bool rebuild_upload_root_map();

    void load_file_thread_finished();
    bool isTextureFileLoaded();
    void setSameFileError(bool hasError);
    bool hasSameFileError();

    // 设置任务获取消息返回的时间
    void setTaskRecvMsgTime();
    // 等待消息是否超时
    bool isWaitMsgTimeout();
    // 获取DB路径
    QString getUncompleteDbFilePath();
    // 从数据库中初始化一些数据
    bool initDataFromLocalDb();
    // 任务重试次数
    void setTaskRetryTimes(int retryTimes);
    int  getTaskRetryTimes();
    // 任务优先级
    void provTaskUploadPriority();
    int  getTaskUploadPriority();
    // 校验上传的合法性
    bool isUploadLegal();
    // 设置当前文件已经上传的大小
    void setUploadFileSize(const QString& local_file, const QString& remote_file, qint64 trans_size, qint64 total_size);
    qint64 getUploadedFileSize();
    void initUploadedFileSize();
    // 照片、控制点
    int getPictureCount();
    int getCoordCount();
private:
    QSharedPointer<TaskInfoCC> taskInfoCC;
    QSharedPointer<LoadFileThread> m_loadFileThread;
    UploadFileListByDb m_fileList;
    t_uid m_uid;                   // 任务所属账户,克隆时任务还属于来源账户,统计任务来源不精准
    QMap<int, int> m_mapUploadRoot;
    QMap<QString, qint64> m_uploadedFileSize;
    bool m_hasSameFileError;
    bool m_is_stopped;              // 是否手动stop（有些行为在手动stop后不能执行）
    bool m_isTextureFileLoaded;     // 是否也加载过texture
    qint64 m_lastRecvMsgTime;       // 最近收到消息的时间
    int m_retryTimes;               // 出错重试次数
    int m_taskUploadPriority;       // 任务上传优先级
    // Properties
public:
    CG_ID m_cg_id;
    t_taskid m_job_id;
    QString m_job_id_alias;         // 任务号别名
    QString m_input_cg_file;        // 原始场景文件
    QString m_cg_file_name;          // 场景名称
    int m_render_ram;               // 内存
    int m_project_id;
    QString m_project_name;         //
    QString m_plugin_config;        // 插件配置名称
    QString m_client_project_path;
    int m_pic_count;    // 照片数量
    int m_coord_count;  // 控制点数量
};

typedef QSharedPointer<TaskElement> TaskElementPtr;

#endif
