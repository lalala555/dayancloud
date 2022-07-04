#include "stdafx.h"
#include "HttpUserStorageCmd.h"

HttpUserStorageCmd::HttpUserStorageCmd(QObject *parent)
    : HttpCmdRequest(RESTAPI_USER_STORAGE, parent)
{
}

QByteArray HttpUserStorageCmd::serializeParams()
{
    QJsonObject object;
    object = qstringToJsonObject("{}");
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpUserStorageCmd::analysisResponsed(const QByteArray& data)
{
    UserStorageResponse* response = new UserStorageResponse();
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
                QJsonObject obj       = jsObj["data"].toObject();
                response->allCapacity = obj["allCapacity"].toVariant().toLongLong();
            }
        } else {
            qDebug() << "parse json error, document is not a jsonobject ";
        }
    } else {
        qDebug() << "parse json error, document is NULL or jsonerror is " << jsonError.errorString();
    }
    emit sendResponseFormat(QSharedPointer<ResponseHead>(response));
}