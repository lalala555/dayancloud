#include "stdafx.h"
#include "LoadAtReportInfoThread.h"
#include "HttpCommand/HttpCmdManager.h"
#include "Common/QRequestTimer.h"

LoadAtReportInfoThread::LoadAtReportInfoThread(qint64 taskId, QObject *parent)
    : QThread(parent)
    , m_taskId(taskId)
    , m_netMgr(new QNetworkAccessManager)
    , m_netRequestTimer(new QRequestTimer)
 // , m_filePath(getReportFileSavePath())
    , m_at_report_data(new at_report_data)
{
	m_filePath = getReportFileSavePath();
//    connect(m_netMgr.data(), SIGNAL(finished(QNetworkReply *)), this, SLOT(onDownloadReportDataFinished(QNetworkReply *)));
}

LoadAtReportInfoThread::~LoadAtReportInfoThread()
{
}

at_report_data * LoadAtReportInfoThread::getAtReportInfo()
{
    return m_at_report_data;
}

void LoadAtReportInfoThread::run()
{
    downloadReportData(m_url);
}

void LoadAtReportInfoThread::loadAtReportDataUrl()
{
    // start();
    HttpCmdManager::getInstance()->queryAtReportInfo(MyConfig.accountSet.userId, m_taskId, this);
}

void LoadAtReportInfoThread::downloadReportData(const QString & url)
{
    QNetworkRequest request;
    QSslConfiguration config;
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::TlsV1_1);
    request.setSslConfiguration(config);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
    request.setUrl(QUrl(url.toLocal8Bit()));

    QNetworkReply* reply = m_netMgr->get(request);

    QEventLoop loop;
    QTimer timer;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    connect(reply, SIGNAL(finished()), &timer, SLOT(stop()));
    connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SIGNAL(downloadProgress(qint64, qint64)));
    timer.start(1000 * 60 * 2);
    loop.exec();

    onDownloadReportDataFinished(reply);
}

bool LoadAtReportInfoThread::parseReportDataInfo(const QString & path)
{
    QJsonObject jsonObj;
    if (!RBHelper::loadJsonFile(path, jsonObj)) {
        emit loadAtReportInfoFinished(CODE_JSON_FILE_LOAD_FAILED);
        return false;
    }

    // tie_point
    if (jsonObj.contains("tie_point") && jsonObj["tie_point"].isObject()) {
        QJsonObject tie_point = jsonObj["tie_point"].toObject();
        m_at_report_data->tie_point.MedianNumberOfPhotosPerPoint = tie_point["MedianNumberOfPhotosPerPoint"].toString();
        m_at_report_data->tie_point.NumberOfPoints = tie_point["NumberOfPoints"].toString();
        m_at_report_data->tie_point.MedianReprojectionError = tie_point["MedianReprojectionError"].toString();
        m_at_report_data->tie_point.MedianNumberOfPointsPerPhoto = tie_point["MedianNumberOfPointsPerPhoto"].toString();
        m_at_report_data->tie_point.RMSOfReprojectionErrors = tie_point["RMSOfReprojectionErrors"].toString();
        m_at_report_data->tie_point.RMSOfDistancesToRays = tie_point["RMSOfDistancesToRays"].toString();
    }

    // project
    if (jsonObj.contains("project") && jsonObj["project"].isObject()) {
        QJsonObject project = jsonObj["project"].toObject();
        m_at_report_data->project.name                   = project["name"].toString();
        m_at_report_data->project.photo_num              = project["photo_num"].toVariant().toLongLong();
        m_at_report_data->project.data_set               = project["data_set"].toString();
        m_at_report_data->project.error_photo_num        = project["error_photo_num"].toInt();
        m_at_report_data->project.calibration_photoo_num = project["calibration_photoo_num"].toVariant().toLongLong();
        m_at_report_data->project.downsampling_rate      = project["downsampling_rate"].toDouble();
        m_at_report_data->project.photo_group_num        = project["photo_group_num"].toInt();
        m_at_report_data->project.RMS                    = project["RMS"].toString();
    }

    // control_Point
    if (jsonObj.contains("control_Point") && jsonObj["control_Point"].isObject()) {
    }

    // photogroups
    if (jsonObj.contains("photogroups") && jsonObj["photogroups"].isArray()) {
        QJsonArray photogroups = jsonObj["photogroups"].toArray();
        for (int i = 0; i < photogroups.size(); i++) {
            QJsonObject photogroup = photogroups[i].toObject();
            report_photogroup* group = new report_photogroup;

            group->name         = photogroup["name"].toString();
            group->width        = photogroup["width"].toString();
            group->height       = photogroup["height"].toString();
            group->focal_length = photogroup["focal_length"].toString();
            group->sensor_size  = photogroup["sensor_size"].toString();
            group->photos_num   = photogroup["photos_num"].toVariant().toLongLong();

            if (photogroup["camera"].isObject()) {
                QJsonObject camera = photogroup["camera"].toObject();
                group->camera.K1 = camera["K1"].toString();
                group->camera.K2 = camera["K2"].toString();
                group->camera.K3 = camera["K3"].toString();
                group->camera.P1 = camera["P1"].toString();
                group->camera.P2 = camera["P2"].toString();
                group->camera.x  = camera["x"].toString();
                group->camera.y  = camera["y"].toString();
                group->camera.focal_length = camera["focal_length"].toString();
            }

            m_at_report_data->photogroups.photogroups.append(group);
        }
    }

    // photo
    if (jsonObj.contains("photo") && jsonObj["photo"].isArray()) {
        QJsonArray photos = jsonObj["photo"].toArray();
        for (int i = 0; i < photos.size(); i++) {
            QJsonObject photo = photos[i].toObject();
            report_photo *rphoto = new report_photo;

            rphoto->file_name = photo["file_name"].toString();
            rphoto->NumberOfPoints          = photo["NumberOfPoints"].toString();
            rphoto->RMSOfDistancesToRays    = photo["RMSOfDistancesToRays"].toString();
            rphoto->RMSOfReprojectionErrors = photo["RMSOfReprojectionErrors"].toString();

            m_at_report_data->photos.photos.append(rphoto);
        }
    }

    emit loadAtReportInfoFinished(CODE_SUCCESS);
    return true;
}

