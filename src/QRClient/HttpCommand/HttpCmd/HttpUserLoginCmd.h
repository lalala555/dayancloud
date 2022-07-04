#ifndef HTTPUSERLOGINCMD_H
#define HTTPUSERLOGINCMD_H

#include <QObject>
#include "HttpCommand/HttpCmdRequest.h"

class HttpUserLoginCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpUserLoginCmd(const QString username, const QString passwd, const QString& accessId, QObject *parent = 0);
    ~HttpUserLoginCmd() {};
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
    QString m_accessId;
    bool m_rememberPassword;
};

class HttpSigninCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpSigninCmd(const QString& userKey, const QString& accessId, QObject *parent = 0);
    ~HttpSigninCmd() {};
    QByteArray serializeParams();

private slots:
    virtual void analysisResponsed(const QByteArray& data);

private:
    QString m_userKey;
    QString m_accessId;
};

class HttpThirdSigninCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpThirdSigninCmd(const QString& userKey, int type, QObject *parent = 0);
    ~HttpThirdSigninCmd() {};
    QByteArray serializeParams();

    private slots:
    virtual void analysisResponsed(const QByteArray& data);
private:
    QString m_userKey;
    int m_type;
};

#endif // HTTPUSERLOGINCMD_H
