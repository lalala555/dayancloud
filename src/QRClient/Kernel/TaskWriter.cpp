#include "stdafx.h"
#include "TaskWriter.h"
#include "Common/SystemUtil.h"
#include "PluginConfig/ProjectMgr.h"
#include "common/FileUtil.h"

TaskCCWriter::TaskCCWriter(QSharedPointer<TaskInfo> task)
    : action_after_test(ACTION_AFTER_TEST_PAUSE)
    , muiltFrames("")
    , submitLayer("")
    , m_task(task)
    , job_id(task->job_id)
    , job_id_alias(task->job_id_alias)
{

}

void TaskCCWriter::init_project_path()
{
    QString client_project_base_dir = LocalSetting::getInstance()->getClientProjectPath();
    client_project_dir = QString("%1/%2/").arg(client_project_base_dir).arg(QString::number(job_id));
    client_project_dir = QDir::cleanPath(client_project_dir);
}

QJsonObject TaskCCWriter::create_object(int version)
{
    CG_ID cgid = CG_ID_CONTEXTCAPTURE;
    CGConfigItem local_cg_config = CConfigDb::Inst()->getCGConfig(cgid);
    QString project_dir = plugin.projectPath;
    if (project_dir.isEmpty()) project_dir = QString::fromStdString(local_cg_config.project_dir);
    QString projet_output = ProjectMgr::getInstance()->getProjectOutput(project.projectName);
    QString soft_install_path = CProfile::Inst()->getCgSoftwarePath(cgid); //todo
    QSharedPointer<TaskInfoCC> taskInfo = qSharedPointerCast<TaskInfoCC>(m_task);
    QString project_name = taskInfo->project_name;
    cg_file = taskInfo->groups[0].groupPath;

    QJsonObject root;
    root.insert("version", version);

    // 写入task_info
    QJsonObject task_info_object;
    // 用户信息
    task_info_object.insert("platform", QString::number(MyConfig.accountSet.platform));
    task_info_object.insert("channel", QString::number(CHANNEL_MODELLING));
    task_info_object.insert("cg_id", QString::number(cgid));
    task_info_object.insert("input_cg_file", cg_file);
    task_info_object.insert("input_project_path", project_dir);
    task_info_object.insert("locationoutput", projet_output);
    task_info_object.insert("task_id", QString::number(job_id));
    task_info_object.insert("job_id", QString::number(job_id));
    task_info_object.insert("job_id_alias", job_id_alias);
    task_info_object.insert("ram", ram);
    task_info_object.insert("pre_frames", test_frames);
    task_info_object.insert("stop_after_test", QString::number(action_after_test));
    task_info_object.insert("is_picture", QString::number(EN_REND_ANIMATION));  // 0效果图，1动画
    task_info_object.insert("frames_per_task", muiltFrames.isEmpty() ? QString::number(1) : muiltFrames);
    task_info_object.insert("is_distribute_render", QString::number(0));
    task_info_object.insert("distribute_node", QString::number(1));
    task_info_object.insert("tiles_type", true ? "strip" : "block"); // block(分块),strip(分条)
    task_info_object.insert("tiles", QString::number(1));
    task_info_object.insert("display_common_tiles", QString::number(1));
    task_info_object.insert("job_stop_time", QString::number(MyConfig.paramSet.timeoutstop.maxTimeoutStop)); // 帧超时  取后台最大值
    task_info_object.insert("task_stop_time", QString::number(MyConfig.paramSet.timeoutstop.maxTimeoutStop));  // 任务超时 
    task_info_object.insert("time_out", QString::number(MyConfig.paramSet.overtime.defaultTimeOut));  // 超时提醒
    task_info_object.insert("project_id", QString::number(project.projectId));
    task_info_object.insert("project_name", project_name);
    task_info_object.insert("config_name", plugin.editName);
    task_info_object.insert("edit_name", plugin.editName);
    task_info_object.insert("render_layer_type", QString::number(0));
    task_info_object.insert("graphics_cards_num", gpuCount);
    root.insert("task_info", task_info_object);

    QJsonObject user_object;
    user_object.insert("user_id", MyConfig.accountSet.userId);
    user_object.insert("user_key", MyConfig.userSet.userKey);
    user_object.insert("user_name", MyConfig.userSet.userName);
    user_object.insert("acount_type", MyConfig.accountSet.accountType);
    user_object.insert("parent_id", MyConfig.accountSet.mainUserId);
    user_object.insert("zone", MyConfig.accountSet.zone);
    user_object.insert("shareMainCapital", MyConfig.accountSet.shareMainCapital);
    root.insert("user_setting", user_object);

    // 管理后台配置
    QJsonObject render_object;
    // Common
    render_object.insert("ignoreMapFlag", MyConfig.renderSet.ignoreMapFlag);
    render_object.insert("ignoreAnalyseWarn", MyConfig.renderSet.ignoreAnalyseWarn);
    render_object.insert("manuallyStartAnalysisFlag", MyConfig.renderSet.manuallyStartAnalysisFlag);

    root.insert("render_setting", render_object);

    // 插件项目配置
    QJsonObject plugin_object;
    QJsonObject software_config_object;
    // software_config_object.insert("os_name", QString::number(plugin.osName));
    software_config_object.insert("os_name", QString::number(1));
    software_config_object.insert("cg_version", plugin.cgVersion);
    // software_config_object.insert("cg_name", plugin.cgName);
    software_config_object.insert("cg_name", "ContextCapture");
    software_config_object.insert("cg_inst_dir", soft_install_path); // 软件安装路径
    QJsonObject plugs_object;
    foreach(auto plug, plugin.infoVos) {
        QStringList plugn_ver = plug.pluginVersion.trimmed().split(" ");
        if(plugn_ver.size() == 2)
            plugs_object.insert(plugn_ver[0], plugn_ver[1]);
    }
    software_config_object.insert("plugins", plugs_object);
    plugin_object.insert("software_config", software_config_object);
    // plugin_object.insert("cg_id", plugin.cgId); 
    plugin_object.insert("cg_id", cgid);
    plugin_object.insert("cg_name", plugin.cgName);
    plugin_object.insert("cg_version", plugin.cgVersion);
    plugin_object.insert("config_name", plugin.editName);
    plugin_object.insert("edit_name", plugin.editName);
    plugin_object.insert("os_name", QString::number(1));
    plugin_object.insert("render_layer_type", QString::number(0));
    plugin_object.insert("project_id", project.projectId);
    plugin_object.insert("project_name", project.projectName);
    plugin_object.insert("project_dir", project_dir);
    plugin_object.insert("project_output", projet_output);
    root.insert("plugin_setting", plugin_object);
    root.insert("software_config", software_config_object);

    // 客户端环境配置
    QJsonObject client_object;
    client_object.insert("os", System::getSysProductName());
    client_object.insert("version", STRFILEVERSION);
    client_object.insert("build", BUILD_TIME);
    client_object.insert("login_name", System::getLoginDisplayName());
    client_object.insert("ip", System::getLanIP());
    client_object.insert("mac", System::getMacAddress());
    client_object.insert("date_time", RBHelper::currentDateTime());
    client_object.insert("status", WSConfig::getInstance()->getWSVersion());
    client_object.insert("mode", mode);
    client_object.insert("client", PRODUCT_NAME);
    client_object.insert("client_inst_path", qApp->applicationDirPath());
    client_object.insert("client_cmd", "rendercmd.exe");
    client_object.insert("client_project_dir", client_project_dir);
    client_object.insert("language", LocalSetting::getInstance()->getLanguageId());
    client_object.insert("param_set_enabled", CConfigDb::Inst()->getEnableSetParam());
    client_object.insert("transmit_engine", LocalSetting::getInstance()->getTransEngine());
    client_object.insert("transmit_line", LocalSetting::getInstance()->getNetWorkName());
    client_object.insert("system_locale", QLocale::system().name());
    client_object.insert("auto_download", CConfigDb::Inst()->getAutoDownload());
    root.insert("client_setting", client_object);

    // 照片组
    QJsonArray groupArray;
    QJsonArray posArray;
    QJsonObject groupInfo;
    QStringList posFiles;
    int picCount = 0;
    qint64 totalPixel = 0;
    for (int i = 0; i < taskInfo->groups.size(); i++) {
        QJsonObject group;
        PictureGroup groupInfo = taskInfo->groups[i];       
        group["group_id"]   = groupInfo.groupId;
        group["group_name"] = groupInfo.groupName;
        group["group_path"] = groupInfo.groupPath;
        group["pic_height"] = QString::number(groupInfo.picHeight);
        group["pic_width"]  = QString::number(groupInfo.picWidth);
        group["pic_count"]  = QString::number(groupInfo.picsCount);

        group["camera_producter"] = groupInfo.cameraProducter;
        group["camera_model"] = groupInfo.cameraModel;
        group["sensor_size"]  = groupInfo.sensorSize;
        group["focal_length"] = groupInfo.focalLength;
        group["total_pixel_str"]  = groupInfo.totalPixelStr;
        group["total_pixel"] = QString::number(groupInfo.totalPixel);

        picCount += groupInfo.picsCount;
        totalPixel += groupInfo.totalPixel;

        // pos info
        QJsonObject posInfo;
        if (!groupInfo.posInfo.filePath.isEmpty()) {         
            posInfo["file_path"]    = groupInfo.posInfo.filePath;
            posInfo["coord_system"]    = groupInfo.posInfo.coordSys;
            posInfo["ignore_lines"] = groupInfo.posInfo.ignoreLines;
            posInfo["splite_char"]  = groupInfo.posInfo.spliteChar;
            posInfo["field_order"]  = groupInfo.posInfo.fieldOrder;                  
        }
        group["pos_info"] = posInfo;

        // pictures
        QJsonArray pictures;
        for (int j = 0; j < groupInfo.pictures.size(); j++) {
            PictureInfo picInfo = groupInfo.pictures[j];
            QJsonObject pic;

            pic["pic_size"]      = picInfo.picSize;
            pic["pic_name"]      = picInfo.picName;
            pic["pic_longitude"] = picInfo.picLongitude;
            pic["pic_latitude"]  = picInfo.picLatitude;
            pic["pic_altitude"]  = picInfo.picAltitude;
            pic["pic_path"]      = picInfo.picPath;
            pic["unique_code"]   = picInfo.fileUniqueCode;
            pic["file_mtime"]    = picInfo.fileMtime;

            pictures.append(pic);
        }
        group["pictures"] = pictures;

        groupArray.append(group);
    }
    groupInfo["pic_count"] = QString::number(picCount);
    groupInfo["job_type"]  = QString::number(taskInfo->groups[0].jobType);
    groupInfo["project_name"] = project_name;
    groupInfo["total_pixel"]  = QString::number(totalPixel);
    groupInfo["groups"] = groupArray;
    root.insert("group", groupInfo);

    // 控制点
    QJsonObject contrlPoint;
    if (!taskInfo->ctrlPointInfo.filePath.isEmpty()) {        
        contrlPoint["coord_system"] = taskInfo->ctrlPointInfo.coordSys;
        contrlPoint["field_order"] = taskInfo->ctrlPointInfo.fieldOrder;
        contrlPoint["ignore_lines"] = taskInfo->ctrlPointInfo.ignoreLines;
        contrlPoint["file_path"] = taskInfo->ctrlPointInfo.filePath;
        contrlPoint["splite_char"] = taskInfo->ctrlPointInfo.spliteChar;   
        contrlPoint["item_count"] = QString::number(taskInfo->ctrlPointInfo.itemDatas.size());
    }
    root.insert("contrl_point", contrlPoint);

    // blocks
    QJsonArray blocks;
    foreach(BlockInfo info, taskInfo->blocks) {
        QJsonObject block;
        block["file_path"] = info.filePath;
        blocks.append(block);
    }
    root.insert("blocks_info", blocks);

    // assets
    QJsonObject assets;
    QJsonArray assetArray;
    QJsonArray sceneArray;
    // 照片组图片
    for (int i = 0; i < taskInfo->groups.size(); i++) {
        PictureGroup group = taskInfo->groups[i];
        for (int j = 0; j < group.pictures.size(); j++) {
            QString local = QDir::fromNativeSeparators(group.pictures[j].picPath);
            assetArray.append(local);
        }
    }

    // project_data.json
    QString proj_file = client_project_dir + "/project_data.json";
    QString local = QDir::fromNativeSeparators(proj_file);
    assetArray.append(local);

    // export_content.json
    QString content_file = client_project_dir + "/export_content.json";
    QString content_local = QDir::fromNativeSeparators(content_file);
    assetArray.append(content_local);

    // pos文件
    QJsonArray posFileArray;
    for (int i = 0; i < taskInfo->groups.size(); i++) {
        PictureGroup group = taskInfo->groups[i];
        QString local = QDir::fromNativeSeparators(taskInfo->posInfo.filePath);
        if (local.isEmpty() || posFileArray.contains(local)) {
            continue;
        } 
        posFileArray.append(local);
    }
    // kml文件
    QJsonArray kmlFileArray;
    if (!taskInfo->settings.kmlFilePath.isEmpty()) {
        QString local = QDir::fromNativeSeparators(taskInfo->settings.kmlFilePath);
        kmlFileArray.append(local);
    }
    // blocks
    QJsonArray blocksArray;
    foreach(BlockInfo info, taskInfo->blocks) {
        QString local = QDir::fromNativeSeparators(info.filePath);
        blocksArray.append(local);
    }
    // contrlpoint
    QJsonArray ctrlPointArray;
    if(!taskInfo->ctrlPointInfo.filePath.isEmpty()) {
        QString local = QDir::fromNativeSeparators(taskInfo->ctrlPointInfo.filePath);
        ctrlPointArray.append(local);
    }
    
    assets["asset"]       = assetArray;
    assets["pos_file"]    = posFileArray;
    assets["scene"]       = sceneArray;
    assets["kml_file"]    = kmlFileArray;
    assets["block_files"] = blocksArray;
    assets["ctrl_point_file"] = ctrlPointArray;
    root.insert("assets", assets);

#if 1
	//参数json 写入 2021/12/11
	RebuildOptionParams para = taskInfo->settings.rebuildOption;

	QJsonObject optionParam;
	optionParam.insert("worldCoordSys", para.coordSys);
	optionParam.insert("lockRange", para.lockRange);
	if (!para.usrCoordContent.isEmpty()) {
		optionParam.insert("userCoordSys", para.usrCoordContent);
	}
	QJsonArray typeArray;
	for (int i = 0; i < para.outputTypes.size(); i++)
	{
		typeArray.append(para.outputTypes[i]);
	}
	optionParam.insert("outputType", typeArray);
	QJsonObject rangeParam;
	if (para.lockRange == 0) {
		QFile kmlFile(para.rangeFile);
		if (kmlFile.open(QIODevice::ReadOnly)) {
			QString content = kmlFile.readAll().toBase64();
			rangeParam.insert("fileName", kmlFile.fileName());
			rangeParam.insert("fileSize", kmlFile.size());
			rangeParam.insert("fileContent", content);
		}
	}
	optionParam.insert("rangeFile", rangeParam);
	// 自定义原点
	if (para.originCoord.enable) {
		QJsonObject originCoord;
		originCoord.insert("coord_x", QString::number(para.originCoord.coord_x, 'f', 6));
		originCoord.insert("coord_y", QString::number(para.originCoord.coord_y, 'f', 6));
		originCoord.insert("coord_z", QString::number(para.originCoord.coord_z, 'f', 6));

		optionParam.insert("originCoord", originCoord);
	}
	//坐标偏移
	if (para.offsetCoord.enable) {
		QJsonObject offsetCoord;
		offsetCoord.insert("coord_x", QString::number(para.offsetCoord.coord_x, 'f', 6));
		offsetCoord.insert("coord_y", QString::number(para.offsetCoord.coord_y, 'f', 6));
		offsetCoord.insert("coord_z", QString::number(para.offsetCoord.coord_z, 'f', 6));

		optionParam.insert("offsetCoord", offsetCoord);

	}
	// 瓦片大小
	optionParam.insert("tileMode", QString::number(para.tileMode));
	optionParam.insert("tileSize", para.tileSize);

	root.insert("param", optionParam);
#endif

//空三分块设置
	if (taskInfo->blockAtSetting.toBlock)
	{
		blockATSettingInfo atSetting = taskInfo->blockAtSetting;
		QJsonObject many_at;
		many_at.insert("block_merge", QString::number(atSetting.toMerge ? 1 : 0));
		QJsonArray kmls;
		for (size_t i = 0; i < atSetting.many_at_kml.size(); i++)
		{
			QFile kmlFile(atSetting.many_at_kml[i]);
			if (kmlFile.open(QIODevice::ReadOnly)) {

				QJsonObject currKmlInfo;	
				currKmlInfo.insert("kml_id", QString::number(i));
				currKmlInfo.insert("kml_name", kmlFile.fileName());
				currKmlInfo.insert("total_pixel", "");
				currKmlInfo.insert("fileSize", kmlFile.size());
				QString content = kmlFile.readAll().toBase64();
				currKmlInfo.insert("content", content);
				kmls.append(currKmlInfo);
			}
		}
		many_at.insert("kmls", kmls);
		root.insert("many_at", many_at);
	}

    return root;
}


