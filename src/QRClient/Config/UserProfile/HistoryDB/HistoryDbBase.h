/***********************************************************************
* Module:  HistoryDbBase.h
* Author:  hqr
* Modified: 2016/07/03 17:43:10
* Purpose: Declaration of the class
***********************************************************************/
#ifndef HISTORYDB_H_
#define HISTORYDB_H_
#include <map>
#include "Common/SqliteHelp/SQLiteProvider/sqlite3.h"
#include "Common/SqliteHelp/SQLiteProvider/sqlite3provider.h"

typedef long long int64;

enum HISTORY_TABLE {
    TABLE_BATCH,
    TABLE_UPLOAD,
    TABLE_DOWNLOAD,
    TABLE_DOWNLOAD_COUNT,
    TABLE_LAYER,
    TABLE_TRANS,
    TABLE_REPORT_LOG
};

#define INVALID_STATUS  100

enum BATCH_STATUS {
    BATCH_UNFINISH = 0,
};

enum DOWNLOAD_STATUS {
    DOWNLOAD_UNFINISH = 0,
    DOWNLOAD_STOP,
    DOWNLOAD_LASTTIME,
    DOWNLOAD_FINISHED,

};

enum UPLOAD_STATUS {
    UPLOAD_UNFINISH = 0,
    UPLOAD_STOP
};

/*enum JOB_TYPE {
    UPLOAD_JOB = 0,
    DOWNLOAD_JOB = 1,
    RENDER_JOB = 2,
    UNKOWN_JOB = 3,
};*/

struct BatchHistoryInfo {
    int index;                  // 自增id
    int taskId;                 // 分析id，不可为空
    int jobId;                  // 任务id
    std::string jobIdAlias;     // 任务id
    int status;                 // 任务状态
    int finalStatus;
    int initialAnalyzeMode;     // 手动分析
    std::string cgfile;         // 场景文件
    std::string artist;         // 制作人
    std::string project_name;   // 项目
    std::string plugin_name;    // 插件配置
    std::string project_dir;    // 结果输出目录
    std::string startTime;
    std::string stopTime;
    std::string tempdir;
    std::string cmdline;
    std::string runlog;
    std::string testFrames;
    std::string renderRam;
    int actionAfterTest;
    int priority;
    int priorityTime;
    BatchHistoryInfo() : taskId(0), jobId(0), finalStatus(0), actionAfterTest(0), priority(0), priorityTime(0){}
};

struct LogReportHistoryInfo {
    enum InfoStatus {
        not_report,
        reporting,
        done_report
    };

    int  index;           //自增id
    int  taskId;          //任务id，不可为空
    int  status;          //任务的状态--完成，传输中，未开始
    bool isUploaded;      //是否成功
    std::string logPath;  //文件路径
    std::string content;  //附加字符数据

    LogReportHistoryInfo() : taskId(0), isUploaded(false), status(not_report), logPath(""){}
};

struct DownloadHistoryInfo {
    int    index;           //自增id    
    int    status;          //任务状态（0：等待下载，1：放弃下载，2：下载完成）
    int    times;           //下载次数
    int    ncheckingstatus; //检查帧状态
    int    priority;
    int    priorityTime;    
    int    storageId;       //存储ID
    int    priorityType;    //优先类型
    int    operateType;     // 自动/手动
    int    downloadJobType; // 下载任务类型（镜头，帧）

    int64  taskId;          //任务id，不可为空
    int64  parentId;        //父任务id
    int64  taskUserId;      // 任务用户ID

    std::string project;         //项目
    std::string sceneName;       //场景名
    std::string layerName;       //层名
    std::string artist;          //制作人
    std::string savePath;        //本地保存路径
    std::string startTime;
    std::string stopTime;
    std::string saveAsPath;      // 另存路径
    std::string outputLabel;     // 输出
    std::string taskAlias;       // 任务别名
    std::string parentTaskAlias; // 父任务别名
    std::string outputSize;      // 任务输出文件大小
    std::string downloadType;    // OSGB/OBJ/3D Tiles/LAS

    DownloadHistoryInfo() : priority(1), priorityTime(0), priorityType(0) {}
};

struct DownloadCountInfo {
    int    index;           //自增id
    int    taskId;          //任务id，不可为空
    std::string requestId;
};

struct UploadHistoryInfo {
    bool vip;               // 是否vip
    int index;              // 自增id
    int64 taskId;             // 任务id，不可为空
    std::string taskidAlias;// 任务名
    int status;             // 任务状态（0：等待上传，1：放弃上传，2：上传完成）
    int times;              // 上传次数
    int dependId;           // 依赖上传id
    std::string startTime;
    std::string stopTime;
    std::string project;    // 项目
    std::string sceneName;  // 场景名
    std::string layerName;  // 层名
    std::string scenePath;  // 场景路径
    int priority;
    int priorityTime;
    UploadHistoryInfo() : priority(0), priorityTime(0) {}
};
struct LayerHistoryInfo {
    int index;                  // 自增
    int fatherid;               // 父任务id
    int64 taskid;              // 任务id
    int status;                 // 任务状态
    int projectid;              // 项目ID
    int layerId;                // 分层id 唯一 不可为空
    int defaultlayer;           // 是否默认层
    std::string projectname;    // 项目名称
    std::string name;           // 场景文件的别名
    std::string renderRam;      // 内存选择
};
struct TransHistoryInfo {
    int   index;             // 自增ID
    int64   taskId;         // 任务号
    int   jobType;          // 任务类型 0-上传，1-下载
    int   transState;        // 上传状态
    int   downloadState;      // 下载状态
    bool  vipTask;           // 克隆任务
    bool  autoDownload;      // 是否是自动下载
    std::string sceneName;    // 场景名称
    std::string scenePath;    // 场景路径
    std::string downloadPath; // 下载路径
    std::string loaclSavePath;// 另存为路径
    std::string camera;      // 相机名称
    std::string artist;      // 制作者
    std::string startTime;   // 开始时间
    std::string endTime;     // 结束时间
    std::string taskAlias;   // 任务号别名
    int  download_mode;      // 下载模式
    int  unicode_download;   // 下载路径为unicode
    std::string downloadType; // 下载文件类型OSGB/OBJ/3D Tiles/LAS
    int pictureCount;
    int coordCount;

