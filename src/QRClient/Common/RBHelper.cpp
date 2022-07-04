#include "stdafx.h"
#include "RBHelper.h"
#include "SystemUtil.h"
#include "HttpCommand/HttpCmdManager.h"
#include <QProcess>

static int randInt(int low, int high)
{
    // Random number between low and high
    return qrand() % ((high + 1) - low) + low;
}

QString randomString(int length = 32, QString allow_symbols = QString("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"))
{
    QString result;
    qsrand(QTime::currentTime().msec());
    for(int i = 0; i < length; ++i) {
        result.append(allow_symbols.at(qrand() % (allow_symbols.length())));
    }
    return result;
}

QString RBHelper::currentDateTime()
{
    QDateTime timeNow = QDateTime::currentDateTime();
    QLocale localeEnglish(QLocale::English);

    QString datetime = localeEnglish.toString(timeNow, "yyyy-MM-dd hh:mm:ss");

    return datetime;
}

QString RBHelper::timeCostToString(qint64 timeCost, int format)
{
    QString times = "";
    //int day = m_timeCost / 60 / 60 / 24;
    int hour = (timeCost / 60 / 60);// % 24;
    int min = (timeCost / 60) % 60;
    int sec = timeCost % 60;

    if (format == EN_FORMAT_WORDS) {
        //if(day > 0)
        //    times += QString::number(day) + QString("天");
        if (hour > 0)
            times += QString::number(hour) + QObject::tr("时");
        if (min > 0)
            times += QString::number(min) + QObject::tr("分");
        if (sec > 0)
            times += QString::number(sec) + QObject::tr("秒");

    } else if(format == EN_FORMAT_SYMBOL) {
        //if(day > 0)
        //    times += QString::number(day) + QString(":");
        if (hour > 0)
            times += QString::number(hour) + QObject::tr(":");
        if (min > 0)
            times += QString::number(min) + QObject::tr(":");
        if (sec > 0)
            times += QString::number(sec) + QObject::tr(":");
    }

    return times;
}

QString RBHelper::getVipDisplay(int level)
{
    QString levelDisplay;
    switch(level) {
    case USER_LEVEL_VIPList_49:
        levelDisplay = QObject::tr("普通用户");
        break;
    case USER_LEVEL_VIPList_55:
        levelDisplay = QObject::tr("白银会员");
        break;
    case USER_LEVEL_VIPList_56:
        levelDisplay = QObject::tr("黄金会员");
        break;
    case USER_LEVEL_VIPList_57:
        levelDisplay = QObject::tr("铂金会员");
        break;
    case USER_LEVEL_VIPList_58:
        levelDisplay = QObject::tr("钻石会员");
        break;
    case USER_LEVEL_VIPList_PIC:
        levelDisplay = QObject::tr("效果图会员");
        break;
    }
    return levelDisplay;
}

QString RBHelper::getLocaleName()
{
    QString localeName;
    int nlanguage = LocalSetting::getInstance()->getLanguageId();
    if(nlanguage == LANG_EN) {
        localeName = "en_US";
    } else if(nlanguage == LANG_CN) {
        localeName = "zh_CN";
    } else if(nlanguage == LANG_JP) {
        localeName = "ja_JP";
    }

    return localeName;
}

QString RBHelper::generateMsgId()
{
    QString currTime = QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz")
        + QString::number(randInt(1000, 9999));

    return currTime;
}

int RBHelper::versionCompare(const QString& lhs, const QString& rhs)
{
    int lhs_major = 0, lhs_minor = 0, lhs_revision = 0, lhs_build = 0;
    sscanf(lhs.toLatin1().data(), "%d.%d.%d.%d", &lhs_major, &lhs_minor, &lhs_revision, &lhs_build);

    int rhs_major = 0, rhs_minor = 0, rhs_revision = 0, rhs_build = 0;
    sscanf(rhs.toLatin1().data(), "%d.%d.%d.%d", &rhs_major, &rhs_minor, &rhs_revision, &rhs_build);

    int d1 = lhs_major - rhs_major;
    int d2 = lhs_minor - rhs_minor;
    int d3 = lhs_revision - rhs_revision;
    int d4 = lhs_build - rhs_build;

    if(d1 == 0) {
        if(d2 == 0) {
            if(d3 == 0) {
                if(d4 == 0) {
                    return 0;
                } else {
                    return d4;
                }
            } else {
                return d3;
            }
        } else {
            return d2;
        }
    } else {
        return d1;
    }

    return 0;
}

bool RBHelper::makeDirectory(const QString& folder)
{
    return QDir().mkpath(folder);
}

QString RBHelper::makePathFromConfigPath(const QString& name)
{
    QString profilePath = RBHelper::getAppDataPath();

    QString finalPath = profilePath.append(name);

    return finalPath;
}

QString RBHelper::makePathFromTempPath(const QString & name)
{
    QString tempPath = getTempPath();

    QString finalPath = tempPath.append(name);

    return finalPath;
}

QString RBHelper::getClientProjectPath()
{
    QString cfgPath = LocalSetting::getInstance()->getClientProjectPath();

    QString client_project_path = cfgPath;

    if(client_project_path.endsWith(QDir::separator())) {
        client_project_path = client_project_path.left(client_project_path.length() - 1);
    }

    return client_project_path;
}

// 删除临时目录
void RBHelper::deleteUserDirectory(t_taskid taskId)
{
    QString filePath = LocalConfig::getInstance()->defaultProduct
        + "/users/"
        + QString::number(MyConfig.userSet.id);
}

// %tmp%/Renderbus
QString RBHelper::getTempPath(const QString& folder)
{
#ifdef Q_OS_WIN
    QString tempPath = QDir::tempPath();
#else
    QString tempPath = QProcessEnvironment::systemEnvironment().value("TEMP");
#endif
    tempPath.append(QDir::separator());
    tempPath.append(PRODUCT_NAME);
    tempPath.append(QDir::separator());
    tempPath.append(folder);

    QString finalPath = QDir::toNativeSeparators(tempPath);

    QDir().mkpath(finalPath);

    return finalPath;
}

// %appdata%Renderbus/RenderbusAnim2018/
QString RBHelper::getAppDataPath()
{
    QString appdata = QProcessEnvironment::systemEnvironment().value("APPDATA");

    appdata.append(QDir::separator());
    appdata.append(PRODUCT_NAME);
    appdata.append(QDir::separator());
    appdata.append(LocalConfig::getInstance()->defaultProduct);
    appdata.append(QDir::separator());

    QDir().mkpath(appdata);

    return appdata;
}
// %appdata%Renderbus/RenderbusAnim2018/local/
QString RBHelper::getAppDataLocalPath()
{
    QString appdata = RBHelper::getAppDataPath();
    appdata.append("local");
    appdata.append(QDir::separator());

    QDir().mkpath(appdata);

    return appdata;
}
// %appdata%Renderbus/RenderbusAnim2018/3002/profiles/users/10001/heqingrui
QString RBHelper::getUserProfilePath()
{
    QString appdata = RBHelper::getAppDataPath();
    QString proxy = CConfigDb::Inst()->getSocialProxyName();
    QString profilePath = QString("%1%2/profiles/%3/users/%4/%5")
        .arg(appdata)
        .arg(LocalSetting::getInstance()->getPlatformIdForLocal())
        .arg(proxy)
        .arg(MyConfig.accountSet.userId)
        .arg(System::getLoginUserName());

    QDir().mkpath(profilePath);

    return profilePath;
}