bool TaskCCWriter::update_cc_task(const TaskInfoCC & taskInfo)
{
    bool bret = false;

    // load task.json
    QString task_file_path = QString("%1/task.json").arg(client_project_dir);

    QJsonObject task_json;
    bret = RBHelper::loadJsonFile(task_file_path, task_json);
    if(!bret) {
        LOGFMTE("[TaskElement] load task json error!");
        return false;
    }
    bret = update_task_json_cc(task_json, taskInfo);
    if(!bret) {
        LOGFMTE("[TaskElement] update cc json file error, id=%lld!", job_id);
        return false;
    }

    // save task.json
    bret = RBHelper::saveJsonFile(task_file_path, task_json);

    return bret;
}

bool TaskCCWriter::update_task_json_cc(QJsonObject& task_json, const TaskInfoCC& taskInfo)
{
    // 系统信息
    QJsonObject task_info;
    if(task_json.contains("task_info") && task_json["task_info"].isObject()) {
        task_info = task_json["task_info"].toObject();
    }
    if(task_info.isEmpty()) {
        LOGFMTE("[TaskElement] task_info is empty!");
        return false;
    }

    task_info["ram"] = QString::number(taskInfo.ram);
    task_info["time_out"] = QString::number(taskInfo.task_timeout);
    task_info["stop_after_test"] = QString::number(taskInfo.action_after_test);
    task_info["pre_frames"] = taskInfo.test_frames;
    task_info["frames_per_task"] = QString::number(taskInfo.frames_per_node);
    task_info["job_stop_time"] = QString::number(taskInfo.frame_overtime);
    task_info["tiles"] = QString::number(taskInfo.tiles);
    task_info["graphics_cards_num"] = QString::number(taskInfo.gpuCount);
    task_info["project_name"] = taskInfo.project_name;

    // 场景渲染参数
    QJsonObject scene_info;
    if(task_json.contains("scene_info_render") && task_json["scene_info_render"].isObject()) {
        scene_info = task_json["scene_info_render"].toObject();
    } else if(task_json.contains("scene_info") && task_json["scene_info"].isObject()) {
        scene_info = task_json["scene_info"].toObject();
    }
    // if(scene_info.isEmpty()) {
    //     LOGFMTE("[TaskElement] scene_info is empty!");
    //     return false;
    // }

    // 通用配置
    QJsonObject scene_info_common;
    if (scene_info.contains("common") && scene_info["common"].isObject()) {
        scene_info_common = scene_info["common"].toObject();
    }
    // if(scene_info_common.isEmpty()) {
    //     LOGFMTE("[TaskElement] scene_info_common is empty!");
    //     return false;
    // }
    scene_info_common["format"] = taskInfo.format;
    scene_info["common"] = scene_info_common;

    // positioning_mode
    scene_info["positioning_mode"] = taskInfo.positioning_mode;
    // texture_compression_quality
    scene_info["texture_compression_quality"] = taskInfo.texture_compression_quality;
    // tile_mode
    scene_info["tile_mode"] = taskInfo.tile_mode;
    // tile_size
    scene_info["tile_size"] = taskInfo.tile_size;
    // groups
    QJsonArray groups;
    for (int i = 0; i < taskInfo.groups.size(); i++) {
        PictureGroup groupInfo = taskInfo.groups[i];
        QJsonObject group;
        group["group_id"]   = groupInfo.groupId;
        group["group_name"] = groupInfo.groupName;
        group["group_path"] = groupInfo.groupPath;
        group["pic_height"] = QString::number(groupInfo.picHeight);
        group["pic_width"]  = QString::number(groupInfo.picWidth);
        group["pic_count"] = QString::number(groupInfo.picsCount);
        groups.append(group);
    }
    scene_info["groups"] = groups;
    // coords
    /*QJsonArray coords;
    for (int i = 0; i < taskInfo.coords.size(); i++) {
        CoordEditInfo coordInfo = taskInfo.coords[i];
        QJsonObject coord;
        coord["name"]    = coordInfo.name;
        coord["coord_x"] = QString::number(coordInfo.coordValue.coord_x);
        coord["coord_y"] = QString::number(coordInfo.coordValue.coord_y);
        coord["coord_z"] = QString::number(coordInfo.coordValue.coord_z);

        QJsonArray pics;
        for (int j = 0; j < coordInfo.pictures.size(); j++) {
            QString local = QDir::fromNativeSeparators(coordInfo.pictures[j]);
            pics.append(local);
        }
        coord["pictures"]  = pics;

        coords.append(coord);
    }
    scene_info["coords"] = coords;*/

    task_json["scene_info_render"] = scene_info;
    task_json["scene_info"] = scene_info;
    task_json["task_info"] = task_info;

    return true;
}

