#ifndef HTTPRECENTTASKCMD_H
#define HTTPRECENTTASKCMD_H

#include <QObject>
#include "HttpCommand/HttpCmdRequest.h"

class HttpRecentTaskCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpRecentTaskCmd(qint64 userId, QObject *parent = 0);
    ~HttpRecentTaskCmd() {};
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    qint64 m_userId;
};

#endif // HTTPRECENTTASKCMD_H
