#include "stdafx.h"
#include "HttpFrameSummaryCmd.h"

HttpFrameSummaryCmd::HttpFrameSummaryCmd(QObject *parent)
    : HttpCmdRequest(RESTAPI_JOB_TASK_TOTAL_INFO, parent)
{
}

QByteArray HttpFrameSummaryCmd::serializeParams()
{
    QJsonObject object;
    object = qstringToJsonObject("{}");
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpFrameSummaryCmd::analysisResponsed(const QByteArray& data)
{
    TotalTaskFrameInfoResponse* response = new TotalTaskFrameInfoResponse();
    response->cmdType = m_cmdType;
    response->rawData = data;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if(!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if(jsDoc.isObject()) {
            QJsonObject jsObj    = jsDoc.object();
            response->version    = jsObj["version"].toString();
            response->result     = jsObj["result"].toBool();
            response->msg        = jsObj["message"].toString();
            response->code       = jsObj["code"].toInt();
            response->serverTime = jsObj["serverTime"].toVariant().toLongLong();

            if(jsObj["data"].isObject()) {
                QJsonObject obj                 = jsObj["data"].toObject();
                response->executingFramesTotal  = obj["executingFramesTotal"].toVariant().toLongLong();
                response->doneFramesTotal       = obj["doneFramesTotal"].toVariant().toLongLong();
                response->failedFramesTotal     = obj["failedFramesTotal"].toVariant().toLongLong();
                response->totalFrames           = obj["totalFrames"].toVariant().toLongLong();
                response->waitingFramesTotal    = obj["waitingFramesTotal"].toVariant().toLongLong();
            }
        } else {
            qDebug() << "parse json error, document is not a jsonobject ";
        }
    } else {
        qDebug() << "parse json error, document is NULL or jsonerror is " << jsonError.errorString();
    }
    emit sendResponseFormat(QSharedPointer<ResponseHead>(response));
}

////////////////////////////////////////////////////////////////////////////////////

HttpTaskRenderingConsume::HttpTaskRenderingConsume(qint64 jobId, QObject *parent)
    : HttpCmdRequest(RESTAPI_JOB_FRAME_RENDER_CONSUME, parent)
    , m_jobId(jobId)
{
}

QByteArray HttpTaskRenderingConsume::serializeParams()
{
    QJsonObject object;
    object.insert("taskId", m_jobId);
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpTaskRenderingConsume::analysisResponsed(const QByteArray& data)
{
    JobConsumeResponse* response = new JobConsumeResponse();
    response->cmdType = m_cmdType;
    response->rawData = data;
    response->taskId  = m_jobId;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if(!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if(jsDoc.isObject()) {
            QJsonObject jsObj        = jsDoc.object();
            response->version        = jsObj["version"].toString();
            response->result         = jsObj["result"].toBool();
            response->msg            = jsObj["message"].toString();
            response->code           = jsObj["code"].toInt();

            if(jsObj["data"].isObject()) {
                QJsonObject obj = jsObj["data"].toObject();
                response->userAccountConsume    = obj["userAccountConsume"].toDouble(0.0);
                response->couponConsume         = obj["couponConsume"].toDouble(0.0);
                response->frameTimeConsumingAve = obj["frameTimeConsumingAve"].toInt(0);
                response->taskArrearage         = obj["taskArrearage"].toInt(0);
                response->taskTimeConsuming     = obj["taskTimeConsuming"].toInt(0);
                response->totalFrames           = obj["totalFrames"].toInt(0);
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
