#include "stdafx.h"
#include "TaskElement.h"
#include "Common/FileUtil.h"
#include "Common/StringUtil.h"
#include "LoadFileThread.h"

QJsonArray stringlist_to_jsonarray(const QList<QString> & list)
{
    QJsonArray array;
    foreach (const QString & s, list)
        array.append(QString(s.toUtf8()));
    return array;
}

QList<QString> jsonarray_to_stringlist(const QJsonArray& array)
{
    QList<QString> stringlist;
    for (int i = 0; i < array.count(); i++) {
        stringlist.append(array.at(i).toString());
    }
    return stringlist;
}

TaskElement::TaskElement(const stTaskElement& element)
    : m_uid(element.userId)
    , m_job_id(element.taskId)
    , m_input_cg_file("")
    , m_cg_file_name("")
    , m_is_stopped(false)
    , m_cg_id(CG_ID_UNKNOWN)
    , m_hasSameFileError(false)
    , m_isTextureFileLoaded(false)
    , m_retryTimes(0)
    , m_taskUploadPriority(50)
    , m_job_id_alias("")
    , m_pic_count(0)
    , m_coord_count(0)
{
    m_client_project_path = RBHelper::getClientProjectPath();

    m_fileList.setDbPath(this->getUncompleteDbFilePath());
    m_fileList.init();
    initUploadedFileSize();

    this->setTaskRecvMsgTime();
}

TaskElement::~TaskElement()
{
    if(!m_loadFileThread.isNull()) {
        m_loadFileThread->quit();
        m_loadFileThread->wait();
    }
    m_mapUploadRoot.clear();
}

void TaskElement::init()
{
    m_isTextureFileLoaded = false;
    m_fileList.clear();
    m_mapUploadRoot.clear();
    initUploadedFileSize();

    // 初始化映射bid
    build_upload_root_map();
}

bool TaskElement::parse_software_type(const QJsonObject& json)
{
    CG_ID cgid = CG_ID_UNKNOWN;
    // 渲染软件信息
    QJsonObject software_config;
    if (json.contains("software_config") && json["software_config"].isObject()) {
        software_config = json["software_config"].toObject();
    }
    if(software_config.isEmpty()) {
        LOGFMTE("[TaskElement] software_config is empty!");
        return false;
    }

    QString cg_name = software_config["cg_name"].toString().toLower().trimmed();
    if(cg_name == "contextcapture") {
        m_cg_id = CG_ID_CONTEXTCAPTURE;
        taskInfoCC.reset(new TaskInfoCC());
    } else {
        return false;
    }

    return true;
}

