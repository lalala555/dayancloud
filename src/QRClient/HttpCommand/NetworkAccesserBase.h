#ifndef NETWORKACCESSER_BASE_H
#define NETWORKACCESSER_BASE_H

#include <QMutex>
#include <QElapsedTimer>
#include "HttpCommand/HttpCommon.h"

class NetworkAccesserBase : public QNetworkAccessManager
{
    Q_OBJECT
public:
    NetworkAccesserBase(QObject *parent = 0);
    ~NetworkAccesserBase();

    QNetworkReply* Get(QNetworkRequest &request, qint64 delaytime = REQUEST_TIMEOUT);
    QNetworkReply* Post(QNetworkRequest &request, QByteArray &data, qint64 delaytime = REQUEST_TIMEOUT);
    QNetworkReply* Upload(QNetworkRequest &request, const QString& absoluteFilePath);

    void setRequestHeader(const HttpRequestHeader& header);
    void destroy();

signals:
    void sendResponse(const QByteArray& response);

protected:
    virtual void setHeaderData(QNetworkRequest &request) {};

private slots:
    void requestTimeout();
    void onRecvResponse(QNetworkReply* reply);
    void onUploadDone(QNetworkReply* reply);
    void onSslError(QNetworkReply* reply, QList<QSslError> errors);
    void onUploadProgress(qint64 bytesSent, qint64 bytesTotal);
private:
    void sendReport(int code, const QString& errorString = "");

protected:
    HttpRequestHeader  m_header;
    QTimer         m_timeout;
    QNetworkReply* m_reply;
    QMutex         m_mutex;
    QElapsedTimer  m_elapsedTimer;
};

#endif // NETWORKACCESSER_BASE_H
