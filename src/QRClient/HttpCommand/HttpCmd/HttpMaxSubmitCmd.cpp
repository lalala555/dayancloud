#include "stdafx.h"
#include "HttpMaxSubmitCmd.h"

HttpMaxSubmitCmd::HttpMaxSubmitCmd(QObject *parent)
    : HttpCmdRequest(0, parent)
{
    m_params.common = new MaxCommitCommonInfo;
    m_params.vray = new MaxCommitVrayInfo;
}

QByteArray HttpMaxSubmitCmd::serializeParams()
{
    QJsonObject object;
    object.insert("taskId", m_params.taskId);
    object.insert("preFrames", m_params.preFrames);
    object.insert("timeOut", m_params.timeOut);
    object.insert("isMaxRender", m_params.isMaxRender);
    object.insert("projectName", m_params.projectName);
    object.insert("projectId", m_params.projectId);
    object.insert("renderNum", m_params.renderNum);
    object.insert("kernel", m_params.kernel);
    object.insert("nodeBlock", m_params.nodeBlock);
    // common
    QJsonObject common;
    QJsonArray all_camera;
    foreach(QString cam, m_params.common->all_camera) {
        all_camera.append(cam);
    }
    QJsonArray element_list;
    foreach(QString elem, m_params.common->element_list) {
        element_list.append(elem);
    }
    QJsonArray renderable_camera;
    foreach(QString rendercam, m_params.common->renderable_camera) {
        all_camera.append(rendercam);
    }
    common.insert("all_camera", all_camera);
    common.insert("element_list", element_list);
    common.insert("renderable_camera", renderable_camera);
    common.insert("element_active", m_params.common->element_active);
    common.insert("element_type", m_params.common->element_type);
    common.insert("frames", m_params.common->frames);
    common.insert("height", m_params.common->height);
    common.insert("kg", m_params.common->kg);
    common.insert("output_file", m_params.common->output_file);
    common.insert("output_file_basename", m_params.common->output_file_basename);
    common.insert("output_file_type", m_params.common->output_file_type);
    common.insert("width", m_params.common->width);
    object.insert("common", common);

    // vary
    QJsonObject vray;
    vray.insert("displacement", m_params.vray->displacement);
    vray.insert("filter_kernel", m_params.vray->filter_kernel);
    vray.insert("filter_on", m_params.vray->filter_on);
    vray.insert("gi", m_params.vray->gi);
    vray.insert("gi_frames", m_params.vray->gi_frames);
    vray.insert("gi_height", m_params.vray->gi_height);
    vray.insert("gi_width", m_params.vray->gi_width);
    vray.insert("image_sampler_type", m_params.vray->image_sampler_type);
    vray.insert("irradiance_map_mode", m_params.vray->irradiance_map_mode);
    vray.insert("irrmap_file", m_params.vray->irrmap_file);
    vray.insert("light_cache_file", m_params.vray->light_cache_file);
    vray.insert("light_cache_mode", m_params.vray->light_cache_mode);
    vray.insert("onlyphoton", m_params.vray->onlyphoton);
    vray.insert("photonnode", m_params.vray->photonnode);
    vray.insert("primary_gi_engine", m_params.vray->primary_gi_engine);
    vray.insert("reflection_refraction", m_params.vray->reflection_refraction);
    vray.insert("secbounce", m_params.vray->secbounce);
    vray.insert("secondary_gi_engine", m_params.vray->secondary_gi_engine);
    vray.insert("subdivs", m_params.vray->subdivs);
    object.insert("vray", vray);

    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpMaxSubmitCmd::analysisResponsed(const QByteArray& data)
{
    maxHandleCommitResponse* response = new maxHandleCommitResponse();
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
                QJsonObject obj = jsObj["data"].toObject();
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

void HttpMaxSubmitCmd::setExternalParams(const QString& params)
{

}