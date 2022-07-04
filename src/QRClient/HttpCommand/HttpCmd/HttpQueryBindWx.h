#ifndef HTTPQUERYBINDWX_H
#define HTTPQUERYBINDWX_H

#include "HttpCommand/HttpCmdRequest.h"

class HttpQueryBindWx : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpQueryBindWx(QObject *parent = 0);
    ~HttpQueryBindWx();
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:

};

#endif // HTTPQUERYBINDWX_H