bool TaskCCWriter::dump_taskjson(QSharedPointer<RenderParamConfig> param, int version)
{
    QJsonObject root;
    QJsonObject argument_body = create_object(version);
    switch(version) {
    case MODE_PLAIN:
        root = argument_body;
        break;
    case MODE_ENCODE:
    {
        root.insert("version", version);
        root.insert("key", MyConfig.userSet.userKey);

        QByteArray encrypted_data = AES::encrypt(MyConfig.userSet.userKey.toLatin1(), QJsonDocument(argument_body).toJson(QJsonDocument::Compact));
        root.insert("data", QString(encrypted_data.toBase64()));
    }
    break;
    default:
        break;
    }

    json_file = client_project_dir + "/task.json";
    RBHelper::saveJsonFile(json_file, root);

    return true;
}

bool TaskCCWriter::dump_uploadjson(QSharedPointer<RenderParamConfig> param)
{
    QSharedPointer<TaskInfoCC> task = qSharedPointerCast<TaskInfoCC>(m_task);

    QJsonObject root;
    QString json_file = client_project_dir + "/upload.json";

    // assets
    QJsonArray assets;
    for (int i = 0; i < task->groups.size(); i++) {
        PictureGroup group = task->groups[0];
        for (int j = 0; j < group.pictures.size(); j++) {
            QString local = QDir::fromNativeSeparators(group.pictures[j].picPath);
            QJsonObject asset = File::buildUploadFileObject(local);
            assets.append(asset);
        }
    }
    root["asset"] = assets;

    // coord pictures
    /*QJsonArray coords;
    for (int i = 0; i < task->coords.size(); i++) {
        CoordEditInfo info = task->coords[0];
        for (int j = 0; j < info.pictures.size(); j++) {
            QString local = QDir::fromNativeSeparators(info.pictures[j]);
            QJsonObject coord = File::buildUploadFileObject(local);
            assets.append(coord);
        }
    }
    root["coords"] = coords;*/


    return RBHelper::saveJsonFile(json_file, root);
}

