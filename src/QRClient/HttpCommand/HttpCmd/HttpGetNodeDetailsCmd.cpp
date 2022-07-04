#include "stdafx.h"
#include "HttpGetNodeDetailsCmd.h"

HttpGetNodeDetailsCmd::HttpGetNodeDetailsCmd(qint64 frameId, QObject *parent)
    : HttpCmdRequest(RESTAPI_JOB_FRAME_NODE_DETAILS, parent)
    , m_frameId(frameId)
{
}

HttpGetNodeDetailsCmd::~HttpGetNodeDetailsCmd()
{
}

QByteArray HttpGetNodeDetailsCmd::serializeParams()
{
    QJsonObject object;
    object.insert("id", m_frameId);
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpGetNodeDetailsCmd::analysisResponsed(const QByteArray& data)
{
    NodeDetailsResponse* response = new NodeDetailsResponse();
    response->cmdType = m_cmdType;
    response->rawData = data;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if (!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if (jsDoc.isObject()) {
            QJsonObject jsObj = jsDoc.object();
            int status = jsObj["status"].toInt(0);
            if (status == 0) {
                response->version = jsObj["version"].toString();
                response->result = jsObj["result"].toBool();
                response->msg = jsObj["message"].toString();
                response->code = jsObj["code"].toInt();
                if (jsObj["data"].isObject()) {
                    QJsonObject obj = jsObj["data"].toObject();
                    response->date = obj["date"].toString();
                    response->ip = obj["ip"].toString();
                    response->heNum = obj["heNum"].toInt();
                    response->memUsedRate = obj["memUsedRate"].toInt();
                    response->totalMem = obj["totalMem"].toString();
                    response->restMem = obj["restMem"].toString();
                    response->usedMem = obj["usedMem"].toString();
                    response->restRom = obj["restRom"].toString();
                    if(obj["cpuRateArr"].isArray()){
                        QJsonArray cpuRates = obj["cpuRateArr"].toArray();
                        for(int i = 0; i < cpuRates.size(); i++){
                            response->cpuRateArr.append(cpuRates[i].toString());
                        }
                    }
                }
                response->serverTime = jsObj["serverTime"].toVariant().toLongLong();
            }
            else {
                response->msg = jsObj["message"].toString();
                response->code = jsObj["status"].toInt();
            }
        }
        else {
            qDebug() << "parse json error, document is not a jsonobject ";
        }
    }
    else {
        qDebug() << "parse json error, document is NULL or jsonerror is " << jsonError.errorString();
    }
    emit sendResponseFormat(QSharedPointer<ResponseHead>(response));
}