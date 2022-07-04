#ifndef HTTPPLATFORMCMD_H
#define HTTPPLATFORMCMD_H

#include <QObject>
#include "HttpCommand/HttpCmdRequest.h"

class HttpPlatformCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpPlatformCmd(QObject *parent = 0);
    ~HttpPlatformCmd() {};
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);
};

#endif // HTTPPLATFORMCMD_H
