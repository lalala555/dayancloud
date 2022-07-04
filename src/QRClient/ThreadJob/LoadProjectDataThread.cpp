#include "stdafx.h"
#include "LoadProjectDataThread.h"

LoadProjectDataThread::LoadProjectDataThread(qint64 taskId, BaseTaskInfo* baseTaskInfo, QObject *parent)
    : QThread(parent)
    , m_taskId(taskId)
    , m_baseTaskInfo(baseTaskInfo)
{
    if (m_baseTaskInfo == nullptr) {
        m_baseTaskInfo = new BaseTaskInfo;
    }
}

LoadProjectDataThread::~LoadProjectDataThread()
{
}

qint64 LoadProjectDataThread::getTaskId()
{
    return m_taskId;
}

BaseTaskInfo* LoadProjectDataThread::getBaseTaskInfo()
{
    return m_baseTaskInfo;
}

QString LoadProjectDataThread::getProjectFilePath()
{
    QString client_project_base_dir = LocalSetting::getInstance()->getClientProjectPath();
    QString client_project_dir = QString("%1/%2/").arg(client_project_base_dir).arg(m_taskId);
    client_project_dir = QDir::cleanPath(client_project_dir);
    QString argument_file = client_project_dir + "/project_data.json";

    return argument_file;
}

bool LoadProjectDataThread::loadPictureGroupInfo(const QJsonObject& task_json)
{
    QJsonArray groups;
    if (task_json.contains("groups") && task_json["groups"].isArray()) {
        groups = task_json["groups"].toArray();
    }
    if (groups.isEmpty()) {
        LOGFMTE("[TaskElement] groups is empty!");
        return false;
    }

    for (int i = 0; i < groups.size(); i++){
        QJsonObject groupItem = groups[i].toObject();    
        PictureGroup* group = new PictureGroup;

        group->groupId   = groupItem["group_id"].toString();
        group->groupName = groupItem["group_name"].toString();
        group->groupPath = groupItem["group_path"].toString();
        group->picHeight = groupItem["pic_height"].toString().toInt();
        group->picWidth  = groupItem["pic_width"].toString().toInt();
        group->picsCount = groupItem["pic_count"].toString().toInt();

        group->cameraProducter = groupItem["camera_producter"].toString();
        group->cameraModel     = groupItem["camera_model"].toString();
        group->sensorSize      = groupItem["sensor_size"].toString();
        group->focalLength     = groupItem["focal_length"].toString();
        group->totalPixelStr   = groupItem["total_pixel_str"].toString();
        group->totalPixel      = groupItem["total_pixel"].toVariant().toULongLong();

        if (groupItem["pictures"].isArray()) {
            QJsonArray pictures = groupItem["pictures"].toArray();
            for (int j = 0; j < pictures.size(); j++) {
                QJsonObject picItem = pictures[j].toObject();
                PictureInfo picInfo;
                picInfo.picName = picItem["pic_name"].toString();
                picInfo.picLongitude   = picItem["pic_longitude"].toDouble();
                picInfo.picLatitude    = picItem["pic_latitude"].toDouble();
                picInfo.picAltitude    = picItem["pic_altitude"].toDouble();
                picInfo.picPath        = picItem["pic_path"].toString();
                picInfo.picSize        = picItem["pic_size"].toVariant().toLongLong();
                picInfo.fileUniqueCode = picItem["unique_code"].toString();
                picInfo.fileMtime      = picItem["file_mtime"].toVariant().toLongLong();

                picInfo.cameraProducter = group->cameraProducter;
                picInfo.cameraModel = group->cameraModel;
                picInfo.sensorSize  = group->sensorSize;
                picInfo.focalLength = group->focalLength;

                group->pictures.append(picInfo);
            }    
        }

        m_baseTaskInfo->groups.append(group);
    }

    return true;
}

bool LoadProjectDataThread::loadCtrlPointInfo(const QJsonObject& task_json)
{
    QJsonObject ctrlPoint;
    if (task_json.contains("contrl_point") && task_json["contrl_point"].isObject()) {
        ctrlPoint = task_json["contrl_point"].toObject();
    }
    if (ctrlPoint.isEmpty()) {
        LOGFMTE("[TaskElement] contrl point is empty!");
        return true;
    }

    m_baseTaskInfo->ctrlPointInfo.filePath = ctrlPoint["file_path"].toString();
    m_baseTaskInfo->ctrlPointInfo.spliteChar = ctrlPoint["splite_char"].toString();
    m_baseTaskInfo->ctrlPointInfo.ignoreLines = ctrlPoint["ignore_lines"].toString();
    m_baseTaskInfo->ctrlPointInfo.fieldOrder = ctrlPoint["field_order"].toString();
    m_baseTaskInfo->ctrlPointInfo.coordSys = ctrlPoint["coord_system"].toString();

    if (ctrlPoint["item_datas"].isArray()) {
        QJsonArray itemdata = ctrlPoint["item_datas"].toArray();
        for (int i = 0; i < itemdata.size(); i++) {
            QJsonObject item = itemdata[i].toObject();
            ConfigDataItem* cdata = new ConfigDataItem;

            cdata->index        = item["index"].toVariant().toLongLong();
            cdata->imageName    = item["name"].toString();
            cdata->picLongitude = item["longitude"].toString();
            cdata->picLatitude  = item["latitude"].toString();
            cdata->picAltitude  = item["altitude"].toString();

            m_baseTaskInfo->ctrlPointInfo.itemDatas.append(cdata);
        }
    }

    return true;
}

