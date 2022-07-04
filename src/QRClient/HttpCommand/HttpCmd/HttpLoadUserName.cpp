#include "stdafx.h"
#include "HttpLoadUserName.h"

HttpLoadUserName::HttpLoadUserName(QObject *parent)
    : HttpCmdRequest(RESTAPI_LOADING_USER_NAME, parent)
{
}

HttpLoadUserName::~HttpLoadUserName()
{
}

QByteArray HttpLoadUserName::serializeParams()
{   
    QJsonObject object;
    object = qstringToJsonObject("{}");
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpLoadUserName::analysisResponsed(const QByteArray& data)
{
    LoadUserNameResponse* response = new LoadUserNameResponse;
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
            if(jsObj["data"].isArray()){
                QJsonArray names = jsObj["data"].toArray();
                for(int i = 0; i < names.count(); i++){
                    QJsonObject name   = names[i].toObject();
                    UserName *username = new UserName;
                    username->userId   = name["userId"].toVariant().toLongLong();
                    username->userName = name["userName"].toString();
                    response->userNames.append(username);
                }
            }
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