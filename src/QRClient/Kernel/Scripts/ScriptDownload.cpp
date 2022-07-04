#include "stdafx.h"
#include "ScriptDownload.h"
#include "Common/FileUtil.h"

ScriptDownloader::ScriptDownloader(QObject* parent)
    : QObject(parent)
{
    connect(&m_manager, SIGNAL(finished(QNetworkReply*)),
            SLOT(downloadFinished(QNetworkReply*)));
}

void ScriptDownloader::downloadScript(const QStringList& scripts)
{
    foreach(auto script, scripts) {
        downloadScript(script);
    }
}

void ScriptDownloader::downloadScript(const QString& script)
{
    QString scriptRootPath = RBHelper::getPyScriptRootPath();
    QString scriptFilePath = scriptRootPath + QDir::separator() + script;

    QString scriptFileDigest;
    if(QFile::exists(scriptFilePath)) {
        QByteArray fileDigest = File::getFileMd5(scriptFilePath);
        scriptFileDigest = fileDigest.toHex();
    }

    int parentId = MyConfig.accountSet.mainUserId;
    int uesrId = MyConfig.accountSet.userId;
    int platformId = LocalSetting::getInstance()->getDefaultsite();
    QString hosturl = WSConfig::getInstance()->getWSHostUrl();
    QString version = WSConfig::getInstance()->getWSVersion();

    QString requestUrl = QString("%1%2script_download.php?sid=%3&uid=%4&puid=%5&file=%6&hash=%7&version=%8&cver=%9")
                         .arg(hosturl)
                         .arg(CRCLIENT2_URL)
                         .arg(platformId)
                         .arg(uesrId)
                         .arg(parentId)
                         .arg(script)
                         .arg(scriptFileDigest)
                         .arg(version)
                         .arg(STRFILEVERSION);

    this->doDownload(QUrl::fromEncoded(requestUrl.toLocal8Bit()), scriptFilePath);
}

void ScriptDownloader::doDownload(const QUrl &url, const QString &saveFilePath)
{
    QNetworkRequest request(url);

    QSslConfiguration config;
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(config);

    request.setRawHeader(QByteArray("saveFilePath"), saveFilePath.toUtf8());

    QNetworkReply *reply = m_manager.get(request);
}

bool ScriptDownloader::saveToDisk(const QString &filename, QIODevice *data)
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

    this->unzipToDisk(filename);

    return true;
}

bool ScriptDownloader::unzipToDisk(const QString &filename)
{
    QFileInfo fi(filename);
    QString destDir = QDir::toNativeSeparators(fi.absolutePath());

    File::unZipPackage(filename, destDir);

    return true;
}

void ScriptDownloader::downloadFinished(QNetworkReply *reply)
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
            } else if(status == 304) {
                this->unzipToDisk(filename);
                result = ERR_CGSCRIPT_DOWNLOAD_OK;
            } else {
                QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
                qDebug() << reason;
            }
        }
    } else {
        LOGFMTE("[ScriptDownload] Network Error : %s", qPrintable(reply->errorString()));
    }

    reply->deleteLater();

    emit resultReady(result, filename);
}

void ScriptDownloader::downloadPlugins()
{
    QString scriptRootPath = RBHelper::getScriptRootPath();
    QString scriptFilePath = QString("%1/%2")
                             .arg(scriptRootPath)
                             .arg("picture_plugin_config.xml");

    int zone = MyConfig.accountSet.zone;
    int lang = LocalSetting::getInstance()->getLanguageId();
    QString hosturl = WSConfig::getInstance()->getWSHostUrl();
    QString version = WSConfig::getInstance()->getWSVersion();
    QString clientVer = QString(STRFILEVERSION);

    QString requestUrl = QString("%1%2picture_plugin_config.php?v=%3&zone=%4&lang=%5&cver=%6")
        .arg(hosturl)
        .arg(CRCLIENT2_URL)
        .arg(version)
        .arg(zone)
        .arg(lang)
        .arg(clientVer);

    this->doDownload(QUrl::fromEncoded(requestUrl.toLocal8Bit()), scriptFilePath);
}