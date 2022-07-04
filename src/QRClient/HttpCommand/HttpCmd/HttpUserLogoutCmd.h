#ifndef HTTPUSERLOGOUT_H
#define HTTPUSERLOGOUT_H

#include <QObject>
#include "HttpCommand/HttpCmdRequest.h"

class HttpUserLogoutCmd: public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpUserLogoutCmd(QObject *parent = 0);
    ~HttpUserLogoutCmd(){};
    QByteArray serializeParams();

private slots:
    virtual void analysisResponsed(const QByteArray& data);
};

#endif // HTTPUSERLOGOUT_H
