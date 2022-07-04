#include "stdafx.h"
#include "HttpRenderLog.h"

HttpRenderLog::HttpRenderLog(qint64 frameId, int pageNum, const QString& renderingType, qint32 pageSize, QObject *parent)
    : HttpCmdRequest(RESTAPI_JOB_FRAME_RENDER_LOG, parent)
    , m_frameId(frameId)
    , m_pageNum(pageNum)
    , m_pageSize(pageSize)
    , m_renderingType(renderingType)
{
}

HttpRenderLog::~HttpRenderLog()
{
}

QByteArray HttpRenderLog::serializeParams()
{
    QJsonObject object;
    object.insert("id", m_frameId);
    object.insert("pageSize", m_pageSize);
    object.insert("pageNum", m_pageNum);
    object.insert("renderingType", m_renderingType);
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpRenderLog::analysisResponsed(const QByteArray& data)
{
    TaskRenderLogResponse* response = new TaskRenderLogResponse;
    response->rawData               = data;
    response->cmdType               = m_cmdType;
    response->frameId               = m_frameId;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if(!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if(jsDoc.isObject()) {
            QJsonObject jsObj = jsDoc.object();
            response->version = jsObj["version"].toString();
            response->result  = jsObj["result"].toBool();
            response->msg     = jsObj["message"].toString();
            response->code    = jsObj["code"].toInt();

            if(jsObj["data"].isObject()) {
                QJsonObject data = jsObj["data"].toObject();

                response->pageCount = data["pageCount"].toInt();
                response->pageNum   = data["pageNum"].toInt();
                response->total     = data["total"].toVariant().toLongLong();
                response->size      = data["size"].toVariant().toLongLong();
                
                if(data["items"].isArray()){
                    QJsonArray items = data["items"].toArray();
                    for (int i = 0; i < items.count(); i++){
                        QString item = items[i].toString();
                        response->renderLogList.append(item);
                    }
                }
            }
            response->serverTime = jsObj["serverTime"].toVariant().toLongLong();
        } else {
            qDebug() << "parse json error, document is not a jsonobject ";
        }
    } else {
        qDebug() << "parse json error, document is NULL or jsonerror is " << jsonError.errorString();
    }
    emit sendResponseFormat(QSharedPointer<ResponseHead>(response));
}

//////////////////////////////////////////////////////////////////////////
HttpAtReportUrl::HttpAtReportUrl(qint64 userId, qint64 taskId, QObject *parent)
    : HttpCmdRequest(RESTAPI_AT_REPORT_DATA_URL, parent)
    , m_taskId(taskId)
    , m_userId(userId)
{

}

HttpAtReportUrl::~HttpAtReportUrl()
{

}

QByteArray HttpAtReportUrl::serializeParams()
{
    QJsonObject object;
    object.insert("userId", m_userId);
    object.insert("taskId", m_taskId);
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpAtReportUrl::analysisResponsed(const QByteArray& data)
{
    AtReportUrlResponse* response = new AtReportUrlResponse;
    response->rawData = data;
    response->cmdType = m_cmdType;
    response->taskId  = m_taskId;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if (!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if (jsDoc.isObject()) {
            QJsonObject jsObj = jsDoc.object();
            response->version = jsObj["version"].toString();
            response->result = jsObj["result"].toBool();
            response->msg = jsObj["message"].toString();
            response->code = jsObj["code"].toInt();
            response->data = jsObj["data"].toString();
            response->serverTime = jsObj["serverTime"].toVariant().toLongLong();
        }
        else {
            qDebug() << "parse json error, document is not a jsonobject ";
        }
    }
    else {
        qDebug() << "parse json error, document is NULL or jsonerror is " << jsonError.errorString();
    }
    emit sendResponseFormat(QSharedPointer<ResponseHead>(response));
}

//////////////////////////////////////////////////////////////////////////
HttpShareResultLink::HttpShareResultLink(qint64 taskId, int days, QObject *parent)
    : HttpCmdRequest(RESTAPI_SHARE_REBUILD_RESULT_LINK, parent)
    , m_taskId(taskId)
    , m_days(days)
{

}

QByteArray HttpShareResultLink::serializeParams()
{
    QJsonObject object;
    object.insert("days", m_days);
    object.insert("taskId", m_taskId);
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpShareResultLink::analysisResponsed(const QByteArray& data)
{
    ShareResultLinkResponse* response = new ShareResultLinkResponse;
    response->rawData = data;
    response->cmdType = m_cmdType;
    response->taskId  = m_taskId;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if (!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if (jsDoc.isObject()) {
            QJsonObject jsObj = jsDoc.object();
            response->version = jsObj["version"].toString();
            response->result = jsObj["result"].toBool();
            response->msg = jsObj["message"].toString();
            response->code = jsObj["code"].toInt();
            if (jsObj["data"].isObject()) {
                QJsonObject dataObj = jsObj["data"].toObject();

                response->shareUrl  = dataObj["shareUrl"].toString();
                response->shareCode = dataObj["code"].toString();
            }
            response->serverTime = jsObj["serverTime"].toVariant().toLongLong();
        } else {
            qDebug() << "parse json error, document is not a jsonobject ";
        }
    } else {
        qDebug() << "parse json error, document is NULL or jsonerror is " << jsonError.errorString();
    }
    emit sendResponseFormat(QSharedPointer<ResponseHead>(response));
}


//////////////////////////////////////////////////////////////////////////
HttpAtPointCloudDataUrl::HttpAtPointCloudDataUrl(qint64 userId, qint64 taskId, QObject *parent)
	: HttpCmdRequest(RESTAPI_AT_POINTCLOUD_DATA_URL, parent)
	, m_taskId(taskId)
	, m_userId(userId)
{

}

HttpAtPointCloudDataUrl::~HttpAtPointCloudDataUrl()
{

}

QByteArray HttpAtPointCloudDataUrl::serializeParams()
{
	QJsonObject object;
	object.insert("userId", m_userId);
	object.insert("taskId", m_taskId);
	return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpAtPointCloudDataUrl::analysisResponsed(const QByteArray& data)
{
	AtPointCloudDatatUrlResponse* response = new AtPointCloudDatatUrlResponse;
	response->rawData = data;
	response->cmdType = m_cmdType;
	response->taskId = m_taskId;

	QJsonParseError jsonError;
	QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
	if (!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
		if (jsDoc.isObject()) {
			QJsonObject jsObj = jsDoc.object();
			response->version = jsObj["version"].toString();
			response->result = jsObj["result"].toBool();
			response->msg = jsObj["message"].toString();
			response->code = jsObj["code"].toInt();

			QJsonObject jsdata = jsObj["data"].toObject();

			response->dataUrl = jsdata["dataUrl"].toString();
			response->controlPointDataUrl = jsdata["controlPointDataUrl"].toString();
			response->imagePrefix = jsdata["imagePrefix"].toString();
			response->serverTime = jsObj["serverTime"].toVariant().toLongLong();
		}
		else {
			qDebug() << "parse json error, document is not a jsonobject ";
		}
	}
	else {
		qDebug() << "parse json error, document is NULL or jsonerror is " << jsonError.errorString();
	}
	emit sendResponseFormat(QSharedPointer<ResponseHead>(response));
}


//////////////////////////////////////////////////////////////////////////
HttpCheckImagePosFileMd5::HttpCheckImagePosFileMd5(qint64 userId, qint64 taskId,QString fileMd5, QObject *parent)
	: HttpCmdRequest(RESTAPI_CHECK_IMAGE_POS_MD5, parent)
	, m_taskId(taskId)
	, m_userId(userId)
	, m_fileMd5(fileMd5)
{

}

HttpCheckImagePosFileMd5::~HttpCheckImagePosFileMd5()
{

}

QByteArray HttpCheckImagePosFileMd5::serializeParams()
{
	QJsonObject object;
	object.insert("userId", m_userId);
	object.insert("taskId", m_taskId);
	object.insert("signature", m_fileMd5);
	return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpCheckImagePosFileMd5::analysisResponsed(const QByteArray& data)
{
    AtDataVerifyResponse* response = new AtDataVerifyResponse;
	response->rawData = data;
	response->cmdType = m_cmdType;
	response->taskId = m_taskId;

	QJsonParseError jsonError;
	QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
	if (!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
		if (jsDoc.isObject()) {
			QJsonObject jsObj = jsDoc.object();
			response->version = jsObj["version"].toString();
			response->result = jsObj["result"].toBool();
			response->msg = jsObj["message"].toString();
			response->code = jsObj["code"].toInt();

			QJsonObject jsdata = jsObj["data"].toObject();
			response->resultCode = jsdata["code"].toInt();

			response->serverTime = jsObj["serverTime"].toVariant().toLongLong();
		}
		else {
			qDebug() << "parse json error, document is not a jsonobject ";
		}
	}
	else {
		qDebug() << "parse json error, document is NULL or jsonerror is " << jsonError.errorString();
	}
	emit sendResponseFormat(QSharedPointer<ResponseHead>(response));
}