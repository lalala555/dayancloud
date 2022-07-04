#include "stdafx.h"
#include "HttpQueryTaskInfo.h"
#include "HttpResponseParse.h"

HttpQueryTaskInfo::HttpQueryTaskInfo(QList<qint64> taskIds, bool needFresh, bool freshView, QObject *parent)
    : HttpCmdRequest(RESTAPI_JOB_QUERY_TASK_INFO, parent)
    , m_taskIds(taskIds)
    , m_needFresh(needFresh)
    , m_freshView(freshView)
{
}

HttpQueryTaskInfo::~HttpQueryTaskInfo()
{
    m_taskIds.clear();
}

QByteArray HttpQueryTaskInfo::serializeParams()
{
    QJsonObject object;
    QJsonArray jsArray;
    foreach(qint64 taskId, m_taskIds) {
        jsArray.append(taskId);
    }
    object.insert("taskIds", jsArray);
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpQueryTaskInfo::analysisResponsed(const QByteArray& data)
{
    MaxTaskItemsResponse* response = new MaxTaskItemsResponse();
    response->cmdType = m_cmdType;
    response->rawData = data;
    response->needFresh = m_needFresh;
    response->freshView = m_freshView;

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
                QJsonObject jsData = jsObj["data"].toObject();
                response->pageCount = jsData["pageCount"].toInt();
                response->pageNum   = jsData["pageNum"].toInt();
                response->size      = jsData["size"].toInt();
                response->total     = jsData["total"].toInt();

                if(jsData["items"].isArray()) {
                    QJsonArray jsArr = jsData["items"].toArray();
                    for(int i = 0; i < jsArr.size(); i++) {
                        if(jsArr[i].isObject()) {
                            QJsonObject obj = jsArr[i].toObject();

                            MaxTaskItem *item = HttpResponseParse::parseTaskJson(obj);
                            if(item->isOpen && obj["respRenderingTaskList"].isArray()) {
                                QJsonArray respList = obj["respRenderingTaskList"].toArray();
                                item->respRenderingTaskList = analysisRespList(respList);
                            }
                            response->items.append(item);
                        }
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

QList<MaxTaskItem *> HttpQueryTaskInfo::analysisRespList(const QJsonArray& jsonArr)
{
    QList<MaxTaskItem *> reqsList;
    for (int j = 0; j < jsonArr.size(); j++) {
        QJsonObject obj = jsonArr[j].toObject();

        MaxTaskItem *item = HttpResponseParse::parseTaskJson(obj);
        if(item->isOpen && obj["respRenderingTaskList"].isArray()) {
            QJsonArray respList = obj["respRenderingTaskList"].toArray();
            item->respRenderingTaskList = analysisRespList(respList);
        }
        reqsList.append(item);
    }
    return reqsList;
}