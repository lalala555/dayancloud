#include "stdafx.h"
#include "TransConfig.h"
#include "Config/UserConfig.h"

const QStringList Engines = {"raysync"}; // Ð§¹ûÍ¼Ö»Áôraysync

bool TransConfig::setTransConfig(const QJsonDocument& doc)
{
    QJsonObject rootObj = doc.object();

    TransConfigInfo transConfig;
    auto keys = rootObj.keys();
    if (!keys.contains("inputBid") || !keys.contains("outputBid") || !keys.contains("parentInputBid")
        || !keys.contains("configBid") || !keys.contains("resqEngines")) {
        return false;
    }
    transConfig.config_bid    = rootObj["configBid"].toString().toInt();
    transConfig.input_bid     = rootObj["inputBid"].toString().toInt();
    transConfig.output_bid    = rootObj["outputBid"].toString().toInt();
    transConfig.p_input_bid   = rootObj["parentInputBid"].toString().toInt();
    QJsonArray enginArray = rootObj["resqEngines"].toArray();
    for (int i = 0; i < enginArray.size(); i++) {
        QJsonObject enginObj = enginArray.at(i).toObject();
        TrasnEngineConInfo engin;

        keys = enginObj.keys();
        if (!keys.contains("isDefault") || !keys.contains("engineName")
            || !keys.contains("automaticCheck") || !keys.contains("resqEngineAccounts")
            || !keys.contains("respTaskEngineLines") || !keys.contains("checkServer")
            || !keys.contains("checkPort") || !keys.contains("checkEnable")
            || !keys.contains("checkExcludType") || !keys.contains("resqEngineAccounts")) {
            return false;
        }
        engin.is_default        = enginObj["isDefault"] == 1 ? true : false;
        engin.engine_name       = enginObj["engineName"].toString();
        engin.automatic_check   = enginObj["automaticCheck"] == 1 ? true : false;
        engin.check_server      = enginObj["checkServer"].toString();
        engin.check_port        = enginObj["checkPort"].toString().toInt();
        engin.check_enable      = enginObj["checkEnable"].toString() == "1" ? true : false;
        engin.check_exclud_type = enginObj["checkExcludType"].toString();

        if (!Engines.contains(engin.engine_name.toLower())) {
            continue;
        }

        QJsonArray accountArray = enginObj["resqEngineAccounts"].toArray();
        for (int acc_index = 0; acc_index < accountArray.size(); acc_index++) {
            QJsonObject accountObj = accountArray.at(acc_index).toObject();
            TransEngineAccountConfInfo account;
            if (!accountObj.contains("bid") || !accountObj.contains("name") || !accountObj.contains("password")) {
                continue;
            }
            account.bid         = accountObj["bid"].toString("0").toInt();
            account.name        = accountObj["name"].toString();
            QString password    = accountObj["password"].toString();
            account.password    = AES::decrypt(MyConfig.userSet.userKey.toLatin1(), QByteArray::fromBase64(password.toLatin1()), "rayvision-2019-x");
            engin.accounts.push_back(account);
        }

        QJsonArray lineArray = enginObj["respTaskEngineLines"].toArray();
        for (int line_index = 0; line_index < lineArray.size(); line_index++) {
            QJsonObject lineObj = lineArray.at(line_index).toObject();
            TransEngineLineConfInfo line;
            if (!lineObj.contains("isDefault") || !lineObj.contains("name")
                || !lineObj.contains("server") || !lineObj.contains("port")) {
                continue;
            }
            line.is_default = lineObj["isDefault"] == 1 ? true : false;
            line.name = lineObj["name"].toString();
            line.server = lineObj["server"].toString();
            line.port = lineObj["port"].toString().toInt();
            engin.lines.push_back(line);
        }
        transConfig.engines.push_back(engin);
    }

    if (rootObj["subUserOutputBids"].isArray()) {
        QJsonArray subBids = rootObj["subUserOutputBids"].toArray();
        for (int i = 0; i < subBids.count(); i++) {
            QJsonObject sub = subBids[i].toObject();
            ChildTransBidInfo info;

            info.userId    = sub["userId"].toString();
            info.outputBid = sub["outputBid"].toString();

            transConfig.childrenBid.insert(info.userId.toLongLong(), info);
        }
    }
    m_TransConfig = transConfig;

    setMySettingBid();
    return true;
}

