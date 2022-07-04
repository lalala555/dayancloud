#include "stdafx.h"
#include "LoadAtDataInfoThread.h"
#include "HttpCommand/HttpCmdManager.h"
#include "Common/QRequestTimer.h"
#include "JobStateMgr.h"

LoadAtDataInfoThread::LoadAtDataInfoThread(qint64 taskId, QObject *parent)
    : QThread(parent)
    , m_taskId(taskId)
    , m_netMgr(new QNetworkAccessManager)
    , m_netRequestTimer(new QRequestTimer)
    , m_at_view_data(new at_view_info::at_view_data)
{
	setAtViewFileSavePath();
//    connect(m_netMgr.data(), SIGNAL(finished(QNetworkReply *)), this, SLOT(onDownloadAtDataFinished(QNetworkReply *)));
}

LoadAtDataInfoThread::~LoadAtDataInfoThread()
{
    this->quit();
}

at_view_info::at_view_data * LoadAtDataInfoThread::getAtViewInfo()
{
    return m_at_view_data;
}

void LoadAtDataInfoThread::run()
{	
#if 0
	emit loadAtPointCloudInfoFinished(CODE_SUCCESS);
	return;

#endif

	//判断文件是否存在
	QFileInfo fileInfo(m_filePath_image_pos);
	if (fileInfo.isFile())
	{
//		system(m_filePath_image_pos.toStdString().c_str());
		QFile::remove(m_filePath_image_pos);
	}

	if (downloadAtViewInfo(m_url_image_pos, m_filePath_image_pos))  //下载 image_pos.json
	{
		if (downloadAtViewInfo(m_url_at_data, m_filePath_at_data)) //下载at_data.json
		{
			emit loadAtPointCloudInfoFinished(CODE_SUCCESS);
		}else
		{
			emit loadAtPointCloudInfoFinished(CODE_JSON_FILE_LOAD_FAILED);
		}
	}else
	{
		emit loadAtPointCloudInfoFinished(CODE_JSON_FILE_LOAD_FAILED);
	}

//     if (downloadAtViewInfo(m_url_at_data, m_filePath_at_data)  &&  //下载at_data.json   &&两个下载不同步，再次空三始终返回false
// 		downloadAtViewInfo(m_url_image_pos, m_filePath_image_pos)) { //下载 image_pos.json
//         emit loadAtPointCloudInfoFinished(CODE_SUCCESS);
//     } else {
//         emit loadAtPointCloudInfoFinished(CODE_JSON_FILE_LOAD_FAILED);
//     } 
}

void LoadAtDataInfoThread::loadAtViewDataUrl( )
{
    // start();
    // HttpCmdManager::getInstance()->queryAtCloudPointInfo(MyConfig.accountSet.userId, m_taskId, this);
    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_taskId);
    if (jobstate == nullptr) return;

    QString path = RBHelper::getTempPath(QString::number(m_taskId));
    path.append(QDir::separator());
    path.append("image_pos.json");      //imag_pos.json和at_data.json文件是同时输出的，image_pos.json文件小很多（5万张影像大概22M），判断imag_pos.json的MD5是否改变 间接判断at_data.json有没有改变
    QString image_pos_md5Hash = getHash(path);   //at_data.json超大文件的mD5值计算待优化，引用间接判断方法，判断image_pos.json MD5
    HttpCmdManager::getInstance()->checkImagePosJsonMD5(MyConfig.accountSet.userId, m_taskId, image_pos_md5Hash, this);
}

bool LoadAtDataInfoThread::downloadAtViewInfo(const QString & url, const QString & _filePath)
{
	m_url = url;
	m_filePath = _filePath;

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
    timer.start(1000 * 60 * 5);
    loop.exec();

	return onDownloadAtDataFinished(reply);
}

