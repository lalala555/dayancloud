#include "stdafx.h"
#include "HttpQueryPublishList.h"
#include "HttpResponseParse.h"

HttpQueryPublishList::HttpQueryPublishList(QString publishName, QString publishId,int status, QObject *parent)
	: HttpCmdRequest(RESTAPI_TASK_QUERYPUBLISH,parent)
	, m_publishName(publishName)
	, m_publishId(publishId)
	, m_status(status)
{
}

HttpQueryPublishList::~HttpQueryPublishList()
{
}

QByteArray HttpQueryPublishList::serializeParams()
{
	QJsonObject object;
	object.insert("publishName", m_publishName);
	object.insert("publishId", m_publishId);
	if(m_status == 0 || m_status == 1)
	object.insert("status", m_status);
	return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpQueryPublishList::analysisResponsed(const QByteArray& data)
{
	PublishItemsResponse* response = new PublishItemsResponse();
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
				if (jsObj["data"].isArray()) {
					QJsonArray jsArr = jsObj["data"].toArray();
					for (int i = 0; i < jsArr.size(); i++) {
						if (jsArr[i].isObject()) {
							QJsonObject obj = jsArr[i].toObject();
							PublishItem *item = HttpResponseParse::parsePublishJson(obj);				
							response->items.append(item);
						}
					}
				}
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