QString RBHelper::getProgramDataPath()
{
#ifdef Q_OS_WIN
    PWSTR buffer = nullptr;
    const auto result = SHGetKnownFolderPath(FOLDERID_ProgramData, 0, nullptr, &buffer);
    QString programData = QString::fromStdWString(buffer);

    programData.append(QDir::separator());
    programData.append(PRODUCT_NAME);

    QDir().mkpath(programData);
#else
    QString programData = QProcessEnvironment::systemEnvironment().value("ALLUSERSPROFILE");
#endif
    return programData;
}

QString RBHelper::getAllUserProfilePath()
{
    QString profiledata = QProcessEnvironment::systemEnvironment().value("ALLUSERSPROFILE");

    profiledata.append(QDir::separator());
    profiledata.append(LocalConfig::getInstance()->defaultProduct);
    profiledata.append(QDir::separator());

    QDir().mkpath(profiledata);

    return profiledata;
}

QString RBHelper::getProgramDataLocalPath()
{
    QString programData = RBHelper::getProgramDataPath();

    QString product = LocalConfig::getInstance()->defaultProduct;

    programData.append(QDir::separator());
    programData.append(product);
    programData.append("/local/");

    QDir().mkpath(programData);

    return programData;
}

QString RBHelper::getOutputPath(t_taskid taskId, t_taskid parentId, const QString& scene, const QString& projectSavePath)
{
    QString pyrootpath = RBHelper::getPyScriptRootPath();
#ifdef Q_OS_WIN
    QString outputPath = pyrootpath.append("/submit/output.exe");
#else
    QString outputPath = pyrootpath.append("/submit/output");
#endif

    QString output = "";
    QFileInfo fileInfo(outputPath);
    if(fileInfo.exists(outputPath)) {
        QProcess outputProcess;
        QStringList params = getOutputParams(taskId, parentId, scene, projectSavePath);

        outputProcess.start(outputPath, params);
        outputProcess.waitForFinished();
        output = QString::fromLocal8Bit(outputProcess.readAllStandardOutput());

        if(output.isEmpty()) {
            LOGFMTE("output.exe error, output path is empty!");
            LOGFMTE("output.exe error msg is %s", qPrintable(outputProcess.errorString()));
        }

        output = QDir::toNativeSeparators(output.trimmed()).trimmed();

        if(!QFileInfo::exists(output)) {
            //QDir().mkpath(output);
        }

        qDebug() << output;
    }

    // output 在linux下执行失败的情况
#ifdef Q_OS_LINUX
    if(output.isEmpty()) {
        output = QString("%1/%2_%3").arg(projectSavePath).arg(taskId).arg(QFileInfo(scene).baseName());
    }
#endif

    return output.isEmpty() ? projectSavePath : output;
}

QString RBHelper::getScriptRootPath()
{
    QString appdata = RBHelper::getAppDataPath();
    QString scriptRootPath;
    scriptRootPath.append(appdata);
    // scriptRootPath.append(QString::number(LocalSetting::getInstance()->getPlatformIdForLocal()));
    scriptRootPath.append("/module/script");

    QDir().mkpath(scriptRootPath);

    return scriptRootPath;
}

QString RBHelper::getPyScriptRootPath()
{
    QString pyRootPath = RBHelper::getScriptRootPath();
    // pyRootPath.append("/python");

    QDir().mkpath(pyRootPath);

    return pyRootPath;
}

// 默认 windows C:/DayanCloud/Download
// 默认 linux ~/DayanCloud/Download
QString RBHelper::getDefaultDownloadDir()
{
    // 加载默认存储路径
    QString drive = System::getSystemDrive();
#ifdef Q_OS_WIN
    
#ifdef FOXRENDERFARM
	QString savePath = drive + "AvicaCloud/Download";
#else
	QString savePath = drive + "DayanCloud/Download";
#endif

#else
    
#ifdef FOXRENDERFARM
	QString savePath = drive + "/AvicaCloud/Download";
#else
	QString savePath = drive + "/DayanCloud/Download";
#endif

#endif
    RBHelper::makeDirectory(savePath);

    return savePath;
}

// 默认 windows C:/DayanCloud/Project
// 默认 linux ~/DayanCloud/Project
QString RBHelper::getDefaultProjectDir()
{
    QString drive = System::getSystemDrive();
#ifdef Q_OS_WIN
#ifdef FOXRENDERFARM
	QString projectPath = drive + "AvicaCloud/Project";
#else
	QString projectPath = drive + "DayanCloud/Project";
#endif
    
#else

#ifdef FOXRENDERFARM
	QString projectPath = drive + "/AvicaCloud/Project";
#else
	QString projectPath = drive + "/DayanCloud/Project";
#endif    
#endif
    RBHelper::makeDirectory(projectPath);

    return projectPath;
}

SCENE_TYPE RBHelper::typeOfScene(const QString& scene)
{
    SCENE_TYPE type = CG_ID_UNKNOWN;

    QFileInfo fileinfo(scene);
    QString ext = fileinfo.suffix().toLower();

    auto cgTypes = WSConfig::getInstance()->cg_support_types;
    auto it = cgTypes.find(ext);
    if(it != cgTypes.end())
        type = (SCENE_TYPE)it->id;

    return type;
}

CG_ID RBHelper::cgidOfScene(const QString& scene)
{
    CG_ID cgid = CG_ID_UNKNOWN;

    QFileInfo fileinfo(scene);
    QString ext = fileinfo.suffix().toLower();

    auto cgTypes = WSConfig::getInstance()->cg_support_types;
    auto it = cgTypes.find(ext);
    if(it != cgTypes.end())
        cgid = (CG_ID)it->id;

    return cgid;
}

bool RBHelper::loadJsonFile(const QString& jsonFile, QJsonObject& jsonObject)
{
    QFile json_file(jsonFile);

    if(!json_file.open(QIODevice::ReadOnly)) {
        // qDebug() << "File open failed :" << jsonFile;
        LOGFMTE("[RBHelper::loadJsonFile] File open failed : %s", qPrintable(jsonFile));
        return false;
    }
    QTextStream in_stream(&json_file);
    in_stream.setCodec("UTF-8");
    QString content = in_stream.readAll();
    content.remove(QChar::Null);

    QJsonParseError error;
    QJsonDocument jsonDocument(QJsonDocument::fromJson(content.toUtf8(), &error));
    if(jsonDocument.isNull() || jsonDocument.isEmpty() || error.error != QJsonParseError::NoError) {
        // qDebug() << "File parse failed :" << jsonFile << error.errorString();
        LOGFMTE("[RBHelper::loadJsonFile] File parse failed : %s, error msg is %s", qPrintable(jsonFile), qPrintable(error.errorString()));
        return false;
    }

    json_file.close();

    jsonObject = jsonDocument.object();

#if 0
    // 不检查文件{}，upload.json可能为空
    if(jsonObject.isEmpty())
        return false;
#endif
    return true;
}

