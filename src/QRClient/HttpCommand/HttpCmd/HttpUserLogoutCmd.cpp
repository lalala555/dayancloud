#include "stdafx.h"
#include "HttpUserLogoutCmd.h"

HttpUserLogoutCmd::HttpUserLogoutCmd(QObject *parent)
    : HttpCmdRequest(RESTAPI_USER_LOGOUT, parent)
{
}

QByteArray HttpUserLogoutCmd::serializeParams()
{
    QJsonObject object;
    object = qstringToJsonObject("{}");
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpUserLogoutCmd::analysisResponsed(const QByteArray& data)
{
    UserLogoutResponse* response = new UserLogoutResponse;
    response->cmdType   = m_cmdType;
    response->rawData   = data;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if(!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if(jsDoc.isObject()) {
            QJsonObject jsObj = jsDoc.object();
            response->version = jsObj["version"].toString();
            response->result  = jsObj["result"].toBool();
            response->msg     = jsObj["message"].toString();
            response->data    = jsObj["data"].toString();
            response->code    = jsObj["code"].toInt();
            response->serverTime = jsObj["serverTime"].toVariant().toLongLong();
        } else {
            qDebug() << "parse json error, document is not a jsonobject ";
        }
    } else {
        qDebug() << "parse json error, document is NULL or jsonerror is " << jsonError.errorString();
    }
    emit sendResponseFormat(QSharedPointer<ResponseHead>(response));
}