bool TaskElement::parse_task_json_cc(const QJsonObject& task_json)
{
    // 系统信息
    QJsonObject task_info;
    if (task_json.contains("task_info") && task_json["task_info"].isObject()) {
        task_info = task_json["task_info"].toObject();
    }
    if (task_info.isEmpty()) {
        LOGFMTE("[TaskElement] task_info is empty!");
        return false;
    }

    taskInfoCC->job_id            = task_info["job_id"].toString("0").toInt();
    taskInfoCC->job_id_alias      = task_info["job_id_alias"].toString();
    taskInfoCC->cg_file           = task_info["input_cg_file"].toString();
    taskInfoCC->project_name      = task_info["project_name"].toString();
    taskInfoCC->plugin_config     = task_info["plugin_config"].toString();
    taskInfoCC->project_id        = task_info["project_id"].toString("0").toInt();
    taskInfoCC->task_timeout      = task_info["time_out"].toString(QString::number(MyConfig.paramSet.overtime.defaultTimeOut)).toInt();
    taskInfoCC->frame_overtime    = task_info["job_stop_time"].toString(QString::number(MyConfig.paramSet.timeoutstop.defaultTimeoutStop)).toInt();
    taskInfoCC->task_overtime     = task_info["task_stop_time"].toString("0").toInt();
    taskInfoCC->ram               = task_info["ram"].toString("64").toInt();
    taskInfoCC->test_frames       = task_info["pre_frames"].toString();
    taskInfoCC->action_after_test = task_info["stop_after_test"].toString("1").toInt();
    taskInfoCC->show_common_tiles = task_info["display_common_tiles"].toString("1").toInt();
    taskInfoCC->tiles             = task_info["tiles"].toString(QString::number(MyConfig.paramSet.renderblocks.defaultBlocks)).toInt();
    taskInfoCC->frames_per_node   = task_info["frames_per_task"].toString(QString::number(MyConfig.paramSet.rendernum.defaultRenderNum)).toInt();
    taskInfoCC->render_type       = task_info["is_picture"].toString(QString::number(EN_REND_ANIMATION)).toInt();
    taskInfoCC->cg_file_name      = QFileInfo(taskInfoCC->cg_file).fileName();
    taskInfoCC->gpuCount          = task_info["graphics_cards_num"].toString("2").toInt();

    m_job_id_alias  = taskInfoCC->job_id_alias;
    m_input_cg_file = taskInfoCC->cg_file;
    m_cg_file_name   = taskInfoCC->cg_file_name;
    m_project_name  = taskInfoCC->project_name;
    m_project_id    = taskInfoCC->project_id;
    m_plugin_config = taskInfoCC->plugin_config;
    m_render_ram    = taskInfoCC->ram;
    // 场景渲染参数
    QJsonObject scene_info;
    if (task_json.contains("scene_info_render") && task_json["scene_info_render"].isObject()) {
        scene_info = task_json["scene_info_render"].toObject();
    } else if (task_json.contains("scene_info") && task_json["scene_info"].isObject()) {
        scene_info = task_json["scene_info"].toObject();
    }
    if (scene_info.isEmpty()) {
        LOGFMTE("[TaskElement] scene_info is empty!");
        return false;
    }

    if (scene_info.contains("group") && scene_info["group"].isObject()) {
        QJsonObject group = scene_info["group"].toObject();
        m_pic_count = group["pic_count"].toInt();
    }
    if (scene_info.contains("coords") && scene_info["coords"].isArray()) {
        QJsonArray coords = scene_info["coords"].toArray();
        m_coord_count = coords.size();
    }

    // 验证区块文件信息是否存在
    if (scene_info["job_type"].toInt() == enBlockProject) {
        if (scene_info.contains("blocks_info") && scene_info["blocks_info"].isArray()) {
            QJsonArray blocks = scene_info["blocks_info"].toArray();
            QStringList files;
            for (int i = 0; i < blocks.size(); i++) {
                QJsonObject file = blocks.at(i).toObject();
                if (file["file_path"].toString().isEmpty()) {
                    files.append(file["file_path"].toString());
                }
            }

            // 发现数量不匹配，则返回错误
            if (blocks.isEmpty() || files.size() != blocks.size()) {
                LOGFMTE("[TaskElement] blocks_info is empty or file count not match!");
                return false;
            }
        }
    }

    // 通用配置
    // QJsonObject scene_info_common;
    // if (scene_info.contains("common") && scene_info["common"].isObject()) {
    //     scene_info_common = scene_info["common"].toObject();
    // }
    // if (scene_info_common.isEmpty()) {
    //     LOGFMTE("[TaskElement] scene_info_common is empty!");
    //     return false;
    // }
    // taskInfoCC->format = scene_info_common["format"].toString();

    // 渲染器配置
    QJsonObject scene_info_renderer;
    if (scene_info.contains("renderer") && scene_info["renderer"].isObject()) {
        scene_info_renderer = scene_info["renderer"].toObject();
    }
    // if (scene_info_renderer.isEmpty()) {
    //     LOGFMTE("[TaskElement] scene_info_renderer is empty!");
    //     return false;
    // }

    return true;
}

