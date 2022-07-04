#include "stdafx.h"
#include "QDownloadManager.h"
#include "FileUtil.h"

QDownloadManager::QDownloadManager(QObject* parent)
    : QObject(parent)
{
    connect(&m_manager, SIGNAL(finished(QNetworkReply*)),
            SLOT(downloadFinished(QNetworkReply*)));
}

void QDownloadManager::doDownload(const QUrl &url, const QString &saveFilePath)
{
    QNetworkRequest request(url);

    QSslConfiguration config;
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(config);

    request.setRawHeader(QByteArray("saveFilePath"), saveFilePath.toUtf8());

    QNetworkReply *reply = m_manager.get(request);
}

bool QDownloadManager::saveToDisk(const QString &filename, QIODevice *data)
{
    QFile file(filename);
    if(file.exists()) {
        QFile::remove(filename);
    }

    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    file.write(data->readAll());
    file.close();

    return true;
}

void QDownloadManager::downloadFinished(QNetworkReply *reply)
{
    int result = ERR_CGSCRIPT_DOWNLOAD_FAILED;

    QNetworkRequest request = reply->request();
    QString filename = request.rawHeader(QByteArray("saveFilePath"));

    if (reply->error() == QNetworkReply::NoError) {
        QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
        if (statusCode.isValid() ) {
            int status = statusCode.toInt();


            if(status == 200) {
                if (saveToDisk(filename, reply)) {
                    result = ERR_CGSCRIPT_DOWNLOAD_OK;
                }
            } else {
                QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
                qDebug() << reason;
            }
        }
    }

    reply->deleteLater();

    emit resultReady(result, filename);
}

void QDownloadManager::downloadAutoUpdater(const QString& updater)
{
    QString tmpPath = QCoreApplication::applicationDirPath();
    QString updaterFilePath = tmpPath + "/" + updater;

    QString exePath = "ray_update/AutoUpdater/";
    QString hosturl = WSConfig::getInstance()->getWSHostUrl();

    QString requestUrl = QString("%1%2%3%4")
                         .arg(hosturl)
                         .arg(CRCLIENT2_URL)
                         .arg(exePath)
                         .arg(updater);

    this->doDownload(QUrl::fromEncoded(requestUrl.toLocal8Bit()), updaterFilePath);
}

void QDownloadManager::downloadCoordSystemFile(const QString& filename)
{
    QString appRootPath = RBHelper::getTempPath("coord_system");
    QString updaterFilePath = appRootPath + "/" + filename;

    QString filepath = "conf/modeling/";
    QString hosturl = WSConfig::getInstance()->getWSHostUrl();

    QString requestUrl = QString("%1%2%3%4")
                         .arg(hosturl)
                         .arg(CRCLIENT2_URL)
                         .arg(filepath)
                         .arg(filename);

    this->doDownload(QUrl::fromEncoded(requestUrl.toLocal8Bit()), updaterFilePath);

}