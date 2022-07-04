#pragma once

#include "HttpCommand/HttpCmdRequest.h"

class HttpUpdateTaskLimitCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpUpdateTaskLimitCmd(qint64 taskId, const QString& limitCount, QObject *parent = 0);
    ~HttpUpdateTaskLimitCmd();
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    qint64 m_taskId;
    QString m_limitCount;
};
