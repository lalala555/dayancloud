#include "stdafx.h"
#include "HttpGetPlatformStatus.h"

HttpGetPlatformStatus::HttpGetPlatformStatus(qint32 zone, QObject *parent)
    : HttpCmdRequest(RESTAPI_TASK_PLATFORM_STATUS, parent)
    , m_zone(zone)
{
}

HttpGetPlatformStatus::~HttpGetPlatformStatus()
{
}

QByteArray HttpGetPlatformStatus::serializeParams()
{
    QJsonObject object;
    object.insert("zone", m_zone + 1);
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpGetPlatformStatus::analysisResponsed(const QByteArray& data)
{
    PlatformStatusResponse* response = new PlatformStatusResponse;
    response->rawData = data;
    response->cmdType = m_cmdType;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if (!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if (jsDoc.isObject()) {
            QJsonObject jsObj = jsDoc.object();
            response->version = jsObj["version"].toString();
            response->result  = jsObj["result"].toBool();
            response->msg     = jsObj["message"].toString();
            response->code    = jsObj["code"].toInt();
            if (jsObj["data"].isArray()) {
                QJsonArray dataArr = jsObj["data"].toArray();
                for(int i = 0; i < dataArr.size(); i++){
                    if(dataArr[i].isObject()){
                        QJsonObject data = dataArr[i].toObject();
                        PlatformStatus* status = new PlatformStatus;
                        status->platform   = data["platform"].toInt();
                        status->status     = data["status"].toInt();
                        status->type       = data["type"].toInt();
                        status->name       = data["name"].toString();
                        status->taskPrefix = data["taskPrefix"].toString();

                        response->platforms.append(status);
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