#include "stdafx.h"
#include "HttpCreatePublish.h"
#include "HttpResponseParse.h"
#include "HttpCommand/HttpCommon.h"

HttpCreatePublish::HttpCreatePublish(const PublishItem &item, QObject *parent)
	: HttpCmdRequest(RESTAPI_TASK_CREATEPUBLISH,parent)
	, m_task_id(item.id)
	, m_publishName(item.publishName)
	, m_coverImgUrl(item.coverImgUrl)
	, m_modelType(item.type)
	, m_desc(item.des)
	, m_isNeedkey(item.isNeedKey)
	, m_secretKey(item.secretKey)
	, m_concurrentCount(item.concurrentCount)
	, m_expiredDay(item.expiredTime)
{
}

HttpCreatePublish::~HttpCreatePublish()
{
}

QByteArray HttpCreatePublish::serializeParams()
{
	QJsonObject object;
	object.insert("taskId", m_task_id);
	object.insert("publishName", m_publishName);
	object.insert("coverImgUrl", m_coverImgUrl);
	object.insert("modelType", m_modelType);
	object.insert("des", m_desc);
	object.insert("isNeedKey", m_isNeedkey);
	object.insert("secretKey", m_secretKey);
	object.insert("concurrentCount", m_concurrentCount);
	object.insert("expiredDay", m_expiredDay);
	return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpCreatePublish::analysisResponsed(const QByteArray& data)
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
			//QDate time = jsdata["expiredTime"].toVariant().toDate();
		//	QString cutOffDate = QString::number(QDateTime::fromString(time.toString("yyyy-MM-dd"), "yyyy-MM-dd").toTime_t());
		//	QString aa = QString("%1").arg(QDate::toString(time &YYYY.mm.dd));
		//	response->item->expiredTime = jsdata["expiredTime"];
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
