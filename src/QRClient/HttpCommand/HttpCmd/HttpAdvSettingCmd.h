#ifndef HTTPADVSETTINGCMD_H
#define HTTPADVSETTINGCMD_H

#include "HttpCommand/HttpCmdRequest.h"

class HttpAdvSettingCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpAdvSettingCmd(int type, QObject *parent = 0);
    ~HttpAdvSettingCmd() {};
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    int m_type;
};

#endif // HTTPADVSETTINGCMD_H
