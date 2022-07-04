#include "stdafx.h"
#include "HttpTaskProcessImg.h"

HttpTaskProcessImg::HttpTaskProcessImg(int taskid, int frameType, QObject *parent)
    : HttpCmdRequest(RESTAPI_JOB_TASK_PROCESS, parent)
    , m_taskId(taskid)
    , m_frameType(frameType)
{
}

HttpTaskProcessImg::~HttpTaskProcessImg()
{
}

QByteArray HttpTaskProcessImg::serializeParams()
{
    QJsonObject object;
    object.insert("frameType", m_frameType);
    object.insert("taskId", m_taskId);
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpTaskProcessImg::analysisResponsed(const QByteArray& data)
{
    TaskProcessImgResponse* response = new TaskProcessImgResponse;
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
                QJsonObject dataObj = jsObj["data"].toObject();
                response->isRenderPhoton      = dataObj["isRenderPhoton"].toBool(false);
                response->currentTaskType     = dataObj["currentTaskType"].toString();
                response->sceneName           = dataObj["sceneName"].toString();
                response->completedTime       = dataObj["completedTime"].toString();
                response->startTime           = dataObj["startTime"].toString();
                response->width               = dataObj["width"].toInt();
                response->height              = dataObj["height"].toInt();
                response->block               = dataObj["block"].toInt();

                if (dataObj["grabInfo"].isArray()) {
                    QJsonArray grabInfoRow = dataObj["grabInfo"].toArray();
                    int rowCount = grabInfoRow.count();
                    for (int r = 0; r < rowCount; r++) {
                        if (grabInfoRow[r].isArray()) {
                            QJsonArray grabInfoCol = grabInfoRow[r].toArray();
                            int columnCount = grabInfoCol.count();
                            for (int c = 0; c < columnCount; c++) {
                                QJsonObject grab = grabInfoCol[c].toObject();
                                TaskGrabInfo* info = new TaskGrabInfo;
                                info->couponFee = grab["couponFee"].toString();
                                info->frameIndex = grab["frameIndex"].toString();
                                info->renderInfo = grab["renderInfo"].toString();
                                info->frameBlock = grab["frameBlock"].toString();
                                info->frameEst = grab["frameEst"].toString();
                                info->grabUrl = grab["grabUrl"].toString();
                                info->feeAmount = grab["feeAmount"].toString();
                                info->frameUsed = grab["frameUsed"].toString();
                                info->frameStatus = grab["frameStatus"].toString();
                                info->framePercent = grab["framePercent"].toString();
                                info->isMaxPrice = grab["isMaxPrice"].toString();
                                info->startTime = grab["startTime"].toString();
                                info->endTime = grab["endTime"].toString();
                                info->width = response->width;
                                info->height = response->height;

                                response->processImgList.append(info);
                            }
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