#include "stdafx.h"
#include "HttpUserPluginConfig.h"

HttpUserPluginConfig::HttpUserPluginConfig(QList<qint64> cgIds, int osName, QObject *parent)
    : HttpCmdRequest(RESTAPI_USER_PLUGIN_CONFIG, parent)
    , m_cgIds(cgIds)
    , m_osName(osName)
{
}

HttpUserPluginConfig::~HttpUserPluginConfig()
{
}

QByteArray HttpUserPluginConfig::serializeParams()
{
    QJsonObject object;

    QJsonArray jsArray;
    foreach (auto cgId, m_cgIds) {
        jsArray.append(cgId);
    }
    object.insert("cgIds", jsArray);    

    // 传默认值时，两种系统都获取
    if(m_osName != OSTYPE_DEFAULT){
        object.insert("osName", m_osName);
    }

    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpUserPluginConfig::analysisResponsed(const QByteArray& data)
{
    PluginConfigResponse* response = new PluginConfigResponse;
    response->rawData              = data;
    response->cmdType              = m_cmdType;
    response->cgIds                = m_cgIds;

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
                    PluginConfigInfo* pConf = new PluginConfigInfo;
                    pConf->cgId            = item["cgId"].toInt();
                    pConf->cgName          = item["cgName"].toString();
                    pConf->cgVersion       = item["cgVersion"].toString();
                    pConf->editName        = item["editName"].toString();
                    pConf->osName          = item["osName"].toInt();
                    pConf->renderLayerType = item["renderLayerType"].toInt();
                    pConf->isDefault       = item["isDefault"].toInt();
                    pConf->projectPath     = item["projectPath"].toString();
                    pConf->isMainUserId    = QString::number(item["isMainUserId"].toInt(0));

                    if(item["respUserPluginInfoVos"].isArray()) {
                        QJsonArray infoVos = item["respUserPluginInfoVos"].toArray();
                        for(int j = 0; j < infoVos.count(); j++) {
                            QJsonObject vos = infoVos[j].toObject();
                            PluginInfoVos pInfo;

                            pInfo.pluginId      = vos["pluginId"].toInt();
                            pInfo.pluginName    = vos["pluginName"].toString();
                            pInfo.pluginVersion = vos["pluginVersion"].toString();
                            pConf->infoVos.append(pInfo);
                        }
                    }

                    response->pluginConfigs.append(pConf);
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