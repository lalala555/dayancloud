/************************************************************************/
/* 获取用户渲染器插件配置
*  /api/rendering/task/common/getUserPluginConfig
/************************************************************************/

#ifndef HTTPUSERPLUGINCONFIG_H
#define HTTPUSERPLUGINCONFIG_H

#include "HttpCommand/HttpCmdRequest.h"

class HttpUserPluginConfig : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpUserPluginConfig(QList<qint64> cgIds, int osName, QObject *parent = 0);
    ~HttpUserPluginConfig();
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    QList<qint64> m_cgIds;
    int m_osName;
};

#endif // HTTPUSERPLUGINCONFIG_H
