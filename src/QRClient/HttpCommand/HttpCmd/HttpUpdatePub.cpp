#include "stdafx.h"
#include "HttpUpdatePub.h"
#include "HttpResponseParse.h"
#include "HttpCommand/HttpCommon.h"

HttpUpdatePub::HttpUpdatePub(const PublishItem &item, QObject *parent)
	: HttpCmdRequest(RESTAPI_TASK_UPDATEPUB,parent)
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

HttpUpdatePub::~HttpUpdatePub()
{
}

QByteArray HttpUpdatePub::serializeParams()
{
	QJsonObject object;
	object.insert("id", m_task_id);
	object.insert("publishName", m_publishName);
	object.insert("coverImgUrl", m_coverImgUrl);
	object.insert("modelType", m_modelType);
	object.insert("des", m_desc);
	object.insert("isNeedKey", m_isNeedkey);
	object.insert("secretKey", m_secretKey);
	object.insert("concurrentCount", m_concurrentCount);
	object.insert("expiredTime", m_expiredDay);
	return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpUpdatePub::analysisResponsed(const QByteArray& data)
{
	CreatePublish* response = new CreatePublish();
	response->cmdType = m_cmdType;
	response->rawData = data;

	QJsonParseError jsonError;
	QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
	if (!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
		if (jsDoc.isObject()) {
			QJsonObject jsdata = jsDoc.object();
			response->version = jsdata["version"].toString();
			response->result = jsdata["result"].toBool();
			response->msg = jsdata["message"].toString();
			response->code = jsdata["code"].toInt();
			//QJsonObject jsdata = jsdata["data"].toObject();
			//response->item = new PublishItem();
			//response->item->id = jsdata["id"].toInt();
			//response->item->publishName = jsdata["publishName"].toString();
			//response->item->coverImgUrl = jsdata["coverImgUrl"].toString();
			////response->item->type = enum PublishItem::modelType(jsdata["modeltype"].toInt());
			//response->item->type = enum PublishItem::modelType(jsdata["modelType"].toInt());
			//response->item->des = jsdata["des"].toString();
			//response->item->isNeedKey = jsdata["isNeedKey"].toInt();
			//response->item->secretKey = jsdata["secretKey"].toString();
			//response->item->concurrentCount = jsdata["concurrentCount"].toInt();
			//response->item->expiredTime = jsdata["expiredTime"].toVariant().toLongLong();
			//QDate time = jsdata["expiredTime"].toVariant().toDate();
		//	QString cutOffDate = QString::number(QDateTime::fromString(time.toString("yyyy-MM-dd"), "yyyy-MM-dd").toTime_t());
		//	QString aa = QString("%1").arg(QDate::toString(time &YYYY.mm.dd));
		//	response->item->expiredTime = jsdata["expiredTime"];
			//response->item->url = jsdata["url"].toString();
		
			response->serverTime = jsdata["serverTime"].toVariant().toLongLong();
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
