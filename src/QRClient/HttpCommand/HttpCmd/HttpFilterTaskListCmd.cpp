#include "stdafx.h"
#include "HttpFilterTaskListCmd.h"
#include "HttpResponseParse.h"

HttpFilterTaskListCmd::HttpFilterTaskListCmd(const QString& timeRange, int showType, int pageNum,
        int pageSize, int renderingFlag, QObject *parent)
    : HttpCmdRequest(RESTAPI_JOB_LIST, parent)
    , m_timeRange(timeRange)
    , m_pageSize(pageSize)
    , m_pageNum(pageNum)
    , m_renderingTaskFlag(renderingFlag)
    , m_showType(showType)
{
}

HttpFilterTaskListCmd::~HttpFilterTaskListCmd()
{
}

QByteArray HttpFilterTaskListCmd::serializeParams()
{
    QJsonObject object;
    object.insert("pageSize", m_pageSize);
    object.insert("pageNum", m_pageNum);
    object.insert("renderingFlag", m_renderingTaskFlag);
    // 使用时间筛选
    if(!m_timeRange.isEmpty()) {
        QStringList timeRound = m_timeRange.split("::");
        QString startDate = timeRound.at(0);
        QString endDate = timeRound.at(timeRound.size() - 1);

        object.insert("startTime", startDate);
        object.insert("endTime",endDate);
    }

    // 是否是回收站
    if(m_showType == TYPE_SHOW_RECYCLE) {
        object.insert("recycleFlag", 1);
    } else {
        // 筛选状态
        QJsonArray arr;
        arr.append(m_showType);
        object.insert("statusList", arr);
    }

    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpFilterTaskListCmd::analysisResponsed(const QByteArray& data)
{
    MaxTaskItemsResponse* response = new MaxTaskItemsResponse();
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

QList<MaxTaskItem *> HttpFilterTaskListCmd::analysisRespList(const QJsonArray& jsonArr)
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
