#ifndef HTTPCOMMONREQUESTCMD_H
#define HTTPCOMMONREQUESTCMD_H

#include <QObject>
#include "HttpCommand/HttpCmdRequest.h"

class HttpCommonRequestCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpCommonRequestCmd(QObject *parent = 0);
    ~HttpCommonRequestCmd() {};
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:

};

#endif // HTTPCOMMONREQUESTCMD_H
