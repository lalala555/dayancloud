#include "stdafx.h"
#include "Config/UserProfile/ConfigDb.h"
#include "Config/UserProfile/LocalConfig.h"

const std::string sql_table_account = "CREATE TABLE IF NOT EXISTS [t_account]( "
                                "[name] VARCHAR NOT NULL UNIQUE ON CONFLICT REPLACE, "
                                "[password] VARCHAR, "
                                "[language] INT NOT NULL DEFAULT 0, "
                                "[auto_login_flag] BOOL DEFAULT 0, "
                                "[save_password_flag] BOOL DEFAULT 0, "
                                "[last_login_time] DATETIME NOT NULL DEFAULT (DATETIME ('now', 'localtime')), "
                                "[head_logo] BLOB);";

const std::string sql_table_cg_software = "CREATE TABLE IF NOT EXISTS [t_cg_software]( "
                                    "[cg_id] INT NOT NULL, "
                                    "[cg_name] VARCHAR(64) NOT NULL, "
                                    "[installed_dir] VARCHAR(1024) NOT NULL); ";

const std::string sql_table_project_config = "CREATE TABLE IF NOT EXISTS [t_project]( "
                                    "[user_id] INT NOT NULL, "
                                    "[proj_id] INT NOT NULL, "
                                    "[proj_name] VARCHAR(64) NOT NULL, "
                                    "[download_path] VARCHAR(1024) NOT NULL, "
                                    "[create_time] DATETIME NOT NULL DEFAULT (DATETIME ('now', 'localtime')), "
                                    "[modify_time] VARCHAR(64) NOT NULL, "
                                    "[unique_code] VARCHAR NOT NULL UNIQUE ON CONFLICT REPLACE);";

const std::string sql_table_cg_install_path = "CREATE TABLE IF NOT EXISTS [t_cg_install_path]( "
                                    "[cg_id] INT NOT NULL, "
                                    "[cg_name] VARCHAR(64) NOT NULL, "
                                    "[cg_version] VARCHAR(64) NOT NULL, "
                                    "[edit_name] VARCHAR(64) NOT NULL, "
                                    "[installed_dir] VARCHAR(1024) NOT NULL); ";

const std::string sql_user_coord_system = "CREATE TABLE IF NOT EXISTS [t_user_coord_system]( "
                                    "[id] INTEGER PRIMARY KEY AUTOINCREMENT,"
                                    "[coord_name] VARCHAR(256) NOT NULL,"
                                    "[coord_definition] VARCHAR(1024) NOT NULL,"
                                    "[coord_system_content] VARCHAR(4096) NOT NULL,"
                                    "[create_time] DATETIME NOT NULL DEFAULT (DATETIME ('now', 'localtime')), "
                                    "[modify_time] VARCHAR(64) NOT NULL); ";

CProfile::CProfile(void)
{
}

void CProfile::start()
{
    QString appdata_local_path = RBHelper::getAppDataLocalPath();
    QString db_path = appdata_local_path.append("/Profile.dat");
    Database::init(db_path.toStdString());
    this->build_table();
}

void CProfile::exit()
{
    Database::exit();
}

void CProfile::build_table()
{
    try {
        SQLiteCommand cmd(*m_conn);
        cmd.setCommandText(sql_table_account);
        cmd.executeNonQuery();
        cmd.setCommandText(sql_table_cg_software);
        cmd.executeNonQuery();
        cmd.setCommandText(sql_table_project_config);
        cmd.executeNonQuery();
        cmd.setCommandText(sql_table_cg_install_path);
        cmd.executeNonQuery();
        cmd.setCommandText(sql_user_coord_system);
        cmd.executeNonQuery();
    } catch(SQLiteException& e) {
        std::cerr << e.what() << std::endl;
    }
}

void CProfile::read_table()
{
    read_table_account();
    read_table_cg_software();
    read_table_proj_config();
    read_user_coord_system();
}

QMap<QString, AccountItem> CProfile::get_accounts()
{
    return m_tabAccount;
}

