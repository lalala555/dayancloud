#include "stdafx.h"
#include "HttpRecentTaskCmd.h"

HttpRecentTaskCmd::HttpRecentTaskCmd(qint64 userId, QObject *parent)
    : HttpCmdRequest(RESTAPI_RECENT_TASK, parent)
    , m_userId(userId)
{
}

QByteArray HttpRecentTaskCmd::serializeParams()
{
    QJsonObject object;
    object.insert("cgId", m_userId);
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpRecentTaskCmd::analysisResponsed(const QByteArray& data)
{
    RecentTaskDataResponse* response = new RecentTaskDataResponse();
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

            if(jsObj["data"].isArray()) {
                QJsonArray obj = jsObj["data"].toArray();
                for (int i = 0; i < obj.count(); i++){
                    QJsonObject data = obj[i].toObject();
                    RecentTaskData recent;

                    recent.date      = data["date"].toString();
                    recent.taskCount = data["taskCount"].toInt();

                    response->recentDatas.append(recent);
                }
            }
        } else {
            qDebug() << "parse json error, document is not a jsonobject ";
        }
    } else {
        qDebug() << "parse json error, document is NULL or jsonerror is " << jsonError.errorString();
    }
    emit sendResponseFormat(QSharedPointer<ResponseHead>(response));
}