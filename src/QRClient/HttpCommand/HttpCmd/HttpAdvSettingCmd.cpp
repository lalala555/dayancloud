#include "stdafx.h"
#include "HttpAdvSettingCmd.h"
#include "HttpCommand/HttpCommon.h"

HttpAdvSettingCmd::HttpAdvSettingCmd(int type, QObject *parent)
    : HttpCmdRequest(RESTAPI_ADVANCED_PARAM_SETTING, parent)
    , m_type(type)
{
}

QByteArray HttpAdvSettingCmd::serializeParams()
{
    QJsonObject object;
    object.insert("type", m_type);

    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpAdvSettingCmd::analysisResponsed(const QByteArray& data)
{
    AdvancedParamSetResponse* response = new AdvancedParamSetResponse();
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
            if(jsObj["data"].isObject()) {
                QJsonObject data = jsObj["data"].toObject();

                QJsonObject timeOut = data["timeOut"].toObject();
                response->timeout.minTimeOut        = timeOut["minTimeOut"].toDouble();
                response->timeout.maxTimeOut        = timeOut["maxTimeOut"].toDouble();
                response->timeout.defaultTimeOut    = timeOut["defaultTimeOut"].toDouble();
                response->timeout.min               = timeOut["min"].toInt();
                response->timeout.max               = timeOut["max"].toInt();
                response->timeout.defaultSecond     = timeOut["defaultSecond"].toInt();

                QJsonObject timeoutStop = data["jobTimeOutStop"].toObject();
                response->timeoutstop.minTimeoutStop     = timeoutStop["minTimeOutStop"].toDouble();
                response->timeoutstop.maxTimeoutStop     = timeoutStop["maxTimeOutStop"].toDouble();
                response->timeoutstop.defaultTimeoutStop = timeoutStop["default"].toDouble();
                response->timeoutstop.min                = timeoutStop["min"].toInt();
                response->timeoutstop.max                = timeoutStop["max"].toInt();
                response->timeoutstop.defaultSecond      = timeoutStop["defaultSecond"].toInt();

                QJsonObject renderNum = data["renderNum"].toObject();
                response->rendernum.minRenderNum     = renderNum["minRenderNum"].toInt();
                response->rendernum.maxRenderNum     = renderNum["maxRenderNum"].toInt();
                response->rendernum.defaultRenderNum = renderNum["defaultRenderNum"].toInt();

                QJsonObject renderBlock = data["renderBlocker"].toObject();
                response->renderblocks.maxBlocks     = renderBlock["maxBlocker"].toInt();
                response->renderblocks.defaultBlocks = renderBlock["defaultRenderBlocker"].toInt();

                QJsonArray rams = data["ram"].toArray();
                for(int i = 0; i < rams.count(); i++) {
                    QJsonObject ram_obj = rams[i].toObject();

                    AdvancedParamSetResponse::Ram ram;
                    ram.name      = ram_obj["name"].toString();
                    ram.value     = ram_obj["value"].toInt();
                    ram.rate      = ram_obj["rate"].toDouble();
                    ram.isDefault = ram_obj["default"].toInt();
                    response->rams.append(ram);
                }

                QJsonArray gpus = data["gpu"].toArray();
                for (int i = 0; i < gpus.count(); i++) {
                    QJsonObject gpu_obj = gpus[i].toObject();

                    AdvancedParamSetResponse::Gpu gpu;
                    gpu.name = gpu_obj["name"].toString();
                    gpu.value = gpu_obj["value"].toInt();
                    gpu.rate = gpu_obj["rate"].toDouble();
                    gpu.isDefault = gpu_obj["default"].toInt();
                    response->gpus.append(gpu);
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
