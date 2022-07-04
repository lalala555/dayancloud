#include "stdafx.h"
#include "HttpGetTransBid.h"

HttpGetTransBid::HttpGetTransBid(QObject *parent)
    : HttpCmdRequest(RESTAPI_USER_TRANS_BID, parent)
{
}

QByteArray HttpGetTransBid::serializeParams()
{
    QJsonObject object = qstringToJsonObject("{}");
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpGetTransBid::analysisResponsed(const QByteArray& data)
{
    TransBidResponse* response = new TransBidResponse();
    response->cmdType = m_cmdType;
    response->rawData = data;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if(!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if(jsDoc.isObject()) {
            QJsonObject jsObj = jsDoc.object();
            int status = jsObj["status"].toInt(0);
            if(status == 0) {
                response->version = jsObj["version"].toString();
                response->result  = jsObj["result"].toBool();
                response->msg     = jsObj["message"].toString();
                response->code    = jsObj["code"].toInt();
                if(jsObj["data"].isObject()) {
                    QJsonObject obj = jsObj["data"].toObject();
                    response->config_bid       = obj["config_bid"].toString();
                    response->input_bid        = obj["input_bid"].toString();
                    response->output_bid       = obj["output_bid"].toString();
                    response->parent_input_bid = obj["parent_input_bid"].toString(response->input_bid);
                }
                response->serverTime = jsObj["serverTime"].toVariant().toLongLong();
            } else {
                response->msg  = jsObj["message"].toString();
                response->code = jsObj["status"].toInt();
            }
        } else {
            qDebug() << "parse json error, document is not a jsonobject ";
        }
    } else {
        qDebug() << "parse json error, document is NULL or jsonerror is " << jsonError.errorString();
    }
    emit sendResponseFormat(QSharedPointer<ResponseHead>(response));
}