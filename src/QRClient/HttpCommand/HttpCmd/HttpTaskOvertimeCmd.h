#ifndef HTTPTASKOVERTIMECMD_H
#define HTTPTASKOVERTIMECMD_H

#include "HttpCommand/HttpCmdRequest.h"

class HttpTaskOvertimeCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpTaskOvertimeCmd(qint64 taskId, qint32 overtime, QObject *parent = 0);
    ~HttpTaskOvertimeCmd() {};
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    qint64  m_taskId;
    qint32  m_overtime;
};

class HttpTaskOvertimeStopCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpTaskOvertimeStopCmd(qint64 taskId, qint32 overtime, QObject *parent = 0);
    ~HttpTaskOvertimeStopCmd() {};
    QByteArray serializeParams();

    private slots:
    void analysisResponsed(const QByteArray& data);

private:
    qint64  m_taskId;
    qint32  m_overtime;
};

#endif // HTTPTASKOVERTIMECMD_H
