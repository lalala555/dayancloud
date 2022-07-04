#ifndef HTTPREQUESTCMDIMPL_H
#define HTTPREQUESTCMDIMPL_H

#include <QObject>
#include <QThread>
#include "NetworkAccesser.h"

class HttpCmdRequest : public QThread
{
    Q_OBJECT

public:
    HttpCmdRequest(int type, QObject *parent = 0);
    virtual ~HttpCmdRequest();
    virtual void post(const QString& url, const HttpRequestHeader& header);
    virtual void get(const QString& url, const HttpRequestHeader& header);
    virtual void upload(const QString& url, const HttpRequestHeader& header);

    virtual QByteArray serializeParams() = 0;
    virtual int getCmdType()
    {
        return m_cmdType;
    }
    virtual QString getCmdName()
    {
        return m_cmdName;
    }
    virtual QString getCmdApiUrl()
    {
        return m_url;
    }
    virtual void initCmdProperty();
    virtual void run();

signals:
    void sendResponseFormat(QSharedPointer<ResponseHead>);

protected:
    virtual QJsonObject qstringToJsonObject(const QString& str);
public slots:
    virtual void analysisResponsed(const QByteArray& data) = 0;
    virtual void destroyRequst();
    virtual void recvResponsed(const QByteArray& data);
protected:
    NetworkAccesser m_accesser;
    int m_cmdType;
    QString m_cmdName;
    QString m_url;
    QByteArray m_data;
};
#endif // HTTPREQUESTCMDIMPL_H
