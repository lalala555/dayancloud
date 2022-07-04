#include "stdafx.h"
#include "HttpUpdateTaskRemarkCmd.h"

HttpUpdateTaskRemarkCmd::HttpUpdateTaskRemarkCmd(qint64 taskId, const QString& remark, QObject *parent)
    : HttpCmdRequest(RESTAPI_JOB_UPDATE_TASK_REMARK, parent)
    , m_taskId(taskId)
    , m_remark(remark)
{
}

HttpUpdateTaskRemarkCmd::~HttpUpdateTaskRemarkCmd()
{
}

QByteArray HttpUpdateTaskRemarkCmd::serializeParams()
{
    QJsonObject object;
    object.insert("taskId", m_taskId);
    object.insert("remark", m_remark);
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpUpdateTaskRemarkCmd::analysisResponsed(const QByteArray& data)
{
    UpdateTaskRemarkResponse* response = new UpdateTaskRemarkResponse();
    response->cmdType = m_cmdType;
    response->rawData = data;
    response->taskId  = m_taskId;
    response->remark  = m_remark;

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