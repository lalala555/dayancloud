#ifndef HTTPUSERRENDERSETTINGCMD_H
#define HTTPUSERRENDERSETTINGCMD_H

#include "HttpCommand/HttpCmdRequest.h"

class HttpUserRenderSettingCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpUserRenderSettingCmd(QObject *parent = 0);
    ~HttpUserRenderSettingCmd() {};
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);
};

#endif // HTTPUSERRENDERSETTINGCMD_H
