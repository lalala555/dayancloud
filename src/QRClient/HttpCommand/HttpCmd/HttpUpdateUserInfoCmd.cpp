#include "stdafx.h"
#include "HttpUpdateUserInfoCmd.h"

HttpUpdateUserInfoCmd::HttpUpdateUserInfoCmd(qint32 taskOverTime, int userid, QObject *parent)
    : HttpCmdRequest(RESTAPI_UPDATE_USER_INFO, parent)
    , m_taskOverTime(taskOverTime)
    , m_userId(userid)
{
}

HttpUpdateUserInfoCmd::~HttpUpdateUserInfoCmd()
{
}

QByteArray HttpUpdateUserInfoCmd::serializeParams()
{
    QJsonObject object;
    // object.insert("taskOverTime", m_taskOverTime); 
    object.insert("taskOverTimeSec", m_taskOverTime);
    object.insert("userId", m_userId);
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpUpdateUserInfoCmd::analysisResponsed(const QByteArray& data)
{
    UpdateUserInfoResponse* response = new UpdateUserInfoResponse();
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
            response->data       = jsObj["data"].toString();
        } else {
            qDebug() << "parse json error, document is not a jsonobject ";
        }
    } else {
        qDebug() << "parse json error, document is NULL or jsonerror is " << jsonError.errorString();
    }
    emit sendResponseFormat(QSharedPointer<ResponseHead>(response));
}