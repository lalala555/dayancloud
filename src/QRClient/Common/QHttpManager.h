#ifndef QHTTPMANAGER_H
#define QHTTPMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
class QRequestTimer;

#define GET_SERVER_LIST_DELAYTIME 1000 * 60 // 获取服务器列表超时时间
#define GET_TRANS_LIST_DELAYTIME  1000 * 60 // 获取传输服务器列表超时时间
#define GET_NOTICE_DELAYTIME      1000 * 30 // 获取客户端通知超时时间

class QHttpManager : public QObject
{
    Q_OBJECT

public:
    QHttpManager(QObject *parent = 0);
    virtual ~QHttpManager();

    void init();
    void downloadServerListAsync(int zone);
	void changePlatform(bool loginType);

signals:
    void responseServerList(const QString&);

private slots:
    void recvDownLoadServerList(QNetworkReply * reply);

private:
    QScopedPointer<QNetworkAccessManager> m_serverNetMgr;
    QScopedPointer<QRequestTimer> m_serverRequestTimer;
    QString m_downloadData;
	bool m_loginType;   //false 新平台 true 旧平台
};

#endif // QHTTPMANAGER_H
