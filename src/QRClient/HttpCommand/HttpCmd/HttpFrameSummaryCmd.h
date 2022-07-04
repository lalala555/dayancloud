#ifndef HTTPFRAMESUMMARYCMD_H
#define HTTPFRAMESUMMARYCMD_H

#include <QObject>
#include "HttpCommand/HttpCmdRequest.h"

class HttpFrameSummaryCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpFrameSummaryCmd(QObject *parent = 0);
    ~HttpFrameSummaryCmd() {};
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);
};

class HttpTaskRenderingConsume : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpTaskRenderingConsume(qint64 jobId, QObject *parent = 0);
    ~HttpTaskRenderingConsume() {};
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    qint64 m_jobId;
};

#endif // HTTPFRAMESUMMARYCMD_H
