#include "stdafx.h"
#include "HttpCommand/HttpCommon.h"
#include "HttpPlatformCmd.h"

HttpPlatformCmd::HttpPlatformCmd(QObject *parent)
    : HttpCmdRequest(RESTAPI_PLATFORM_LIST, parent)
{
}

QByteArray HttpPlatformCmd::serializeParams()
{
    return QByteArray();
}

void HttpPlatformCmd::analysisResponsed(const QByteArray& data)
{
    PlatformResponse* response = new PlatformResponse();
    response->cmdType = m_cmdType;
    response->rawData = data;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if(!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if(jsDoc.isObject()) {
            QJsonObject jsObj = jsDoc.object();
            response->version = jsObj["version"].toString();
            response->result  = jsObj["result"].toBool();
            response->msg     = jsObj["message"].toString();
            response->code    = jsObj["code"].toInt();
            if(jsObj["data"].isArray()) {
                QJsonArray jsArr = jsObj["data"].toArray();
                for(int i = 0; i < jsArr.size(); i++) {
                    if(jsArr[i].isObject()) {
                        PlatformInfo info;
                        QJsonObject obj = jsArr[i].toObject();
                        //info.id = obj["id"].toInt();
                        info.id = i;
                        info.platform = obj["platform"].toInt();
                        info.name     = obj["name"].toString();
                        info.status   = obj["status"].toInt();
                        response->Platforms.append(info);
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
