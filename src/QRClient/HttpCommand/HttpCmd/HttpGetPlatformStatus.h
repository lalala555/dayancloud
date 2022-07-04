#pragma once

#include "HttpCommand/HttpCmdRequest.h"

class HttpGetPlatformStatus : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpGetPlatformStatus(qint32 zone, QObject *parent = 0);
    ~HttpGetPlatformStatus();
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    qint32 m_zone;
};
