#include "stdafx.h"
#include "HttpLoadingTaskPatamter.h"

HttpLoadingTaskPatamter::HttpLoadingTaskPatamter(qint64 taskId, QObject *parent)
    : HttpCmdRequest(RESTAPI_JOB_LOAD_TASK_PARAMTER, parent)
    , m_taskId(taskId)
{
}

HttpLoadingTaskPatamter::~HttpLoadingTaskPatamter()
{
}

QByteArray HttpLoadingTaskPatamter::serializeParams()
{
    QJsonObject object;
    object.insert("taskId", m_taskId);
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpLoadingTaskPatamter::analysisResponsed(const QByteArray& data)
{
    TaskParamterResponse* response = new TaskParamterResponse();
    response->cmdType = m_cmdType;
    response->rawData = data;
    response->taskId  = m_taskId;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if(!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if(jsDoc.isObject()) {
            QJsonObject jsObj        = jsDoc.object();
            response->version        = jsObj["version"].toString();
            response->result         = jsObj["result"].toBool();
            response->msg            = jsObj["message"].toString();
            response->code           = jsObj["code"].toInt();

            if(jsObj["data"].isObject()) {
                QJsonObject obj = jsObj["data"].toObject();
                response->sceneName      = obj["sceneName"].toString();
                response->taskStatusText = obj["taskStatusText"].toString();
                response->cgFilePath     = obj["cgFilePath"].toString();

                if(obj["taskParam"].isArray()) {
                    QJsonArray jsArr = obj["taskParam"].toArray();
                    for(int i = 0; i < jsArr.size(); i++) {
                        QString p = jsArr[i].toString();
                        response->taskParam.append(p);
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