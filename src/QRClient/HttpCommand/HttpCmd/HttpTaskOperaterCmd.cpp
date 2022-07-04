#include "stdafx.h"
#include "HttpTaskOperaterCmd.h"

HttpTaskOperaterCmd::HttpTaskOperaterCmd(int optType, QList<qint64> taskIds, const QString& option, QObject *parent)
    : HttpCmdRequest(optType, parent)
    , m_taskIds(taskIds)
    , m_option(option)
{
}

HttpTaskOperaterCmd::HttpTaskOperaterCmd(int optType, qint64 taskId, const QString& option, QObject *parent)
    : HttpCmdRequest(optType, parent)
    , m_taskId(taskId)
    , m_option(option)
{
}

QByteArray HttpTaskOperaterCmd::serializeParams()
{
    QJsonObject object;
    if(!m_taskIds.isEmpty()){
        QJsonArray jsArray;
        foreach(qint64 taskid, m_taskIds){
            jsArray.append(taskid);
        }  
        object.insert("taskIds", jsArray);
    }else{
        m_taskIds.append(m_taskId);
        QJsonArray jsArray;
        jsArray.append(m_taskId);
        object.insert("taskIds", jsArray);
    }
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpTaskOperaterCmd::analysisResponsed(const QByteArray& data)
{
    TaskOperateResponse* response = new TaskOperateResponse();
    response->cmdType = m_cmdType;
    response->rawData = data;
    response->taskIds = m_taskIds;
    response->option = m_option;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if(!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if(jsDoc.isObject()) {
            QJsonObject jsObj = jsDoc.object();
            response->version = jsObj["version"].toString();
            response->result  = jsObj["result"].toBool();
            response->msg     = jsObj["message"].toString();
            response->code    = jsObj["code"].toInt();
            response->data    = jsObj["data"].toInt();
            response->serverTime = jsObj["serverTime"].toVariant().toLongLong();
        } else {
            qDebug() << "parse json error, document is not a jsonobject ";
        }
    } else {
        qDebug() << "parse json error, document is NULL or jsonerror is " << jsonError.errorString();
    }
    emit sendResponseFormat(QSharedPointer<ResponseHead>(response));
}