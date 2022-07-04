/***********************************************************************
* Module:  PluginMgr.h
* Author:  hqr
* Created: 2018/11/06 10:28:05
* Modifier: hqr
* Modified: 2018/11/06 10:28:05
* Purpose: Declaration of the class
***********************************************************************/
#pragma once
#ifndef PLUGINMGR_H
#define PLUGINMGR_H

#include "HttpCommand/HttpCommon.h"
Q_DECLARE_METATYPE(PluginConfigInfo)
typedef QList<PluginConfigInfo> PluginList;
typedef QMap<qint32, QList<CgVersion>> CgVersionMap;
typedef QMap<qint32, QList<CgPlugin>>  CgPluginMap;

class PluginMgr
{
    PluginMgr(void);
public:
    ~PluginMgr(void) {};
    static PluginMgr* getInstance()
    {
        static PluginMgr instance;
        return &instance;
    }

    void clear();
    void clearSoftConfig(qint32 cgId);
    void clearCgSoftList(qint32 cgId, const QString& osName = "windows");
    void addPluginConfig(const PluginConfigInfo& pluginInfo);

    void addCgSoftVersion(const CgVersion& cgSoft, const QString& osName);
    void addCgPluginVersion(qint32 cgId, const CgPlugin& pluginVer, const QString& osName);

    bool isConfigedSoftAndPlugins();
    int  cgNameToCgId(const QString& name);
public:
    PluginList getConfigListById(qint32 cgId);
    PluginList getAllConfigPlugins();
    QVector<QString> getConfigedNameListById(qint32 cgId);
    QList<PluginInfoVos> getPluginInfoVos(qint32 cgId, const QString& configName);
    PluginConfigInfo getConfigedByName(qint32 cgId, const QString& configName);
    void clearPluginConfig(qint32 cgId);
    PluginConfigInfo getConfigedByName(const QString& configName);

    QList<CgVersion> getCgSoftVersionList(qint32 cgId, const QString& osName);
    QList<CgPlugin>  getCgPluginVersionList(qint32 cgId, qint32 cvId, const QString& osName);
    CgVersion findCgSoftVersionByVersion(qint32 cgId, const QString& cgVer, const QString& osName);
    CgPlugin  findCgPluginByName(qint32 cgId, qint32 cvId, const QString& pluginName, const QString& osName);
    int  findCgPluginByPluginVerId(qint32 cgId, qint32 cvId, const QString& pluginVer, const QString& osName);

    QString getDefaultPluginConfig(int cgid);
    void setDefaultPluginConfig(int cgid, const QString& pluginConfig);
    QString matchLocalDefaultConfig(qint32 cgId, const QString& localDefaultConfig);
    int getBasePageDefaultCgId();
    void setBasePageDefaultCgId(int cgId);
    QString getBasePagePluginConfig();
    void setBasePagePluginConfig(const QString& pluginConfig);

    // osname
    QString getOsNameString(int os);
    QString getLayerRenderTypeString(int type);

    // 获取所有cgid
    QList<int> getAllSoftIds();
    QString softName(int cgId);

    QMap<qint32, PluginList> m_configList;
    QMap<QString, CgVersionMap> m_cgVersionList;
    QMap<QString, CgPluginMap>  m_cgPluginList;
    bool isInitialized; // 是否已初始化数据
};

#endif // PLUGINMGR_H