bool LoadAtDataInfoThread::parseAtViewDataInfo(const QString & path)
{
    QJsonObject jsonObj;
    if (!RBHelper::loadJsonFile(path, jsonObj)) {     
        return false;
    }

#if 0
    // tie_point
    if (jsonObj.contains("tie_point") && jsonObj["tie_point"].isObject()) {
        QJsonObject tie_point = jsonObj["tie_point"].toObject();
        m_at_view_data->tie_point.MedianNumberOfPhotosPerPoint = tie_point["MedianNumberOfPhotosPerPoint"].toString();
        m_at_view_data->tie_point.NumberOfPoints = tie_point["NumberOfPoints"].toString();
        m_at_view_data->tie_point.MedianReprojectionError = tie_point["MedianReprojectionError"].toString();
        m_at_view_data->tie_point.MedianNumberOfPointsPerPhoto = tie_point["MedianNumberOfPointsPerPhoto"].toString();
        m_at_view_data->tie_point.RMSOfReprojectionErrors = tie_point["RMSOfReprojectionErrors"].toString();
        m_at_view_data->tie_point.RMSOfDistancesToRays = tie_point["RMSOfDistancesToRays"].toString();
    }

    // project
    if (jsonObj.contains("project") && jsonObj["project"].isObject()) {
        QJsonObject project = jsonObj["project"].toObject();
        m_at_view_data->project.name                   = project["name"].toString();
        m_at_view_data->project.photo_num              = project["photo_num"].toVariant().toLongLong();
        m_at_view_data->project.data_set               = project["data_set"].toString();
        m_at_view_data->project.error_photo_num        = project["error_photo_num"].toInt();
        m_at_view_data->project.calibration_photoo_num = project["calibration_photoo_num"].toVariant().toLongLong();
        m_at_view_data->project.downsampling_rate      = project["downsampling_rate"].toDouble();
        m_at_view_data->project.photo_group_num        = project["photo_group_num"].toInt();
        m_at_view_data->project.RMS                    = project["RMS"].toString();
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

            m_at_view_data->photogroups.photogroups.append(group);
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

            m_at_view_data->photos.photos.append(rphoto);
        }
    }
	
#endif
    return true;
}

void LoadAtDataInfoThread::setAtViewFileSavePath()
{
	m_filePath = "";
    QString path = RBHelper::getTempPath(QString::number(m_taskId));
    path.append(QDir::separator());
    path.append("at_data.json");
	m_filePath_at_data = path;

	QString path_pos = RBHelper::getTempPath(QString::number(m_taskId));
	path_pos.append(QDir::separator());
	path_pos.append("image_pos.json");
	m_filePath_image_pos = path_pos;
}

void LoadAtDataInfoThread::setImagePosJsonMd5(const QString &_imagePosJson_md5)
{
	m_imagePosMd5 = _imagePosJson_md5;
}

void LoadAtDataInfoThread::onRecvResponse(QSharedPointer<ResponseHead> response)
{
    if (response->cmdType == RESTAPI_AT_POINTCLOUD_DATA_URL) {
        if (response->code == HTTP_ERROR_SUCCESS) {
            QSharedPointer<AtPointCloudDatatUrlResponse> report = qSharedPointerCast<AtPointCloudDatatUrlResponse>(response);
            if (!report->dataUrl.isEmpty()) {
                m_url_at_data = QString("%1%2").arg(CConfigDb::Inst()->getHost()).arg(report->dataUrl);		
				int npos = m_url_at_data.lastIndexOf("at_data.json");
				m_url_image_pos = m_url_at_data.mid(0, npos) + "image_pos.json";
               
                start();
                return;
            }
        }
		emit loadAtPointCloudInfoFinished(CODE_QUERY_AT_VIEW_URL_FAILED);
    }
    else if (response->cmdType == RESTAPI_CHECK_IMAGE_POS_MD5) {
        if (response->code == HTTP_ERROR_SUCCESS) {
            emit loadAtPointCloudInfoFinished(CODE_SUCCESS); // 结果相同，直接返回成功
            return;
        } 
        // 校验MD5接口失败，也要重新获取下载链接
        HttpCmdManager::getInstance()->queryAtCloudPointInfo(MyConfig.accountSet.userId, m_taskId, this);
    }
}

bool LoadAtDataInfoThread::onDownloadAtDataFinished(QNetworkReply * reply)
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
            qDebug() << "[QDownloadAtData] down file error " << m_filePath << reply->errorString();
        }

    } else {
        switch (statusCode) {
        case 301:
        case 302:
        case 303:
        case 305:
        case 307: {
            if (handleRedirect(reply))
                return false;
        }
        break;
        }
    }

    reply->abort();
    reply->deleteLater();

    return parseAtViewDataInfo(m_filePath);
}

bool LoadAtDataInfoThread::handleRedirect(QNetworkReply *reply)
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
        downloadAtViewInfo(m_url,m_filePath);
        m_retryTimes++;
        return true;
    }
    return false;
}

const QString LoadAtDataInfoThread::getHash(QString _filePath)
{
    QString sMd5;
    QFile file(_filePath);

    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray bArray = QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5);
        sMd5 = QString(bArray.toHex()).toUpper();
    }

    file.close();
    return sMd5;
}