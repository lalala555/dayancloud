#include "stdafx.h"
#include "HttpDeletePublish.h"
#include "HttpResponseParse.h"
#include "HttpCommand/HttpCommon.h"

HttpDeletePublish::HttpDeletePublish(int id, QObject *parent)
	: HttpCmdRequest(RESTAPI_TASK_DETELEPUBLISH,parent)
	, m_publishid(id)
{
}

HttpDeletePublish::~HttpDeletePublish()
{
}

QByteArray HttpDeletePublish::serializeParams()
{
	QJsonObject object;
	object.insert("id", m_publishid);
	return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpDeletePublish::analysisResponsed(const QByteArray& data)
{
	ResponseHead* response = new ResponseHead();
	response->cmdType = m_cmdType;
	response->rawData = data;
	QJsonParseError jsonError;
	QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
	if (!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
		if (jsDoc.isObject()) {
			QJsonObject jsObj = jsDoc.object();		
			response->code = jsObj["code"].toInt();
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
