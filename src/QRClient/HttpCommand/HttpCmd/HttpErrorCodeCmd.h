#ifndef HTTPERRORCODECMD_H
#define HTTPERRORCODECMD_H

#include "HttpCommand/HttpCmdRequest.h"

class HttpErrorCodeCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpErrorCodeCmd(int lang, QList<int> codes, QObject *parent = 0);
    ~HttpErrorCodeCmd() {};
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    int m_lang;
    QList<int> m_codes;
};

#endif // HTTPERRORCODECMD_H
