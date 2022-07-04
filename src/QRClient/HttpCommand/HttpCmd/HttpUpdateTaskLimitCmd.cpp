#include "stdafx.h"
#include "HttpUpdateTaskLimitCmd.h"

HttpUpdateTaskLimitCmd::HttpUpdateTaskLimitCmd(qint64 taskId, const QString& limitCount, QObject *parent)
    : HttpCmdRequest(RESTAPI_JOB_UPDATE_TASK_LIMIT, parent)
    , m_taskId(taskId)
    , m_limitCount(limitCount)
{
}

HttpUpdateTaskLimitCmd::~HttpUpdateTaskLimitCmd()
{
}

QByteArray HttpUpdateTaskLimitCmd::serializeParams()
{
    QJsonObject object;
    object.insert("taskId", m_taskId);
    object.insert("taskLimit", m_limitCount);
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpUpdateTaskLimitCmd::analysisResponsed(const QByteArray& data)
{
    UpdateTaskLimitResponse* response = new UpdateTaskLimitResponse();
    response->cmdType = m_cmdType;
    response->rawData = data;
    response->taskId = m_taskId;
    response->limitCount = m_limitCount;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if (!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if (jsDoc.isObject()) {
            QJsonObject jsObj = jsDoc.object();
            response->version = jsObj["version"].toString();
            response->result = jsObj["result"].toBool();
            response->msg = jsObj["message"].toString();
            response->code = jsObj["code"].toInt();
            response->serverTime = jsObj["serverTime"].toVariant().toLongLong();
            response->data = jsObj["data"].toString();

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