bool RBHelper::saveJsonFile(const QString& jsonFile, const QJsonObject& jsonObject)
{
    QFile saveFile(jsonFile);

    QFileInfo fi(saveFile);
    QDir().mkpath(fi.absolutePath());

    if(!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return false;
    }

    QJsonDocument saveDoc(jsonObject);
#ifdef DEBUG
    saveFile.write(saveDoc.toJson());
#else
    saveFile.write(saveDoc.toJson(QJsonDocument::Compact));
#endif
    return true;
}

QString RBHelper::getUserStorageFolder(enAssetType type)
{
    int m_uid = MyConfig.accountSet.userId;
    int puid = MyConfig.accountSet.mainUserId;

    // 默认不共享资产
    int user_folder_id = (m_uid / 500) * 500;
#if 0
    // 默认主子账号分离
    int sa = MyConfig.renderSet.separateAccountFlag;
    switch(sa) {
    case ACCOUNT_SHARE_NONE:
        break;
    case ACCOUNT_SHARE_SCENE:
        if(type == TYPE_SCENE) {
            user_folder_id = (puid / 500) * 500;
        }
        break;
    case ACCOUNT_SHARE_SCENE_ASSET:
        user_folder_id = (puid / 500) * 500;
        break;
    default:
        break;
    }
#else
    if(MyConfig.userSet.isChildAccount()) {
        if(MyConfig.accountSet.shareMainCapital) {
            user_folder_id = (puid / 500) * 500;
            m_uid = puid;
        }
    }
#endif

    QString storageDir = QString("/%1/%2").arg(user_folder_id).arg(m_uid);

    return storageDir;
}

QString RBHelper::getOutputStorageFolder()
{
    int m_uid = MyConfig.userSet.id;
    int user_folder_id = (m_uid / 500) * 500;

    QString storageDir = QString("/%1/%2").arg(user_folder_id).arg(m_uid);

    return storageDir;
}

bool RBHelper::isNumber(const QString& text)
{
    for(int i = 0; i < text.size(); ++i) {     // 当数组元素不为结束符时.遍历字符串.
        if(text[i] >= '0' && text[i] <= '9') { // 如果是数字.
            continue;
        } else {
            return true;
        }
    }
    return false;
}

bool RBHelper::isFloatValid(float speed)
{
    if(speed >= -0.00001 && speed <= 0.00001) {
        return false;
    }
    return true;
}

QIcon RBHelper::getServerLoadIcon(enPlatformStatus load)
{
    QString status = "idle";
    switch(load) {
    case PLATFORM_STATUS_DISABLED:
        status = "none";
        return QIcon();
    case PLATFORM_STATUS_IDLE:
        status = "idle";
        break;
    case PLATFORM_STATUS_NORMAL:
        status = "normal";
        break;
    case PLATFORM_STATUS_BUSY:
        status = "busy";
        break;
    case PLATFORM_STATUS_OVERLOADED:
        status = "busy";
        break;
    default:
        break;
    }

    QString lang = "english";
    int langId = LocalSetting::getInstance()->getLanguageId();
    switch(langId) {
    case LANG_CN:
        lang = "chinese";
        break;
    case LANG_EN:
        lang = "english";
        break;
    case LANG_JP:
        lang = "english";
        break;
    }

    QString loadStatus = QString(":/status/images/status/icon_%1_%2.png").arg(status).arg(lang);
    QIcon loadIcon(loadStatus);

    return loadIcon;
}

QString RBHelper::getOutputPathForThread(t_taskid taskId, t_taskid parentId, const QString& scene, const QString& projectSavePath, const QString& pypath)
{
    QString pyrootpath = pypath;
#ifdef Q_OS_WIN
    QString outputPath = pyrootpath.append("\\submit\\output.exe");
#else
    QString outputPath = pyrootpath.append("/submit/output");
#endif
    QFileInfo fileInfo(outputPath);
    QString output = projectSavePath;
    if(fileInfo.exists(outputPath)) {
        QProcess outputProcess;
        QStringList params = getOutputParams(taskId, parentId, scene, projectSavePath);

        outputProcess.start(outputPath, params);
        outputProcess.waitForFinished();
        output = QString::fromLocal8Bit(outputProcess.readAllStandardOutput());

        if(output.isEmpty()) {
            LOGFMTE("output.exe error, output path is empty!");
        }

        output = QDir::toNativeSeparators(output.trimmed()).trimmed();

        if(!QFileInfo::exists(output)) {
            // RBHelper::makeDirectory(output);
        }

        qDebug() << output;
    }
    return output;
}

bool RBHelper::isDeveloper()
{
    QString appDir = QCoreApplication::applicationDirPath();
    QString developFlagFile = appDir + "/developer";
    if(QFile::exists(developFlagFile))
        return true;

    QString rootFile("C:/developer");
    if(QFile::exists(rootFile))
        return true;

    return false;
}

