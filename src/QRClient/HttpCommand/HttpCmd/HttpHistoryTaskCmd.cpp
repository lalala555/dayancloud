#include "stdafx.h"
#include "HttpHistoryTaskCmd.h"

HttpHistoryTaskCmd::HttpHistoryTaskCmd(QObject *parent)
    : HttpCmdRequest(RESTAPI_JOB_TASK_LIST, parent)
    , m_pageSize(40)
    , m_pageNum(1)
    , m_historyTaskFlag(1)
{
}

QByteArray HttpHistoryTaskCmd::serializeParams()
{
    QJsonObject object;
    object.insert("pageSize",m_pageSize);
    object.insert("pageNum",m_pageNum);
    object.insert("historyTaskFlag",m_historyTaskFlag);
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpHistoryTaskCmd::analysisResponsed(const QByteArray& data)
{
    HistoryTaskResponse* response = new HistoryTaskResponse();
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
            if(jsObj["data"].isArray()) {
                QJsonArray jsArr = jsObj["data"].toArray();
                for(int i = 0; i < jsArr.size(); i++) {
                    if(jsArr[i].isObject()) {
                        RenderingTaskItem* item = new RenderingTaskItem;
                        QJsonObject obj = jsArr[i].toObject();
                        item->id                = obj["id"].toVariant().toLongLong();
                        item->channel           = obj["channel"].toInt();
                        item->completedDate     = obj["completedDate"].toVariant().toLongLong();
                        item->doneFrames        = obj["doneFrames"].toInt();
                        item->editName          = obj["doneFrames"].toString();
                        item->executingFrames   = obj["doneFrames"].toString();
                        item->failedFrames      = obj["failedFrames"].toInt();
                        item->framesRange       = obj["framesRange"].toString();
                        item->inputProjectPath  = obj["inputProjectPath"].toString();
                        item->isAutoCommit      = obj["isAutoCommit"].toInt();
                        item->isDelete          = obj["isDelete"].toInt();
                        item->lastestUpdateDate = obj["lastestUpdateDate"].toVariant().toLongLong();
                        item->layerName         = obj["layerName"].toString();
                        item->munuTaskId        = obj["munuTaskId"].toString();
                        item->outputFileName    = obj["outputFileName"].toString();
                        item->outputFilePath    = obj["outputFilePath"].toString();
                        item->platform          = obj["platform"].toInt();
                        item->producer          = obj["producer"].toString();
                        item->projectId         = obj["projectId"].toInt();
                        item->projectName       = obj["projectName"].toString();
                        item->projectPath       = obj["projectPath"].toString();
                        item->renderCamera      = obj["renderCamera"].toString();
                        item->renderConsume     = obj["renderConsume"].toDouble();
                        item->renderDuration    = obj["renderDuration"].toVariant().toLongLong();
                        item->sceneName         = obj["sceneName"].toString();
                        item->statusText        = obj["statusText"].toString();
                        item->submitDate        = obj["submitDate"].toVariant().toLongLong();
                        item->taskStatus        = obj["taskStatus"].toInt();
                        item->totalFrames       = obj["totalFrames"].toInt();
                        item->userId            = obj["userId"].toVariant().toLongLong();
                        item->userName          = obj["userName"].toString();
                        response->items.append(item);
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