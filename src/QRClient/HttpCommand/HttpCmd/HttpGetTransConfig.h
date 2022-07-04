#ifndef HttpGetTransConfig_H
#define HttpGetTransConfig_H

#include "HttpCommand/HttpCmdRequest.h"

class HttpGetTransConfig : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpGetTransConfig(QObject *parent = 0);
    ~HttpGetTransConfig() {};
    QByteArray serializeParams();
private slots:
    void analysisResponsed(const QByteArray& data);
};

#endif // HttpGetTransConfig_H
