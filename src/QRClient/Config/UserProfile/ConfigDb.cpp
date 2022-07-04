#include "stdafx.h"
#include "Config/UserProfile/ConfigDb.h"
#include "Config/UserProfile/LocalConfig.h"

const std::string sql_table_schedule = "CREATE TABLE IF NOT EXISTS [t_schedule]( "
                                 "[id] INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                                 "[operation] INT NOT NULL, "
                                 "[starttime] CHAR(32) NOT NULL, "
                                 "[endtime] CHAR(32) NOT NULL); ";

const std::string sql_table_cg_config = "CREATE TABLE IF NOT EXISTS [t_cg_config]( "
                                  "[cgid] INT PRIMARY KEY ON CONFLICT IGNORE NOT NULL ON CONFLICT IGNORE, "
                                  "[cfg_name] VARCHAR(512), "
                                  "[project_output] VARCHAR(2048),"
                                  "[project_dir] VARCHAR(2048));";  // todo project_output = c:/Renderfarm/Download

const std::string sql_table_device = "CREATE TABLE IF NOT EXISTS [t_device]( "
                               "[user_name] CHAR(128) PRIMARY KEY ON CONFLICT IGNORE NOT NULL UNIQUE ON CONFLICT IGNORE, "
                               "[access_id] CHAR(128));";

CConfigDb::CConfigDb(void)
{
    m_strSocialProxyName = "";
}

void CConfigDb::start()
{
    QString userprofile_path = RBHelper::getUserProfilePath();
    QString db_path = userprofile_path.append("/Preferences.dat");
    Database::init(db_path.toStdString());
}

void CConfigDb::exit()
{
    m_tabSchedule.clear();
    m_tabCGConfig.clear();
    m_tabDevice.clear();

    Database::exit();
}

void CConfigDb::build_table()
{
    try {
        SQLiteCommand cmd(*m_conn);
        cmd.setCommandText(sql_table_schedule);
        cmd.executeNonQuery();
        cmd.setCommandText(sql_table_cg_config);
        cmd.executeNonQuery();
        cmd.setCommandText(sql_table_device);
        cmd.executeNonQuery();
    } catch(SQLiteException& e) {
        LOGFMTE("CConfigDb::create table error! [%s]", e.what());
    }
}

void CConfigDb::read_table()
{
    read_table_cg_config();
    read_table_schedule();
    read_device_auth();
}

