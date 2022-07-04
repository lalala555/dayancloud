#include "stdafx.h"
#include "HttpCreateJobId.h"

HttpCreateJobId::HttpCreateJobId(t_taskid oldJobId, QObject *parent)
    : HttpCmdRequest(RESTAPI_JOB_CREATE, parent)
    , m_sid(0)
    , m_commandserver(0)
    , m_idCount(1)
    , m_createType(REND_CMD_CLONE_TASK)
    , m_oldJobId(oldJobId)
{
}

HttpCreateJobId::HttpCreateJobId(int count, int createType, QObject *parent)
    : HttpCmdRequest(RESTAPI_JOB_CREATE, parent)
    , m_sid(0)
    , m_commandserver(0)
    , m_idCount(count)
    , m_createType(createType)
    , m_oldJobId(0)
{
}

HttpCreateJobId::HttpCreateJobId(socket_id sid, CommandServer* commandserver,
                                 int count, int createType, QObject *parent)
    : HttpCmdRequest(RESTAPI_JOB_CREATE, parent)
    , m_idCount(count)
    , m_sid(sid)
    , m_commandserver(commandserver)
    , m_createType(createType)
    , m_oldJobId(0)
{
}

QByteArray HttpCreateJobId::serializeParams()
{
    QJsonObject object;
    object.insert("count", m_idCount);
    object.insert("cloneOriginalId", (int)m_oldJobId);
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpCreateJobId::analysisResponsed(const QByteArray& data)
{
    JobCreateResponse* response = new JobCreateResponse;
    response->rawData       = data;
    response->cmdType       = m_cmdType;
    response->sid           = m_sid;
    response->commandServer = m_commandserver;
    response->createType    = m_createType;
    response->cloneJobId    = m_oldJobId;

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
                // 解析任务号
                if(dataObj["taskIdList"].isArray()) {
                    QJsonArray jsArr = dataObj["taskIdList"].toArray();
                    for(int i = 0; i < jsArr.size(); i++) {
                        response->taskids.append(jsArr[i].toVariant().toLongLong());
                    }
                }
                // 解析任务号别名
                if(dataObj["aliasTaskIdList"].isArray()) {
                    QJsonArray aliasArr = dataObj["aliasTaskIdList"].toArray();
                    for(int i = 0; i < aliasArr.size(); i++) {
                        response->taskIdAlias.append(aliasArr[i].toString());
                    }
                }
                response->userId = dataObj["userId"].toInt(MyConfig.userSet.id);
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