void CProfile::insert_account(const AccountItem& item)
{
    SQLiteTransaction trans = m_conn->beginTransaction();
    try {

        SQLiteCommand cmd(*m_conn);

        cmd.setCommandText("REPLACE INTO [t_account](name, password, language, auto_login_flag, save_password_flag) VALUES (?,?,?,?,?);");
        SQLiteParameter param1 = cmd.createParameter();
        param1.set(1, item.name);
        SQLiteParameter param2 = cmd.createParameter();
        param2.set(2, CToken::SimpleEncrypt(item.password.c_str()));
        SQLiteParameter param3 = cmd.createParameter();
        param3.set(3, item.language);
        SQLiteParameter param4 = cmd.createParameter();
        param4.set(4, item.auto_login_flag);
        SQLiteParameter param5 = cmd.createParameter();
        param5.set(5, item.save_password_flag);

        cmd.executeNonQuery();
        trans.commit();
    } catch(SQLiteException& e) {
        trans.rollback();
        std::cerr << e.what() << std::endl;
    }
}

void CProfile::remove_account(const QString& name)
{
    std::string user_name = name.toLocal8Bit().constData();
    try {
        std::ostringstream oss;
        oss << "DELETE FROM [t_account] WHERE name='" << user_name << "'";
        std::string sql = oss.str();

        SQLiteCommand cmd(*m_conn);
        cmd.setCommandText(sql);

        cmd.executeNonQuery();

        m_tabAccount.remove(name);
    } catch(SQLiteException& e) {
        std::cerr << e.what() << std::endl;
    }
}

void CProfile::build_table_common()
{
    SQLiteCommand cmd(*m_conn);
    std::string sql;

    sql = "INSERT INTO [t_common]([name], [value], [type]) VALUES ('language', '0', 'int');";
    cmd.setCommandText(sql);
    cmd.executeNonQuery();

    sql = "INSERT INTO [t_common]([name], [value], [type]) VALUES ('default_platform', '2', 'int');";
    cmd.setCommandText(sql);
    cmd.executeNonQuery();

    QString proj_dir = RBHelper::getDefaultProjectDir();
    QString qsql = QString("INSERT INTO [t_common]([name], [value], [type]) VALUES ('project_dir', '%1', 'char');").arg(proj_dir);
    sql = qsql.toStdString(); // todo c:/renderfarm/project/
    cmd.setCommandText(sql);
    cmd.executeNonQuery();
}

void CProfile::read_table_account()
{
    if(m_conn == 0) return;

    m_tabAccount.clear();

    try {
        std::string sql = "select * from t_account";

        SQLiteCommand cmd(*m_conn);
        cmd.setCommandText(sql);
        SQLiteDataReader dr = cmd.executeReader();

        AccountItem item;
        while(dr.read()) {
            for(int i = 0; i < dr.getFieldCount(); i++) {
                if(!dr.isDbNull(i)) {
                    switch(i) {
                    case 0:
                        item.name = dr.getString(i);
                        break;
                    case 1:
                        item.password = CToken::SimpleDecrypt(dr.getString(i).c_str());
                        break;
                    case 2:
                        item.language = dr.getInt32(i);
                        break;
                    case 3:
                        item.auto_login_flag = dr.getInt32(i);
                        break;
                    case 4:
                        item.save_password_flag = dr.getInt32(i);
                        break;
                    case 5:
                        item.last_login_time = dr.getString(i);
                        break;
                    }
                }
            }
            m_tabAccount[QString::fromStdString(item.name)] = item;
        }
    } catch(SQLiteException& e) {
        std::cerr << e.what() << std::endl;
    }
}

void CProfile::read_table_cg_software()
{
    if(m_conn == 0) return;

    m_tabCgSoftware.clear();

    try {
        std::string sql = "select * from t_cg_software";

        SQLiteCommand cmd(*m_conn);
        cmd.setCommandText(sql);
        SQLiteDataReader dr = cmd.executeReader();

        CgSoftwareItem item;

        while(dr.read()) {
            for(int i = 0; i < dr.getFieldCount(); i++) {
                if(!dr.isDbNull(i)) {
                    switch(i) {
                    case 0:
                        item.cg_id = dr.getInt32(i);
                        break;
                    case 1:
                        item.cg_name = dr.getString(i);
                        break;
                    case 2:
                        item.installed_dir = dr.getString(i);
                        break;
                    }
                }
            }
            m_tabCgSoftware.push_back(item);
        }
    } catch(SQLiteException& e) {
        std::cerr << e.what() << std::endl;
    }
}

