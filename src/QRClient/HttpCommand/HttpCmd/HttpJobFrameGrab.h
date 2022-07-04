#ifndef HTTPJOBFRAMEGRAB_H
#define HTTPJOBFRAMEGRAB_H

#include <QObject>
#include "HttpCommand/HttpCmdRequest.h"

class HttpJobFrameGrab : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpJobFrameGrab(qint32 frameId, QObject *parent = 0);
    ~HttpJobFrameGrab();
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    qint32  m_frameId;
};


class HttpJobFrameRenderingTime : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpJobFrameRenderingTime(qint32 frameId, QObject *parent = 0);
    ~HttpJobFrameRenderingTime();
    QByteArray serializeParams();

    private slots:
    void analysisResponsed(const QByteArray& data);

private:
    qint32  m_frameId;
};

#endif // HTTPJOBFRAMEGRAB_H
