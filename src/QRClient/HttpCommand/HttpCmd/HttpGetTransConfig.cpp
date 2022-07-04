#include "stdafx.h"
#include "HttpGetTransConfig.h"

HttpGetTransConfig::HttpGetTransConfig(QObject *parent)
    : HttpCmdRequest(RESTAPI_USER_TRANS_CONFIG, parent)
{
}

QByteArray HttpGetTransConfig::serializeParams()
{
    QJsonObject object = qstringToJsonObject("{}");
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpGetTransConfig::analysisResponsed(const QByteArray& data)
{
    TransConfigResponse* response = new TransConfigResponse();
    response->cmdType = m_cmdType;
    response->rawData = data;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if (!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if (jsDoc.isObject()) {
            QJsonObject jsObj = jsDoc.object();
            response->version = jsObj["version"].toString();
            response->result = jsObj["result"].toBool();
            response->msg = jsObj["message"].toString();
            response->code = jsObj["code"].toInt();
            if (jsObj["data"].isObject()) {
                QJsonObject obj = jsObj["data"].toObject();
                response->trans_config = obj;
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