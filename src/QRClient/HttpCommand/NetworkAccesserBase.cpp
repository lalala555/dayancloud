#include "stdafx.h"
#include "NetworkAccesserBase.h"
#include <QNetworkCookie>
#include <QNetworkProxyFactory>
#include <QHttpPart>
#include "Common/SystemUtil.h"

NetworkAccesserBase::NetworkAccesserBase(QObject *parent)
    : QNetworkAccessManager(parent)
    , m_reply(nullptr)
{
    QNetworkProxyFactory::setUseSystemConfiguration(false);
    connect(&m_timeout, SIGNAL(timeout()), this, SLOT(requestTimeout()));
    connect(this, SIGNAL(finished(QNetworkReply*)), this, SLOT(onRecvResponse(QNetworkReply*)));
    connect(this, SIGNAL(sslErrors(QNetworkReply*, QList<QSslError>)), this, SLOT(onSslError(QNetworkReply*, QList<QSslError>)));
}

NetworkAccesserBase::~NetworkAccesserBase()
{
    m_timeout.deleteLater();
    if (m_reply)
        m_reply->deleteLater();
}

QNetworkReply* NetworkAccesserBase::Get(QNetworkRequest &request, qint64 delaytime)
{
    setHeaderData(request);
    // 避免重复请求（在等待返回的时候）
    if(m_reply && m_reply->isRunning()) {
        m_reply->abort();
        m_reply->close();
    }
    m_reply = this->get(request);

    qint64 timeout = delaytime;
    if(LocalConfig::getInstance()->requestTimeout >= delaytime) {
        timeout = LocalConfig::getInstance()->requestTimeout;
    }

    if(m_reply && m_reply->isRunning()) {
        m_timeout.stop();
        m_timeout.start(timeout);
    }

    m_elapsedTimer.restart();

    return m_reply;
}

QNetworkReply* NetworkAccesserBase::Post(QNetworkRequest &request, QByteArray &data, qint64 delaytime)
{
    setHeaderData(request);
    // 避免重复请求（在等待返回的时候）
    if(m_reply && m_reply->isRunning()) {
        m_reply->abort();
        m_reply->close();
    }

    qint64 timeout = delaytime;
    if(LocalConfig::getInstance()->requestTimeout >= delaytime) {
        timeout = LocalConfig::getInstance()->requestTimeout;
    }

    this->clearAccessCache();
    m_reply = this->post(request, data);
    m_reply->setProperty("body", data);
    if(m_reply && m_reply->isRunning()) {
        m_timeout.stop();
        m_timeout.start(timeout);
    }

    m_elapsedTimer.restart();

    return m_reply;
}

QNetworkReply * NetworkAccesserBase::Upload(QNetworkRequest &request, const QString& absoluteFilePath)
{
    setHeaderData(request);

    QFileInfo fi(absoluteFilePath);
    if (!fi.exists())
        return nullptr;
    QString fileName = fi.fileName();
    QString header = QString("form-data; name=\"file\"; filename=\"%1\"").arg(fileName);

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(header));

    QFile *file = new QFile(absoluteFilePath);
    file->open(QIODevice::ReadOnly);
    filePart.setBodyDevice(file);
    file->setParent(multiPart);

    multiPart->append(filePart);

    m_reply = this->post(request, multiPart);
    multiPart->setParent(m_reply);

    connect(m_reply, SIGNAL(finished()), this, SLOT(onUploadDone()));
    connect(m_reply, SIGNAL(uploadProgress(qint64, qint64)), this, SLOT(onUploadProgress(qint64, qint64)));

    /*qint64 timeout = LocalConfig::getInstance()->requestTimeout;
    timeout = 3000;
    if (m_reply && m_reply->isRunning()) {
        m_timeout.stop();
        m_timeout.start(timeout);
    }*/

    return m_reply;
}

void NetworkAccesserBase::setRequestHeader(const HttpRequestHeader& header)
{
    m_header = header;
}

void NetworkAccesserBase::requestTimeout()
{
    QMutexLocker locker(&m_mutex);

    if(!m_timeout.isActive())
        return;
    m_timeout.stop();
    if(m_reply && m_reply->isRunning()) {
        m_reply->abort();
        m_reply->close();
        LOGFMTD("[NetworkAccesser] waiting response timeout!!");
    }
}

void NetworkAccesserBase::onRecvResponse(QNetworkReply* reply)
{
    auto url = reply->request().url().toString();
    auto body = reply->property("body").toByteArray();

    qDebug() << "URL:" << url << " Cost time: " << m_elapsedTimer.elapsed();

    m_timeout.stop();
    reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    QByteArray data = "empty response";
    QString errorMsg = "";

    QNetworkReply::NetworkError errorCode = reply->error();
    if(errorCode == QNetworkReply::NoError) {
        data = reply->readAll();
        QString content = QString(data);
        if(content == "null" || content.isEmpty()) {
            data = "empty response";
        }
    } else {
        errorMsg = reply->errorString();
        LOGFMTE("Network error [%s] code [%d]", qPrintable(errorMsg), errorCode);

        data = reply->readAll();
    }
    reply->abort();
    reply->close();
    sendReport(errorCode, errorMsg);
    emit sendResponse(data);
}

void NetworkAccesserBase::onUploadDone(QNetworkReply * reply)
{
    qDebug() << "file upload success " << reply->errorString();
}

void NetworkAccesserBase::onSslError(QNetworkReply* reply, QList<QSslError> errors)
{
    reply->ignoreSslErrors();
}

void NetworkAccesserBase::onUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    qDebug() << "file upload progress sent: " << bytesSent << " total: " << bytesTotal;
}

void NetworkAccesserBase::destroy()
{
    this->clearAccessCache();
    m_timeout.stop();
    disconnect(&m_timeout, SIGNAL(timeout()), this, SLOT(requestTimeout()));
    disconnect(this, SIGNAL(finished(QNetworkReply*)), this, SLOT(onRecvResponse(QNetworkReply*)));
}

void NetworkAccesserBase::sendReport(int code, const QString& errorString)
{
    QJsonObject json;
    if (m_reply) json.insert("url", m_reply->url().toString());
    json.insert("time_consuming", m_elapsedTimer.elapsed());
    json.insert("code", code);
    if (!errorString.isEmpty())
        json.insert("error_string", errorString);
    QString data = QJsonDocument(json).toJson(QJsonDocument::Compact);
}