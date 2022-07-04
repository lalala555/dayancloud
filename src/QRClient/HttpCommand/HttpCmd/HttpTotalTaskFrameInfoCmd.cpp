#include "stdafx.h"
#include "HttpTotalTaskFrameInfoCmd.h"

HttpTotalTaskFrameInfoCmd::HttpTotalTaskFrameInfoCmd(QObject *parent)
    : HttpCmdRequest(RESTAPI_JOB_TASK_TOTAL_INFO, parent)
{
    this->initCmdProperty();
}

QByteArray HttpTotalTaskFrameInfoCmd::serializeParams()
{
    QJsonObject objct;
    objct = qstringToJsonObject("{}");
    return QJsonDocument(objct).toJson(QJsonDocument::Compact);
}

void HttpTotalTaskFrameInfoCmd::analysisResponsed(const QByteArray& data)
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