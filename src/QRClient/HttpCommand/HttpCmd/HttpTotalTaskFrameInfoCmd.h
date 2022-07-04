#ifndef HTTPTOTALTASKFRAMEINFOCMD_H
#define HTTPTOTALTASKFRAMEINFOCMD_H

#include <QObject>
#include "HttpCommand/HttpCmdRequest.h"

class HttpTotalTaskFrameInfoCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpTotalTaskFrameInfoCmd(QObject *parent = 0);
    ~HttpTotalTaskFrameInfoCmd() {};
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);
};

#endif // HTTPTOTALTASKFRAMEINFOCMD_H