bool TaskCCWriter::dump_tipsjson(QSharedPointer<RenderParamConfig> param)
{
    QSharedPointer<TaskInfoCC> task = qSharedPointerCast<TaskInfoCC>(m_task);

    QJsonObject root;
    QString json_file = client_project_dir + "/tips.json";
    return RBHelper::saveJsonFile(json_file, root);
}

bool TaskCCWriter::dump_assetjson(QSharedPointer<RenderParamConfig> param)
{
    QSharedPointer<TaskInfoCC> task = qSharedPointerCast<TaskInfoCC>(m_task);

    QJsonObject root;
    QJsonArray scenes;
    for (int i = 0; i < task->groups.size(); i++) {
        PictureGroup group = task->groups[0];
        // QJsonObject scene = File::buildUploadFileObject(group.groupPath);
        scenes.append(group.groupPath);
    }
    root.insert("scene_file", scenes);

    QString json_file = client_project_dir + "/asset.json";
    return RBHelper::saveJsonFile(json_file, root);
}

bool TaskCCWriter::dump_argument_json(QSharedPointer<RenderParamConfig> param, QString& argumentFile, int version)
{
    QString client_project_base_dir = LocalSetting::getInstance()->getClientProjectPath();
    client_project_dir = QString("%1/%2/").arg(client_project_base_dir).arg(job_id);
    client_project_dir = QDir::cleanPath(client_project_dir);

    QJsonObject root;
    QJsonObject argument_body = create_object(version);
    switch (version) {
    case MODE_PLAIN:
        root = argument_body;
        break;
    case MODE_ENCODE: {
        root.insert("version", version);
        root.insert("key", MyConfig.userSet.userKey);

        QByteArray encrypted_data = AES::encrypt(MyConfig.userSet.userKey.toLatin1(), QJsonDocument(argument_body).toJson(QJsonDocument::Compact));
        root.insert("data", QString(encrypted_data.toBase64()));
    }
       break;
    default:
        break;
    }

    // temp path
    client_tmp_path = QString("projects/%1").arg(QString::number(job_id));
    client_tmp_path = RBHelper::getTempPath(client_tmp_path);

    argument_file = client_tmp_path + "/argument.json";
    argumentFile = argument_file;
    RBHelper::saveJsonFile(argument_file, root);

    return true;
}

