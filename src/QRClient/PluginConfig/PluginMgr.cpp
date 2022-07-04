#include "stdafx.h"
#include "PluginMgr.h"
#include "ProjectMgr.h"

PluginMgr::PluginMgr(void)
    : isInitialized(false)
{
}

void PluginMgr::clear()
{
    m_configList.clear();
}

void PluginMgr::clearCgSoftList(qint32 cgId, const QString& osName)
{
    auto cgVer = m_cgVersionList.find(osName);
    if(cgVer != m_cgVersionList.end()) {
        auto verMap = cgVer.value().find(cgId);
        if(verMap != cgVer.value().end()) {
            m_cgVersionList[osName][cgId].clear();
        }
    }

    auto plgList = m_cgPluginList.find(osName);
    if(plgList != m_cgPluginList.end()) {
        m_cgPluginList[osName][cgId].clear();
    }
}

void PluginMgr::clearSoftConfig(qint32 cgId)
{
    auto config = m_configList.find(cgId);
    if(config != m_configList.end()) {
        m_configList[cgId].clear();
    }
}

void PluginMgr::addPluginConfig(const PluginConfigInfo& pluginInfo)
{
    qint32 cgId = pluginInfo.cgId;
    auto it = m_configList.find(cgId);
    QList<PluginConfigInfo> infos;

    if(it == m_configList.end()) {
        infos.append(pluginInfo);
        m_configList.insert(cgId, infos);

    } else {
        m_configList[cgId].append(pluginInfo);
    }
}

PluginList PluginMgr::getConfigListById(qint32 cgId)
{
    PluginList infos;

    auto it = m_configList.find(cgId);
    if(it != m_configList.end())
        infos = it.value();

    return infos;
}

PluginList PluginMgr::getAllConfigPlugins()
{
    PluginList infos;
    foreach(PluginList info, m_configList.values()) {
        infos.append(info);
    }
    return infos;
}

QVector<QString> PluginMgr::getConfigedNameListById(qint32 cgId)
{
    QVector<QString> configNames;
    PluginList infos = this->getConfigListById(cgId);
    foreach(PluginConfigInfo info, infos) {
        configNames.append(info.editName);
    }
    return configNames;
}

QList<PluginInfoVos> PluginMgr::getPluginInfoVos(qint32 cgId, const QString& configName)
{
    QList<PluginInfoVos> pluginInfos;
    PluginList infos = this->getConfigListById(cgId);
    foreach(PluginConfigInfo info, infos) {
        if(info.editName == configName) {
            pluginInfos = info.infoVos;
            break;
        }
    }
    return pluginInfos;
}

PluginConfigInfo PluginMgr::getConfigedByName(qint32 cgId, const QString& configName)
{
    PluginConfigInfo info;
    info.cgId = -1;
    auto it = m_configList.find(cgId);
    if(it != m_configList.end()) {
        foreach(PluginConfigInfo config, it.value()) {
            if(config.editName == configName) {
                info = config;
                break;
            }
        }
    }
    return info;
}

PluginConfigInfo PluginMgr::getConfigedByName(const QString& configName)
{
    PluginConfigInfo info;
    info.cgId = -1;
    QList<PluginList> allConfigs = m_configList.values();
    foreach(QList<PluginConfigInfo> cfgList, allConfigs) {
        foreach(PluginConfigInfo cfg, cfgList) {
            if(cfg.editName == configName) {
                return cfg;
            }
        }
    }
    return info;
}

QString PluginMgr::getDefaultPluginConfig(int cgid)
{
    CGConfigItem item = CConfigDb::Inst()->getCGConfig(cgid);
    QString pluginConfig = QString::fromStdString(item.cfg_name);
    return pluginConfig;
}

void PluginMgr::setDefaultPluginConfig(int cgid, const QString& pluginConfig)
{
    // 设置默认的配置
    CGConfigItem item = CConfigDb::Inst()->getCGConfig(cgid);
    item.cfg_name = pluginConfig.toStdString();
    item.cgid = cgid;
    CConfigDb::Inst()->updateCGConfig(item);

    this->setBasePageDefaultCgId(cgid);
    this->setBasePagePluginConfig(pluginConfig);
}

