#include "stdafx.h"
#include "QHttpManager.h"
#include "QRequestTimer.h"

QHttpManager::QHttpManager(QObject *parent)
    : QObject(parent)
    , m_serverNetMgr(new QNetworkAccessManager)
    , m_serverRequestTimer(new QRequestTimer)
	, m_loginType(false)
{
    connect(m_serverNetMgr.data(), SIGNAL(finished(QNetworkReply *)), this, SLOT(recvDownLoadServerList(QNetworkReply *)));
}

QHttpManager::~QHttpManager()
{
}

void QHttpManager::init()
{
}

void QHttpManager::downloadServerListAsync(int zone)
{
    int langId = LocalSetting::getInstance()->getLanguageId();
    QString wsHostUrl = WSConfig::getInstance()->getWSHostUrl();
    QString wsVersion = WSConfig::getInstance()->getWSVersion();

#ifdef Q_OS_WIN
    QString os = "windows";
#elif defined(Q_OS_LINUX)
    QString os = "linux";
#elif defined Q_OS_MAC
    QString os = "macosx";
#endif


    QString requestUrl = QString("%1%2channels_modelling.php?v=%3&zone=%4&lang=%5&cver=%6&product=%7")
                        .arg(wsHostUrl)
                        .arg(CRCLIENT2_URL)
                        .arg(wsVersion)
                        .arg(zone)
                        .arg(langId)
                        .arg(STRFILEVERSION)
                        .arg(LocalConfig::getInstance()->defaultProduct);

	if (m_loginType)
	{		
		requestUrl = QString("%1%2channels_modelling_v2.php?v=%3&zone=%4&lang=%5&cver=%6&product=%7")
			.arg(wsHostUrl)
			.arg(CRCLIENT2_URL)
			.arg(wsVersion)
			.arg(zone)
			.arg(langId)
			.arg(STRFILEVERSION)
			.arg(LocalConfig::getInstance()->defaultProduct);
	}

    QNetworkRequest request;
    QSslConfiguration config;
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::TlsV1_1);
    request.setSslConfiguration(config);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/xml"));
    request.setUrl(QUrl(requestUrl.toLocal8Bit()));

    QNetworkReply* reply = m_serverNetMgr->get(request);
    m_serverRequestTimer->setTimeout(reply, GET_SERVER_LIST_DELAYTIME);
}

void QHttpManager::changePlatform(bool loginType)
{
	m_loginType = loginType;
}

void QHttpManager::recvDownLoadServerList(QNetworkReply * reply)
{
    reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    if(reply->error() == QNetworkReply::NoError) {
        QByteArray datas = reply->readAll();
        QString content = QString(datas);
        if(content == "null" || content.isEmpty()) {
            emit responseServerList("");
        } else {
            emit responseServerList(content);
        }
    } else {
        QString errorMsg = reply->errorString();
        emit responseServerList("");
        LOGFMTE("[recvDownLoadServerList] download ServerList failed, msg = %s...", qPrintable(errorMsg));
    }
}
