#include "stdafx.h"
#include "HttpTaskNewList.h"
#include "HttpResponseParse.h"
#include <QtConcurrent/QtConcurrent>

HttpTaskNewList::HttpTaskNewList(const QString& timeFilter, int showType, int pageNum, int pageSize, int renderingFlag, QObject *parent)
    : HttpCmdRequest(RESTAPI_JOB_NEWLIST, parent)
    , m_pageSize(pageSize)
    , m_pageNum(pageNum)
    , m_renderingFlag(renderingFlag)
    , m_timeFilter(timeFilter)
    , m_showType(showType)
    , m_isRepeat(false)
{
}

QByteArray HttpTaskNewList::serializeParams()
{
    QJsonObject object;
    object.insert("pageSize", m_pageSize);
    object.insert("pageNum", m_pageNum);
    object.insert("renderingFlag", m_renderingFlag);
	object.insert("bFilterOsgb", 1);
    // 使用时间筛选
    if(!m_timeFilter.isEmpty()) {
        QStringList timeRound = m_timeFilter.split("::");
        QString startDate = timeRound.at(0);
        QString endDate = timeRound.at(timeRound.size() - 1);

        object.insert("startTime", startDate);
        object.insert("endTime",endDate);
    }

    if(m_showType != TYPE_SHOW_ALLD) {
        // 是否是回收站
        if(m_showType == TYPE_SHOW_RECYCLE) {
            object.insert("recycleFlag", 1);
        } else {
            // 筛选状态
            QJsonArray arr;
            arr.append(m_showType);
            object.insert("statusList", arr);
        }
    }
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpTaskNewList::analysisResponsed(const QByteArray& data)
{
    // QtConcurrent::run(this, &HttpTaskNewList::analysisThread, data);
    this->analysisThread(data);
}

void HttpTaskNewList::analysisThread(const QByteArray& data)
{
    MaxTaskItemsResponse* response = new MaxTaskItemsResponse();
    response->cmdType = m_cmdType;
    response->rawData = data;
    response->isRepeat = m_isRepeat;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if(!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if(jsDoc.isObject()) {
            QJsonObject jsObj = jsDoc.object();
            response->version = jsObj["version"].toString();
            response->result  = jsObj["result"].toBool();
            response->msg     = jsObj["message"].toString();
            response->code    = jsObj["code"].toInt();
           /* if(jsObj["data"].isObject()) {
                QJsonObject jsData = jsObj["data"].toObject();
                response->pageCount = jsData["pageCount"].toInt();
                response->pageNum   = jsData["pageNum"].toInt();
                response->size      = jsData["size"].toInt();
                response->total     = jsData["total"].toInt();*/

                if(jsObj["data"].isArray()) {
                    QJsonArray jsArr = jsObj["data"].toArray();
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
            //}
            response->serverTime = jsObj["serverTime"].toVariant().toLongLong();
        } else {
            qDebug() << "parse json error, document is not a jsonobject ";
        }
    } else {
        qDebug() << "parse json error, document is NULL or jsonerror is " << jsonError.errorString();
    }
    emit sendResponseFormat(QSharedPointer<ResponseHead>(response));
}

QList<MaxTaskItem *> HttpTaskNewList::analysisRespList(const QJsonArray& jsonArr)
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