#ifndef HTTPUSERSTORAGECMD_H
#define HTTPUSERSTORAGECMD_H

#include <QObject>
#include "HttpCommand/HttpCmdRequest.h"

class HttpUserStorageCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpUserStorageCmd(QObject *parent = 0);
    ~HttpUserStorageCmd() {};
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);
};

#endif // HTTPUSERSTORAGECMD_H
