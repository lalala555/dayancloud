#ifndef HTTPERRORLISTCMD_H
#define HTTPERRORLISTCMD_H

#include "HttpCommand/HttpCmdRequest.h"

class HttpErrorListCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpErrorListCmd(int lang, QObject *parent = 0);
    ~HttpErrorListCmd() {};
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    int m_lang;
};

#endif // HTTPERRORLISTCMD_H