bool LoadProjectDataThread::loadPositionInfo(const QJsonObject& task_json)
{
    QJsonObject posInfo;
    if (task_json.contains("pos_info") && task_json["pos_info"].isObject()) {
        posInfo = task_json["pos_info"].toObject();
    }
    if (posInfo.isEmpty()) {
        LOGFMTE("[TaskElement] position info is empty!");
        return true;
    }

    m_baseTaskInfo->posInfo.filePath = posInfo["file_path"].toString();
    m_baseTaskInfo->posInfo.spliteChar = posInfo["splite_char"].toString();
    m_baseTaskInfo->posInfo.ignoreLines = posInfo["ignore_lines"].toString();
    m_baseTaskInfo->posInfo.fieldOrder = posInfo["field_order"].toString();
    m_baseTaskInfo->posInfo.coordSys = posInfo["coord_system"].toString();

    if (posInfo["item_datas"].isArray()) {
        QJsonArray itemdata = posInfo["item_datas"].toArray();
        for (int i = 0; i < itemdata.size(); i++) {
            QJsonObject item = itemdata[i].toObject();
            ConfigDataItem* cdata = new ConfigDataItem;

            cdata->index        = item["index"].toVariant().toLongLong();
            cdata->imageName    = item["name"].toString();
            cdata->picLongitude = item["longitude"].toString();
            cdata->picLatitude  = item["latitude"].toString();
            cdata->picAltitude  = item["altitude"].toString();

            m_baseTaskInfo->posInfo.itemDatas.append(cdata);
        }
    }

    return true;
}

bool LoadProjectDataThread::loadBlockInfo(const QJsonObject& task_json)
{
    QJsonArray blocks;
    if (task_json.contains("blocks_info") && task_json["blocks_info"].isArray()) {
        blocks = task_json["blocks_info"].toArray();
    }
    if (blocks.isEmpty()) {
        LOGFMTE("[TaskElement] blocks info is empty!");
        return true;
    }

    for (int i = 0; i < blocks.size(); i++) {
        QJsonObject block = blocks[i].toObject();
        BlockInfo info;

        info.filePath = block["file_path"].toString();

        m_baseTaskInfo->blocks.append(info);
    }

    return true;
}

void LoadProjectDataThread::run()
{
    int code = loadProjectData();
    emit loadProjectDataFinished(code);
}

int LoadProjectDataThread::loadProjectData()
{
    bool bret = false;

    QString task_file_path = getProjectFilePath();

    QJsonObject task_json;
    bret = RBHelper::loadJsonFile(task_file_path, task_json);
    if (!bret) {
        LOGFMTE("[TaskElement] load task json error!");
        return CODE_JSON_FILE_LOAD_FAILED;
    }

    // load root data
    if (task_json.contains("job_type")) {
        m_baseTaskInfo->jobType = task_json["job_type"].toString().toInt();
    }
    if (task_json.contains("project_name")) {
        m_baseTaskInfo->projectName = task_json["project_name"].toString();
    }
    if (task_json.contains("is_submit_ctrl_point")) {
        m_baseTaskInfo->isSubmitCtrlPoint = task_json["is_submit_ctrl_point"].toBool(false);
    }
    if (task_json.contains("is_submit_pos_info")) {
        m_baseTaskInfo->isSubmitPosInfo = task_json["is_submit_pos_info"].toBool(false);
    }
    if (task_json.contains("total_pixel")) {
        m_baseTaskInfo->totalPixel = task_json["total_pixel"].toString();
    }

    m_baseTaskInfo->blocks.clear();
    qDeleteAll(m_baseTaskInfo->groups);
    m_baseTaskInfo->groups.clear();
    qDeleteAll(m_baseTaskInfo->ctrlPointInfo.itemDatas);
    m_baseTaskInfo->ctrlPointInfo.itemDatas.clear();
    qDeleteAll(m_baseTaskInfo->posInfo.itemDatas);
    m_baseTaskInfo->posInfo.itemDatas.clear();

    bret = loadPictureGroupInfo(task_json);
    if (!bret) {
        LOGFMTE("[TaskElement] load pictrue group info error!");
        return CODE_JSON_FILE_LOAD_FAILED;
    }

    bret = loadCtrlPointInfo(task_json);
    if (!bret) {
        LOGFMTE("[TaskElement] load ctrl point info error!");
        return CODE_JSON_FILE_LOAD_FAILED;
    }

    bret = loadPositionInfo(task_json);
    if (!bret) {
        LOGFMTE("[TaskElement] load position info error!");
        return CODE_JSON_FILE_LOAD_FAILED;
    }

    bret = loadBlockInfo(task_json);
    if (!bret) {
        LOGFMTE("[TaskElement] load block info error!");
        return CODE_JSON_FILE_LOAD_FAILED;
    }

    return CODE_SUCCESS;
}