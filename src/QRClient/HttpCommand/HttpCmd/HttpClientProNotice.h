#ifndef HTTPCLIENTPRONOTICE_H
#define HTTPCLIENTPRONOTICE_H

#include "HttpCommand/HttpCmdRequest.h"

class HttpClientProNotice : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpClientProNotice(int channel, int langFlag, QObject *parent = 0);
    ~HttpClientProNotice();
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    int m_channel;
    int m_langFlag;
};

#endif // HTTPCLIENTPRONOTICE_H
