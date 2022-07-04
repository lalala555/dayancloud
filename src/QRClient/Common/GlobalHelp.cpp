#include "stdafx.h"
#include "Common/LangTranslator.h"
#include "Logging/LogManager.h"
#include "HttpCommand/HttpRestApi.h"

IResponseSink* CGlobalHelp::g_mainSink = Q_NULLPTR;

void CGlobalHelp::BeepOn()
{
    LocalConfig::getInstance()->init();
    LocalSetting::getInstance()->init();

    LogManager::init();

    // 初始化配置文件
    CProfile::Inst()->start();

    // 读取安装时生成的language.cfg配置文件 记录是安装时选择的语言
    InitLanguageCfg();

    LangTranslator::translateLanguage(LocalSetting::getInstance()->getLanguageId());
}

void CGlobalHelp::BeepOff()
{
    LOGFMTW("Global exit begin...");
    LocalConfig::getInstance()->uninit();

    LangTranslator::destory();

    CProfile::Inst()->exit();
    CProfile::Destroy();

    CConfigDb::Inst()->exit();
    CConfigDb::Destroy();

    LOGFMTW("Global exit finish...");
}

bool CGlobalHelp::Response(int type, intptr_t wParam, intptr_t lParam)
{
    if(CGlobalHelp::g_mainSink == nullptr)
        return false;

    return CGlobalHelp::g_mainSink->Response(type, wParam, lParam);
}

void CGlobalHelp::InitLanguageCfg()
{
    QString langFile = QCoreApplication::applicationDirPath() + "/language.cfg";
    QSettings langSetting(langFile, QSettings::IniFormat);
    int binit = langSetting.value("/setting/init").toInt();
    if (!binit) { // 未被读取过
//       int langId = langSetting.value("/setting/language").toInt();       
#ifdef FOXRENDERFARM
	   LocalSetting::getInstance()->setLanguageId(1);
#else
	   LocalSetting::getInstance()->setLanguageId(0);
#endif
	   langSetting.setValue("/setting/init", 1);
    }
}

void CGlobalHelp::InitEnv(const QString& appdataDir)
{
    QString appdata_env_path = appdataDir + ENV_FILE;
    QFile::remove(appdata_env_path);
    // userid=962712
    // username=heqingrui
    // installdir=C:\Users\enfuzion\AppData\Roaming\Rayvision\RenderBus4.0
    // plugindir=C:\Users\enfuzion\AppData\Roaming\RenderBus\1005\Profiles\users\nono\enfuzion\pluginconf
    // projectdir=C:\RenderFarm\Project

    int user_id = MyConfig.accountSet.userId;
    QString user_name = MyConfig.userSet.userName;
    QString cli_install_dir = QDir(QCoreApplication::applicationDirPath()).absolutePath();
    QString cli_install_path = QDir(QCoreApplication::applicationFilePath()).absolutePath();
    QString project_dir = QString("%1/").arg(LocalSetting::getInstance()->getClientProjectPath());

    QSettings envset(appdata_env_path, QSettings::IniFormat);
    envset.setIniCodec("UTF-8");
    envset.setValue("UserEnv", "");
    envset.setValue("UserEnv/product", PRODUCT_NAME);
    envset.setValue("UserEnv/userid", user_id);
    envset.setValue("UserEnv/username", user_name);
    envset.setValue("UserEnv/installdir", cli_install_dir);
    envset.setValue("UserEnv/appfilepath", cli_install_path);
    envset.setValue("UserEnv/projectdir", project_dir);
    envset.setValue("UserEnv/skipupload", MyConfig.renderSet.justUploadConfigFlag);
    envset.setValue("UserEnv/guiversion", STRFILEVERSION);
    envset.setValue("UserEnv/platform", LocalSetting::getInstance()->getDefaultsite());
    envset.setValue("UserEnv/zone", MyConfig.accountSet.zone);
    envset.setValue("UserEnv/userkey", MyConfig.userSet.userKey);
    envset.setValue("UserEnv/storage_texture", MyConfig.storageSet.commonBid);
    envset.setValue("UserEnv/default_project", "0");
    envset.setValue("UserEnv/agent", AGENT_NAME);

    // 环境信息写注册表
    QString baseName = LocalConfig::getInstance()->defaultProduct;
    if (baseName.isEmpty()) baseName = BASE_PRODUCT_NAME;
    QString reg_env_path = QString("HKEY_LOCAL_MACHINE\\SOFTWARE\\Rayvision\\%1\\Env").arg(baseName);
    QSettings regsets(reg_env_path, QSettings::NativeFormat);
    regsets.setIniCodec("UTF-8");
    regsets.setValue("product", PRODUCT_NAME);
    regsets.setValue("userid", user_id);
    regsets.setValue("username", user_name);
    regsets.setValue("installdir", cli_install_dir);
    regsets.setValue("appfilepath", cli_install_path);
    regsets.setValue("projectdir", project_dir);
    regsets.setValue("skipupload", MyConfig.renderSet.justUploadConfigFlag);
    regsets.setValue("guiversion", STRFILEVERSION);
    regsets.setValue("platform", LocalSetting::getInstance()->getDefaultsite());
    regsets.setValue("zone", MyConfig.accountSet.zone);
    regsets.setValue("userkey", MyConfig.userSet.userKey);
    regsets.setValue("storage_texture", MyConfig.storageSet.commonBid);
    regsets.setValue("default_project", "0");
    regsets.setValue("agent", AGENT_NAME);
}

void CGlobalHelp::DestoryEnv(const QString& appdataDir)
{
    QString appdata_env_path = appdataDir + QDir::separator() + ENV_FILE;
    QFile::remove(appdata_env_path);
}
