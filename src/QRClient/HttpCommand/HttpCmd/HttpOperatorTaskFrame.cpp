#include "stdafx.h"
#include "HttpOperatorTaskFrame.h"

HttpOperatorTaskFrame::HttpOperatorTaskFrame(qint64 taskId, qint64 frameId, int selectAll, int optType, QObject *parent)
    : HttpCmdRequest(optType, parent)
    , m_taskId(taskId)
    , m_selectAll(selectAll)
{
    m_frameIds.append(frameId);
}

HttpOperatorTaskFrame::HttpOperatorTaskFrame(qint64 taskId, QList<qint64> frameIds, int selectAll, int optType, QObject *parent)
    : HttpCmdRequest(optType, parent)
    , m_taskId(taskId)
    , m_selectAll(selectAll)
    , m_frameIds(frameIds)
{
}

HttpOperatorTaskFrame::~HttpOperatorTaskFrame()
{
}

QByteArray HttpOperatorTaskFrame::serializeParams()
{
    QJsonObject object;
    if(!m_frameIds.isEmpty()){
        QJsonArray jsArray;
        foreach(qint64 frameId, m_frameIds){
            jsArray.append(frameId);
        }  
        object.insert("ids", jsArray);
    }
    object.insert("selectAll", m_selectAll);
    object.insert("taskIds", m_taskId);
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpOperatorTaskFrame::analysisResponsed(const QByteArray& data)
{
    TaskFrameOperateResponse* response = new TaskFrameOperateResponse();
    response->cmdType  = m_cmdType;
    response->rawData  = data;
    response->frameIds = m_frameIds;
    response->taskId   = m_taskId;

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