bool TaskCCWriter::dump_groupinfo_json(QSharedPointer<RenderParamConfig> param)
{
    QString client_project_base_dir = LocalSetting::getInstance()->getClientProjectPath();
    client_project_dir = QString("%1/%2/").arg(client_project_base_dir).arg(job_id);
    client_project_dir = QDir::cleanPath(client_project_dir);
    QSharedPointer<TaskInfoCC> taskInfo = qSharedPointerCast<TaskInfoCC>(param->task);

    QJsonObject root;
    root["job_type"] = QString::number(taskInfo->groups[0].jobType);
    root["project_name"] = taskInfo->project_name;
    root["is_submit_ctrl_point"] = !taskInfo->ctrlPointInfo.itemDatas.isEmpty();
    root["is_submit_pos_info"] = !taskInfo->posInfo.itemDatas.isEmpty();

    QJsonArray groups; 
    qint64 totalPixel = 0;
    for (int i = 0; i < taskInfo->groups.size(); i++) {
        PictureGroup group = taskInfo->groups[i];

        QJsonObject groupJson;
        groupJson["group_id"]   = group.groupId;
        groupJson["group_name"] = group.groupName;
        groupJson["group_path"] = group.groupPath;
        groupJson["pic_height"] = QString::number(group.picHeight);
        groupJson["pic_width"]  = QString::number(group.picWidth);
        groupJson["pic_count"]  = QString::number(group.picsCount);

        groupJson["camera_producter"] = group.cameraProducter;
        groupJson["camera_model"]     = group.cameraModel;
        groupJson["sensor_size"]      = group.sensorSize;
        groupJson["focal_length"]     = group.focalLength;
        groupJson["total_pixel_str"]  = group.totalPixelStr;
        groupJson["total_pixel"]      = QString::number(group.totalPixel);
        totalPixel += group.totalPixel;

        QJsonArray pictures;
        for (int j = 0; j < group.pictures.size(); j++) {
            PictureInfo picInfo = group.pictures[j];
            QJsonObject pic;

            pic["pic_size"]      = picInfo.picSize;
            pic["pic_name"]      = picInfo.picName;
            pic["pic_longitude"] = picInfo.picLongitude;
            pic["pic_latitude"]  = picInfo.picLatitude;
            pic["pic_altitude"]  = picInfo.picAltitude;
            pic["pic_path"]      = picInfo.picPath;
            pic["unique_code"]   = picInfo.fileUniqueCode;
            pic["file_mtime"]    = picInfo.fileMtime;

            pictures.append(pic);
        }
        groupJson["pictures"] = pictures;

        groups.append(groupJson);
    }
    root["groups"] = groups;
    root["total_pixel"] = QString::number(totalPixel);

    QJsonObject ctrlJson;
    ctrlJson["file_path"]     = taskInfo->ctrlPointInfo.filePath;
    ctrlJson["coord_system"]     = taskInfo->ctrlPointInfo.coordSys;
    ctrlJson["ignore_lines"]  = taskInfo->ctrlPointInfo.ignoreLines;
    ctrlJson["splite_char"]   = taskInfo->ctrlPointInfo.spliteChar;
    ctrlJson["field_order"]   = taskInfo->ctrlPointInfo.fieldOrder;
    QJsonArray ctrlItemDatas;
    for (int i = 0; i < taskInfo->ctrlPointInfo.itemDatas.size(); i++) {
        QJsonObject item;
        ConfigDataItem* cdata = taskInfo->ctrlPointInfo.itemDatas[i];
        if (cdata == nullptr) continue;

        item["index"]     = cdata->index;
        item["name"]      = cdata->imageName;
        item["longitude"] = cdata->picLongitude;
        item["latitude"]  = cdata->picLatitude;
        item["altitude"]  = cdata->picAltitude;

        ctrlItemDatas.append(item);
    }
    ctrlJson["item_datas"] = ctrlItemDatas;
    root["contrl_point"]   = ctrlJson;

    QJsonObject posJson;
    posJson["file_path"] = taskInfo->posInfo.filePath;
    posJson["coord_system"] = taskInfo->posInfo.coordSys;
    posJson["ignore_lines"] = taskInfo->posInfo.ignoreLines;
    posJson["splite_char"] = taskInfo->posInfo.spliteChar;
    posJson["field_order"] = taskInfo->posInfo.fieldOrder;
    QJsonArray posItemDatas;
    for (int i = 0; i < taskInfo->posInfo.itemDatas.size(); i++) {
        QJsonObject item;
        ConfigDataItem* cdata = taskInfo->posInfo.itemDatas[i];
        if(cdata == nullptr) continue;

        item["index"]     = cdata->index;
        item["name"]      = cdata->imageName;
        item["longitude"] = cdata->picLongitude;
        item["latitude"]  = cdata->picLatitude;
        item["altitude"]  = cdata->picAltitude;

        posItemDatas.append(item);
    }
    posJson["item_datas"] = posItemDatas;
    root["pos_info"] = posJson;

    QJsonArray blocks;
    foreach(BlockInfo info, taskInfo->blocks) {
        QJsonObject block;
        block["file_path"] = info.filePath;
        blocks.append(block);
    }
    root.insert("blocks_info", blocks);

	//增加参数配置


    QString argument_file = client_project_dir + "/project_data.json";
    RBHelper::saveJsonFile(argument_file, root);

    return true;
}

