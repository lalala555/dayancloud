#include "stdafx.h"
#include "HttpFrameRecommitInfoCmd.h"

HttpFrameRecommitInfoCmd::HttpFrameRecommitInfoCmd(qint64 frameid, int pageNum, qint64 pageSize, QObject *parent)
    : HttpCmdRequest(RESTAPI_JOB_FRAME_RECOMMIT_INFO, parent)
    , m_frameId(frameid)
    , m_pageNum(pageNum)
    , m_pageSize(pageSize)
{
}

HttpFrameRecommitInfoCmd::~HttpFrameRecommitInfoCmd()
{
}

QByteArray HttpFrameRecommitInfoCmd::serializeParams()
{
    QJsonObject object;
    object.insert("id", m_frameId);
    object.insert("pageNum", m_pageNum);
    object.insert("pageSize", m_pageSize);
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpFrameRecommitInfoCmd::analysisResponsed(const QByteArray& data)
{
    FrameRecommitInfoResponse* response = new FrameRecommitInfoResponse();
    response->cmdType = m_cmdType;
    response->rawData = data;
    response->frameId = m_frameId;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if (!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if (jsDoc.isObject()) {
            QJsonObject jsObj = jsDoc.object();
            response->version = jsObj["version"].toString();
            response->result = jsObj["result"].toBool();
            response->msg = jsObj["message"].toString();
            response->code = jsObj["code"].toInt();

            if (jsObj["data"].isObject()) {
                QJsonObject dataObj = jsObj["data"].toObject();
                response->pageCount = dataObj["pageCount"].toVariant().toLongLong();
                response->pageNum   = dataObj["pageNum"].toVariant().toLongLong();
                response->size      = dataObj["size"].toVariant().toLongLong();
                response->total     = dataObj["total"].toVariant().toLongLong();

                if (dataObj["items"].isArray()) {
                    QJsonArray items = dataObj["items"].toArray();
                    for (int i = 0; i < items.size(); i++) {
                        FrameRecommitInfo *recommitInfo = new FrameRecommitInfo;
                        QJsonObject info = items[i].toObject();

                        recommitInfo->taskId = info["taskId"].toVariant().toLongLong(0);
                        recommitInfo->userId = info["userId"].toVariant().toLongLong(0);
                        recommitInfo->jobId = info["jobId"].toVariant().toLongLong(0);

                        recommitInfo->startTime = info["startTime"].toVariant().toLongLong(0);
                        recommitInfo->frameExecuteTime = info["frameExecuteTime"].toVariant().toLongLong(0);
                        recommitInfo->endTime = info["endTime"].toVariant().toLongLong(0);

                        recommitInfo->feeAmount = info["feeAmount"].toDouble(0.0);
                        recommitInfo->framePrice = info["framePrice"].toDouble(0.0);
                        recommitInfo->couponFee = info["couponFee"].toDouble(0.0);

                        recommitInfo->frameName = info["frameName"].toString("");

                        response->recommitItems.append(recommitInfo);
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