#include "stdafx.h"
#include "HttpUpdateTaskLevelCmd.h"

HttpUpdateTaskLevelCmd::HttpUpdateTaskLevelCmd(qint64 taskId, const QString& level, QObject *parent)
    : HttpCmdRequest(RESTAPI_JOB_UPDATE_TASK_LEVEL, parent)
    , m_taskId(taskId)
    , m_level(level)
{
}

HttpUpdateTaskLevelCmd::~HttpUpdateTaskLevelCmd()
{
}

QByteArray HttpUpdateTaskLevelCmd::serializeParams()
{
    QJsonObject object;
    object.insert("taskId", m_taskId);
    object.insert("taskUserLevel", m_level);
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpUpdateTaskLevelCmd::analysisResponsed(const QByteArray& data)
{
    UpdateTaskLevelResponse* response = new UpdateTaskLevelResponse();
    response->cmdType = m_cmdType;
    response->rawData = data;
    response->taskId  = m_taskId;
    response->level   = m_level;

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