#include "stdafx.h"
#include "HttpGetOutputSizeCmd.h"

HttpGetOutputSizeCmd::HttpGetOutputSizeCmd(qint64 taskid, int fromPage, QObject *parent)
    : HttpCmdRequest(RESTAPI_OUTPUT_GETSIZE, parent)
    , m_fromPage(fromPage)
{
    m_taskIds.append(taskid);
}

HttpGetOutputSizeCmd::HttpGetOutputSizeCmd(const QList<qint64>& taskids, int fromPage, QObject *parent)
    : HttpCmdRequest(RESTAPI_OUTPUT_GETSIZE, parent)
    , m_fromPage(fromPage)
    , m_taskIds(taskids)
{
}

HttpGetOutputSizeCmd::~HttpGetOutputSizeCmd()
{
}

QByteArray HttpGetOutputSizeCmd::serializeParams()
{
    QJsonObject object;
    QJsonArray jsArray;
    foreach(qint64 taskId, m_taskIds) {
        jsArray.append(taskId);
    }
    object.insert("taskIds", jsArray);
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpGetOutputSizeCmd::analysisResponsed(const QByteArray& data)
{
    TaskOutputSizeResponse* response = new TaskOutputSizeResponse();
    response->cmdType  = m_cmdType;
    response->rawData  = data;
    response->fromPage = m_fromPage;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if (!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if (jsDoc.isObject()) {
            QJsonObject jsObj = jsDoc.object();
            response->version = jsObj["version"].toString();
            response->result  = jsObj["result"].toBool();
            response->msg     = jsObj["message"].toString();
            response->code    = jsObj["code"].toInt();

            if (jsObj["data"].isObject()) {
                QJsonObject dataJson = jsObj["data"].toObject();
                response->pageCount  = dataJson["pageCount"].toVariant().toLongLong();
                response->pageNum    = dataJson["pageNum"].toVariant().toLongLong();
                response->size       = dataJson["size"].toVariant().toLongLong();
                response->total      = dataJson["total"].toVariant().toLongLong();

                if (dataJson["items"].isArray()) {
                    QJsonArray items = dataJson["items"].toArray();
                    for (int i = 0; i < items.size(); i++) {
                        if (items[i].isObject()) {
                            QJsonObject item = items[i].toObject();
                            TaskOutputSize taskSize;

                            taskSize.taskId = item["taskId"].toVariant().toLongLong();
                            taskSize.outputSize = item["size"].toVariant().toLongLong();

                            response->tasksSize.insert(taskSize.taskId, taskSize);
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
