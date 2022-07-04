#include "stdafx.h"
#include "HttpUserFeedbackCmd.h"

HttpUserFeedbackCmd::HttpUserFeedbackCmd(const QString& userName, const QString& detail,
        const QString& contactWay, int type, int channel, const QString& version, QObject *parent)
    : HttpCmdRequest(RESTAPI_USER_FEEDBACK, parent)
    , m_userName(userName)
    , m_detail(detail)
    , m_contactWay(contactWay)
    , m_type(type)
    , m_channel(channel)
    , m_version(version)
{
}

HttpUserFeedbackCmd::~HttpUserFeedbackCmd()
{
}

QByteArray HttpUserFeedbackCmd::serializeParams()
{
    QJsonObject object;
    object.insert("userName", m_userName);
    object.insert("detail", m_detail);
    object.insert("contactWay", m_contactWay);
    object.insert("type", m_type);
    object.insert("channel", m_channel);
    object.insert("version", m_version);
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpUserFeedbackCmd::analysisResponsed(const QByteArray& data)
{
    UserFeedbackResponse* response = new UserFeedbackResponse();
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