QString RBHelper::getTransmitErrorDisplay(int ec)
{
    QString displayString;
    switch(ec) {
    case transmit_callback_error_type_t::success:
        displayString = QObject::tr("成功");
        break;
    case transmit_callback_error_type_t::failure:
        displayString = QObject::tr("失败");
        break;
    case transmit_callback_error_type_t::invalid_storage_id:
        displayString = QObject::tr("无效的存储ID");
        break;
    case transmit_callback_error_type_t::transmit_server_not_exist:
        displayString = QObject::tr("Rayvision传输服务器挂了");
        break;
    case transmit_callback_error_type_t::insufficient_permission:
        displayString = QObject::tr("权限不足");
        break;
    case transmit_callback_error_type_t::local_other_failure:
        displayString = QObject::tr("不确定的本地错误");
        break;
    case transmit_callback_error_type_t::local_acquire_event_failure:
        displayString = QObject::tr("本地创建内部事件失败");
        break;
    case transmit_callback_error_type_t::local_recv_invalid_data:
        displayString = QObject::tr("本地接收到无效的数据");
        break;
    case transmit_callback_error_type_t::local_invalid_channel_data:
        displayString = QObject::tr("本地连接的通道数据无效");
        break;
    case transmit_callback_error_type_t::local_path_stat_failure:
        displayString = QObject::tr("本地路径访问失败");
        break;
    case transmit_callback_error_type_t::local_path_not_exist:
        displayString = QObject::tr("检查该文件是否存在,一定要从详情中拷贝出出错的文件名,直接访问");
        break;
    case transmit_callback_error_type_t::local_path_is_file:
        displayString = QObject::tr("本地路径是个文件");
        break;
    case transmit_callback_error_type_t::local_path_is_directory:
        displayString = QObject::tr("本地路径是个目录");
        break;
    case transmit_callback_error_type_t::local_pathname_is_too_long:
        displayString = QObject::tr("本地路径太长，引擎不支持");
        break;
    case transmit_callback_error_type_t::local_create_file_failure:
        displayString = QObject::tr("本地文件创建失败");
        break;
    case transmit_callback_error_type_t::local_open_file_failure:
        displayString = QObject::tr("本地文件打开失败");
        break;
    case transmit_callback_error_type_t::local_read_file_failure:
        displayString = QObject::tr("本地文件读取失败");
        break;
    case transmit_callback_error_type_t::local_write_file_failure:
        displayString = QObject::tr("本地文件写入失败");
        break;
    case transmit_callback_error_type_t::local_offset_file_failure:
        displayString = QObject::tr("本地文件偏移失败");
        break;
    case transmit_callback_error_type_t::local_rename_path_failure:
        displayString = QObject::tr("本地路径重命名失败");
        break;
    case transmit_callback_error_type_t::local_delete_path_failure:
        displayString = QObject::tr("本地路径删除失败");
        break;
    case transmit_callback_error_type_t::local_file_incomplete:
        displayString = QObject::tr("本地文件不完整");
        break;
    case transmit_callback_error_type_t::local_file_overflow:
        displayString = QObject::tr("本地文件溢出");
        break;
    case transmit_callback_error_type_t::local_create_directory_failure:
        displayString = QObject::tr("本地目录创建失败");
        break;
    case transmit_callback_error_type_t::local_open_directory_failure:
        displayString = QObject::tr("本地目录打开失败");
        break;
    case transmit_callback_error_type_t::local_parse_folder_content_failure:
        displayString = QObject::tr("本地解析待下载（待显示）的目录信息失败");
        break;
    case transmit_callback_error_type_t::local_parse_storage_information_failure:
        displayString = QObject::tr("本地解析存储服务器的存储根目录信息失败");
        break;
    case transmit_callback_error_type_t::local_set_save_directory_failure:
        displayString = QObject::tr("本地设置下载文件的保存目录失败（Rayvision引擎）");
        break;
    case transmit_callback_error_type_t::local_upload_same_file:
        displayString = QObject::tr("本地有多个线程在上传同一个文件");
        break;
    case transmit_callback_error_type_t::local_send_request_failure:
        displayString = QObject::tr("本地发送请求到服务器失败");
        break;
    case transmit_callback_error_type_t::local_wait_file_data_timeout:
        displayString = QObject::tr("本地在传输文件时，长时间收不到任何文件数据，导致引擎自动断开连接");
        break;
    case transmit_callback_error_type_t::local_wait_message_response_timeout:
        displayString = QObject::tr("本地发起的请求，长时间收不到服务器的响应，导致引擎自动断开连接");
        break;
    case transmit_callback_error_type_t::local_request_oss_failure:
        displayString = QObject::tr("本地与OSS服务器通讯失败");
        break;
    case transmit_callback_error_type_t::local_get_sts_failure:
        displayString = QObject::tr("本地获取OSS服务器的临时登录密钥失败");
        break;
    case transmit_callback_error_type_t::local_parse_sts_failure:
        displayString = QObject::tr("本地解析从W7返回的OSS服务器的临时登录密钥失败");
        break;
    case transmit_callback_error_type_t::local_create_aspera_session_failure:
        displayString = QObject::tr("本地Aspera引擎创建会话失败");
        break;
    case transmit_callback_error_type_t::local_open_channel_failure:
        displayString = QObject::tr("本地连接打开通道失败");
        break;
    case transmit_callback_error_type_t::local_cancel_transmit:
        displayString = QObject::tr("本地断开连接");
        break;
    case transmit_callback_error_type_t::local_system_error:
        displayString = QObject::tr("本地系统错误");
        break;
    case transmit_callback_error_type_t::local_aspera_error:
        displayString = QObject::tr("本地Aspera错误");
        break;
    case transmit_callback_error_type_t::local_ambiguous_target:
        displayString = QObject::tr("本地未给出明确的目的路径");
        break;
    case transmit_callback_error_type_t::local_license_failure:
        displayString = QObject::tr("本地license错误");
        break;
    case transmit_callback_error_type_t::local_auth_failure:
        displayString = QObject::tr("本地安全认证错误");
        break;
    case transmit_callback_error_type_t::local_open_too_many_files:
        displayString = QObject::tr("本地打开太多文件");
        break;
    case transmit_callback_error_type_t::local_pathname_is_invalid:
        displayString = QObject::tr("本地路径无效");
        break;
    case transmit_callback_error_type_t::local_path_already_exist:
        displayString = QObject::tr("本地路径已经存在");
        break;
    case transmit_callback_error_type_t::local_file_is_too_big:
        displayString = QObject::tr("本地文件太大");
        break;
    case transmit_callback_error_type_t::local_file_table_overflow:
        displayString = QObject::tr("本地文件列表太大");
        break;
    case transmit_callback_error_type_t::local_nothing_to_transfer:
        displayString = QObject::tr("本地没有需要传输的任务");
        break;
    case transmit_callback_error_type_t::local_disk_no_space:
        displayString = QObject::tr("本地磁盘满");
        break;
    case transmit_callback_error_type_t::local_encrypt_data_failure:
        displayString = QObject::tr("本地加密数据失败");
        break;
    case transmit_callback_error_type_t::local_decrypt_data_failure:
        displayString = QObject::tr("本地解密数据失败");
        break;
    case transmit_callback_error_type_t::local_aspera_port_error:
        displayString = QObject::tr("本地Aspera端口错误");
        break;
    case transmit_callback_error_type_t::remote_other_failure:
        displayString = QObject::tr("不确定的远端错误");
        break;
    case transmit_callback_error_type_t::remote_acquire_event_failure:
        displayString = QObject::tr("远端创建内部事件失败");
        break;
    case transmit_callback_error_type_t::remote_recv_invalid_data:
        displayString = QObject::tr("远端接收到无效的数据");
        break;
    case transmit_callback_error_type_t::remote_invalid_channel_data:
        displayString = QObject::tr("远端连接的通道数据无效");
        break;
    case transmit_callback_error_type_t::remote_path_stat_failure:
        displayString = QObject::tr("远端路径访问失败");
        break;
    case transmit_callback_error_type_t::remote_path_not_exist:
        displayString = QObject::tr("远端路径不存在");
        break;
    case transmit_callback_error_type_t::remote_path_is_file:
        displayString = QObject::tr("远端路径是个文件");
        break;
    case transmit_callback_error_type_t::remote_path_is_directory:
        displayString = QObject::tr("远端路径是个目录");
        break;
    case transmit_callback_error_type_t::remote_pathname_is_too_long:
        displayString = QObject::tr("远端路径太长，引擎不支持");
        break;
    case transmit_callback_error_type_t::remote_create_file_failure:
        displayString = QObject::tr("远端文件创建失败");
        break;
    case transmit_callback_error_type_t::remote_open_file_failure:
        displayString = QObject::tr("远端文件打开失败");
        break;
    case transmit_callback_error_type_t::remote_read_file_failure:
        displayString = QObject::tr("远端文件读取失败");
        break;
    case transmit_callback_error_type_t::remote_write_file_failure:
        displayString = QObject::tr("远端文件写入失败");
        break;
    case transmit_callback_error_type_t::remote_offset_file_failure:
        displayString = QObject::tr("远端文件偏移失败");
        break;
    case transmit_callback_error_type_t::remote_rename_path_failure:
        displayString = QObject::tr("远端路径重命名失败");
        break;
    case transmit_callback_error_type_t::remote_delete_path_failure:
        displayString = QObject::tr("远端路径删除失败");
        break;
    case transmit_callback_error_type_t::remote_file_incomplete:
        displayString = QObject::tr("远端文件不完整");
        break;
    case transmit_callback_error_type_t::remote_file_overflow:
        displayString = QObject::tr("远端文件溢出");
        break;
    case transmit_callback_error_type_t::remote_upload_same_file:
        displayString = QObject::tr("远端发现有多个连接在上传同一个文件");
        break;
    case transmit_callback_error_type_t::remote_create_directory_failure:
        displayString = QObject::tr("远端目录创建失败");
        break;
    case transmit_callback_error_type_t::remote_open_directory_failure:
        displayString = QObject::tr("远端目录打开失败");
        break;
    case transmit_callback_error_type_t::remote_make_folder_content_failure:
        displayString = QObject::tr("远端构建待下载（待显示）的目录信息失败");
        break;
    case transmit_callback_error_type_t::remote_make_storage_information_failure:
        displayString = QObject::tr("远端构建存储服务器的存储根目录信息失败");
        break;
    case transmit_callback_error_type_t::remote_set_save_directory_failure:
        displayString = QObject::tr("远端设置上传文件的保存目录失败（Rayvision引擎）");
        break;
    case transmit_callback_error_type_t::remote_cancel_transmit:
        displayString = QObject::tr("远端断开连接");
        break;
    case transmit_callback_error_type_t::remote_pathname_is_invalid:
        displayString = QObject::tr("远端路径无效");
        break;
    case transmit_callback_error_type_t::remote_path_already_exist:
        displayString = QObject::tr("远端路径已经存在");
        break;
    case transmit_callback_error_type_t::remote_file_is_too_big:
        displayString = QObject::tr("远端文件太大");
        break;
    case transmit_callback_error_type_t::remote_file_table_overflow:
        displayString = QObject::tr("远端文件列表太大");
        break;
    case transmit_callback_error_type_t::remote_nothing_to_transfer:
        displayString = QObject::tr("远端没有需要传输的任务");
        break;
    case transmit_callback_error_type_t::remote_disk_no_space:
        displayString = QObject::tr("远端磁盘满");
        break;
    case transmit_callback_error_type_t::remote_path_outside_docroot:
        displayString = QObject::tr("远端路径超出访问范围");
        break;
    case transmit_callback_error_type_t::remote_encrypt_data_failure:
        displayString = QObject::tr("远端加密数据失败");
        break;
    case transmit_callback_error_type_t::remote_decrypt_data_failure:
        displayString = QObject::tr("远端解密数据失败");
        break;
    case transmit_callback_error_type_t::unknown:
    default:
        displayString = QObject::tr("未知错误");
        break;
    }

    return displayString;
}

