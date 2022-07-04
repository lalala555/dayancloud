#include "stdafx.h"
#include "HttpRenderSoftVersion.h"

HttpRenderSoftVersion::HttpRenderSoftVersion(qint64 cgId, const QString& osName, QObject *parent)
    : HttpCmdRequest(RESTAPI_USER_RENDER_SOFT_VERSION, parent)
    , m_cgId(cgId)
    , m_osName(osName)
{
}

HttpRenderSoftVersion::~HttpRenderSoftVersion()
{
}

QByteArray HttpRenderSoftVersion::serializeParams()
{
    QJsonObject object;
    object.insert("cgId", m_cgId);
    object.insert("osName", m_osName);
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpRenderSoftVersion::analysisResponsed(const QByteArray& data)
{
    RenderSoftVersionResponse* response = new RenderSoftVersionResponse;
    response->rawData              = data;
    response->cmdType              = m_cmdType;
    response->osName               = m_osName;
    response->cgId                 = m_cgId;

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

                if(data["cgPlugin"].isArray()){
                    QJsonArray plugins = data["cgPlugin"].toArray();
                    for(int i = 0; i < plugins.count(); i++){
                        QJsonObject plugin = plugins[i].toObject();
                        CgPlugin* item     = new CgPlugin;

                        item->cvId       = plugin["cvId"].toVariant().toLongLong();
                        item->pluginName = plugin["pluginName"].toString();

                        if(plugin["pluginVersions"].isArray()){
                            QJsonArray ver = plugin["pluginVersions"].toArray();
                            for(int j = 0; j < ver.count(); j++){
                                QJsonObject pVer = ver[j].toObject();
                                PluginVersions version;
                                version.pluginId      = pVer["pluginId"].toVariant().toLongLong();
                                version.pluginName    = pVer["pluginName"].toString();
                                version.pluginVersion = pVer["pluginVersion"].toString();

                                item->pluginVersions.append(version);
                            }
                        }

                        response->cgPlugins.append(item);
                    }
                }

                if(data["cgVersion"].isArray()){
                    QJsonArray cgVer = data["cgVersion"].toArray();
                    for(int k = 0; k < cgVer.count(); k++){
                        QJsonObject verObj = cgVer[k].toObject();
                        CgVersion *cv      = new CgVersion;

                        cv->id        = verObj["id"].toVariant().toLongLong();
                        cv->cgId      = verObj["cgId"].toVariant().toLongLong();
                        cv->cgName    = verObj["cgName"].toString();
                        cv->cgVersion = verObj["cgVersion"].toString();
                        
                        response->cgVersions.append(cv);
                    }
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