/***********************************************************************
* Module:  Profile.h
* Author:  hqr
* Created: 2018/11/01 15:23:35
* Modifier: hqr
* Modified: 2018/11/01 15:23:35
* Purpose: Declaration of the class
***********************************************************************/
#pragma once
#ifndef CPROFILE_H
#define CPROFILE_H

#include "Common/MainFrame.h"
#include "Common/SqliteHelp/Database.h"
#include <qglobal.h>
#include <QMetaType>
#include <QMap>
#include <QString>

struct AccountItem {
    std::string name;
    std::string password;
    qint32 language;
    bool auto_login_flag;
    bool save_password_flag;
    std::string last_login_time;
};

struct CgSoftwareItem {
    qint32 cg_id;
    std::string cg_name;
    std::string installed_dir;
    bool operator==(const CgSoftwareItem& item) {
        return this->cg_id == item.cg_id && this->cg_name == item.cg_name && this->installed_dir == item.installed_dir;
    }
};

struct ProjectConfigItem{
    qint64 user_id;
    int proj_id;
    std::string proj_name;
    std::string download_path;
    std::string create_time;
    std::string modify_time;
    std::string unique_code;
    std::string last_proj_name; // 前一次选择的项目名（创建过程中可能多次修改项目名，导致添加多余记录）

    bool operator==(const ProjectConfigItem& item) {
        return this->user_id == item.user_id && this->proj_id == item.proj_id &&
            this->proj_name == item.proj_name && this->download_path == item.download_path &&
            this->create_time == item.create_time &&this->modify_time == item.modify_time && 
            this->unique_code == item.unique_code && this->last_proj_name == item.last_proj_name;
    }
};
Q_DECLARE_METATYPE(ProjectConfigItem)
Q_DECLARE_METATYPE(ProjectConfigItem*)

struct CgInstallItem {
    qint32 cg_id;
    std::string cg_name;
    std::string cg_version;
    std::string edit_name;// 配置名
    std::string installed_dir;
};

struct UserCoordSystem{
    qint32 id;
    std::string coord_name;
    std::string coord_definition;
    std::string coord_system_content;
    std::string create_time;
    std::string modify_time;

    UserCoordSystem() :coord_name(""), coord_system_content(""), coord_definition(""){}
};

class CProfile : public Database, public TSingleton<CProfile>
{
public:
    CProfile(void);
    virtual ~CProfile(void) {};

    virtual void start();
    virtual void exit();

    QMap<QString, AccountItem> get_accounts();
    void insert_account(const AccountItem& item);
    void remove_account(const QString& name);

    // projconfig
    QList<ProjectConfigItem*> get_proj_config(qint64 userId);
    void insert_proj_config(ProjectConfigItem* item);
    void update_proj_config(ProjectConfigItem* item);
    void remove_proj_config(ProjectConfigItem* item);
    bool is_proj_config_exist(qint64 userId, const QString& proj_name);
    QString get_proj_download_path(qint64 userId, const QString& proj_name);

    // cg software
    QString getCgSoftwarePath(qint32 cgId);
    void insertCgSoftwarePath(CgSoftwareItem item);
    void updateCgSofwarePath(qint32 cgId, const QString& installDir);
    CgSoftwareItem* getCgSoftwareItem(qint32 cgId);
    
    // cg install path

    // user coord system
    QString getUserCoordSystemContent(const QString& name);
    void insertUserCoordSystem(const UserCoordSystem& coordSystem);
    void updateUserCoordSystem(const QString& name, const UserCoordSystem& coordSystem);
    UserCoordSystem getUserCoordSystem(const QString& name);
    QList<UserCoordSystem> getUserCoordSystems();

private:
    virtual void build_table();
    virtual void read_table();
    virtual void build_table_common();

private:
    void read_table_account();
    void read_table_cg_software();
    void read_table_proj_config();
    void remove_proj_config_from_local(ProjectConfigItem* item);
    void read_table_cg_install_path();
    void read_user_coord_system();

private:
    QMap<QString, AccountItem> m_tabAccount;
    QVector<CgSoftwareItem> m_tabCgSoftware;
    QMap<qint64, QMap<QString, ProjectConfigItem*>> m_tabProjConfigs;
    QMap<QString, UserCoordSystem> m_userCoordSystem;
};

#endif // CPROFILE_H
