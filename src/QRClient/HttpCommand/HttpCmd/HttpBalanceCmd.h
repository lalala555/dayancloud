#ifndef HTTPBALANCECMD_H
#define HTTPBALANCECMD_H

#include "HttpCommand/HttpCmdRequest.h"

class HttpBalanceCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpBalanceCmd(QObject *parent = 0);
    ~HttpBalanceCmd() {};
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);
};

#endif // HTTPBALANCECMD_H
