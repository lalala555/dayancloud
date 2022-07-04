#ifndef HTTPBINDWXTOUSER_H
#define HTTPBINDWXTOUSER_H

#include "HttpCommand/HttpCmdRequest.h"

class HttpBindWxToUser : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpBindWxToUser(qint64 wxId, qint32 bindtype, QObject *parent = 0);
    ~HttpBindWxToUser();
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    qint64 m_wxId;
    qint32 m_bindtype;
};

#endif // HTTPBINDWXTOUSER_H