// projconfig
void CProfile::read_table_proj_config()
{
    if (m_conn == 0) return;

    m_tabProjConfigs.clear();

    try {
        std::string sql = "select * from t_project";

        SQLiteCommand cmd(*m_conn);
        cmd.setCommandText(sql);
        SQLiteDataReader dr = cmd.executeReader();

        while (dr.read()) {
            ProjectConfigItem *item = new ProjectConfigItem;

            for (int i = 0; i < dr.getFieldCount(); i++) {
                if (!dr.isDbNull(i)) {
                    switch (i) {
                    case 0:
                        item->user_id = dr.getInt64(i);
                        break;
                    case 1:
                        item->proj_id = dr.getInt32(i);
                        break;
                    case 2:
                        item->proj_name = dr.getString(i);
                        break;
                    case 3:
                        item->download_path = dr.getString(i);
                        break;
                    case 4:
                        item->create_time = dr.getString(i);
                        break;
                    case 5:
                        item->modify_time = dr.getString(i);
                        break;
                    case 6:
                        item->unique_code = dr.getString(i);
                        break;
                    }
                }
            }
            m_tabProjConfigs[item->user_id].insert(QString::fromStdString(item->unique_code), item);
        }
    }
    catch (SQLiteException& e) {
        std::cerr << e.what() << std::endl;
    }
}

QList<ProjectConfigItem*> CProfile::get_proj_config(qint64 userId)
{
    auto it = m_tabProjConfigs.find(userId);
    if(it != m_tabProjConfigs.end()){
        return it.value().values();
    }
    return QList<ProjectConfigItem*>();
}

void CProfile::insert_proj_config(ProjectConfigItem* item)
{
    SQLiteTransaction trans = m_conn->beginTransaction();
    try {

        SQLiteCommand cmd(*m_conn);

        cmd.setCommandText("REPLACE INTO [t_project](user_id, proj_id, proj_name, download_path, create_time, modify_time, unique_code) VALUES (?,?,?,?,?,?,?);");
        SQLiteParameter param1 = cmd.createParameter();
        param1.set(1, item->user_id);
        SQLiteParameter param2 = cmd.createParameter();
        param2.set(2, item->proj_id);
        SQLiteParameter param3 = cmd.createParameter();
        param3.set(3, item->proj_name);
        SQLiteParameter param4 = cmd.createParameter();
        param4.set(4, item->download_path);
        SQLiteParameter param5 = cmd.createParameter();
        param5.set(5, item->create_time);
        SQLiteParameter param6 = cmd.createParameter();
        param6.set(6, item->modify_time);
        SQLiteParameter param7 = cmd.createParameter();
        param7.set(7, item->unique_code);

        cmd.executeNonQuery();
        trans.commit();

        m_tabProjConfigs[item->user_id][QString::fromStdString(item->unique_code)] = item;
    }
    catch (SQLiteException& e) {
        trans.rollback();
        std::cerr << e.what() << std::endl;
    }
}

void CProfile::update_proj_config(ProjectConfigItem* item)
{
    insert_proj_config(item);
}

bool CProfile::is_proj_config_exist(qint64 userId, const QString& proj_name)
{
    auto it = m_tabProjConfigs.find(userId);
    if (it == m_tabProjConfigs.end())
        return false;

    foreach(ProjectConfigItem* item, it.value()) {
        if (item->proj_name == proj_name.toStdString() ||
            item->last_proj_name == proj_name.toStdString()) {
            return true;
        }
    }
    return false;
}

QString CProfile::get_proj_download_path(qint64 userId, const QString& proj_name)
{
    auto it = m_tabProjConfigs.find(userId);
    if (it == m_tabProjConfigs.end())
        return "";

    foreach(ProjectConfigItem* item, it.value()) {
        if (item->proj_name == proj_name.toStdString()) {
            return QString::fromStdString(item->download_path);
        }
    }
    return "";
}