void PluginMgr::addCgSoftVersion(const CgVersion& cgSoft, const QString& osName)
{
    // 1. 通过osname找到对应的软件库
    // 2. 再通过cgid寻找cgver list
    auto it = m_cgVersionList.find(osName.toLower());
    qint32 cgId = cgSoft.cgId;

    // 没有找到就新插入一个
    CgVersionMap cgVerMap;
    if(it == m_cgVersionList.end()) {

        QList<CgVersion> softs;
        softs.append(cgSoft);
        cgVerMap.insert(cgId, softs);

    } else {
        cgVerMap.clear();
        cgVerMap = it.value();
        QList<CgVersion> softs;

        // 寻找是不是存在
        auto cg = cgVerMap.find(cgId);
        if(cg == cgVerMap.end()) {
            softs.append(cgSoft);
            cgVerMap.insert(cgId, softs);

        } else {
            cgVerMap[cgId].append(cgSoft);
        }
    }
    m_cgVersionList[osName] = cgVerMap;
}

void PluginMgr::addCgPluginVersion(qint32 cgId, const CgPlugin& pluginVer, const QString& osName)
{
    // 1. 通过osname找到对应的软件库
    // 2. 再通过cgid寻找cgver list
    auto it = m_cgPluginList.find(osName.toLower());

    // 没有找到就新插入一个
    CgPluginMap cgPluginMap;
    QList<CgPlugin> plugins;
    if(it == m_cgPluginList.end()) {
        plugins.clear();
        plugins.append(pluginVer);
        cgPluginMap.insert(cgId, plugins);

    } else {
        cgPluginMap.clear();
        cgPluginMap = it.value();

        // 寻找是不是存在
        auto cgPlg = cgPluginMap.find(cgId);
        if(cgPlg == cgPluginMap.end()) {

            plugins.clear();
            plugins.append(pluginVer);
            cgPluginMap.insert(cgId, plugins);

        } else {
            cgPluginMap[cgId].append(pluginVer);
        }
    }
    m_cgPluginList[osName] = cgPluginMap;
}

QList<CgVersion> PluginMgr::getCgSoftVersionList(qint32 cgId, const QString& osName)
{
    QList<CgVersion> softList;
    auto it = m_cgVersionList.find(osName.toLower());
    if(it != m_cgVersionList.end()) {
        CgVersionMap cgMap = it.value();
        auto cg = cgMap.find(cgId);
        if(cg != cgMap.end()) {
            softList = cg.value();
        }
    }
    return softList;
}

QList<CgPlugin>  PluginMgr::getCgPluginVersionList(qint32 cgId, qint32 cvId, const QString& osName)
{
    QList<CgPlugin> plugins;

    auto it = m_cgPluginList.find(osName.toLower());
    if(it == m_cgPluginList.end())
        return plugins;

    CgPluginMap plgMap = it.value();
    auto plg = plgMap.find(cgId);
    if(plg == plgMap.end())
        return plugins;

    foreach(CgPlugin plugin, plg.value()) {
        if(plugin.cvId == cvId) {
            plugins.append(plugin);
        }
    }
    return plugins;
}

CgVersion PluginMgr::findCgSoftVersionByVersion(qint32 cgId, const QString& cgVer, const QString& osName)
{
    CgVersion cgSoft;
    QList<CgVersion> list = this->getCgSoftVersionList(cgId, osName.toLower());
    QString nameVer;
    foreach(CgVersion cg, list) {
        nameVer = cg.cgName + cg.cgVersion;
        if(nameVer == cgVer) {
            cgSoft = cg;
            break;
        }
    }
    return cgSoft;
}

CgPlugin  PluginMgr::findCgPluginByName(qint32 cgId, qint32 cvId, const QString& pluginName, const QString& osName)
{
    CgPlugin plugin;
    QList<CgPlugin> list = this->getCgPluginVersionList(cgId, cvId, osName.toLower());
    foreach(CgPlugin plu, list) {
        if(plu.pluginName == pluginName) {
            plugin = plu;
            break;
        }
    }
    return plugin;
}

