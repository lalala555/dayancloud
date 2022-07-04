#include "stdafx.h"
#include "HttpTaskFrameRenderingInfo.h"

HttpTaskFrameRenderingInfo::HttpTaskFrameRenderingInfo(qint64 taskId, qint32 pageNum,
        qint32 pageSize, const QString& searchKey, const QList<int>& stateList, QObject *parent)
    : HttpCmdRequest(RESTAPI_JOB_FRAME_RENDER_INFO, parent)
    , m_taskId(taskId)
    , m_pageSize(pageSize)
    , m_pageNum(pageNum)
    , m_searchKey(searchKey)
    , m_stateList(stateList)
{
}

HttpTaskFrameRenderingInfo::~HttpTaskFrameRenderingInfo()
{
}

QByteArray HttpTaskFrameRenderingInfo::serializeParams()
{
    QJsonObject object;
    object.insert("taskId", m_taskId);
    object.insert("pageSize", m_pageSize);
    object.insert("pageNum", m_pageNum);
    if(!m_searchKey.isEmpty()) {
        object.insert("searchKeyword", m_searchKey);
    }
    if(!m_stateList.isEmpty()) {
        QJsonArray jsArray;
        foreach(int state, m_stateList) {
            jsArray.append(state);
        }
        object.insert("statusList", jsArray);
    }
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpTaskFrameRenderingInfo::analysisResponsed(const QByteArray& data)
{
    TaskFrameRenderingInfoResponse* response = new TaskFrameRenderingInfoResponse();
    response->cmdType   = m_cmdType;
    response->rawData   = data;
    response->taskId    = m_taskId;
    response->searchKey = m_searchKey;
    response->stateList = m_stateList;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if(!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if(jsDoc.isObject()) {
            QJsonObject jsObj        = jsDoc.object();
            response->version        = jsObj["version"].toString();
            response->result         = jsObj["result"].toBool();
            response->msg            = jsObj["message"].toString();
            response->code           = jsObj["code"].toInt();

            if(jsObj["data"].isObject()) {
                QJsonObject obj = jsObj["data"].toObject();
                response->pageCount       = obj["pageCount"].toInt();
                response->pageNum         = obj["pageNum"].toInt();
                response->size            = obj["size"].toInt();
                response->total           = obj["total"].toInt();

                if(obj["items"].isArray()) {
                    QJsonArray items = obj["items"].toArray();
                    for (int i = 0; i < items.size(); i++) {
                        JobState::DetailInfo *detail = new JobState::DetailInfo;
                        QJsonObject info = items[i].toObject();

                        detail->id                = info["id"].toVariant().toLongLong(0);
                        detail->taskId            = info["taskId"].toVariant().toLongLong(0);
                        detail->userId            = info["userId"].toVariant().toLongLong(0);

                        detail->arrearsFee        = info["arrearsFee"].toDouble(0.0);
                        detail->couponFee         = info["couponFee"].toDouble(0.0);
                        detail->feeAmount         = info["feeAmount"].toDouble(0.0);
                        detail->feeType           = info["feeType"].toString("");

                        detail->frameBlock        = info["frameBlock"].toBool(0);
                        detail->frameExecuteTime  = info["frameExecuteTime"].toInt(0);
                        detail->frameIndex        = info["frameIndex"].toString("");
                        detail->framePrice        = info["framePrice"].toString("");
                        detail->frameRam          = info["frameRam"].toInt(0);
                        detail->frameRamRender    = info["renderRam"].toInt(0);
                        detail->frameStatus       = info["frameStatus"].toInt(0);
                        detail->frameStatusText   = info["frameStatusText"].toString("");
                        detail->frameType         = info["frameType"].toInt(0);

                        detail->gopName           = info["gopName"].toString("");
                        detail->isCopy            = info["isCopy"].toString("");
                        detail->munuJobId         = info["munuJobId"].toString("");
                        detail->munuTaskId        = info["munuTaskId"].toString("");
                        detail->recommitFlag      = info["recommitFlag"].toBool(false);

                        detail->startTime         = info["startTime"].toVariant().toLongLong(0);
                        detail->endTime           = info["endTime"].toVariant().toLongLong(0);
                        detail->averageCpu        = info["averageCpu"].toVariant().toLongLong(0);
                        detail->averageMemory     = info["averageMemory"].toVariant().toLongLong(0);
                        detail->taskOverTime      = info["taskOverTime"].toVariant().toLongLong(0);
                        detail->isOverTime        = info["isOverTime"].toVariant().toInt(0);
                        detail->frameProgress     = info["frameProgress"].toString("-");  

						detail->kmlName = info["kmlName"].toString();
						detail->pixel = info["pixel"].toString();
						detail->picCount = info["picCount"].toString();



                        response->detailItems.append(detail);
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