void TaskCCWriter::backup_and_remove_josn_file()
{
    // task.json
    QString task_json = client_project_dir + "/task.json";
    QString task_json_bk = client_project_dir + "/task.json.bk";
    if (QFileInfo::exists(task_json_bk)) {
        QFile(task_json_bk).remove();
    }
    if (QFileInfo::exists(task_json)) {
        QFile(task_json).rename(task_json_bk);
    }

    // upload.json
    QString upload_json = client_project_dir + "/upload.json";
    QString upload_json_bk = client_project_dir + "/upload.json.bk";
    if (QFileInfo::exists(upload_json_bk)) {
        QFile(upload_json_bk).remove();
    }
    if (QFileInfo::exists(upload_json)) {
        QFile(upload_json).rename(upload_json_bk);
    }

    // asset.json
    QString asset_json = client_project_dir + "/asset.json";
    QString asset_json_bk = client_project_dir + "/asset.json.bk";
    if (QFileInfo::exists(asset_json_bk)) {
        QFile(asset_json_bk).remove();
    }
    if (QFileInfo::exists(asset_json)) {
        QFile(asset_json).rename(asset_json_bk);
    }

    // tips.json
    QString tips_json = client_project_dir + "/tips.json";
    QString tips_json_bk = client_project_dir + "/tips.json.bk";
    if (QFileInfo::exists(tips_json_bk)) {
        QFile(tips_json_bk).remove();
    }
    if (QFileInfo::exists(tips_json)) {
        QFile(tips_json).rename(tips_json_bk);
    }

    // project_data.json
    QString project_data_json = client_project_dir + "/project_data.json";
    QString project_data_json_bk = client_project_dir + "/project_data.json.bk";
    if (QFileInfo::exists(project_data_json_bk)) {
        QFile(project_data_json_bk).remove();
    }
    if (QFileInfo::exists(project_data_json)) {
        QFile(project_data_json).rename(project_data_json_bk);
    }
}