QString RBHelper::getTransmitErrorUrl(int ec)
{
    QString helpUrl = WSConfig::getInstance()->get_link_address("url_transmit_error");
    QString displayString = QString("<a href=\"%1\"><span style=\"color:#675ffe;\">%2</span></a>")
        .arg(helpUrl)
        .arg(QObject::tr("查看解决方案"));

    return displayString;
}

QUrl RBHelper::getLinkUrl(const QString & uri, const QString& extra)
{
    QString host = HttpCmdManager::getInstance()->getRequestHost();
    QString path = WSConfig::getInstance()->get_link_address(uri);
    QString from = WSConfig::getInstance()->get_link_address("path_from");
    QString url = QString("%1/%2?rayvision_token=%3&platform=%4&raySyncUserKey=%5&channel=%6&lang=%7&from=%8%9");
    if (path.contains("?")) {
        url = QString("%1/%2&rayvision_token=%3&platform=%4&raySyncUserKey=%5&channel=%6&lang=%7&from=%8%9");
    }
    url = url
          .arg(host)
          .arg(path)
          .arg(MyConfig.userSet.userKey)
          .arg(LocalSetting::getInstance()->getDefaultsite())
          .arg(MyConfig.userSet.raySyncUserKey)
          .arg(MyConfig.userSet.channel)
          .arg(getLocaleName())
          .arg(from)
          .arg(extra);

    return QUrl(url);
}

QUrl RBHelper::getRaysyncLinkUrl(const QString & uri)
{
    QString path = WSConfig::getInstance()->get_link_address(uri);
    QString url = QString("%1?token=%2&platform=%3&raySyncUserKey=%4&channel=%5&lang=%6")
        .arg(path)
        .arg(MyConfig.userSet.userKey)
        .arg(LocalSetting::getInstance()->getDefaultsite())
        .arg(MyConfig.userSet.raySyncUserKey)
        .arg(MyConfig.userSet.channel)
        .arg(getLocaleName());

    return QUrl(url);
}

int RBHelper::convertSpeedUnit(float speed, int display)
{
    float temp = speed;
    if(display == DISPLAY_UNIT_BIT) {
        temp = temp * 8.0f;

        float g = temp / 1024 / 1024 / 1024;
        float m = temp / 1024 / 1024;
        float k = temp / 1024;
        float b = temp;

        if(g > 1.0) return SPEED_UNIT_Gb;
        if(m > 1.0) return SPEED_UNIT_Mb;
        if(k > 1.0) return SPEED_UNIT_Kb;
        if(b > 0.0) return SPEED_UNIT_BIT;

    } else if(display == DISPLAY_UNIT_BYTE) {

        float g = temp / 1024 / 1024 / 1024;
        float m = temp / 1024 / 1024;
        float k = temp / 1024;
        float b = temp;

        if(g > 1.0) return SPEED_UNIT_GB;
        if(m > 1.0) return SPEED_UNIT_MB;
        if(k > 1.0) return SPEED_UNIT_KB;
        if(b > 0.0) return SPEED_UNIT_BYTE;
    }

    return -1;
}

float RBHelper::convertTransSpeed(float speed, int unit)
{
    float tspeed = 0.0f;
    QString speedstr;

    switch(unit) {
    case SPEED_UNIT_BIT:
    {
        speedstr = QString::number(speed * 8.0f, 'f', 2);
    }
    break;
    case SPEED_UNIT_Kb:
    {
        speedstr = QString::number(speed * 8.0f / 1024.0f, 'f', 2);
    }
    break;
    case SPEED_UNIT_Mb:
    {
        speedstr = QString::number(speed * 8.0f / 1024.0f / 1024.0f, 'f', 2);
    }
    break;
    case SPEED_UNIT_Gb:
    {
        speedstr = QString::number(speed * 8.0f / 1024.0f / 1024.0f / 1024.0f, 'f', 2);
    }
    break;
    case SPEED_UNIT_BYTE:
    {
        speedstr = QString::number(speed, 'f', 2);
    }
    break;
    case SPEED_UNIT_KB:
    {
        speedstr = QString::number(speed / 1024.0f, 'f', 2);
    }
    break;
    case SPEED_UNIT_MB:
    {
        speedstr = QString::number(speed / 1024.0f / 1024.0f, 'f', 2);
    }
    break;
    case SPEED_UNIT_GB:
    {
        speedstr = QString::number(speed / 1024.0f / 1024.0f / 1024.0f, 'f', 2);
    }
    break;
    default:
        speedstr = QString::number(speed, 'f', 2);
        break;
    }

    tspeed = speedstr.toFloat();
    if(tspeed > 999.0f) tspeed = 999.0f;
    return tspeed;
}

