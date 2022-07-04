#ifndef HTTPGETTRANSBID_H
#define HTTPGETTRANSBID_H

#include "HttpCommand/HttpCmdRequest.h"

class HttpGetTransBid : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpGetTransBid(QObject *parent = 0);
    ~HttpGetTransBid() {};
    QByteArray serializeParams();
private slots:
    void analysisResponsed(const QByteArray& data);
};

#endif // HTTPGETTRANSBID_H