void CProfile::remove_proj_config(ProjectConfigItem* cfg)
{
    if (cfg == NULL) return;
    std::string name = cfg->proj_name;
    try {
        SQLiteTransaction trans = m_conn->beginTransaction();
        SQLiteCommand cmd(*m_conn);

        char sql[4096] = { 0 };
        sprintf(sql, "delete from t_project where proj_name=? and user_id=?");

        cmd.setCommandText(sql);

        SQLiteParameter param1 = cmd.createParameter();
        param1.set(1, name);
        SQLiteParameter param2 = cmd.createParameter();
        param2.set(2, cfg->user_id);

        cmd.executeNonQuery();
        trans.commit();

        remove_proj_config_from_local(cfg);
    }
    catch (SQLiteException& e) {
        std::cerr << e.what() << std::endl;
    }
}

void CProfile::remove_proj_config_from_local(ProjectConfigItem* cfg)
{
    auto it = m_tabProjConfigs.find(cfg->user_id);
    if (it == m_tabProjConfigs.end())
        return;

    foreach(ProjectConfigItem* item, it.value()) {
        if (item->unique_code == cfg->unique_code) {
            m_tabProjConfigs[item->user_id].remove(QString::fromStdString(item->unique_code));
        }
    }
}

// cg software
CgSoftwareItem* CProfile::getCgSoftwareItem(qint32 cgId)
{
    CgSoftwareItem* item = NULL;
    for (int i = 0; i < m_tabCgSoftware.count(); i++) {
        if (cgId == m_tabCgSoftware[i].cg_id) {
            item = &m_tabCgSoftware[i];
            break;
        }
    }
    return item;
}

QString CProfile::getCgSoftwarePath(qint32 cgId)
{
    if (this->getCgSoftwareItem(cgId) == NULL)
        return "";
    return QString::fromStdString(this->getCgSoftwareItem(cgId)->installed_dir);
}

void CProfile::insertCgSoftwarePath(CgSoftwareItem item)
{
    SQLiteTransaction trans = m_conn->beginTransaction();
    try {

        SQLiteCommand cmd(*m_conn);

        cmd.setCommandText("REPLACE INTO [t_cg_software](cg_id, cg_name, installed_dir) VALUES (?,?,?);");
        SQLiteParameter param1 = cmd.createParameter();
        param1.set(1, item.cg_id);
        SQLiteParameter param2 = cmd.createParameter();
        param2.set(2, item.cg_name);
        SQLiteParameter param3 = cmd.createParameter();
        param3.set(3, item.installed_dir);

        cmd.executeNonQuery();
        trans.commit();

        if (this->getCgSoftwareItem(item.cg_id) != NULL) {
            int index = m_tabCgSoftware.indexOf(item);
            m_tabCgSoftware[index] = item;
        } else {
            m_tabCgSoftware.push_back(item);
        }    
    }
    catch (SQLiteException& e) {
        trans.rollback();
        std::cerr << e.what() << std::endl;
    }
}

void CProfile::updateCgSofwarePath(qint32 cgId, const QString& installDir)
{
    CgSoftwareItem* item = this->getCgSoftwareItem(cgId);
    if (item == NULL) return;
    item->installed_dir = installDir.toStdString();

    SQLiteTransaction trans = m_conn->beginTransaction();
    try {

        SQLiteCommand cmd(*m_conn);

        cmd.setCommandText("update t_cg_software set installed_dir=? where cg_id=?;");
        SQLiteParameter param1 = cmd.createParameter();
        param1.set(1, item->installed_dir);
        SQLiteParameter param2 = cmd.createParameter();
        param2.set(2, item->cg_id);

        cmd.executeNonQuery();
        trans.commit();

        int index = m_tabCgSoftware.indexOf(*item);
        m_tabCgSoftware[index] = *item;
    }
    catch (SQLiteException& e) {
        trans.rollback();
        std::cerr << e.what() << std::endl;
    }
}

