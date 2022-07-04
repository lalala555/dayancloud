#ifndef HTTPDOWNLOADCOUNTCMD_H
#define HTTPDOWNLOADCOUNTCMD_H

#include "HttpCommand/HttpCmdRequest.h"

class HttpDownloadStartCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpDownloadStartCmd(qint64 taskId, QObject *parent = 0) 
        : HttpCmdRequest(RESTAPI_DOWNLOAD_START, parent), m_taskId(taskId)
    {
    };
    ~HttpDownloadStartCmd() {};
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    qint64 m_taskId;
};

class HttpDownloadCompleteCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpDownloadCompleteCmd(qint64 taskId, const QString& requestId, QObject *parent = 0)
        : HttpCmdRequest(RESTAPI_DOWNLOAD_COMPLETE, parent), m_taskId(taskId), m_requestId(requestId)
    {
    };
    ~HttpDownloadCompleteCmd() {}
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    qint64 m_taskId;
    QString m_requestId;
};

class HttpDownloadCountCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpDownloadCountCmd(QList<qint64> taskIds, QObject *parent = 0)
        : HttpCmdRequest(RESTAPI_DOWNLOAD_COUNT, parent), m_taskIds(taskIds)
    {
    }
    ~HttpDownloadCountCmd() {}
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    QList<qint64> m_taskIds;
};

class HttpDownloadInfoCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpDownloadInfoCmd(qint64 taskId, QObject *parent = 0)
        : HttpCmdRequest(RESTAPI_DOWNLOAD_INFO, parent), m_taskId(taskId)
    {
    };
    ~HttpDownloadInfoCmd() {}
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    qint64 m_taskId;
};

#endif // HTTPDOWNLOADCOUNTCMD_H
