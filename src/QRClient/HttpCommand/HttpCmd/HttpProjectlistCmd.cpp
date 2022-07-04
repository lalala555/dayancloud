#include "stdafx.h"
#include "HttpProjectlistCmd.h"

HttpProjectlistCmd::HttpProjectlistCmd(qint64 operateType, QObject *parent)
    : HttpCmdRequest(RESTAPI_PROJECT_LIST, parent)
    , m_operateType(operateType)
{
}

HttpProjectlistCmd::HttpProjectlistCmd(qint64 operateType, QString proName, QObject *parent)
    : HttpCmdRequest(RESTAPI_PROJECT_LIST, parent)
    , m_operateType(operateType)
    , m_projectName(proName)
{
}

QByteArray HttpProjectlistCmd::serializeParams()
{
    QJsonObject object;
    object.insert("type",m_operateType);
    if(m_operateType == ADD_PROJECT) {
        object.insert("newName",m_projectName);
        object.insert("status","0");
    } else if(m_operateType == DELETE_PROJECT) {
        object.insert("delName",m_projectName);
    }
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpProjectlistCmd::analysisResponsed(const QByteArray& data)
{
    ProjectNamesResponse* response = new ProjectNamesResponse;
    response->rawData              = data;
    response->cmdType              = m_cmdType;
    response->optType              = m_operateType;

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
                QJsonObject dataObj = jsObj["data"].toObject();
                if(dataObj["projectNameList"].isArray()) {
                    QJsonArray list = dataObj["projectNameList"].toArray();
                    for(int i = 0; i < list.count(); i++) {
                        QJsonObject obj = list[i].toObject();
                        ProjectConfigInfo* item = new ProjectConfigInfo;

                        item->projectId   = obj["projectId"].toVariant().toLongLong();
                        item->projectName = obj["projectName"].toString();

                        response->projectList.append(item);
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

//////////////////////////////////////////////////////////////////////////

HttpProjectNamesCmd::HttpProjectNamesCmd(QObject *parent)
    : HttpCmdRequest(RESTAPI_LOADING_PROJECT_NAME, parent)
{
}

QByteArray HttpProjectNamesCmd::serializeParams()
{
    QJsonObject object = qstringToJsonObject("{}");
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpProjectNamesCmd::analysisResponsed(const QByteArray& data)
{
    ProjectListResponse* response = new ProjectListResponse;
    response->rawData = data;
    response->cmdType = m_cmdType;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if (!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if (jsDoc.isObject()) {
            QJsonObject jsObj = jsDoc.object();
            response->version = jsObj["version"].toString();
            response->result = jsObj["result"].toBool();
            response->msg = jsObj["message"].toString();
            response->code = jsObj["code"].toInt();

            if (jsObj["data"].isArray()) {
                QJsonArray dataObj = jsObj["data"].toArray();
                for (int i = 0; i < dataObj.count(); i++) {
                    QJsonObject obj = dataObj[i].toObject();
                    ProjectConfigInfo* item = new ProjectConfigInfo;

                    item->projectId = obj["projectId"].toVariant().toLongLong();
                    item->projectName = obj["projectName"].toString();

                    response->projectList.append(item);
                }
                response->serverTime = jsObj["serverTime"].toVariant().toLongLong();
            }
        } else {
            qDebug() << "parse json error, document is not a jsonobject ";
        }
    } else {
        qDebug() << "parse json error, document is NULL or jsonerror is " << jsonError.errorString();
    }
    emit sendResponseFormat(QSharedPointer<ResponseHead>(response));

}