int  PluginMgr::findCgPluginByPluginVerId(qint32 cgId, qint32 cvId, const QString& pluginVer, const QString& osName)
{
    QString pluver = "";
    QList<CgPlugin> list = this->getCgPluginVersionList(cgId, cvId, osName.toLower());
    foreach(CgPlugin plu, list) {
        foreach(PluginVersions ver, plu.pluginVersions) {
            pluver = ver.pluginVersion;
            if(pluver == pluginVer) {
                return ver.pluginId;
            }
        }
    }
    return 0;
}

/*
    1. 用于匹配当前页面与通用设置页面的配置是否一致
    2. 如果本地配置一致则返回通用设置下的默认配置
    3. 如果没有找到，则返回当前cgId的默认配置（说明不是当前cgid的配置）
    4. 如果本地没有设置默认配置，则返回第一个配置为默认
*/
QString PluginMgr::matchLocalDefaultConfig(qint32 cgId, const QString& localDefaultConfig)
{
    QList<PluginConfigInfo> vConfig = this->getConfigListById(cgId);
    // 比对当前页面的配置里是否与本地默认项目匹配
    foreach(PluginConfigInfo cfg, vConfig) {
        if(cfg.editName == localDefaultConfig) {
            return localDefaultConfig;
        }
    }
    // 没有找到匹配项，使用该cgId的默认配置
    QString cfgName = QString::fromStdString(CConfigDb::Inst()->getCGConfig(cgId).cfg_name);
    // 可能本地的配置和线上的配置已经不匹配，需要重新判断
    PluginConfigInfo info = this->getConfigedByName(cfgName);
    if(info.editName != cfgName) {
        cfgName = "";
    }
    if(cfgName.isEmpty() && !vConfig.isEmpty()) {
        cfgName = vConfig.at(0).editName;
    }
    return cfgName;
}

void PluginMgr::clearPluginConfig(qint32 cgId)
{
    m_configList[cgId].clear();
}

QString PluginMgr::getOsNameString(int os)
{
    QString osName = "";
    switch(os) {
    case 0:
        osName = "Linux";
        break;
    case 1:
        osName = "Windows";
        break;
    }
    return osName;
}

QString PluginMgr::getLayerRenderTypeString(int type)
{
    QString renderType = "";
    switch(type) {
    case 0:
        renderType = "Render Layer";
        break;
    case 1:
        renderType = "Render setup";
        break;
    }
    return renderType;
}

QList<int> PluginMgr::getAllSoftIds()
{
    return m_configList.keys();
}

QString PluginMgr::softName(int cgId)
{
    QString name = "undefine";

    auto cgSoftwares = WSConfig::getInstance()->cg_softwares;
    for (int i = 0; i < cgSoftwares.size(); i++) {
        auto software = cgSoftwares[i];
        QString softname = software.name;

        if(cgId == software.id){
            return softname;
        }
    }
    return name;
}

int PluginMgr::getBasePageDefaultCgId()
{
    return CConfigDb::Inst()->getDefaultCgId();
}

void PluginMgr::setBasePageDefaultCgId(int cgId)
{
    CConfigDb::Inst()->setDefaultCgId(cgId);
}

QString PluginMgr::getBasePagePluginConfig()
{
    QString pluginConfig = QString::fromStdString(CConfigDb::Inst()->getPluginConfig());
    return pluginConfig;
}

void PluginMgr::setBasePagePluginConfig(const QString& pluginConfig)
{
    CConfigDb::Inst()->setPluginConfig(pluginConfig.toStdString());
}

int PluginMgr::cgNameToCgId(const QString& name)
{
    int cgid = CG_ID_UNKNOWN;

    auto cgSoftwares = WSConfig::getInstance()->cg_softwares;
    for (int i = 0; i < cgSoftwares.size(); i++) {
        auto software = cgSoftwares[i];
        QString softname = software.name;

        if(name.toLower().trimmed() == softname.toLower().trimmed()){
            return software.id;
        }
    }
    return cgid;
}

bool PluginMgr::isConfigedSoftAndPlugins()
{
    return !m_configList.isEmpty();
}