// 读取工程文件，得到需要上传的文件列表,每次创建新的任务时需要调用load_project
bool TaskElement::load_project_file(bool is_clone)
{
    // 初始化清理
    init();

    bool bret = false;

    QString task_file_path = QString("%1/%2/task.json").arg(m_client_project_path).arg(m_job_id);

    QJsonObject task_json;
    bret = RBHelper::loadJsonFile(task_file_path, task_json);
    if(!bret) {
        LOGFMTE("[TaskElement] load task json error!");
        return false;
    }

    bret = parse_software_type(task_json);
    if(!bret) {
        LOGFMTE("[TaskElement] parse cg type error!");
        return false;
    }

   if(m_cg_id == CG_ID_CONTEXTCAPTURE) {
        // 获取脚本自定义设置
        bret = parse_task_json_cc(task_json);
        if(!bret) {
            LOGFMTE("[TaskElement] parse cc task json file error, id=%d!", m_job_id);
            return false;
        }
    }

    bret = load_common_file_list();
    if(!bret) {
        LOGFMTE("[TaskElement] load common json files error!");
        return false;
    }

    bret = load_upload_file_list();
    if(!bret) {
        LOGFMTE("[TaskElement] load upload json file error!");
        return false;
    }

    bret = load_texture_file_list();
    if(!bret) {
        LOGFMTE("[TaskElement] load texture files error!");
        return false;
    }

    return true;
}

bool TaskElement::load_common_file_list()
{
    // 添加配置文件
    // task.json/asset.json/tips.json/upload.json
    QString client_project_path = QString("%1/%2/").arg(m_client_project_path).arg(m_job_id);
    QString task_file_name   = "task.json";
    QString asset_file_name  = "asset.json";
    QString tips_file_name   = "tips.json";
    QString upload_file_name = "upload.json";
    QString project_data_name   = "project_data.json";
    QString export_content_name = "export_content.json";

    QString task_file_path   = client_project_path + task_file_name;
    QString asset_file_path  = client_project_path + asset_file_name;
    QString tips_file_path   = client_project_path + tips_file_name;
    QString upload_file_path = client_project_path + upload_file_name;
    QString project_data_path   = client_project_path + project_data_name;
    QString export_content_path = client_project_path + export_content_name;

    if(!add_config_to_filelist(task_file_path.toStdWString())) {
        LOGFMTE("[TaskElement] add task.json failed, ++");
        return false;
    }
    if(!add_config_to_filelist(asset_file_path.toStdWString())) {
        LOGFMTE("[TaskElement] add asset.json failed, ++");
        return false;
    }
    if(!add_config_to_filelist(tips_file_path.toStdWString())) {
        LOGFMTE("[TaskElement] add tips.json failed, ++");
        return false;
    }
    if(!add_config_to_filelist(upload_file_path.toStdWString())) {
        LOGFMTE("[TaskElement] add upload.json failed, ++");
        return false;
    }
    if (!add_config_to_filelist(project_data_path.toStdWString())) {
        LOGFMTE("[TaskElement] add project_data.json failed, ++");
        return false;
    }
    if (!add_config_to_filelist(export_content_path.toStdWString())) {
        LOGFMTE("[TaskElement] add export_content.json failed, ++");
        return false;
    }

    return true;
}

bool TaskElement::load_upload_file_list()
{
    bool bret = false;
    // load upload.json
    QString upload_file_path = QString("%1/%2/upload.json").arg(m_client_project_path).arg(m_job_id);
    QJsonObject upload_json;
    bret = RBHelper::loadJsonFile(upload_file_path, upload_json);
    if(!bret) return false;

    return true;
}

bool TaskElement::load_texture_file_list()
{
    if(isTextureFileLoaded())
        return true;

    if(m_loadFileThread.isNull())
        m_loadFileThread.reset(new LoadFileThread());

    if(m_loadFileThread->isRunning())
        return true;

    QString upload_file_path = QString("%1/%2/upload.json").arg(m_client_project_path).arg(m_job_id);
    m_loadFileThread->setUploadListDb(&m_fileList);
    m_loadFileThread->setAddFilesParam(upload_file_path, m_job_id, m_project_id, m_project_name);
    m_loadFileThread->setSeneceInfo(getSceneName(), getSceneFile(), QString());
    m_loadFileThread->setUploadRootMap(m_mapUploadRoot);
    m_loadFileThread->start();

    return true;
}

