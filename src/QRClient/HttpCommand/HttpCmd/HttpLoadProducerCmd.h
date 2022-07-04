#ifndef HTTPLOADPRODUCERCMD_H
#define HTTPLOADPRODUCERCMD_H

#include "HttpCommand/HttpCmdRequest.h"

class HttpLoadProducerCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpLoadProducerCmd(QObject *parent = 0);
    ~HttpLoadProducerCmd();

    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);
};

#endif // HTTPLOADPRODUCERCMD_H