void CConfigDb::build_table_common()
{
    SQLiteCommand cmd(*m_conn);
    std::string sql;

    try {
        sql = "INSERT INTO [t_common]([name], [value], [type]) VALUES ('auto_system_startup', '0', 'bool');";
        cmd.setCommandText(sql);
        cmd.executeNonQuery();
        sql = "INSERT INTO [t_common]([name], [value], [type]) VALUES ('auto_update_check', '1', 'bool');";
        cmd.setCommandText(sql);
        cmd.executeNonQuery();
        sql = "INSERT INTO [t_common]([name], [value], [type]) VALUES ('auto_download', '1', 'bool');";
        cmd.setCommandText(sql);
        cmd.executeNonQuery();
        sql = "INSERT INTO [t_common]([name], [value], [type]) VALUES ('enable_set_param', '1', 'bool');";
        cmd.setCommandText(sql);
        cmd.executeNonQuery();
        sql = "INSERT INTO [t_common]([name], [value], [type]) VALUES ('plugin_config', '', 'char');";
        cmd.setCommandText(sql);
        cmd.executeNonQuery();
        sql = "INSERT INTO [t_common]([name], [value], [type]) VALUES ('popup_set_project', '0', 'int');";
        cmd.setCommandText(sql);
        cmd.executeNonQuery();
        sql = "INSERT INTO [t_common]([name], [value], [type]) VALUES ('project_id', '0', 'int');";
        cmd.setCommandText(sql);
        cmd.executeNonQuery();
        sql = "INSERT INTO [t_common]([name], [value], [type]) VALUES ('project_name', '', 'char');";
        cmd.setCommandText(sql);
        cmd.executeNonQuery();
        sql = "INSERT INTO [t_common]([name], [value], [type]) VALUES ('frame_timeout', '24', 'int');";
        cmd.setCommandText(sql);
        cmd.executeNonQuery();
        sql = "INSERT INTO [t_common]([name], [value], [type]) VALUES ('render_timeout', '20', 'int');";
        cmd.setCommandText(sql);
        cmd.executeNonQuery();
        sql = "INSERT INTO [t_common]([name], [value], [type]) VALUES ('script_count', '1', 'int');";
        cmd.setCommandText(sql);
        cmd.executeNonQuery();
        sql = "INSERT INTO [t_common]([name], [value], [type]) VALUES ('script_timeout', '18000', 'int');";
        cmd.setCommandText(sql);
        cmd.executeNonQuery();
        sql = "INSERT INTO [t_common]([name], [value], [type]) VALUES ('cg_id', '', 'int');";
        cmd.setCommandText(sql);
        cmd.executeNonQuery();
        sql = "INSERT INTO [t_common]([name], [value], [type]) VALUES ('job_show_days', '30', 'int');";
        cmd.setCommandText(sql);
        cmd.executeNonQuery();
        sql = "INSERT INTO [t_common]([name], [value], [type]) VALUES ('upload_show_days', '30', 'int');";
        cmd.setCommandText(sql);
        cmd.executeNonQuery();
        sql = "INSERT INTO [t_common]([name], [value], [type]) VALUES ('download_show_days', '30', 'int');";
        cmd.setCommandText(sql);
        cmd.executeNonQuery();
        sql = "INSERT INTO [t_common]([name], [value], [type]) VALUES ('save_path', '', 'char');";  // todo c:/Renderfarm/Download/
        cmd.setCommandText(sql);
        cmd.executeNonQuery();
        sql = "INSERT INTO [t_common]([name], [value], [type]) VALUES ('enable_proj_download_path', '0', 'bool');";
        cmd.setCommandText(sql);
        cmd.executeNonQuery();
        sql = "INSERT INTO [t_common]([name], [value], [type]) VALUES ('updated_download_list', '0', 'bool');";
        cmd.setCommandText(sql);
        cmd.executeNonQuery();
        sql = "INSERT INTO [t_common]([name], [value], [type]) VALUES ('frame_timeout_s', '86400', 'int');";
        cmd.setCommandText(sql);
        cmd.executeNonQuery();
        sql = "INSERT INTO [t_common]([name], [value], [type]) VALUES ('render_timeout_s', '86400', 'int');";
        cmd.setCommandText(sql);
        cmd.executeNonQuery();
    } catch(SQLiteException& e) {
        LOGFMTE("CConfigDb::create table data error! [%s]", e.what());
    }
}

void CConfigDb::read_table_cg_config()
{
    if(m_conn == 0) return;

    m_tabCGConfig.clear();
    try {
        SQLiteCommand cmd(*m_conn);

        std::string sql = "select * from t_cg_config";
        cmd.setCommandText(sql);
        SQLiteDataReader dr = cmd.executeReader();

        CGConfigItem item;
        int index = 0;
        while(dr.read()) {
            for(int i = 0; i < dr.getFieldCount(); i++) {
                if(!dr.isDbNull(i)) {
                    switch(i) {
                    case 0:
                        item.cgid = dr.getInt32(i);
                        break;
                    case 1:
                        item.cfg_name = dr.getString(i);
                        break;
                    case 2:
                        item.project_output = dr.getString(i);
                        break;
                    case 3:
                        item.project_dir = dr.getString(i);
                        break;
                    }
                }
            }
            item.idx = index;

            m_tabCGConfig.push_back(item);
            index ++;
        }
    } catch(SQLiteException& e) {
        LOGFMTE("CConfigDb::read table data error! [%s]", e.what());
    }
}