QString RBHelper::convertTransSpeedToStr(float speed, int display)
{
    float _10gigabyte = (float)10.0 * 1024 * 1024 * 1024;
    if(speed > _10gigabyte) {
        LOGFMTW("[WARN]Speed value overflow %f", speed);
        speed = _10gigabyte;
    }
    int unit = convertSpeedUnit(speed, display);
    float tspeed = convertTransSpeed(speed, unit);
    QString speedStr = "";

    switch(unit) {
    case SPEED_UNIT_BIT:
    {
        speedStr = QString("%1%2").arg(tspeed).arg("bps");
    }
    break;
    case SPEED_UNIT_Kb:
    {
        speedStr = QString("%1%2").arg(tspeed).arg("Kbps");
    }
    break;
    case SPEED_UNIT_Mb:
    {
        speedStr = QString("%1%2").arg(tspeed).arg("Mbps");
    }
    break;
    case SPEED_UNIT_Gb:
    {
        speedStr = QString("%1%2").arg(tspeed).arg("Gbps");
    }
    break;
    case SPEED_UNIT_BYTE:
    {
        speedStr = QString("%1%2").arg(tspeed).arg("B/s");
    }
    break;
    case SPEED_UNIT_KB:
    {
        speedStr = QString("%1%2").arg(tspeed).arg("KB/s");
    }
    break;
    case SPEED_UNIT_MB:
    {
        speedStr = QString("%1%2").arg(tspeed).arg("MB/s");
    }
    break;
    case SPEED_UNIT_GB:
    {
        speedStr = QString("%1%2").arg(tspeed).arg("GB/s");
    }
    break;
    default:
    {
        if(display == DISPLAY_UNIT_BIT) {
            speedStr = "0 Kbps";
        } else if(display == DISPLAY_UNIT_BYTE) {
            speedStr = "0 KB/s";
        }
    }
    break;
    }

    return speedStr;
}

QString RBHelper::getFileDeleteLogPath(t_taskid taskid)
{
    QString path = getTempPath("file_delete_log");
    QDir dir(path);
    if(!dir.exists()) {
        dir.mkdir(path);
    }
    QString tasklog = QString("%1_%2.log").arg(taskid).arg(QDate::currentDate().toString("yyyy-MM-dd"));
    path.append(QDir::separator());
    path.append(tasklog);

    return path;
}

// %tmp%/Renderbus/report/%folder%
QString RBHelper::getReportPath(const QString& folder)
{
    QString path = getTempPath("report");
    path.append(QDir::separator());
    path.append(folder);

    QString finalPath = QDir::toNativeSeparators(path);

    QDir().mkpath(finalPath);

    return path;
}

QString RBHelper::getReportPathBatch(const QString& folder, t_taskid taskid, int type, const QString& suffix)
{
    QString path = getReportPath(folder);
    path.append(QDir::separator());

    QString tasklog = QString("%1_%2_%3")
        .arg(taskid)
        .arg(type)
        //.arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"))
        .arg(suffix);

    path.append(tasklog);

    return path;
}

QString RBHelper::getSelectedFolder()
{
    QString strFolderName = "";
    QFileDialog filedialog;

    filedialog.setWindowTitle(QObject::tr("选择文件夹"));
    filedialog.setDirectory(".");
    filedialog.setOption(QFileDialog::ShowDirsOnly);
    filedialog.setFileMode(QFileDialog::Directory);
    if(filedialog.exec() == QDialog::Accepted) {
        strFolderName = filedialog.selectedFiles()[0];
    }

    return strFolderName;
}

QStringList RBHelper::getSelectedFiles(const QStringList& filters)
{
    QStringList strFileNames;
    QFileDialog filedialog;

    filedialog.setWindowTitle(QObject::tr("选择文件"));
    filedialog.setDirectory(".");
    filedialog.setOption(QFileDialog::ShowDirsOnly);
    filedialog.setFileMode(QFileDialog::ExistingFiles);
    filedialog.setNameFilters(filters);
    if (filedialog.exec() == QDialog::Accepted) {
        strFileNames = filedialog.selectedFiles();
    }

    return strFileNames;
}

QString RBHelper::getStatusText(qint32 status)
{
    switch(status) {
    case PLATFORM_STATUS_DISABLED:
        return QObject::tr("(暂停开放)");
        break;
    case PLATFORM_STATUS_IDLE:
        return QObject::tr("空闲");
        break;
    case PLATFORM_STATUS_NORMAL:
        return QObject::tr("正常");
        break;
    case PLATFORM_STATUS_BUSY:
        return QObject::tr("繁忙");
        break;
    case PLATFORM_STATUS_OVERLOADED:
        return QObject::tr("爆满");
        break;
    default:
        return QObject::tr("正常");
        break;
    }
}

QString RBHelper::getPlatformStatusText(qint32 status)
{
    QString  txt = getStatusText(status);
    QString text = "<span style='color:%1;text-decoration:none;'>%2</span>";
    switch(status) {
    case PLATFORM_STATUS_DISABLED:
        text = text.arg("#606060").arg(txt);
        break;
    case PLATFORM_STATUS_IDLE:
        text = text.arg("#675ffe").arg(txt);
        break;
    case PLATFORM_STATUS_NORMAL:
        text = text.arg("#675ffe").arg(txt);
        break;
    case PLATFORM_STATUS_BUSY:
        text = text.arg("#FF8A00").arg(txt);
        break;
    case PLATFORM_STATUS_OVERLOADED:
        text = text.arg("#E25C59").arg(txt);
        break;
    default:
        text = text.arg("#675ffe").arg(txt);
        break;
    }
    return text;
}

void RBHelper::startSpeedTest(const QString & url, const QString& transmitUrl)
{
    QString speedApp = QString("%1/%2%3").arg(qApp->applicationDirPath()).arg("Rayspeed").arg(APP_SUFFIX);

    LOGFMTI("[RBHelper]Start netcheck app, app path [%s]", qPrintable(speedApp));

    int lang = LocalSetting::getInstance()->getLanguageId();
    QProcess *process = new QProcess;
    QStringList params;
    params << QString::number(lang) << url;
    if(!transmitUrl.isEmpty()) {
        params << transmitUrl;
    }
    process->start(speedApp, params, QIODevice::ReadOnly);
    QObject::connect(process, static_cast<void(QProcess::*)(int)>(&QProcess::finished), process, &QProcess::deleteLater);
}

double RBHelper::second2hour(qint32 sec, int decimals)
{
    double h = sec / 3600.0;
    h = QString::number(h, 'f', decimals).toDouble();
    return h;
}

qint32 RBHelper::hour2second(double hour)
{
    return hour * 3600;
}

QStringList RBHelper::getOutputParams(t_taskid taskId, t_taskid parentId, const QString& scene, const QString& projectSavePath)
{
    QStringList params;
    params << "--ui" << QString::number(MyConfig.accountSet.userId)
           << "--fi" << QString::number(MyConfig.accountSet.mainUserId)
           << "--ti" << QString::number(taskId)
           << "--pi" << QString::number(parentId)
           << "--si" << QString::number(LocalSetting::getInstance()->getDefaultsite())
           << "-s" << scene
           << "-r" << projectSavePath;
    return params;
}

