#ifndef HTTPQRCODECMD_H
#define HTTPQRCODECMD_H

#include "HttpCommand/HttpCmdRequest.h"

class HttpQRCodeCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpQRCodeCmd(QObject *parent = 0);
    ~HttpQRCodeCmd() {};
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);
};

#endif // HTTPQRCODECMD_H
