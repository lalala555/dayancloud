#ifndef HTTPCLIENTSYSNOTICE_H
#define HTTPCLIENTSYSNOTICE_H

#include "HttpCommand/HttpCmdRequest.h"

class HttpClientSysNotice : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpClientSysNotice(int channel, int langFlag, int noticeType = -1, QObject *parent = 0);
    ~HttpClientSysNotice();
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    int m_channel;
    int m_langFlag;
    int m_noticeType;
};

#endif // HTTPCLIENTSYSNOTICE_H
