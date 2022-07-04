#ifndef HTTPUSERINFOCMD_H
#define HTTPUSERINFOCMD_H

#include <QObject>
#include "HttpCommand/HttpCmdRequest.h"

class HttpUserInfoCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpUserInfoCmd(QObject *parent = 0);
    ~HttpUserInfoCmd() {};
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);
};

class HttpQueryPicturePrice : public HttpCmdRequest
{
    Q_OBJECT
public:
    HttpQueryPicturePrice(QObject *parent = 0);
    ~HttpQueryPicturePrice() {};
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);
};

class HttpCreateAuthKeyCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpCreateAuthKeyCmd(QObject *parent = 0);
    ~HttpCreateAuthKeyCmd() {};
    QByteArray serializeParams();

    private slots:
    void analysisResponsed(const QByteArray& data);
};

#endif // HTTPUSERINFOCMD_H
