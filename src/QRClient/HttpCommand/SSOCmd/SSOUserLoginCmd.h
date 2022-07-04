#ifndef SSOUSERLOGINCMD_H
#define SSOUSERLOGINCMD_H

#include <QObject>
#include "HttpCommand/HttpCmdRequest.h"

class SSOUserLoginCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    SSOUserLoginCmd(const QString& username, const QString passwd, QObject *parent = 0);
    ~SSOUserLoginCmd() {};
    QByteArray serializeParams();
    void setUserName(const QString& username)
    {
        m_userName = username;
    }
    void setPassword(const QString& password)
    {
        m_passWord = password;
    }

private:
    QString passwordEncrypt(const QString& dest);
private slots:
    virtual void analysisResponsed(const QByteArray& data);
private:
    QString m_userName;
    QString m_passWord;
    bool m_rememberPassword;
};

#endif // SSOUSERLOGINCMD_H
