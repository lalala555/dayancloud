#include "stdafx.h"
#include "HttpCheckPublishDuplicate.h"
#include "HttpResponseParse.h"
#include "HttpCommand/HttpCommon.h"

HttpCheckPublishDuplicate::HttpCheckPublishDuplicate(int id, QObject *parent)
	: HttpCmdRequest(RESTAPI_TASK_CHECKID,parent)
	, m_publishJobId(id)
{
}

HttpCheckPublishDuplicate::~HttpCheckPublishDuplicate()
{
}

QByteArray HttpCheckPublishDuplicate::serializeParams()
{
	QJsonObject object;
	object.insert("shellJobId", m_publishJobId);
	return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpCheckPublishDuplicate::analysisResponsed(const QByteArray& data)
{
	CheckId* response = new CheckId();
	response->cmdType = m_cmdType;
	response->rawData = data;
	QJsonParseError jsonError;
	QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
	if (!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
		if (jsDoc.isObject()) {
			QJsonObject jsObj = jsDoc.object();		
			response->code = jsObj["code"].toInt();
			QJsonObject jsdata = jsObj["data"].toObject();
			response->isDuplicate = jsdata["isDuplicate"].toInt();
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
