#ifndef HTTPLOADINGTASKPATAMTER_H
#define HTTPLOADINGTASKPATAMTER_H

#include "HttpCommand/HttpCmdRequest.h"

class HttpLoadingTaskPatamter : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpLoadingTaskPatamter(qint64 taskId, QObject *parent = 0);
    ~HttpLoadingTaskPatamter();
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    qint64 m_taskId;
};

#endif // HTTPLOADINGTASKPATAMTER_H
