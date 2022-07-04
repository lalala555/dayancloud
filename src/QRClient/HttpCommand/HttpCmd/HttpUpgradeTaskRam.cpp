#include "stdafx.h"
#include "HttpUpgradeTaskRam.h"

HttpUpgradeTaskRam::HttpUpgradeTaskRam(qint64 taskId, int ram, QObject *parent)
    : HttpCmdRequest(RESTAPI_JOB_TASK_UPGRADE_TASK_RAM, parent)
    , m_taskId(taskId)
    , m_ram(ram)
{
}

HttpUpgradeTaskRam::HttpUpgradeTaskRam(const QList<qint64>& taskIds, int ram, QObject *parent)
    : HttpCmdRequest(RESTAPI_JOB_TASK_UPGRADE_TASK_RAM, parent)
    , m_taskIds(taskIds)
    , m_ram(ram)
{
}

HttpUpgradeTaskRam::~HttpUpgradeTaskRam()
{
}

QByteArray HttpUpgradeTaskRam::serializeParams()
{
    QJsonObject object;
    if(!m_taskIds.isEmpty()){
        QJsonArray jsArray;
        foreach(qint64 taskid, m_taskIds){
            jsArray.append(taskid);
        }  
        object.insert("taskIds", jsArray);
        object.insert("taskRam", m_ram);

    }else{
        m_taskIds.append(m_taskId);
        QJsonArray jsArray;
        jsArray.append(m_taskId);
        object.insert("taskIds", jsArray);
        object.insert("taskRam", m_ram);
    }
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpUpgradeTaskRam::analysisResponsed(const QByteArray& data)
{
    UpgradeTaskRamResponse* response = new UpgradeTaskRamResponse();
    response->cmdType = m_cmdType;
    response->rawData = data;
    response->taskIds = m_taskIds;

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