TransConfigInfo TransConfig::getTransConfig()
{
    return m_TransConfig;
}

bool TransConfig::CheckUserSupportEngine(const QString& engine) {
    for (auto i : m_TransConfig.engines) {
        if (i.engine_name == engine)
        {
            return true;
        }
   }
    return false;
}

QString TransConfig::GetUserDefaultEngine() {
    QString name;
    for (auto i : m_TransConfig.engines) {
        name = i.engine_name;
        if (i.is_default) {
            return i.engine_name;
        }
    }
    return name;
}

QList<QString> TransConfig::GetEngineList() {
    QList<QString> lstEngine;
    for (auto i : m_TransConfig.engines) {
       lstEngine.push_back(i.engine_name);
    }
    return lstEngine;
}

bool TransConfig::CheckUserSupportTransLine(const QString& engine, const QString& transLine) 
{
    for (auto i : m_TransConfig.engines) {
        if (i.engine_name == engine) {
            for (auto line : i.lines) {
                if (line.name == transLine) {
                    return true;
                }
            }
        }
    }
    return false;
}

QString TransConfig::GetDefaultTransLine(const QString& engine, const QString& transLine) 
{
    QString trans_line;
    for (auto i : m_TransConfig.engines) {
        if (i.engine_name == engine) {
           
            for (auto line : i.lines) {
               
                if (line.is_default) {
                    return line.name;
                }
                trans_line = line.name;
            }
        }
    }
    return trans_line;
}

QString TransConfig::GetNetServerName(const QString& engine, const QString& host) 
{
    for (auto i : m_TransConfig.engines) {
        if (i.engine_name == engine) {
            for (auto line : i.lines) {
                if (line.name == host) {
                    return line.server;
                }
            }
        }
    }

    return "";
}

QString TransConfig::GetDefaultNetServerName(const QString& engine) 
{
    QString name;
    for (auto i : m_TransConfig.engines) {
        if (i.engine_name == engine) {
            for (auto line : i.lines) {
                if (line.is_default) {
                    return line.server;
                }
                name = line.server;
            }
        }
    }

    return name;
}

bool TransConfig::GetCurTransFileCheckHost(const QString& engine,const QString& line,QString& checkHost,
    quint32& checkPort,bool& needCheck) 
{
    for (auto i : m_TransConfig.engines) {
        if (i.engine_name == engine) {
            checkHost = i.check_server;
            checkPort = i.check_port;
            needCheck = i.check_enable;
        }
    }
    return true;
}


bool TransConfig::GetCurTransFileNeedCheck(const QString& engine, const QString& line) 
{
    bool needCheck = false;
    for (auto i : m_TransConfig.engines) {
        if (i.engine_name == engine) {
            needCheck = i.check_enable;
            break;
        }
    }
    return needCheck;
}

QList<QString> TransConfig::GetSpecialFileType(const QString& engine, const QString& line) 
{
    QList<QString> special;
    for (auto i : m_TransConfig.engines) {
        if (i.engine_name == engine) {
            special = i.check_exclud_type.split(",");//TODO,need confirm
            break;
        }
    }
    return special;
}

void TransConfig::setMySettingBid()
{
    MyConfig.storageSet.commonBid = m_TransConfig.config_bid;
    MyConfig.storageSet.uploadBid = m_TransConfig.input_bid;
    MyConfig.storageSet.downloadBid = m_TransConfig.output_bid;
    MyConfig.storageSet.cloneBid = m_TransConfig.config_bid;
    if (MyConfig.userSet.isChildAccount()) {
        if (MyConfig.accountSet.shareMainCapital) {
            MyConfig.storageSet.uploadBid = m_TransConfig.p_input_bid;
        }
    }
    MyConfig.storageSet.childrenBid = m_TransConfig.childrenBid;
}