// 将脚本生成的工程配置文件添加到传输队列
bool TaskElement::add_config_to_filelist(const std::wstring& local_file)
{
    QFileInfo file_info(QString::fromStdWString(local_file));
    if(!file_info.exists()) {
        LOGFMTE("[TaskElement] Set config by task failed: file not exist! task=%d file=%s",
            m_job_id, qPrintable(QString::fromStdWString(local_file)));
        return false;
    }

    // /(uid / 500) * 500 / uid
    int user_folder_id = (m_uid / 500) * 500;
    QString file_path = file_info.absoluteFilePath();
    QString file_name = file_info.fileName();
    QString remote_root = QString("/%1/%2/%3/cfg/%4").arg(user_folder_id)
        .arg(m_uid)
        .arg(m_job_id)
        .arg(file_name);

    this->add_file_to_filelist(file_path, remote_root, TYPE_CFG);

    return true;
}

// 添加新文件，记录文件状态为FILE_NOT_START
bool TaskElement::add_file_to_filelist(const QString& localPath, const QString& remotePath, enAssetType fileType)
{
    UncompleteFile file;
    file.status = FILE_NOT_START;
    file.status += FILE_CHECK_SUCCESS;

    file.projectid       = m_project_id;
    file.projectsymbol   = m_project_name;
    file.local_file      = localPath;
    file.remote_file     = remotePath;
    file.taskid          = m_job_id;
    file.rootid          = QString::number(m_mapUploadRoot[fileType]);
    file.pathMd5         = String::getStringMd5(localPath + remotePath);
    file.loadtexturefile = 1;
    file.scene           = getSceneFile();
    file.framename       = getSceneName();
    file.updatetime      = QString::number(QDateTime::currentDateTime().toTime_t());

    qint64 fileSize = 0;
    QFileInfo fileInfo(localPath);
    if(fileInfo.exists()) {
        fileSize = fileInfo.size();
    }
    file.filesize = fileSize;

    // 单个文件属性(主要是第一次保存的时候有用)
    if(fileType == TYPE_TEXT) {
        file.filetype = TYPE_SCENE;
    } else if(fileType == TYPE_CFG) {
        file.filetype = TYPE_CFG;
    }
    LOGFMTD("[TaskElement] load file localPath=%s remotePath=%s", qPrintable(localPath), qPrintable(remotePath));
    m_fileList.insertOneRecord(file);

    return true;
}

bool TaskElement::is_task_completed()
{
    if(m_fileList.getCompleteCount() == m_fileList.getTotalCount()) {
        return true;
    }
    return false;
}

bool TaskElement::is_task_done()
{
    int completed = m_fileList.getCompleteCount();
    int failed = m_fileList.getFailedCount();
    int total = m_fileList.getTotalCount();
    LOGFMTI("[TaskElement] is_task_done, taskid=%d, CompleteCount=%d, FailedCount=%d, TotalCout=%d",
            m_job_id, completed, failed, total);

    bool is_done = completed + failed == total;
    if(is_done) {
        this->setTaskRetryTimes(this->getTaskRetryTimes() + 1);
    }
    return is_done;
}

bool TaskElement::build_upload_root_map()
{
    StorageSet baolong = MyConfig.storageSet;

    m_mapUploadRoot[TYPE_TEXT]  = baolong.uploadBid;
    m_mapUploadRoot[TYPE_SCENE] = baolong.uploadBid;
    m_mapUploadRoot[TYPE_CFG]   = baolong.commonBid;

    return true;
}

// 重新修正上传baolong id
bool TaskElement::rebuild_upload_root_map()
{
    this->build_upload_root_map();

    QList<UncompleteFile> transList;
    int total = m_fileList.getTransformListCount(transList);

    if(!m_fileList.execSqlPrepare()) return false;
    m_fileList.operationBegin();
    auto it = transList.begin();
    for(it; it != transList.end(); ++it) {
        it->rootid = QString::number(m_mapUploadRoot[(*it).filetype]);
        m_fileList.appendFile(*it);
    }
    m_fileList.operationCommit();

    return true;
}

void TaskElement::load_file_thread_finished()
{
    if(m_loadFileThread.isNull()) return;

    if(m_loadFileThread->isRunning()) {
        m_loadFileThread->quit();
        m_loadFileThread->wait();
    }

    // 验证数据完整性
    if (this->isUploadLegal()) {
        m_isTextureFileLoaded = true;
    }
    
    m_loadFileThread.reset();
}

bool TaskElement::isTextureFileLoaded()
{
    return m_isTextureFileLoaded;
}

