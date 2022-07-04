#pragma once

#include "HttpCommand/HttpCmdRequest.h"

class HttpReportLogCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpReportLogCmd(qint64 taskId, const QString& filePath, QObject *parent = 0);
    ~HttpReportLogCmd();
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    qint64 m_taskId;
    QString m_filePath;
};
