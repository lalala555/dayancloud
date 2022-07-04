#include "stdafx.h"
#include "HttpBindWxToUser.h"

HttpBindWxToUser::HttpBindWxToUser(qint64 wxId, qint32 bindtype, QObject *parent)
    : HttpCmdRequest(RESTAPI_BIND_WX_TO_USER, parent)
    , m_wxId(wxId)
    , m_bindtype(bindtype)
{
}

HttpBindWxToUser::~HttpBindWxToUser()
{
}

QByteArray HttpBindWxToUser::serializeParams()
{
    QJsonObject object;
    object.insert("id", m_wxId);
    object.insert("bindType", m_bindtype);
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpBindWxToUser::analysisResponsed(const QByteArray& data)
{
    WxBindingToUserResponse* response = new WxBindingToUserResponse();
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
            response->data    = jsObj["data"].toString();
            response->serverTime = jsObj["serverTime"].toVariant().toLongLong();
        } else {
            qDebug() << "parse json error, document is not a jsonobject ";
        }
    } else {
        qDebug() << "parse json error, document is NULL or jsonerror is " << jsonError.errorString();
    }
    emit sendResponseFormat(QSharedPointer<ResponseHead>(response));
}