void CConfigDb::read_table_schedule()
{
    if(m_conn == 0) return;

    m_tabSchedule.clear();
    try {
        SQLiteCommand cmd(*m_conn);

        std::string sql = "select * from t_schedule";
        cmd.setCommandText(sql);
        SQLiteDataReader dr = cmd.executeReader();

        ScheduleItem item;

        while(dr.read()) {
            for(int i = 0; i < dr.getFieldCount(); i++) {
                if(!dr.isDbNull(i)) {
                    switch(i) {
                    case 0:
                        item.idx = dr.getInt32(i);
                        break;
                    case 1:
                        item.action = dr.getInt32(i);
                        break;
                    case 2:
                        item.startTime = dr.getString(i);
                        break;
                    case 3:
                        item.endTime = dr.getString(i);
                        break;
                    }
                }
            }
            m_tabSchedule.push_back(item);
        }
    } catch(SQLiteException& e) {
        LOGFMTE("CConfigDb::read table data error! [%s]", e.what());
    }
}

void CConfigDb::read_device_auth()
{
    if(m_conn == 0) return;

    m_tabDevice.clear();
    try {
        SQLiteCommand cmd(*m_conn);

        std::string sql = "select * from t_device";
        cmd.setCommandText(sql);
        SQLiteDataReader dr = cmd.executeReader();

        std::string userName, accessId;

        while(dr.read()) {
            for(int i = 0; i < dr.getFieldCount(); i++) {
                if(!dr.isDbNull(i)) {
                    switch(i) {
                    case 0:
                        userName = dr.getString(i);
                        break;
                    case 1:
                        accessId = dr.getString(i);
                        break;
                    }
                }
            }

            m_tabDevice[userName] = accessId;
        }
    } catch(SQLiteException& e) {
        LOGFMTE("CConfigDb::read table data error! [%s]", e.what());
    }
}

QString CConfigDb::getHost()
{
    return m_hostUrl;
}

void CConfigDb::setHost(const QString & host)
{
    m_hostUrl = host;
}

std::vector<ScheduleItem> CConfigDb::getSchedules()
{
    return m_tabSchedule;
}