    TransHistoryInfo()
    {
        index = -1;
        taskId = -1;
        jobType = UNKOWN_JOB;
        transState = -1;
        downloadState = -1;
        vipTask = false;
        autoDownload = false;
        sceneName = "";
        scenePath = "";
        downloadPath = "";
        loaclSavePath = "";
        camera = "";
        artist = "";
        startTime = "";
        endTime = "";
        taskAlias = "";
        download_mode = DOWNLOAD_WITH_DIR;
        unicode_download = DOWNLOAD_USE_ANSI;
        pictureCount = -1;
        coordCount = -1;
    }
};

enum SYNC_TYPE {
    SYNC_ADD_TASK,
    SYNC_UPDATE_TASK,
    SYNC_UPDATE_TASK_STATUS,
    SYNC_UPDATE_TASK_PRIORITY,
    SYNC_UPDATE_TASK_SCENE,
    SYNC_UPDATE_TASK_DEPEND,
    SYNC_UPDATE_TASK_VIP,
    SYNC_UPDATE_TASK_TRANS_STATE,
    SYNC_UPDATE_TASK_SCENE_PATH,
    SYNC_UPDATE_TASK_DOWNLOAD_PATH,
    SYNC_UPDATE_TASK_LOCALSAVE_PATH,
    SYNC_DEL_TASK,
    SYNC_UPDATE_TASK_PARENT_INFO,
    SYNC_UPDATE_TASK_SAVE_PATH,
    SYNC_DEL_REPORT_LOG,
    SYNC_UPDATE_TASK_REPORT_LOG,
    SYNC_UPDATE_TASK_OUTPUT_SIZE,
};

class SyncData
{
public:
    int table; // 表 0 batch,1 upload,2 download
    int type;
    int taskId;

    virtual ~SyncData() {}
};

template<class INFO>
class SyncAddTask : public SyncData
{
public:
    INFO* info;
    ~SyncAddTask()
    {
        delete info;
        info = nullptr;
    }
};

class SyncUpdateTaskStatus : public SyncData
{
public:
    int finalStatus;
    int status;
    int tid;    // batch页面为真正的task id
    int nChecking;
    std::string strProject;
    std::string strSceneName;
    std::string strLocalPath;
    std::string completeTime;
    std::string downloadList;
    ~SyncUpdateTaskStatus()
    {
    }
};
template<class INFO>
class SyncUpdateBatchTask : public SyncData
{
public:
    INFO* info;
    ~SyncUpdateBatchTask()
    {
        delete info;
        info = nullptr;
    }
};
class SyncUpdateDependTask: public SyncData
{
public:
    int nDependTask;
    ~SyncUpdateDependTask()
    {

    }
};
class SyncUpdateVip: public SyncData
{
public:
    bool bVip;
    ~SyncUpdateVip()
    {

    }
};
class SyncUpdateTaskScene: public SyncData
{
public:
    std::string strScene;
    ~SyncUpdateTaskScene()
    {

    }
};
class SyncUpdateTaskPriority: public SyncData
{
public:
    int priority;
    int priorityTime;
    int customPriority;
    ~SyncUpdateTaskPriority()
    {

    }
};
class SyncLayerTaskStatus: public SyncData
{
public:
    int status;
    int tid;    // 任务号
    std::string renderRam; // 渲染内存
    ~SyncLayerTaskStatus()
    {
    }
};
class SyncUpdateTaskScenePath: public SyncData
{
public:
    std::string strScenePath;
    ~SyncUpdateTaskScenePath()
    {

    }
};
class SyncUpdateTaskDownloadPath: public SyncData
{
public:
    std::string strDownloadPath;
    ~SyncUpdateTaskDownloadPath()
    {

    }
};
class SyncUpdateTaskLocalSavePath: public SyncData
{
public:
    std::string strLocalSavePath;
    ~SyncUpdateTaskLocalSavePath()
    {

    }
};
class SyncUpdateTaskTransState: public SyncData
{
public:
    int transState;
    int taskType;
    ~SyncUpdateTaskTransState()
    {

    }
};

struct SyncDelTask: public SyncData {

};

class SyncUpdateTaskParentInfo : public SyncData
{
public:
    int tParentId;
    std::string strParentAlias;
    ~SyncUpdateTaskParentInfo()
    {
    }
};
class SyncUpdateTaskSavePath : public SyncData
{
public:
    std::string savePath;
    ~SyncUpdateTaskSavePath()
    {

    }
};
class SyncUpdateTaskOutputSize : public SyncData
{
public:
    std::string outputSize;
    ~SyncUpdateTaskOutputSize()
    {

    }
};
class SyncUpdateTaskRequestId : public SyncData
{
public:
    std::string requestId;
    ~SyncUpdateTaskRequestId()
    {

    }
};
class SyncUpdateReportLog : public SyncData
{
public:
    LogReportHistoryInfo info;
    ~SyncUpdateReportLog()
    {

    }
};

struct SyncDelReportLog : public SyncData {
    std::string logpath;
};

class CHistoryDbBase
{
public:
    CHistoryDbBase(void);
    virtual ~CHistoryDbBase(void);
};
#endif // HISTORYDB_H_ 
