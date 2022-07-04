#include "stdafx.h"
#include "HttpJobFrameGrab.h"

HttpJobFrameGrab::HttpJobFrameGrab(qint32 frameId, QObject *parent)
    : HttpCmdRequest(RESTAPI_JOB_FRAME_GRAB, parent)
    , m_frameId(frameId)
{
}

HttpJobFrameGrab::~HttpJobFrameGrab()
{
}

QByteArray HttpJobFrameGrab::serializeParams()
{
    QJsonObject object;
    object.insert("id", m_frameId);
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpJobFrameGrab::analysisResponsed(const QByteArray& data)
{
    JobFrameGrabResponse* response = new JobFrameGrabResponse;
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
            if(jsObj["data"].isArray()) {
                QJsonArray data = jsObj["data"].toArray();
                for (int i = 0; i < data.count(); i++) {
                    QJsonObject item = data[i].toObject();

                    FrameGrabInfo* info  = new FrameGrabInfo;
                    info->renderUrl = item["render"].toString();
                    info->progressUrl = item["progress"].toString();
                    response->processImgList.append(info);
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

/////////////////////////////////////////////////////////////////////

HttpJobFrameRenderingTime::HttpJobFrameRenderingTime(qint32 frameId, QObject *parent)
    : HttpCmdRequest(RESTAPI_JOB_FRAME_RENDERING_TIME, parent)
    , m_frameId(frameId)
{
}

HttpJobFrameRenderingTime::~HttpJobFrameRenderingTime()
{
}

QByteArray HttpJobFrameRenderingTime::serializeParams()
{
    QJsonObject object;
    object.insert("id", m_frameId);
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpJobFrameRenderingTime::analysisResponsed(const QByteArray& data)
{
    JobFrameRenderingTimeResponse* response = new JobFrameRenderingTimeResponse;
    response->cmdType = m_cmdType;
    response->rawData = data;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if(!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if(jsDoc.isObject()) {
            QJsonObject jsObj = jsDoc.object();
            response->version = jsObj["version"].toString();
            response->result = jsObj["result"].toBool();
            response->msg = jsObj["message"].toString();
            response->code = jsObj["code"].toInt();
            response->progressText = jsObj["data"].toString();
            response->serverTime = jsObj["serverTime"].toVariant().toLongLong();
        } else {
            qDebug() << "parse json error, document is not a jsonobject ";
        }
    } else {
        qDebug() << "parse json error, document is NULL or jsonerror is " << jsonError.errorString();
    }

    emit sendResponseFormat(QSharedPointer<ResponseHead>(response));
}