QString RBHelper::getOutputCmd(t_taskid taskId, t_taskid parentId, const QString& scene, const QString& projectSavePath)
{
    QString pyrootpath = RBHelper::getPyScriptRootPath();
#ifdef Q_OS_WIN
    QString outputPath = pyrootpath.append("/submit/output.exe");
#else
    QString outputPath = pyrootpath.append("/submit/output");
#endif

    QString cmd = "";
    QStringList params = getOutputParams(taskId, parentId, scene, projectSavePath);

    // 拼接cmd
    cmd.append(outputPath);
    cmd.append(" ");

    // params
    foreach (QString pa , params){
        cmd.append(pa);
        cmd.append(" ");
    }

    return cmd;
}

QString RBHelper::frameStatusToString(int status)
{
    QString stateText;
    switch (status) {
    case JobState::DetailInfo::task_frame_status_wait:
        stateText = QObject::tr("等待中");
        break;
    case JobState::DetailInfo::task_frame_status_rendering:
        stateText = QObject::tr("重建中");
        break;
    case JobState::DetailInfo::task_frame_status_stop:
        stateText = QObject::tr("停止");
        break;
    case JobState::DetailInfo::task_frame_status_completed:
        stateText = QObject::tr("完成");
        break;
    case JobState::DetailInfo::task_frame_status_failed:
        stateText = QObject::tr("失败");
        break;
    case JobState::DetailInfo::task_frame_status_wait_pre_finished:
        stateText = QObject::tr("等待预处理完成");
        break;
    case JobState::DetailInfo::task_frame_status_wait_photon_frame_finished:
        stateText = QObject::tr("等待光子帧渲染完成");
        break;
    case JobState::DetailInfo::task_frame_status_wait_priority_finished:
        stateText = QObject::tr("等待优先渲染完成");
        break;
    case JobState::DetailInfo::task_frame_status_wait_photon_finished:
        stateText = QObject::tr("等待光子作业渲染完成");
        break;
    case JobState::DetailInfo::task_frame_status_wait_resolve_finished:
        stateText = QObject::tr("等待解算作业渲染完成");
        break;
    case JobState::DetailInfo::task_frame_status_overtime_stop:
        stateText = QObject::tr("超时停止");
        break;
    default:
        stateText = QObject::tr("-");
        break;
    }
    return stateText;
}

QString RBHelper::ramUsageToString(qint64 value)
{
    if (value <= 0)
        return "-";

    QString ramUsageText;

    float num = value;
    QStringList list;
    list << "KB" << "MB" << "GB" << "TB";

    QStringListIterator i(list);
    QString unit("B");

    while (num >= 1024.0 && i.hasNext()) {
        unit = i.next();
        num /= 1024.0;
    }
    return ramUsageText.setNum(num, 'f', 2) + unit;
}

QString RBHelper::pixelsToString(qint64 value)
{
    if(value <= 0)
        return "-";

    QString pixelsText;

    float num = value;
    num /= (1000 * 1000 * 1000);

    return pixelsText.setNum(num, 'f', 3) + "GP";
}

QString RBHelper::cpuUsageToString(qint32 value)
{
    if (value <= 0)
        return "-";

    QString cpuUsageText;

    if (value > 100)
        value = 100;

    return cpuUsageText.setNum(value) + "%";
}

QString RBHelper::feeCostToString(double couponFee, double arrearsFee, double feeAmount)
{
    if (MyConfig.userSet.isChildAccount() && MyConfig.accountSet.hideJobCharge)
        return "-";

    QString cost = "";
    if (feeAmount > 0) {
#ifdef FOXRENDERFARM
        cost += QString("$%1").arg(feeAmount);
#else
        cost += QString("￥%1").arg(feeAmount);
#endif
    }
    if (couponFee > 0) {
        cost += QString("C%1").arg(couponFee);
    }
    if (arrearsFee > 0) {
        cost += QObject::tr("(欠费%1)").arg(arrearsFee);
    }
    return cost;
}

QString RBHelper::UtcToDateTimeString(qint64 time)
{
    if (time <= 0)
        return "-";

    QDateTime timedate = QDateTime::fromMSecsSinceEpoch(time);
    QString timestr = timedate.toString("yyyy-MM-dd hh:mm:ss");
    return timestr;
}

QString RBHelper::getWindowGrabPicPath(QWidget *widget, qint64 taskId, const QString& page, const QRect& rect)
{
    if (widget == NULL)
        return "";

    // path
    QString grabPath = RBHelper::makePathFromTempPath("grab");
    QDir dir(grabPath);
    if (!dir.exists()) {
        if (!RBHelper::makeDirectory(grabPath))
            return "";
    }
    grabPath = QString("%1/%2_%3_grab_%4.jpg").arg(grabPath).arg(page).arg(taskId).arg(RBHelper::generateMsgId());

    // write file
    QPixmap pix = widget->grab(rect);
    if (rect.isNull()) {
        pix = widget->grab();
    }
    QFile file(grabPath);
    file.open(QIODevice::WriteOnly);
    pix.save(&file, "jpg");

    return grabPath;
}

QString RBHelper::getUploadCachePath()
{
    QString dbpath = QString("%1/upload_cache").arg(RBHelper::getUserProfilePath());
    return dbpath;
}

QString RBHelper::getLocalCgSofts()
{
    // return ScriptPluginManager::getInstance()->getInstalledSofts();
    return "";
}

qint64 RBHelper::getJsonContainsValueCount(const QString& filePath, const QString& key)
{
    QJsonObject jsonObj;
    if (!RBHelper::loadJsonFile(filePath, jsonObj)) {
        return 0;
    }

    qint64 count = 0;
    if (jsonObj[key].isArray()) {
        count = jsonObj[key].toArray().count();
    } else if (jsonObj[key].isObject()) {
        count = jsonObj[key].toObject().count();
    }

    return count;
}

QString RBHelper::getFilterTime(int filterTime)
{
    QDate currDate = MyConfig.currentDateTime.date();
    QDate start;
    QDate end = currDate;
    filterTime = enFilterAll; // todo 默认显示全部
    switch (filterTime) {
    case enFilterAll:
        start = currDate.addYears(-5);
        break;
    case enFilter1Year:
        start = currDate.addYears(-1);
        break;
    case enFilter3Month:
        start = currDate.addMonths(-3);
        break;
    case enFilter1Month:
        start = currDate.addMonths(-1);
        break;
    case enFilter1Week:
        start = currDate.addDays(-7);
        break;
    default:
        break;
    }

    QString searchContent = start.toString("yyyy-MM-dd") + " 00:00:00" + "::" + end.toString("yyyy-MM-dd") + " 23:59:59";
    return searchContent;
}

QString RBHelper::getScriptHelperApp()
{
    QString appPath = QCoreApplication::applicationDirPath();
    appPath.append(QDir::separator());
    appPath.append(SCRIPT_HELPER_APP);

    appPath = QDir::toNativeSeparators(appPath);
    return appPath;
}