QString LoadAtReportInfoThread::getReportFileSavePath()
{
    QString path = RBHelper::getTempPath(QString::number(m_taskId));
    path.append(QDir::separator());
    path.append("report.json");

    return path;
}

void LoadAtReportInfoThread::onRecvResponse(QSharedPointer<ResponseHead> response)
{
    if (response->cmdType == RESTAPI_AT_REPORT_DATA_URL) {
        if (response->code == HTTP_ERROR_SUCCESS) {
            QSharedPointer<AtReportUrlResponse> report = qSharedPointerCast<AtReportUrlResponse>(response);
            if (!report->data.isEmpty()) {
                m_url = QString("%1%2").arg(CConfigDb::Inst()->getHost()).arg(report->data);
                // downloadReportData(m_url);
                start();
                return;
            }
        }
        emit loadAtReportInfoFinished(CODE_QUERY_AT_REPORT_URL_FAILED);
    }
}

void LoadAtReportInfoThread::onDownloadReportDataFinished(QNetworkReply * reply)
{
    // 获取响应的信息，状态码为200表示正常
    QByteArray imageData;
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode == 200) {
        if (reply->error() == QNetworkReply::NoError) {
            QFile file(m_filePath);
            if (file.open(QIODevice::ReadWrite)) {
                file.write(reply->readAll());
            }
            file.close();
        } else {
            QString strError = reply->errorString();
            qDebug() << "[QDownloadImage] down file error " << m_filePath << reply->errorString();
        }

    } else {
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
        case 304:
            break;
        }
    }

    parseReportDataInfo(m_filePath);

    reply->abort();
    reply->deleteLater();
}

bool LoadAtReportInfoThread::handleRedirect(QNetworkReply *reply)
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
    } else {
        return false;
    }

    // 重试3次
    if (m_retryTimes < 3) {
        m_url = rUrl.toString();
        downloadReportData(m_url);
        m_retryTimes++;
        return true;
    }
    return false;
}