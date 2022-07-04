#include "stdafx.h"
#include "HttpLoadTaskThumbnail.h"

HttpLoadTaskThumbnail::HttpLoadTaskThumbnail(qint64 frameId, int frameStatus, QObject *parent)
    : HttpCmdRequest(RESTAPI_JOB_TASK_THUMBNAIL, parent)
    , m_frameId(frameId)
    , m_frameStatus(frameStatus)
{
}

HttpLoadTaskThumbnail::~HttpLoadTaskThumbnail()
{
}

QByteArray HttpLoadTaskThumbnail::serializeParams()
{
    QJsonObject object;
    object.insert("id",m_frameId);
    object.insert("frameStatus",m_frameStatus);
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpLoadTaskThumbnail::analysisResponsed(const QByteArray& data)
{
    TaskThumbnailResponse* response = new TaskThumbnailResponse;
    response->rawData               = data;
    response->cmdType               = m_cmdType;
    response->frameId               = m_frameId;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if(!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if(jsDoc.isObject()) {
            QJsonObject jsObj = jsDoc.object();
            response->version = jsObj["version"].toString();
            response->result  = jsObj["result"].toBool();
            response->msg     = jsObj["message"].toString();
            response->code    = jsObj["code"].toInt();

            if(jsObj["data"].isArray()) {
                QJsonArray data = jsObj["data"].toArray();
                for (int i = 0; i < data.count(); i++){
                    QString item = data[i].toString();
                    response->thumbnails.append(item);
                }
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