void TaskElement::setSameFileError(bool hasError)
{
    m_hasSameFileError = hasError;
}

bool TaskElement::hasSameFileError()
{
    return m_hasSameFileError;
}

// 设置任务获取消息返回的时间
void TaskElement::setTaskRecvMsgTime()
{
    m_lastRecvMsgTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
}

// 等待消息是否超时
bool TaskElement::isWaitMsgTimeout()
{
    qint64 curTime = QDateTime::currentDateTime().toMSecsSinceEpoch();

    // 如果有5分钟没有收到消息，则表示引擎已经掉线
    if(curTime - m_lastRecvMsgTime > 1000 * 60 * 5) {
        LOGFMTE("[TaskElement] Wait engine msg timeout, elapsed time is %lld", curTime - m_lastRecvMsgTime);
        return true;
    }

    return false;
}

QString TaskElement::getUncompleteDbFilePath()
{
    QString dbpath = QString("%1/upload_cache/%2")
        .arg(RBHelper::getUserProfilePath())
        .arg(m_job_id);

    RBHelper::makeDirectory(dbpath);

    return QDir::toNativeSeparators(dbpath);
}

bool TaskElement::initDataFromLocalDb()
{
    UncompleteFile unfinishedFile = m_fileList.queryOneData(m_job_id);
    if(unfinishedFile.local_file.isEmpty() && unfinishedFile.remote_file.isEmpty())
        return false;

    // 校验数据库中的内容是否与json数据一致
    if (!this->isUploadLegal()) {
        return false;
    }

    m_project_id = unfinishedFile.projectid;
    m_project_name = unfinishedFile.projectsymbol;
    m_isTextureFileLoaded = unfinishedFile.loadtexturefile == 1 ? true : false;
    m_cg_file_name = unfinishedFile.framename;
    m_input_cg_file = unfinishedFile.scene;

    return true;
}

// 任务重试次数
void TaskElement::setTaskRetryTimes(int retryTimes)
{
    m_retryTimes = retryTimes;
}

int TaskElement::getTaskRetryTimes()
{
    return m_retryTimes;
}

// 任务优先级
void TaskElement::provTaskUploadPriority()
{
    m_taskUploadPriority += 1;
}

int TaskElement::getTaskUploadPriority()
{
    return m_taskUploadPriority;
}

bool TaskElement::isUploadLegal()
{
    return true;

    // json文件中的贴图文件数量
    QString upload_file_path = QString("%1/%2/upload.json").arg(m_client_project_path).arg(m_job_id);
    int localTextCount = RBHelper::getJsonContainsValueCount(upload_file_path, "asset");

    // 数据库中总文件个数
    int dbTotalCount = m_fileList.getTotalCount();

    if (dbTotalCount > 0 && localTextCount > dbTotalCount) {
        return false;
    }
    return true;
}

void TaskElement::setUploadFileSize(const QString& local_file, const QString& remote_file, qint64 trans_size, qint64 total_size)
{
    QString pathMd5 = local_file + remote_file;
    pathMd5 = String::getStringMd5(pathMd5);

    qint64 cur_size = m_uploadedFileSize[pathMd5];
    if (cur_size < total_size) {
        if (trans_size > 0 && total_size > 0) {
            m_uploadedFileSize[pathMd5] = trans_size;
        } else {
            m_uploadedFileSize[pathMd5] = 0;
        }
            
    } else {
        m_uploadedFileSize[pathMd5] = total_size;
    }
}

qint64 TaskElement::getUploadedFileSize()
{
    if (m_uploadedFileSize.isEmpty())
        return 0;

    qint64 uploadSize = 0;
    QList<qint64> uploaded = m_uploadedFileSize.values();
    foreach(qint64 size, uploaded) {
        uploadSize += size;
    }
    return uploadSize;
}

int TaskElement::getPictureCount()
{
    return m_pic_count;
}

int TaskElement::getCoordCount()
{
    return m_coord_count;
}

void TaskElement::initUploadedFileSize()
{
    QList<UncompleteFile> fileList = m_fileList.getFinishedLists();
    foreach(UncompleteFile file, fileList) {
        m_uploadedFileSize.insert(file.pathMd5, file.filesize);
    }
}