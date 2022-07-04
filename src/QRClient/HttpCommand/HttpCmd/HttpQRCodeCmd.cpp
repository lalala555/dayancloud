#include "stdafx.h"
#include "HttpQRCodeCmd.h"
#include "HttpCommand/HttpCommon.h"

HttpQRCodeCmd::HttpQRCodeCmd(QObject *parent)
    : HttpCmdRequest(RESTAPI_INVITE_QRCODE, parent)
{
}

QByteArray HttpQRCodeCmd::serializeParams()
{
    QJsonObject object;

    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpQRCodeCmd::analysisResponsed(const QByteArray& data)
{
    QRCodeResponse* response = new QRCodeResponse();
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
            if(jsObj["data"].isString()) {
                QString base64Data = jsObj["data"].toString();
                response->qrCodeData = QByteArray::fromBase64(base64Data.toLatin1());
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
