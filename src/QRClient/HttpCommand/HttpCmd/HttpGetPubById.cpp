#include "stdafx.h"
#include "HttpGetPubById.h"
#include "HttpResponseParse.h"

HttpGetPubById::HttpGetPubById(int id, QObject *parent)
	: HttpCmdRequest(RESTAPI_TASK_GETPUBLISHBYID,parent)
	, m_publishId(id)
{
}

HttpGetPubById::~HttpGetPubById()
{
}

QByteArray HttpGetPubById::serializeParams()
{
	QJsonObject object;
	object.insert("id", m_publishId);
	return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpGetPubById::analysisResponsed(const QByteArray& data)
{
	CreatePublish* response = new CreatePublish();
	response->cmdType = m_cmdType;
	response->rawData = data;

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
			response->item = new PublishItem();
			response->item->id = jsdata["id"].toInt();
			response->item->publishName = jsdata["publishName"].toString();
			response->item->coverImgUrl = jsdata["coverImgUrl"].toString();
			//response->item->type = enum PublishItem::modelType(jsdata["modeltype"].toInt());
			response->item->type = enum PublishItem::modelType(jsdata["modelType"].toInt());
			response->item->des = jsdata["des"].toString();
			response->item->isNeedKey = jsdata["isNeedKey"].toInt();
			response->item->secretKey = jsdata["secretKey"].toString();
			response->item->concurrentCount = jsdata["concurrentCount"].toInt();
			response->item->expiredTime = jsdata["expiredTime"].toVariant().toLongLong();
			response->item->url = jsdata["url"].toString();
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
