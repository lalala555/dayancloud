#pragma once

#include "HttpCommand/HttpCmdRequest.h"

class HttpGetNodeDetailsCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpGetNodeDetailsCmd(qint64 frameId, QObject *parent = 0);
    ~HttpGetNodeDetailsCmd();
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    qint64 m_frameId;
};
