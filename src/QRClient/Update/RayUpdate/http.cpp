#include "stdafx.h"
#include "RayUpdate.h"
#include "http.h"
#include <QUrlQuery>
#include <QUuid>

UpdateRequestTimer::UpdateRequestTimer(QObject *parent)
    : QObject(parent)
    , m_reply(Q_NULLPTR)
    , m_timer(new QTimer) {
    connect(m_timer.data(), SIGNAL(timeout()), this, SLOT(onRequestTimeout()));
}

UpdateRequestTimer::~UpdateRequestTimer() {
}

void UpdateRequestTimer::setTimeout(QNetworkReply* reply, int delayTime) {
    Q_ASSERT(reply);
    if (reply && reply->isRunning()) {
        m_reply = reply;
        m_timer->stop();
        m_timer->start(delayTime);
    }
}

void UpdateRequestTimer::onRequestTimeout() {
    if (!m_timer->isActive())
        return;

    if (m_reply && m_reply->isRunning()) {
        m_reply->abort();
        m_reply->close();
        qDebug("[onRequestTimeout] download content timeout!!");
    }
}

Http::Http()
{
    connect(&http_, SIGNAL(finished(QNetworkReply*)), SLOT(NetworkFinished(QNetworkReply*)));
    connect(&reply_time_, SIGNAL(requestTimeout(QNetworkReply*)), SLOT(onReplyTimeout(QNetworkReply*)));
    download_speed_ = 0.0;
	download_path_ = QDir::tempPath() + "/Rayvision/update/"+ QUuid::createUuid().toString()+"/";
	
    is_stoped_ = false;
}

Http::~Http()
{
    stop();
}

void Http::get(const QUrl& url)
{
    QNetworkRequest request(url);
    QNetworkReply* reply = http_.get(request);
    reply_time_.setTimeout(reply, 6000);
#ifndef QT_NO_SSL
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslErrors(QList<QSslError>)));
#endif

}

void Http::doDownload(const QUrl& url)
{
    QString u = url.toString();
    QNetworkRequest request(url);
    time_.restart();
    QNetworkReply* reply = http_.get(request);
    reply_time_.setTimeout(reply, 6000*1000);
#ifndef QT_NO_SSL
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslErrors(QList<QSslError>)));
#endif
    connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(updateDataReadProgress(qint64, qint64)));

    currentDownloads.append(reply);
}

QString Http::saveFileName(const QUrl& url)
{
    QString path = url.path();
    QString basename = QFileInfo(path).fileName();
    basename = url.query();
    if (basename.isEmpty()) {
        basename = "download";
    }

    if (QFile::exists(basename)) {
        // already exists, don't overwrite
        int i = 0;
        basename += '.';
        while (QFile::exists(basename + QString::number(i))) {
            ++i;
        }

        basename += QString::number(i);
    }

    return basename;
}

bool Http::saveToDisk(const QString& filename, QIODevice* data)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug( "Could not open %s for writing: %s\n",
                qPrintable(filename),
                qPrintable(file.errorString()));
        return false;
    }

    file.write(data->readAll());
    file.close();

    return true;
}

void Http::stop()
{
    if (is_stoped_) {
        return;
    }
    is_stoped_ = true;
    for (auto i : currentDownloads) {
        i->abort();
    }
    currentDownloads.clear();
}

void Http::DownloadFinished(QNetworkReply* reply)
{

    QUrl url = reply->url();
    QUrlQuery urlquery(url);
    QString key = "file";
    QString filename = urlquery.queryItemValue(key);
    QString dst_path = urlquery.queryItemValue("dst_path");
    QString save_file_full_name = download_path_ + dst_path + filename;
    QDir mdir(download_path_);
    if (!mdir.mkpath(download_path_ + dst_path)) {
        qDebug() << " mkdir " << download_path_ + dst_path << " failed";
        emit downloadDone(-1, urlquery);
        return;
    }

    save_file_full_name = QDir::toNativeSeparators(save_file_full_name);
    if (reply->error()) {
        qDebug("Download of %s failed: %s\n", url.toEncoded().constData(), qPrintable(reply->errorString()));
        emit downloadDone(-1, urlquery);
        return;
    } else {
        /*  QString filename = saveFileName(url);*/
        if (saveToDisk(save_file_full_name, reply)) {
            qDebug("Download of %s succeeded\n (saved to %s)\n",
                   url.toEncoded().constData(), qPrintable(save_file_full_name));
        } else {
            qDebug() << "save file :" << save_file_full_name << "error";
            emit downloadDone(-1, urlquery);
            return;
        }
    }

    currentDownloads.removeAll(reply);
    reply->deleteLater();
    emit downloadDone(1, urlquery);
    if (currentDownloads.isEmpty()) {
        emit downloadDone(0, urlquery);
    }
}

void Http::GetCheckFinished(QNetworkReply* reply)
{
    if (reply->error()) {
        qDebug("check of %s failed: %s\n", reply->url().toEncoded().constData(), qPrintable(reply->errorString()));
        emit getCheckDone(-1, "");
        return;
    } else {
        QString reply_json = reply->readAll();

        qDebug() << "check reply" << reply_json;
        emit getCheckDone(0, reply_json);
        return;
    }
}

//void Http::execute() {
//    QStringList args = QCoreApplication::instance()->arguments();
//    args.takeFirst();           // skip the first argument, which is the program's name
//    if (args.isEmpty()) {
//
//        QCoreApplication::instance()->quit();
//        return;
//    }
//
//    foreach(QString arg, args) {
//        QUrl url = QUrl::fromEncoded(arg.toLocal8Bit());
//        doDownload(url);
//    }
//}

void Http::sslErrors(const QList<QSslError>& sslErrors)
{
	QNetworkReply* reply = (QNetworkReply*)sender();
	reply->ignoreSslErrors();
}

//这个东西吧，就单文件下载吧
void Http::updateDataReadProgress(qint64 bytesRead, qint64 totalRead)
{
    emit speedReport(QString::number(bytesRead) + "|" + QString::number(totalRead));
}

void Http::onReplyTimeout(QNetworkReply * reply) {
    if (is_stoped_) {
        return;
    }
    qDebug("reply time out");
    emit getCheckDone(-1, "");
}

void Http::NetworkFinished(QNetworkReply* reply)
{
    if (is_stoped_) {
        return;
    }
    QNetworkReply::NetworkError code = reply->error();
    QUrl url = reply->url();
    // QByteArray arr = reply->readAll();
    qDebug("url %s finished", url.toEncoded().constData());
    QUrlQuery urlquery(url);

    if (urlquery.hasQueryItem("file")) {
        DownloadFinished(reply);
        return;
    } else if (urlquery.hasQueryItem("check")) {
        GetCheckFinished(reply);
        return;
    }

}