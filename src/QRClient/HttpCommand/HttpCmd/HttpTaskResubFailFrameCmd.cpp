#include "stdafx.h"
#include "HttpTaskResubFailFrameCmd.h"

HttpTaskResubFailFrameCmd::HttpTaskResubFailFrameCmd(int resubStatus, qint64 taskId, QObject *parent)
    : HttpCmdRequest(RESTAPI_JOB_TASK_FAILED_FRAME_RECOMMIT, parent)
{
    m_resubStatus.append(resubStatus);
    m_taskIds.append(taskId);
	m_option = "";
}

HttpTaskResubFailFrameCmd::HttpTaskResubFailFrameCmd(const QList<int>& resubStatus, const QList<qint64>& taskIds, QObject *parent)
	: HttpCmdRequest(RESTAPI_JOB_TASK_FAILED_FRAME_RECOMMIT, parent)
	, m_taskIds(taskIds)
	, m_resubStatus(resubStatus)
	, m_option("")
{
}

void HttpTaskResubFailFrameCmd::setSurveyPointOption(const QString& filePath)  //控制点提交空三默认使用控制点做平差提交
{
	m_option = "at_survey";
	m_atType = 1;  // 0使用GPS/POS做刚体变换
				   // 1使用控制点做平差
	m_surveyPointFile = filePath;
}

QByteArray HttpTaskResubFailFrameCmd::serializeParams()
{
    QJsonObject object;
    QJsonArray jsArray;
    QJsonArray resubArr;
    foreach(qint64 taskId, m_taskIds){
        jsArray.append(taskId);
    }
    foreach (int type , m_resubStatus){
        resubArr.append(type);
    }
    object.insert("status", resubArr);
    object.insert("taskIds", jsArray);

#if 0
	QJsonObject optionParam;
	object.insert("option", m_option);

	if (m_option == "at_survey")
	{
		QJsonObject surveyParam;
		if (QFileInfo::exists(m_surveyPointFile)) {
			QFile surveyFile(m_surveyPointFile);
			if (surveyFile.open(QIODevice::ReadOnly)) {
				QString content = surveyFile.readAll().toBase64();
				surveyParam.insert("fileSize", surveyFile.size());
				surveyParam.insert("fileContent", content);
			}
		}
		optionParam.insert("atType", m_atType);
		optionParam["control_point_content"] = surveyParam;
	}
	object.insert("param", optionParam);

#endif

    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpTaskResubFailFrameCmd::analysisResponsed(const QByteArray& data)
{

	qDebug() << data;
// 	QJsonDocument doc = QJsonDocument::fromJson(data);
// 	QJsonObject  obj = doc.object();

    FailFrameResubOperateResponse* response = new FailFrameResubOperateResponse();
    response->cmdType = m_cmdType;
    response->rawData = data;
    response->taskIds     = m_taskIds;
    response->resubStatus = m_resubStatus;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if(!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if(jsDoc.isObject()) {
            QJsonObject jsObj = jsDoc.object();
            response->version = jsObj["version"].toString();
            response->result  = jsObj["result"].toBool();
            response->msg     = jsObj["message"].toString();
            response->code    = jsObj["code"].toInt();
            if(!jsObj["data"].isNull() && jsObj["data"].isArray()) {
                QJsonArray jsArr = jsObj["data"].toArray();
                for(int i = 0; i < jsArr.size(); i++) {
                    if(jsArr[i].isObject()) {
                        QJsonObject obj = jsArr[i].toObject();
                        resubOptData rpsData;
                        rpsData.munuTaskId = obj["munuTaskId"].toString();
                        rpsData.taskStatus = obj["taskStatus"].toInt();
                        response->datas.append(rpsData);
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
HttpUploadJsonFileCmd::HttpUploadJsonFileCmd(qint64 taskId, const QString& filePath, QObject *parent)
    : HttpCmdRequest(RESTAPI_UPLOAD_JSON_FILE_URL, parent)
    , m_taskId(taskId)
    , m_filePath(filePath)
{
}

QByteArray HttpUploadJsonFileCmd::serializeParams()
{
	QString content;
	QFile file(m_filePath);
	if (file.open(QIODevice::ReadOnly)) {	
		content = file.readAll();
		file.close();
	}

	QJsonObject object;
	object.insert("taskId", m_taskId);
	object.insert("fileName", QFileInfo(m_filePath).fileName());
	object.insert("content", content);
	return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpUploadJsonFileCmd::analysisResponsed(const QByteArray& data)
{
    ReportLogResponse* response = new ReportLogResponse();
    response->cmdType  = m_cmdType;
    response->rawData  = data;
    response->taskId   = m_taskId;
    response->filePath = m_filePath;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if (!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if (jsDoc.isObject()) {
            QJsonObject jsObj = jsDoc.object();
            response->version = jsObj["version"].toString();
            response->result = jsObj["result"].toBool();
            response->msg = jsObj["message"].toString();
            response->code = jsObj["code"].toInt();
            response->data = jsObj["data"].toInt();
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