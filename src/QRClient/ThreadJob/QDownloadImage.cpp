#include "stdafx.h"
#include "QDownloadImage.h"
#include <QtDebug>
#include <QApplication>
#include <QThread>
#include <QEventLoop>

QDownloadImage::QDownloadImage(qint64 frameId, int index, const QString& strUrl,
                               const QString& strLocal, bool onlyPixelData, QObject *parent)
    : QObject(parent)
    , m_strLocal(strLocal)
    , m_strUrl(strUrl)
    , m_index(index)
    , m_netMgr(nullptr)
    , m_frameId(frameId)
    , m_retryTimes(0)
    , m_onlyPixelData(onlyPixelData)
{
    setAutoDelete(true);
}

QDownloadImage::~QDownloadImage()
{
    m_index = 0;
    m_strLocal.clear();
    m_strUrl.clear();
}

void QDownloadImage::run()
{
    QNetworkAccessManager m_netMgr;
   
    QNetworkRequest request;
    request.setUrl(QUrl(RBHelper::encodingUrl(m_strUrl)));
    if (m_onlyPixelData) {
        QSslConfiguration config;
        config.setPeerVerifyMode(QSslSocket::VerifyNone);
        request.setSslConfiguration(config);
        request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
    } else {
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
    }
    
    QNetworkReply *reply = m_netMgr.get(request);

    // 阻塞式下载，确保下载正确
    QEventLoop loop;
    QTimer timer;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit())); // 15秒超时，避免卡主线程
    connect(reply, SIGNAL(finished()), &timer, SLOT(stop()));
    timer.start(1000 * 15);
    loop.exec();

    downloadFinished(reply);
}

void QDownloadImage::downloadFinished(QNetworkReply *reply)
{
    // 获取响应的信息，状态码为200表示正常
    QByteArray imageData;
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode == 200) {
        if (reply->error() == QNetworkReply::NoError) {
            if (m_onlyPixelData == false) {
                QFile file(m_strLocal);
                if (file.open(QIODevice::ReadWrite)) {
                    file.write(reply->readAll());
                }
                file.close();
            } else {
                imageData = reply->readAll();
            }
        } else {
            QString strError = reply->errorString();
            qDebug() << "[QDownloadImage] down file error " << m_strLocal << reply->errorString();
        }

    }  else {
        switch (statusCode) {
        case 301:
        case 302:
        case 303:
        case 305:
        case 307: {
            if (handleRedirect(reply))
                return;
        }
            break;
        }
    }

    if (m_onlyPixelData) {
        emit onFinished(m_frameId, m_index, imageData);
    } else {
        emit onFinished(m_frameId, m_index, m_strLocal);
    }
    

    reply->abort();
    reply->deleteLater();
}

bool QDownloadImage::handleRedirect(QNetworkReply *reply)
{
    QUrl rUrl;
    QList<QPair<QByteArray, QByteArray> > fields = reply->rawHeaderPairs();
    foreach(const QNetworkReply::RawHeaderPair &header, fields) {
        if (header.first.toLower() == "location") {
            rUrl = QUrl::fromEncoded(header.second);
            break;
        }
    }

    if (!rUrl.isValid()) {
        return false;
    }

    if (rUrl.isRelative()) {
        rUrl = reply->request().url().resolved(rUrl);
    }
    // Check redirect url protocol
    QString scheme = rUrl.scheme();
    if (scheme == QLatin1String("http") || scheme == QLatin1String("https")) {
        QString previousUrlScheme = reply->request().url().scheme();
        // Check if we're doing an unsecure redirect (https -> http)
        if (previousUrlScheme == QLatin1String("https")
            && scheme == QLatin1String("http")) {
            return false;
        }
    }
    else {
        return false;
    }

    // 重试3次
    if (m_retryTimes < 3) {
        m_strUrl = rUrl.toString();
        this->run();
        m_retryTimes++;
        return true;
    }
    return false;
}