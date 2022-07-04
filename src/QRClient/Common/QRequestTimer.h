#ifndef QREQUESTTIMER_H
#define QREQUESTTIMER_H

#include <QObject>
#include <QTimer>
#include <QNetworkReply>

#define HTTP_REQUEST_TIMEOUT 1000 * 60

class QRequestTimer : public QObject
{
    Q_OBJECT

public:
    QRequestTimer(QObject* parent = 0);
    ~QRequestTimer();

    void setTimeout(QNetworkReply* reply, int delayTime = HTTP_REQUEST_TIMEOUT);
signals:
    void requestTimeout(QNetworkReply*);
private slots:
    void onRequestTimeout();
private:
    QNetworkReply* m_reply;
    QScopedPointer<QTimer> m_timer;
};

#endif // QREQUESTTIMER_H