void CProfile::read_user_coord_system()
{
    if (m_conn == 0) return;

    m_tabProjConfigs.clear();

    try {
        std::string sql = "select * from t_user_coord_system";

        SQLiteCommand cmd(*m_conn);
        cmd.setCommandText(sql);
        SQLiteDataReader dr = cmd.executeReader();

        while (dr.read()) {
            UserCoordSystem item ;

            for (int i = 0; i < dr.getFieldCount(); i++) {
                if (!dr.isDbNull(i)) {
                    switch (i) {
                    case 0:
                        item.id = dr.getInt64(i);
                        break;
                    case 1:
                        item.coord_name = dr.getString(i);
                        break;
                    case 2:
                        item.coord_definition = dr.getString(i);
                        break;
                    case 3:
                        item.coord_system_content = dr.getString(i);
                        break;
                    case 4:
                        item.create_time = dr.getString(i);
                        break;
                    case 5:
                        item.modify_time = dr.getString(i);
                        break;
                    }
                }
            }
            m_userCoordSystem.insert(QString::fromStdString(item.coord_name), item);
        }
    } catch (SQLiteException& e) {
        std::cerr << e.what() << std::endl;
    }
}

QString CProfile::getUserCoordSystemContent(const QString & name)
{
    return QString::fromStdString(getUserCoordSystem(name).coord_system_content);
}

void CProfile::insertUserCoordSystem(const UserCoordSystem & coordSystem)
{
    SQLiteTransaction trans = m_conn->beginTransaction();
    try {

        SQLiteCommand cmd(*m_conn);

        cmd.setCommandText("REPLACE INTO [t_user_coord_system](coord_name, coord_definition, coord_system_content, create_time, modify_time) VALUES (?,?,?,?,?);");
        SQLiteParameter param1 = cmd.createParameter();
        param1.set(1, coordSystem.coord_name);
        SQLiteParameter param2 = cmd.createParameter();
        param2.set(2, coordSystem.coord_definition);
        SQLiteParameter param3 = cmd.createParameter();
        param3.set(3, coordSystem.coord_system_content);
        SQLiteParameter param4 = cmd.createParameter();
        param4.set(4, coordSystem.create_time);
        SQLiteParameter param5= cmd.createParameter();
        param5.set(5, coordSystem.modify_time);

        cmd.executeNonQuery();
        trans.commit();

        m_userCoordSystem[QString::fromStdString(coordSystem.coord_name)] = coordSystem;
    } catch (SQLiteException& e) {
        trans.rollback();
        std::cerr << e.what() << std::endl;
    }
}

void CProfile::updateUserCoordSystem(const QString & name, const UserCoordSystem & coordSystem)
{
    SQLiteTransaction trans = m_conn->beginTransaction();
    try {

        SQLiteCommand cmd(*m_conn);

        cmd.setCommandText("update t_user_coord_system set coord_system_content=?, coord_definition=?, modify_time=? where coord_name=?;");
        SQLiteParameter param1 = cmd.createParameter();
        param1.set(1, coordSystem.coord_system_content);
        SQLiteParameter param2 = cmd.createParameter();
        param2.set(2, coordSystem.modify_time);
        SQLiteParameter param3 = cmd.createParameter();
        param3.set(3, coordSystem.coord_definition);
        SQLiteParameter param4 = cmd.createParameter();
        param4.set(4, name.toStdString());

        cmd.executeNonQuery();
        trans.commit();

        m_userCoordSystem[QString::fromStdString(coordSystem.coord_name)] = coordSystem;

    } catch (SQLiteException& e) {
        trans.rollback();
        std::cerr << e.what() << std::endl;
    }
}

UserCoordSystem CProfile::getUserCoordSystem(const QString & name)
{
    UserCoordSystem coord;
    auto it = m_userCoordSystem.begin();
    while (it != m_userCoordSystem.end()) {
        if (it.value().coord_name == name.toStdString()
            || it.value().coord_definition == name.toStdString()) {
            coord = it.value();
            break;
        }       
        ++it;
    }
    
    return coord;
}

QList<UserCoordSystem> CProfile::getUserCoordSystems()
{
    return m_userCoordSystem.values();
}