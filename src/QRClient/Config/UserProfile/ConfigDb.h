/***********************************************************************
* Module:  ConfigDb.h
* Author:  hqr
* Created: 2018/11/05 11:46:16
* Modifier: hqr
* Modified: 2018/11/05 11:46:16
* Purpose: Declaration of the class
***********************************************************************/
#pragma once
#ifndef CONFIGDB_H_
#define CONFIGDB_H_

#include <QString>
#include "Common/MainFrame.h"
#include "Common/SqliteHelp/Database.h"

// 表socialmedia
typedef struct tagSocialMediaInfo {
    int nId;
    std::string userName;
    std::string password;
    std::string proxy;
    std::string loginState;
} SnsItem;

// 表t_schedule
typedef struct tagScheduleItem {
    int idx;
    int action;
    std::string startTime;
    std::string endTime;
} ScheduleItem;

// 表t_cg_config
typedef struct tagCGConfigItem {
    int idx;
    int cgid;
    std::string cfg_name;
    std::string project_output;
    std::string project_dir;

    tagCGConfigItem()
    {
        idx = -1;
        cgid = -1;
        project_output = ""; // todo
        cfg_name = "";
    }
} CGConfigItem;

class CConfigDb : public Database, public TSingleton<CConfigDb>
{
public:
    CConfigDb(void);
    virtual ~CConfigDb(void) {};

    void start();
    void exit();
public:
    QString getHost(); // format: https://pic.renderbus.com/
    void setHost(const QString& host);

    // 开机启动设置
    bool getAutoStartup();
    void setAutoStartup(bool bauto);
    // 自动检查更新设置
    bool getAutoUpdate();
    void setAutoUpdate(bool benable);
    // 自动下载设置
    bool getAutoDownload();
    void setAutoDownload(bool bauto);
    // 启用参数设置
    bool getEnableSetParam();
    void setEnableSetParam(bool benable);
    // 脚本分析并发数Parallel
    int getScriptCount();
    void setScriptCount(int count);
    // 脚本分析超时设置(小时)
    int getScriptTimeout();
    void setScriptTimeout(int timeout);
    // 超时提醒设置
    int getRenderTimeoutRemind();
    void setRenderTimeoutRemind(int timeout);
    // 超时停止设置
    int getFrameTimeoutStop();
    void setFrameTimeoutStop(int timeoutStop);
    // 拖拽弹出项目选择
    int getPopupSetProject();
    void setPopupSetProject(int popup);
    // 默认项目
    void setProject(int id, const std::string& name);
    int getProjectId();
    std::string getProjectName();
    // 默认配置
    std::string getPluginConfig();
    void setPluginConfig(const std::string& plugin_config);
    // 默认软件
    int getDefaultCgId();
    void setDefaultCgId(int cgId);

    // 计划任务
    std::vector<ScheduleItem> getSchedules();
    bool addSchedule(int action, const std::string& startTime, const std::string& endTime);
    bool removeSchedule(int action, const std::string& startTime, const std::string& endTime);

    // CG软件目录配置
    CGConfigItem getCGConfig(int cgid);
    void updateCGConfig(const CGConfigItem& item);
    bool addOrUpdateCGConfig(const CGConfigItem& item);
    bool addCGConfig(int cgid, const std::string& cfgName, const std::string& output, const std::string& dir);
    bool updateCGConfig(int cgid, const std::string& cfgName, const std::string& output, const std::string& dir);
    bool removeCGConfig(int cgid);

    // 设备验证
    std::string getAccessId(const std::string& userName);
    bool setAccessId(const std::string& userName, const std::string& accessId);

    // 下载相关
    bool isEnableProjectDownloadPath();
    void setEnableProjectDownloadPath(bool enable);
    // 下载列表的路径是否已经更新（版本更新标志位）
    bool isUpdateDownloadList();
    void setUpdateDownloadList(bool isUpdate);

    // 超时提醒设置,以秒为单位
    int getRenderTimeoutRemindSec();
    void setRenderTimeoutRemindSec(int timeout_s);
    // 超时停止设置 ,以秒为单位
    int getFrameTimeoutStopSec();
    void setFrameTimeoutStopSec(int timeoutStop_s);
    // 显示类型
    int  getShowTaskType();
    void setShowTaskType(int nType);
    // proxy
    QString getSocialProxyName();
    void setSocialProxyName(QString proxy);
private:
    virtual void build_table();
    virtual void read_table();
    virtual void build_table_common();

private:
    void read_table_cg_config();
    void read_table_schedule();
    void read_device_auth();

private:
    QString m_hostUrl;

    std::vector<ScheduleItem> m_tabSchedule;
    std::vector<CGConfigItem> m_tabCGConfig;
    std::map<std::string, std::string> m_tabDevice;

    int m_showType;
    QString m_strSocialProxyName;
};

#endif // CONFIGDB_H_
