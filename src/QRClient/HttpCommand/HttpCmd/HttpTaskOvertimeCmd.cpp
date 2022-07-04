#include "stdafx.h"
#include "HttpTaskOvertimeCmd.h"

HttpTaskOvertimeCmd::HttpTaskOvertimeCmd(qint64 taskId, qint32 overtime, QObject *parent)
    : HttpCmdRequest(RESTAPI_JOB_UPDATE_TASK_OVERTIME, parent)
    , m_taskId(taskId)
    , m_overtime(overtime)
{
}

QByteArray HttpTaskOvertimeCmd::serializeParams()
{
    QJsonObject object;
    object.insert("taskId", m_taskId);
    object.insert("taskOverTime", m_overtime);
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpTaskOvertimeCmd::analysisResponsed(const QByteArray& data)
{
    UpdateTaskOvertimeResponse* response = new UpdateTaskOvertimeResponse();
    response->cmdType   = m_cmdType;
    response->rawData   = data;
    response->taskId    = m_taskId;
    response->overtime  = m_overtime;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if(!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if(jsDoc.isObject()) {
            QJsonObject jsObj = jsDoc.object();
            response->version    = jsObj["version"].toString();
            response->result     = jsObj["result"].toBool();
            response->msg        = jsObj["message"].toString();
            response->code       = jsObj["code"].toInt();
            response->serverTime = jsObj["serverTime"].toVariant().toLongLong();
            response->data       = jsObj["data"].toString();

        } else {
            qDebug() << "parse json error, document is not a jsonobject ";
        }
    } else {
        qDebug() << "parse json error, document is NULL or jsonerror is " << jsonError.errorString();
    }
    emit sendResponseFormat(QSharedPointer<ResponseHead>(response));
}


HttpTaskOvertimeStopCmd::HttpTaskOvertimeStopCmd(qint64 taskId, qint32 overtime, QObject *parent)
    : HttpCmdRequest(RESTAPI_JOB_UPDATE_TASK_TIMEOUT, parent)
    , m_taskId(taskId)
    , m_overtime(overtime)
{
}

QByteArray HttpTaskOvertimeStopCmd::serializeParams()
{
    QJsonObject object;
    QJsonArray jsArray;
    jsArray.append(m_taskId);
    object.insert("taskIds", jsArray);
    object.insert("overTime", m_overtime);
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpTaskOvertimeStopCmd::analysisResponsed(const QByteArray& data)
{
    UpdateTaskOvertimeResponse* response = new UpdateTaskOvertimeResponse();
    response->cmdType = m_cmdType;
    response->rawData = data;
    response->taskId = m_taskId;
    response->overtime = m_overtime;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if (!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if (jsDoc.isObject()) {
            QJsonObject jsObj = jsDoc.object();
            response->version    = jsObj["version"].toString();
            response->result     = jsObj["result"].toBool();
            response->msg        = jsObj["message"].toString();
            response->code       = jsObj["code"].toInt();
            response->serverTime = jsObj["serverTime"].toVariant().toLongLong();
        } else {
            qDebug() << "parse json error, document is not a jsonobject ";
        }
    } else {
        qDebug() << "parse json error, document is NULL or jsonerror is " << jsonError.errorString();
    }
    emit sendResponseFormat(QSharedPointer<ResponseHead>(response));
}