bool CConfigDb::addSchedule(int action, const std::string& startTime, const std::string& endTime)
{
    ScheduleItem tSchedule;
    tSchedule.action = action;
    tSchedule.startTime = startTime;
    tSchedule.endTime = endTime;

    // 写入数据库
    try {
        SQLiteTransaction trans = m_conn->beginTransaction();
        SQLiteCommand cmd(*m_conn);

        cmd.setCommandText("insert into t_schedule(operation,starttime,endtime)values(?,?,?)");
        SQLiteParameter param1 = cmd.createParameter();
        param1.set(1, tSchedule.action);
        SQLiteParameter param2 = cmd.createParameter();
        param2.set(2, tSchedule.startTime.c_str());
        SQLiteParameter param3 = cmd.createParameter();
        param3.set(3, tSchedule.endTime.c_str());
        cmd.executeNonQuery();
        trans.commit();
    } catch(SQLiteException& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    m_tabSchedule.push_back(tSchedule);

    return true;
}

bool CConfigDb::removeSchedule(int action, const std::string& startTime, const std::string& endTime)
{
    for(auto itor = m_tabSchedule.begin(); itor != m_tabSchedule.end(); ++itor) {
        if(m_conn) {
            if(itor->action == action && itor->startTime == startTime && itor->endTime == endTime) {
                try {
                    SQLiteCommand cmd(*m_conn);
                    char sql[512] = { 0 };
                    sprintf(sql, "delete from t_schedule where operation=%d and starttime='%s' and endtime='%s'",
                              action,
                              startTime.c_str(),
                              endTime.c_str());
                    cmd.setCommandText(sql);

                    cmd.executeNonQuery();
                } catch(SQLiteException& e) {
                    std::cerr << e.what() << std::endl;
                    return false;
                }

                m_tabSchedule.erase(itor);

                return true;
            }
        }
    }

    return false;
}

CGConfigItem CConfigDb::getCGConfig(int cgid)
{
    CGConfigItem item;
    for (int i = 0; i < m_tabCGConfig.size(); i++) {
        if(m_tabCGConfig[i].cgid == cgid) {
            item = m_tabCGConfig[i];
            return item;
        }
    }
    // 根据系统来设定默认下载路径
    if(item.project_output.empty()){
        item.project_output = RBHelper::getDefaultDownloadDir().toStdString();
    }    
    return item;
}

void CConfigDb::updateCGConfig(const CGConfigItem& item)
{
    for (int i = 0; i < m_tabCGConfig.size(); i++) {
        CGConfigItem cfg = m_tabCGConfig.at(i);
        if(item.cgid == cfg.cgid) {
            m_tabCGConfig[i] = item;
            return;
        }
    }
    m_tabCGConfig.push_back(item); // 删除情况要重新添加
}

bool CConfigDb::addOrUpdateCGConfig(const CGConfigItem& item)
{
    // 写入数据库
    if(m_conn) {
        try {
            SQLiteTransaction trans = m_conn->beginTransaction();
            SQLiteCommand cmd(*m_conn);

            cmd.setCommandText("replace into t_cg_config(cgid,cfg_name,project_output,project_dir)values(?,?,?,?)");
            SQLiteParameter param1 = cmd.createParameter();
            param1.set(1, item.cgid);
            SQLiteParameter param2 = cmd.createParameter();
            param2.set(2, item.cfg_name.c_str());
            SQLiteParameter param3 = cmd.createParameter();
            param3.set(3, item.project_output.c_str());
            SQLiteParameter param4 = cmd.createParameter();
            param4.set(4, item.project_dir.c_str());
            cmd.executeNonQuery();
            trans.commit();
        } catch(SQLiteException& e) {
            std::cerr << e.what() << std::endl;
            return false;
        }
    }
    return true;
}

bool CConfigDb::addCGConfig(int cgid, const std::string& cfgName, const std::string& output, const std::string& dir)
{
    CGConfigItem tCGItem = this->getCGConfig(cgid);
    tCGItem.cgid = cgid;
    tCGItem.cfg_name = cfgName;
    tCGItem.project_output = output;
    tCGItem.project_dir = dir;

    bool isSuccess = this->addOrUpdateCGConfig(tCGItem);
    if(isSuccess) {
        this->updateCGConfig(tCGItem);
    }

    return true;
}

bool CConfigDb::updateCGConfig(int cgid, const std::string& cfgName, const std::string& output, const std::string& dir)
{
    CGConfigItem tCGItem = this->getCGConfig(cgid);
    tCGItem.cgid = cgid;
    tCGItem.cfg_name = cfgName;
    tCGItem.project_output = output;
    tCGItem.project_dir = dir;

    bool isSuccess = this->addOrUpdateCGConfig(tCGItem);
    if(isSuccess) {
        this->updateCGConfig(tCGItem);
    }

    return true;
}

bool CConfigDb::removeCGConfig(int cgid)
{
    for(auto itor = m_tabCGConfig.begin(); itor != m_tabCGConfig.end(); ++itor) {
        if(m_conn) {
            if(itor->cgid == cgid) {
                try {
                    SQLiteCommand cmd(*m_conn);
                    char sql[512] = { 0 };
                    sprintf(sql, "delete from t_cg_config where cgid=%d", cgid);
                    cmd.setCommandText(sql);

                    cmd.executeNonQuery();
                } catch(SQLiteException& e) {
                    std::cerr << e.what() << std::endl;
                    return false;
                }

                m_tabCGConfig.erase(itor);

                return true;
            }
        }
    }
    return false;
}

std::string CConfigDb::getAccessId(const std::string& userName)
{
    auto it = m_tabDevice.find(userName);
    if(it != m_tabDevice.end())
        return it->second;

    return "";
}

bool CConfigDb::setAccessId(const std::string& userName, const std::string& accessId)
{
    // 写入数据库
    try {
        SQLiteTransaction trans = m_conn->beginTransaction();
        SQLiteCommand cmd(*m_conn);

        cmd.setCommandText("replace into t_device(userName,accessId)values(?,?)");
        SQLiteParameter param1 = cmd.createParameter();
        param1.set(1, userName);
        SQLiteParameter param2 = cmd.createParameter();
        param2.set(2, accessId);
        cmd.executeNonQuery();
        trans.commit();

        m_tabDevice[userName] = accessId;
    } catch(SQLiteException& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    return true;
}

bool CConfigDb::getAutoStartup()
{
    return get_bool_value("auto_system_startup");
}

void CConfigDb::setAutoStartup(bool bauto)
{
    set_bool_value("auto_system_startup", bauto);
}

bool CConfigDb::getAutoDownload()
{
    return get_bool_value("auto_download");
}

void CConfigDb::setAutoDownload(bool bauto)
{
    set_bool_value("auto_download", bauto);
}

bool CConfigDb::getEnableSetParam()
{
    return get_bool_value("enable_set_param");
}

void CConfigDb::setEnableSetParam(bool benable)
{
    set_bool_value("enable_set_param", benable);
}

bool CConfigDb::getAutoUpdate()
{
    return get_bool_value("auto_update_check");
}

void CConfigDb::setAutoUpdate(bool benable)
{
    set_bool_value("auto_update_check", benable);
}

int CConfigDb::getScriptCount()
{
    int count = get_int_value("script_count");
    if(count < 1) count = 1;
    if(count > 5) count = 5;

    return count;
}

void CConfigDb::setScriptCount(int count)
{
    set_int_value("script_count", count);
}

int CConfigDb::getScriptTimeout()
{
    int timeout = get_int_value("script_timeout");
    int hour = timeout / 3600;
    if(hour <= 0) hour = 1;

    return hour;
}

void CConfigDb::setScriptTimeout(int timeout)
{
    set_int_value("script_timeout", timeout * 3600);
}

int CConfigDb::getRenderTimeoutRemind()
{
    return get_int_value("render_timeout");
}

void CConfigDb::setRenderTimeoutRemind(int timeout)
{
    set_int_value("render_timeout", timeout);
}

int CConfigDb::getFrameTimeoutStop()
{
    return get_int_value("frame_timeout");
}

void CConfigDb::setFrameTimeoutStop(int timeoutStop)
{
    set_int_value("frame_timeout", timeoutStop);
}

int CConfigDb::getPopupSetProject()
{
    return get_int_value("popup_set_project");
}

void CConfigDb::setPopupSetProject(int popup)
{
    set_int_value("popup_set_project", popup);
}

void CConfigDb::setProject(int id, const std::string& name)
{
    set_int_value("project_id", id);
    set_string_value("project_name", name);
}

int CConfigDb::getProjectId()
{
    return get_int_value("project_id");
}

std::string CConfigDb::getProjectName()
{
    return get_string_value("project_name");
}

std::string CConfigDb::getPluginConfig()
{
    return get_string_value("plugin_config");
}

void CConfigDb::setPluginConfig(const std::string& plugin_config)
{
    set_string_value("plugin_config", plugin_config);
}

int CConfigDb::getDefaultCgId()
{
    return get_int_value("cg_id");
}

void CConfigDb::setDefaultCgId(int cgId)
{
    set_int_value("cg_id", cgId);
}

bool CConfigDb::isEnableProjectDownloadPath()
{
    return get_bool_value("enable_proj_download_path");
}

void CConfigDb::setEnableProjectDownloadPath(bool enable)
{
    set_bool_value("enable_proj_download_path", enable);
}

bool CConfigDb::isUpdateDownloadList()
{
    return get_bool_value("updated_download_list");
}

void CConfigDb::setUpdateDownloadList(bool isUpdate)
{
    set_bool_value("updated_download_list", isUpdate);
}

int CConfigDb::getRenderTimeoutRemindSec()
{
    return get_int_value("render_timeout_s");
}

void CConfigDb::setRenderTimeoutRemindSec(int timeout)
{
    set_int_value("render_timeout_s", timeout);
}

int CConfigDb::getFrameTimeoutStopSec()
{
    return get_int_value("frame_timeout_s");
}

void CConfigDb::setFrameTimeoutStopSec(int timeoutStop)
{
    set_int_value("frame_timeout_s", timeoutStop);
}

int CConfigDb::getShowTaskType()
{
    return m_showType;
}

void CConfigDb::setShowTaskType(int nType)
{
    m_showType = nType;
}

QString CConfigDb::getSocialProxyName()
{
    return m_strSocialProxyName;
}

void CConfigDb::setSocialProxyName(QString proxy)
{
    m_strSocialProxyName = proxy;
}
