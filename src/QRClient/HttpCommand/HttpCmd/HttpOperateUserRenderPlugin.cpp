#include "stdafx.h"
#include "HttpOperateUserRenderPlugin.h"
#include "HttpCommand/HttpCommon.h"

HttpOperateUserRenderPlugin::HttpOperateUserRenderPlugin(int optType, OperateUserRenderPlugin* opt, QObject *parent)
    : HttpCmdRequest(optType, parent)
    , m_opt(opt)
{
}

HttpOperateUserRenderPlugin::~HttpOperateUserRenderPlugin()
{

}

QByteArray HttpOperateUserRenderPlugin::serializeParams()
{
    QJsonObject object;
    if(m_opt != nullptr) {
        if(m_cmdType == RESTAPI_USER_REMOVE_RENDER_PLUGIN || 
            m_cmdType == RESTAPI_USER_DEFAULT_RENDER_PLUGIN) {

            object.insert("editName", m_opt->editName);
            object.insert("type", m_opt->optType);
            object.insert("isMainUserId", m_opt->isMainUserId);

        } else if(m_cmdType == RESTAPI_USER_ADD_RENDER_PLUGIN || 
            m_cmdType == RESTAPI_USER_EDIT_RENDER_PLUGIN ) {

            object.insert("cgId", m_opt->cgId);
            object.insert("cgName", m_opt->cgName);
            object.insert("cgVersion", m_opt->cgVersion);
            object.insert("editName", m_opt->editName);
            object.insert("renderSystem", m_opt->renderSystem);
            object.insert("isMainUserId", m_opt->isMainUserId);
            if(m_opt->cgName.toLower().contains("maya")) {
                object.insert("renderLayerType", m_opt->renderLayerType);
                object.insert("projectPath", m_opt->projectPath);
            }    
            QJsonArray jsArray;
            foreach(qint64 pluginId, m_opt->pluginIds) {
                jsArray.append(pluginId);
            }
            object.insert("pluginIds", jsArray);
        }
    }
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpOperateUserRenderPlugin::analysisResponsed(const QByteArray& data)
{
    OperateUserRenderPluginResponse* response = new OperateUserRenderPluginResponse();
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
            response->data    = jsObj["data"].toString();
            response->serverTime = jsObj["serverTime"].toVariant().toLongLong();
        } else {
            qDebug() << "parse json error, document is not a jsonobject ";
        }
    } else {
        qDebug() << "parse json error, document is NULL or jsonerror is " << jsonError.errorString();
    }
    emit sendResponseFormat(QSharedPointer<ResponseHead>(response));
}