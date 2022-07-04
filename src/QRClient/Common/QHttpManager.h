#ifndef QHTTPMANAGER_H
#define QHTTPMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
class QRequestTimer;

#define GET_SERVER_LIST_DELAYTIME 1000 * 60 // ��ȡ�������б�ʱʱ��
#define GET_TRANS_LIST_DELAYTIME  1000 * 60 // ��ȡ����������б�ʱʱ��
#define GET_NOTICE_DELAYTIME      1000 * 30 // ��ȡ�ͻ���֪ͨ��ʱʱ��

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
	bool m_loginType;   //false ��ƽ̨ true ��ƽ̨
};

#endif // QHTTPMANAGER_H
