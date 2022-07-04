#include "stdafx.h"
#include "HttpQueryPublishTask.h"
#include "HttpResponseParse.h"
#include "HttpCommand/HttpCommon.h"

HttpQueryPublishTask::HttpQueryPublishTask(int id, QObject *parent)
	: HttpCmdRequest(RESTAPI_TASK_QUERYPUBTASK,parent)
	, m_task_id(id)
	
{
}

HttpQueryPublishTask::~HttpQueryPublishTask()
{
}

QByteArray HttpQueryPublishTask::serializeParams()
{
	QJsonObject object;
	object.insert("publishId", m_task_id);
	return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpQueryPublishTask::analysisResponsed(const QByteArray& data)
{
	PublishTaskResponse* response = new PublishTaskResponse();
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
			response->code = jsObj["code"].toInt();
			if (jsObj["data"].isArray()) {
				QJsonArray jsArr = jsObj["data"].toArray();
				for (int i = 0; i < jsArr.size(); i++) {
					if (jsArr[i].isObject()) {
						QJsonObject obj = jsArr[i].toObject();
						PubTask *task = HttpResponseParse::parsePubTask(obj);
						response->tasks.append(task);
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
