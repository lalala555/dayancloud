#include "stdafx.h"
#include "HttpUserRenderSettingCmd.h"
#include "HttpCommand/HttpCommon.h"

HttpUserRenderSettingCmd::HttpUserRenderSettingCmd(QObject *parent)
    : HttpCmdRequest(RESTAPI_USER_RENDER_SETTING, parent)
{
}

QByteArray HttpUserRenderSettingCmd::serializeParams()
{
    QJsonObject object = qstringToJsonObject("{}");
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpUserRenderSettingCmd::analysisResponsed(const QByteArray& data)
{
    RenderSettingResponse* response = new RenderSettingResponse;
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
                QJsonObject renderObj = jsObj["data"].toObject();
                response->singleNodeRenderFrames    = renderObj["singleNodeRenderFrames"].toInt();
                response->maxIgnoreMapFlag          = renderObj["maxIgnoreMapFlag"].toInt();
                response->autoCommit                = renderObj["autoCommit"].toInt();
                response->separateAccountFlag       = renderObj["separateAccountFlag"].toInt();
                response->miFileSwitchFlag          = renderObj["mifileSwitchFlag"].toInt();
                response->assFileSwitchFlag         = renderObj["assfileSwitchFlag"].toInt();
                response->manuallyStartAnalysisFlag = renderObj["manuallyStartAnalysisFlag"].toInt();
                response->downloadDisable           = renderObj["downloadDisable"].toInt();
                // response->taskOverTime           = renderObj["taskOverTime"].toInt();
                response->taskOverTimeSec           = renderObj["taskOverTimeSec"].toInt();
                response->ignoreMapFlag             = renderObj["ignoreMapFlag"].toInt();
                response->isVrayLicense             = renderObj["isVrayLicense"].toInt();
                response->justUploadConfigFlag      = renderObj["justUploadConfigFlag"].toInt();
                response->justUploadCgFlag          = renderObj["justUploadCgFlag"].toInt();
                response->mandatoryAnalyseAllAgent  = renderObj["mandatoryAnalyseAllAgent"].toInt();
                response->downloadLimit             = renderObj["downloadLimit"].toInt(0);
                response->downloadLimitDay          = renderObj["downloadLimitDay"].toInt(20);
                response->cloneLimitDay             = renderObj["cloneLimitDay"].toInt(30);
                response->submitDisable             = renderObj["submitDisable"].toInt(0);
                response->ignoreAnalyseWarn         = renderObj["ignoreAnalyseWarn"].toInt(0);
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