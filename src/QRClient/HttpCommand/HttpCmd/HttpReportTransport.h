#ifndef HTTPREPORTTRANSPORT_H
#define HTTPREPORTTRANSPORT_H

#include "HttpCommand/HttpCmdRequest.h"

class HttpReportTransport : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpReportTransport(qint64 taskId, const QJsonObject& jsonObject, QObject *parent = 0);
    ~HttpReportTransport() {};
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    qint64 m_taskId;
    QJsonObject m_data;
};

#endif // HTTPREPORTTRANSPORT_H
