#include "stdafx.h"
#include "HttpUploadPic.h"
#include "HttpResponseParse.h"
#include "HttpCommand/HttpCommon.h"

HttpUploadPic::HttpUploadPic(QString filepath, QObject *parent)
	: HttpCmdRequest(RESTAPI_TASK_UPLOADPIC,parent)
	,m_filepath(filepath)
{
	//m_file = file;
}

HttpUploadPic::~HttpUploadPic()
{
}

QByteArray HttpUploadPic::serializeParams()
{
	//return m_filepath.toLocal8Bit();
	return m_filepath.toUtf8();

}

void HttpUploadPic::analysisResponsed(const QByteArray& data)
{
	UploadPic* response = new UploadPic();
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
			response->picurl = jsObj["data"].toString();
		
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
