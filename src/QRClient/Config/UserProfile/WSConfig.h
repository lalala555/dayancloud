/***********************************************************************
* Module:  WSConfig.h
* Author:  hqr
* Modified: 2017/04/01 10:37:49
* Purpose: Declaration of the class
***********************************************************************/
#ifndef WS_CONFIG_H
#define WS_CONFIG_H

#include <QObject>
#include <QString>
#include <QVector>
#include <vector>
#include <map>
#include "kernel/macro.h"

enum enArgumentMode {
    MODE_PLAIN = 0,
    MODE_ENCODE = 1
};

typedef struct _platform_info {
    bool valid;
    int id;
    QString name;
    QString alias;
    QString tooltip;
    enPlatformStatus system_status;
    bool gpu_flag;
    bool qingyun_flag;
    bool is_default;
    int argument_mode; // 分析加密控制

    bool isPlatformEnable()
    {
        return system_status > PLATFORM_STATUS_DISABLED;
    }
    _platform_info() : valid(false), argument_mode(MODE_PLAIN) {};
} platform_info;

typedef struct _site_info {
    QString host;
    QString accept_version;
    QString content;
    QString content_digest;
    QVector<platform_info> platforms;
} site_info;


typedef struct _cg_software {
    qint64  id;
    QString name;
    QString suffix;
    QString script;
    QString logo;
    bool linux_support;
} CGSoftware;

typedef struct _scene_type {
    QString ext;
    CGSoftware software;
} SceneType;

class WSConfig
{
    WSConfig() {};
    ~WSConfig() {};
public:
    static WSConfig* getInstance()
    {
        static WSConfig instance;
        return &instance;
    }

    QString getWSHostUrl() const;
    QString getWSVersion() const;

    bool init();
    bool parse_site(const QString &server_list_xml);
    int get_default_platform(int ptid);
    platform_info get_platform(int ptid);
    QString get_host_url();
    QString get_accept_version();
    void get_content_degist(QString& content, QString& degist);
    bool is_gpu_platform(int ptid);
    bool is_qingyun_platform(int ptid);
    int get_argument_mode(int ptid);
    // Links
    QString get_link_address(const QString& key);
    QString get_key_value(const QString& key);
public:
    site_info site;
    QVector<CGSoftware> cg_softwares;
    QMap<qint64, CGSoftware> cg_support_softwares;
    QMap<QString, CGSoftware> cg_support_types;
private:
    QString m_wsVersion;
    QString m_wsHostUrl;

    QMap<QString, QString> m_url_map;
    QMap<QString, QString> m_dict;
};

#endif // _WS_CONFIG_H
