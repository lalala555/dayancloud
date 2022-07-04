#include "stdafx.h"
#include "HttpQueryBindWx.h"

HttpQueryBindWx::HttpQueryBindWx(QObject *parent)
    : HttpCmdRequest(RESTAPI_QUERY_BINDING_WX, parent)
{
}

HttpQueryBindWx::~HttpQueryBindWx()
{
}

QByteArray HttpQueryBindWx::serializeParams()
{
    QJsonObject object;
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpQueryBindWx::analysisResponsed(const QByteArray& data)
{
    QueryBindinWxResponse* response = new QueryBindinWxResponse();
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

            if(jsObj["data"].isObject() && !jsObj["data"].isNull()) {
                QJsonObject obj = jsObj["data"].toObject();
                response->id          = obj["id"].toVariant().toLongLong();
                response->createDate  = obj["createDate"].toVariant().toLongLong();
                response->fromWhere   = obj["fromWhere"].toInt();
                response->headimgurl  = obj["headimgurl"].toString();
                response->isAdmin     = obj["isAdmin"].toInt();
                response->isBind      = obj["isBind"].toInt();
                response->isDelete    = obj["isDelete"].toInt();
                response->nickname    = obj["nickname"].toString();
                response->openId      = obj["openId"].toString();
                response->shareSource = obj["shareSource"].toString();
                response->unionId     = obj["unionId"].toString();
                response->updateDate  = obj["updateDate"].toVariant().toLongLong();
                response->userId      = obj["userId"].toVariant().toLongLong();
                response->wechatId    = obj["wechatId"].toString();
                response->isWxScanCode = true;
            }else{
                response->isWxScanCode = false;
            }
        } else {
            qDebug() << "parse json error, document is not a jsonobject ";
        }
    } else {
        qDebug() << "parse json error, document is NULL or jsonerror is " << jsonError.errorString();
    }
    emit sendResponseFormat(QSharedPointer<ResponseHead>(response));
}