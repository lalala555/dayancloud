#include "stdafx.h"
#include "HttpTaskSummaryCmd.h"

HttpTaskSummaryCmd::HttpTaskSummaryCmd(qint32 userId, QObject *parent)
    : HttpCmdRequest(RESTAPI_TASK_SUMMARY, parent)
    , m_userId(userId)
{
}

QByteArray HttpTaskSummaryCmd::serializeParams()
{
    QJsonObject object;
    object.insert("userId", m_userId);
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpTaskSummaryCmd::analysisResponsed(const QByteArray& data)
{
    TaskSummaryResponse* response = new TaskSummaryResponse();
    response->cmdType = m_cmdType;
    response->rawData = data;

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

            if(jsObj["data"].isObject()) {
                QJsonObject obj = jsObj["data"].toObject();
                response->doneTask         = obj["doneTask"].toInt();
                response->failureTask      = obj["failureTask"].toInt();
                response->renderingTask    = obj["renderingTask"].toInt();
                response->waitingTask      = obj["waitingTask"].toInt();
            }
        } else {
            qDebug() << "parse json error, document is not a jsonobject ";
        }
    } else {
        qDebug() << "parse json error, document is NULL or jsonerror is " << jsonError.errorString();
    }
    emit sendResponseFormat(QSharedPointer<ResponseHead>(response));
}