QString RBHelper::getCgProductName(const QString& name)
{
    QString pname = "";
    QString cgname = name.toLower().trimmed();
    if (cgname.contains("3ds max") || cgname.contains("3dsmax")) {
        pname = "3ds max";
    } else if (cgname.contains("maya")) {
        pname = "maya";
    } else if (cgname.contains("houdini")) {
        pname = "houdini";
    } else if (cgname.contains("cinema 4d")) {
        pname = "cinema 4d";
    } else if (cgname.contains("clarisse")) {
        pname = "clarisse";
    } else if (cgname.contains("sketch up") || cgname.contains("sketchup")) {
        pname = "sketchup";
    }

    return pname;
}

bool RBHelper::unavailableTime(const QString& time, int checkYear)
{
    QDateTime date = QDateTime::fromString(time, "yyyy-MM-dd hh:mm:ss");
    int year = date.toString("yyyy").toInt();
    return year > checkYear;
}

QRegion RBHelper::roundRect(const QRect& rect, int r, bool topLeft, bool topRight, bool bottomLeft, bool bottomRight)
{
    QRegion region;

    // middle and borders
    region += rect.adjusted(r, 0, -r, 0);
    region += rect.adjusted(0, r, 0, -r);

    // top left
    QRect corner(rect.topLeft(), QSize(r * 2, r * 2));
    region += QRegion(corner, topLeft ? QRegion::Ellipse : QRegion::Rectangle);

    // top right
    corner.moveTopRight(rect.topRight());
    region += QRegion(corner, topRight ? QRegion::Ellipse : QRegion::Rectangle);

    // bottom left
    corner.moveBottomLeft(rect.bottomLeft());
    region += QRegion(corner, bottomLeft ? QRegion::Ellipse : QRegion::Rectangle);

    // bottom right
    corner.moveBottomRight(rect.bottomRight());
    region += QRegion(corner, bottomRight ? QRegion::Ellipse : QRegion::Rectangle);

    return region;
}

QString RBHelper::encodingUrl(const QString& url, const QString& include)
{
    return QUrl::toPercentEncoding(url, "/_[]+-.=|{}~&*():?<>", include.toLocal8Bit());
}

QString RBHelper::getJsonMakerPath()
{
    QString makerName = "modeling_analyze.exe";
    QString fullPath = getScriptRootPath();
    fullPath.append("/modeling/");
    fullPath.append(makerName);

    return fullPath;
}

QString RBHelper::getSplitChar(int index)
{
    QString ch = "";
    switch (index)
    {
    case EN_SPLITE_COMMA:
        ch = ",";
        break;
    case EN_SPLITE_SEMICOLON:
        ch = ";";
        break;
    case EN_SPLITE_POINT:
        ch = ".";
        break;
    case EN_SPLITE_SPACE:
        ch = " ";
        break;
    case EN_SPLITE_TAB:
        ch = "\t";
        break;
    default:
        break;
    }

    return ch;
}

QUrl RBHelper::getSimpleLinkUrl(const QString& uri)
{
    QString host = HttpCmdManager::getInstance()->getRequestHost();
    QString path = WSConfig::getInstance()->get_link_address(uri);
    QString url = QString("%1/%2&from=%3").arg(host).arg(path).arg(WSConfig::getInstance()->get_link_address("path_from"));

    return QUrl(url);
}

QString RBHelper::getFieldOrderString(int index)
{
    QString order = "";
    switch (index)
    {
    case enXYZ:
        order = "name|X|Y|Z";
        break;
    case enYXZ:
        order = "name|Y|X|Z";
        break;
    case enLatitudeFrontLongitude:
        order = "name|latitude|longitude|altitude";
        break;
    case enLongitudeFrontLatitude:
        order = "name|longitude|latitude|altitude";
        break;
    default:
        order = "name|X|Y|Z";
        break;
    }

    return order;
}

int RBHelper::getFieldOrderIndex(const QString& order)
{
    int index = enXYZ;
    if(order == "name|X|Y|Z") {
        index = enXYZ;
    } else if(order == "name|Y|X|Z") {
        index = enYXZ;
    } else if (order == "name|latitude|longitude|altitude") {
        index = enLatitudeFrontLongitude;
    } else if (order == "name|longitude|latitude|altitude") {
        index = enLongitudeFrontLatitude;
    }
    return index;
}

int RBHelper::getSplitCharIndex(const QString& ch)
{
    int index = EN_SPLITE_COMMA;

    if (ch == ",") {
        index = EN_SPLITE_COMMA;
    } else if (ch == ";") {
        index = EN_SPLITE_SEMICOLON;
    }else if (ch == ".") {
        index = EN_SPLITE_POINT;
    } else if (ch == " ") {
        index = EN_SPLITE_SPACE;
    } else if (ch == "\t") {
        index = EN_SPLITE_TAB;
    }

    return index;
}

bool RBHelper::checkProjectExist(qint64 taskId, const QString& fileName)
{
    QString client_project_base_dir = LocalSetting::getInstance()->getClientProjectPath();
    QString client_project_dir = QString("%1/%2/").arg(client_project_base_dir).arg(taskId);
    client_project_dir = QDir::cleanPath(client_project_dir);
    QString project_file = client_project_dir + "/" + fileName;

    return QFileInfo::exists(project_file);
}

QString RBHelper::getOutputTypeString(OutputType type)
{
    QString typeStr;
    switch (type)
    {
    case OUTPUT_TYPE_OSGB:
        typeStr = "OSGB";
        break;
    case OUTPUT_TYPE_OBJ:
        typeStr = "OBJ";
        break;
    case OUTPUT_TYPE_LAS:
        typeStr = "LAS";
        break;
    case OUTPUT_TYPE_3D_TILES:
        typeStr = "Cesium 3D Tiles";
        break;
    case OUTPUT_TYPE_TIFF:
        typeStr = "TIFF";
        break;
    case OUTPUT_TYPE_EDIT_OBJ:
        typeStr = "Editable OBJ";
        break;
    case OUTPUT_TYPE_3MX:
        typeStr = "3MX";
        break;
    case OUTPUT_TYPE_FBX:
        typeStr = "FBX";
        break;
    case OUTPUT_TYPE_S3C:
        typeStr = "S3C";
        break;
    default:
        break;
    }
    return typeStr;
}

QString RBHelper::getOutputTypeDisplay(OutputType type)
{
    QString typeStr;
    switch (type)
    {
    case OUTPUT_TYPE_OSGB:
        typeStr = "OSGB";
        break;
    case OUTPUT_TYPE_OBJ:
        typeStr = "OBJ";
        break;
    case OUTPUT_TYPE_LAS:
        typeStr = "LAS";
        break;
    case OUTPUT_TYPE_3D_TILES:
        typeStr = "Cesium 3D Tiles";
        break;
    case OUTPUT_TYPE_TIFF:
        typeStr = QObject::tr("正射TIFF");
        break;
    case OUTPUT_TYPE_EDIT_OBJ:
        typeStr = QObject::tr("可编辑的OBJ");
        break;
    case OUTPUT_TYPE_3MX:
        typeStr = "3MX";
        break;
    case OUTPUT_TYPE_FBX:
        typeStr = "FBX";
        break;
    case OUTPUT_TYPE_S3C:
        typeStr = "S3C";
        break;
    default:
        break;
    }
    return typeStr;
}

QString RBHelper::getOutputFileName(OutputType type)
{
    QString fileName = QString("%1.zip").arg(getOutputTypeString(type));

    return fileName;
}