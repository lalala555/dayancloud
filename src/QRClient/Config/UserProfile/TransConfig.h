/********************************************************
 * Description  :
 * Author       : laizhichun
 * Email        : laizhichun@rayvision.com
 * Version      : 1.0
 * Create       : 2019-6-25 15:55:00
 * Modifier     : laizhichun
 * Modify       : 2019-6-25 15:55:00
 * Copyright(C) : RAYVISION
 ********************************************************/
#pragma once
#ifndef TRANS_CONFIG_H
#define TRANS_CONFIG_H


#include <QString>
#include <QVector>
#include <QJsonDocument>

struct TransEngineAccountConfInfo {
    int bid;
    QString name;
    QString password;
};

//Aspera 需要的账户密码信息
struct TransEngineLineConfInfo {
    bool is_default;
    QString name;
    QString server;
    int port;
};

struct TrasnEngineConInfo {
    bool is_default;
    QString engine_name;
    bool automatic_check;
    QVector<TransEngineAccountConfInfo> accounts;
    QVector<TransEngineLineConfInfo> lines;
    QString check_server;
    int check_port;
    bool check_enable;
    QString check_exclud_type;
};

struct TransConfigInfo {
    int config_bid;
    int output_bid;
    int input_bid;
    int p_input_bid;
    QVector<TrasnEngineConInfo> engines;
    QMap<qint64, ChildTransBidInfo> childrenBid;
};

class TransConfig
{
public:
    static TransConfig* getInstance() {
        static TransConfig instance;
        return &instance;
    }
    bool setTransConfig(const QJsonDocument& doc);
    TransConfigInfo getTransConfig();
    bool CheckUserSupportEngine(const QString & engine);
    QString GetUserDefaultEngine();
    QList<QString> GetEngineList();
    bool CheckUserSupportTransLine(const QString & engine, const QString & transLine);
    QString GetDefaultTransLine(const QString & engine, const QString & transLine);
    QString GetNetServerName(const QString & engine, const QString & host);
    QString GetDefaultNetServerName(const QString & engine);
    bool GetCurTransFileCheckHost(const QString & engine, const QString & line, QString & checkHost, quint32 & checkPort, bool & needCheck);
    bool GetCurTransFileNeedCheck(const QString & engine, const QString & line);
    QList<QString> GetSpecialFileType(const QString& engine, const QString& line);
    void setMySettingBid();
private:
    TransConfigInfo m_TransConfig;
};
#endif // !TRANS_CONFIG_H