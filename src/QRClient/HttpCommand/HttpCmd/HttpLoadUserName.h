#ifndef HTTPLOADUSERNAME_H
#define HTTPLOADUSERNAME_H

#include "HttpCommand/HttpCmdRequest.h"

class HttpLoadUserName : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpLoadUserName(QObject *parent = 0);
    ~HttpLoadUserName();

    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

};

#endif // HTTPLOADUSERNAME_H
