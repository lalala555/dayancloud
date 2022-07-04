#include "stdafx.h"
#include "HttpSubTaskCmd.h"

HttpSubTaskCmd::HttpSubTaskCmd(qint64 taskId, const QString& artist, const QString& sceneName, QObject *parent)
    : HttpCmdRequest(RESTAPI_JOB_SUBMIT, parent)
    , m_taskId(taskId)
    , m_artist(artist)
    , m_sceneName(sceneName)
    , m_submitTime(QDateTime::currentDateTime())
    , m_option("render")
{
}

void HttpSubTaskCmd::setATOption(int type)
{
    m_option = "AT";
    m_atType = type;
}

void HttpSubTaskCmd::setRebuildOption(const RebuildOptionParams& params)
{
    m_option = "rebuild";
    m_params = params;
}

void HttpSubTaskCmd::setSurveyPointOption(const QString& filePath)  //控制点提交空三默认使用控制点做平差提交
{
    m_option = "at_survey";
	m_atType = 1;  // 0使用GPS/POS做刚体变换
				   // 1使用控制点做平差
    m_surveyPointFile = filePath;
}

QByteArray HttpSubTaskCmd::serializeParams()
{
    QJsonObject object;
    object.insert("taskId", m_taskId);
    object.insert("producer", m_artist);
    object.insert("sceneName", m_sceneName);
    object.insert("async", false);

    QJsonObject optionParam;
    object.insert("option", m_option);
    if(m_option == "AT") {  // 空三
        optionParam.insert("atType", m_atType);
    } else if(m_option == "rebuild") { // 重建
        optionParam.insert("worldCoordSys", m_params.coordSys);
        optionParam.insert("lockRange", m_params.lockRange);
        if (!m_params.usrCoordContent.isEmpty()) {
            optionParam.insert("userCoordSys", m_params.usrCoordContent);
        }

        QJsonArray jsArray;
        foreach(auto type, m_params.outputTypes)
        {
            jsArray.append(type);
        }
        optionParam.insert("outputType", jsArray);

        QJsonObject rangeParam;
        if(m_params.lockRange == 0) {
            QFile kmlFile(m_params.rangeFile);
            if(kmlFile.open(QIODevice::ReadOnly)) {
                QString content = kmlFile.readAll().toBase64();
                rangeParam.insert("fileName", kmlFile.fileName());
                rangeParam.insert("fileSize", kmlFile.size());
                rangeParam.insert("fileContent", content);
            }
        }
        optionParam.insert("rangeFile", rangeParam);

        // 自定义原点
        if (m_params.originCoord.enable) {
            QJsonObject originCoord;
            originCoord.insert("coord_x", QString::number(m_params.originCoord.coord_x, 'f', 6));
            originCoord.insert("coord_y", QString::number(m_params.originCoord.coord_y, 'f', 6));
            originCoord.insert("coord_z", QString::number(m_params.originCoord.coord_z, 'f', 6));

            optionParam.insert("originCoord", originCoord);
        }

		// 成果坐标偏移
		if (m_params.offsetCoord.enable) {
			QJsonObject xyzoffset;
			xyzoffset.insert("coord_x", QString::number(m_params.offsetCoord.coord_x, 'f', 6));
			xyzoffset.insert("coord_y", QString::number(m_params.offsetCoord.coord_y, 'f', 6));
			xyzoffset.insert("coord_z", QString::number(m_params.offsetCoord.coord_z, 'f', 6));

			optionParam.insert("offsetCoord", xyzoffset);
		}

        // 瓦片大小
        optionParam.insert("tileMode", QString::number(m_params.tileMode));
        optionParam.insert("tileSize", m_params.tileSize);

    } else if (m_option == "at_survey") {
        QJsonObject surveyParam;
        if (QFileInfo::exists(m_surveyPointFile)) {
            QFile surveyFile(m_surveyPointFile);
            if (surveyFile.open(QIODevice::ReadOnly)) {
                QString content = surveyFile.readAll().toBase64();
//                surveyParam.insert("fileName", surveyFile.fileName());
                surveyParam.insert("fileSize", surveyFile.size());
                surveyParam.insert("fileContent", content);
            }
        }
		optionParam.insert("atType", m_atType);
        optionParam["control_point_content"] = surveyParam;
    }
    object.insert("param", optionParam);

    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpSubTaskCmd::analysisResponsed(const QByteArray& data)
{
    TaskSubmitResponse* response = new TaskSubmitResponse;
    response->cmdType   = m_cmdType;
    response->taskId    = m_taskId;
    response->rawData   = data;
    response->option    = m_option;
    qDebug() << data;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if(!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if(jsDoc.isObject()) {
            QJsonObject jsObj = jsDoc.object();
            response->version = jsObj["version"].toString();
            response->result  = jsObj["result"].toBool();
            response->msg     = jsObj["message"].toString();
            response->data    = jsObj["data"].toString();
            response->code    = jsObj["code"].toInt();
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
HttpRecommitCCAT::HttpRecommitCCAT(qint64 taskId, int type, QObject *parent)
    : HttpCmdRequest(RESTAPI_JOB_RESUBMIT_CCAT, parent)
    , m_taskId(taskId)
    , m_atType(type)
{

}

QByteArray HttpRecommitCCAT::serializeParams()
{
    QJsonObject object;
    QJsonArray taskIds;
    taskIds.append(QString::number(m_taskId));
    object.insert("taskIds", taskIds);

    QJsonObject param;
    param["atType"] = m_atType;
    object.insert("param", param);

    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpRecommitCCAT::analysisResponsed(const QByteArray& data)
{
    TaskSubmitCCATResponse* response = new TaskSubmitCCATResponse;
    response->cmdType = m_cmdType;
    response->taskId = m_taskId;
    response->rawData = data;
    qDebug() << data;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if (!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if (jsDoc.isObject()) {
            QJsonObject jsObj = jsDoc.object();
            response->version = jsObj["version"].toString();
            response->result = jsObj["result"].toBool();
            response->msg = jsObj["message"].toString();
            response->data = jsObj["data"].toString();
            response->code = jsObj["code"].toInt();
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
HttpCloneTask::HttpCloneTask(qint64 taskId, const QString& artist, const QString& projectName, const RebuildOptionParams& params, QObject *parent)
    : HttpCmdRequest(RESTAPI_JOB_SUBMIT_CLONE_TASK, parent)
    , m_taskId(taskId)
    , m_artist(artist)
    , m_params(params)
    , m_projectName(projectName)
{

}

QByteArray HttpCloneTask::serializeParams()
{
    QJsonObject object;
    object.insert("taskId", m_taskId);
    object.insert("producer", m_artist);
    object.insert("timeOut", 43200);
    object.insert("projectName", m_projectName);

    QJsonObject optionParam;
    optionParam.insert("worldCoordSys", m_params.coordSys);
    optionParam.insert("lockRange", m_params.lockRange);
    if (!m_params.usrCoordContent.isEmpty()) {
        optionParam.insert("userCoordSys", m_params.usrCoordContent);
    }

    QJsonArray jsArray;
    foreach(auto type, m_params.outputTypes){
        jsArray.append(type);
    }
    optionParam.insert("outputType", jsArray);

    QJsonObject rangeParam;
    if (m_params.lockRange == 0) {
        QFile kmlFile(m_params.rangeFile);
        if (kmlFile.open(QIODevice::ReadOnly)) {
            QString content = kmlFile.readAll().toBase64();
            rangeParam.insert("fileName", kmlFile.fileName());
            rangeParam.insert("fileSize", kmlFile.size());
            rangeParam.insert("fileContent", content);
        }
    }
    optionParam.insert("rangeFile", rangeParam);

    // 自定义原点
    if (m_params.originCoord.enable) {
        QJsonObject originCoord;
		originCoord.insert("coord_x", QString::number(m_params.originCoord.coord_x, 'f', 6));
        originCoord.insert("coord_y", QString::number(m_params.originCoord.coord_y, 'f', 6));
        originCoord.insert("coord_z", QString::number(m_params.originCoord.coord_z, 'f', 6));

        optionParam.insert("originCoord", originCoord);
    }

    // 瓦片大小
    optionParam.insert("tileMode", QString::number(m_params.tileMode));
    optionParam.insert("tileSize", m_params.tileSize);

    object.insert("contextCaptureParams", optionParam);

    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpCloneTask::analysisResponsed(const QByteArray& data)
{
    TaskCloneResponse* response = new TaskCloneResponse;
    response->cmdType = m_cmdType;
    response->taskId = m_taskId;
    response->rawData = data;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if (!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if (jsDoc.isObject()) {
            QJsonObject jsObj = jsDoc.object();
            response->version = jsObj["version"].toString();
            response->result = jsObj["result"].toBool();
            response->msg = jsObj["message"].toString();
            response->data = jsObj["data"].toString();
            response->code = jsObj["code"].toInt();
            response->serverTime = jsObj["serverTime"].toVariant().toLongLong();
        } else {
            qDebug() << "parse json error, document is not a jsonobject ";
        }
    } else {
        qDebug() << "parse json error, document is NULL or jsonerror is " << jsonError.errorString();
    }
    emit sendResponseFormat(QSharedPointer<ResponseHead>(response));
}