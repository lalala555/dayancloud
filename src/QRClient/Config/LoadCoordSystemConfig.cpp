#include "stdafx.h"
#include "LoadCoordSystemConfig.h"

LoadCoordSystemConfig::LoadCoordSystemConfig(QObject *parent)
    : QObject(parent)
{
    // 默认数据
    m_configs << "Access local coordinate system (EPSG:null)";

    connect(&m_downloadCfg, &QDownloadManager::resultReady, this, &LoadCoordSystemConfig::onDownloadFinished);
}

LoadCoordSystemConfig::~LoadCoordSystemConfig()
{
}

void LoadCoordSystemConfig::initConfig()
{
    m_downloadCfg.downloadCoordSystemFile("coord_system.json");
}

void LoadCoordSystemConfig::loadConfig()
{
    // 1. 先读取网络路径的数据
    // 2. 如果下载失败，则读取本地数据
    // 3. 如果本地也获取失败，则返回默认值
    if (!loadOnlineConfig()) {
        if (!loadLocalConfig()) {
            LOGFMTE("[LoadCoordSystemConfig] Load coord system file failed!", );
        }
    }
}

QStringList LoadCoordSystemConfig::getSystemConfig()
{
    return m_configs;
}

bool LoadCoordSystemConfig::loadLocalConfig()
{
    QString configFile = QString("%1/resource/%2").arg(QCoreApplication::applicationDirPath()).arg("coord_system.json");
    if (!QFileInfo::exists(configFile)) return false;

    return readConfigJson(configFile);
}

bool LoadCoordSystemConfig::loadOnlineConfig()
{
    QString appRootPath = RBHelper::getTempPath("coord_system");
    QString filePath = appRootPath + "/" + "coord_system.json";
    if (!QFileInfo::exists(filePath)) return false;

    return readConfigJson(filePath);
}

bool LoadCoordSystemConfig::readConfigJson(const QString& filePath)
{
    QJsonObject json;
    if (!RBHelper::loadJsonFile(filePath, json))
        return false;

    if (json["coord_system"].isArray()) {
        QJsonArray arr = json["coord_system"].toArray();
        m_configs.clear();
        for (int i = 0; i < arr.size(); i++) {
            QString cfg = arr[i].toString();
            if (!m_configs.contains(cfg)) {
                m_configs.append(cfg);
            }      
        }
    }

    return true;
}

void LoadCoordSystemConfig::onDownloadFinished(int code, const QString& data)
{
    loadConfig();
}