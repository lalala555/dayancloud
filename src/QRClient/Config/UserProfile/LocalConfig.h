/***********************************************************************
* Module:  LocalConfig.h
* Author:  hqr
* Created: 2018/11/01 12:01:37
* Modifier: hqr
* Modified: 2018/11/01 12:01:37
* Purpose: Declaration of the class
***********************************************************************/
#ifndef PERSONAL_CONFIG_H
#define PERSONAL_CONFIG_H

#include <string>

class LocalConfig
{
    LocalConfig();
    ~LocalConfig();
public:
    static LocalConfig* getInstance()
    {
        static LocalConfig instance;
        return &instance;
    }
    void init();
    void uninit();
    void writeServerPort(quint16 port);

public:
    QString defaultProduct;
    QString defaultPartner;
    int defaultLogLevel = LOG_LEVEL_INFO;
    unsigned short defaultCmdPort = 41011;
    unsigned short defaultCmdPortMax;
    int detailInterval = 30 * 1000;
    int requestTimeout;
    int speedTestTimeout;
    int upgradeTimeInterval = 30; // 更新检查间隔min
    int refreshTimeInterval = 2; // 刷新间隔min
    QString baseProduct;

private:
    QString getConfingXml();
    void removeConfigXml();
};

#endif
