#ifndef HTTPUPDATEUSERINFOCMD_H
#define HTTPUPDATEUSERINFOCMD_H

#include <QObject>
#include "HttpCommand/HttpCmdRequest.h"

class HttpUpdateUserInfoCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpUpdateUserInfoCmd(qint32 taskOverTime, int userid, QObject *parent = 0);
    ~HttpUpdateUserInfoCmd();
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);
private:
    qint32 m_taskOverTime;
    int m_userId;
};

#endif // HTTPUPDATEUSERINFOCMD_H
