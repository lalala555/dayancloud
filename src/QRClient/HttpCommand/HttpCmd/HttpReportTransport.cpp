#include "stdafx.h"
#include "HttpReportTransport.h"
#include "HttpCommand/HttpCommon.h"

HttpReportTransport::HttpReportTransport(qint64 taskId, const QJsonObject& jsonObject, QObject *parent)
    : HttpCmdRequest(RESTAPI_TRANSPORTINFO, parent)
    , m_taskId(taskId)
    , m_data(jsonObject)
{
}

QByteArray HttpReportTransport::serializeParams()
{
    QJsonObject object = m_data;
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpReportTransport::analysisResponsed(const QByteArray& data)
{
    ResponseHead* response = new ResponseHead();
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
            response->serverTime = jsObj["serverTime"].toVariant().toLongLong();
        } else {
            qDebug() << "parse json error, document is not a jsonobject ";
        }
    } else {
        qDebug() << "parse json error, document is NULL or jsonerror is " << jsonError.errorString();
    }
    emit sendResponseFormat(QSharedPointer<ResponseHead>(response));
}
