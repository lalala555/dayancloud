#include "stdafx.h"
#include "HttpSearchNew.h"
#include "HttpResponseParse.h"

HttpSearchNew::HttpSearchNew(const QString& searchKey, int pageNum,
                             int pageSize, int renderingFlag, QObject *parent)
    : HttpCmdRequest(RESTAPI_JOB_NEWLIST, parent)
   // , m_pageSize(pageSize)
   // , m_pageNum(pageNum)
    , m_searchKey(searchKey)
    , m_renderingTaskFlag(renderingFlag)
    , m_isRepeat(false)
{
}

HttpSearchNew::HttpSearchNew(const FilterContent& content, QObject *parent)
	: HttpCmdRequest(RESTAPI_JOB_NEWLIST, parent)
	// , m_pageSize(content.pageSize)
	// , m_pageNum(content.pageNum)
	, m_searchKey(content.searchWord)
    , m_renderingTaskFlag(1)
    , m_content(content)
    , m_isRepeat(false)
{
}

QByteArray HttpSearchNew::serializeParams()
{
    QJsonObject object;
  //  object.insert("pageSize", m_pageSize);
  //  object.insert("pageNum", m_pageNum);
    object.insert("renderingFlag", m_renderingTaskFlag);
    object.insert("startTimeStamp", 0);
	object.insert("bFilterOsgb", 1);
    if(!m_searchKey.isEmpty())
        object.insert("searchKeyword", m_searchKey);
    if(!m_content.stateList.isEmpty()) {
        QJsonArray jsArray;
        foreach(int state, m_content.stateList) {
            if (state != TYPE_SHOW_ALLD) {
                jsArray.append(state);
            }           
        }
        object.insert("statusList", jsArray);
    }
    if(!m_content.projList.isEmpty()) {
        QJsonArray jsArray;
        foreach(int projId, m_content.projList) {
            jsArray.append(projId);
        }
        object.insert("projectIdList", jsArray);
    }
    if(!m_content.userList.isEmpty()) {
        QJsonArray jsArray;
        foreach(int userId, m_content.userList) {
            jsArray.append(userId);
        }
        object.insert("userIdList", jsArray);
    }
    // todo
    if(!m_content.producerList.isEmpty()) {
        QJsonArray jsArray;
        foreach(QString producer, m_content.producerList) {
            jsArray.append(producer);
        }
        object.insert("producerList", jsArray);
    }
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpSearchNew::analysisResponsed(const QByteArray& data)
{
	LOGFMTI("output %s", qPrintable(data));
    qDebug() << data;
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
            /*if(jsObj["data"].isObject()) {
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
              //  }
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

QList<MaxTaskItem *> HttpSearchNew::analysisRespList(const QJsonArray& jsonArr)
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