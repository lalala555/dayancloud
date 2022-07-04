#include "stdafx.h"
#include "HttpDownloadCount.h"
#include "HttpCommand/HttpCommon.h"
#include "Common/SystemUtil.h"

QByteArray HttpDownloadStartCmd::serializeParams()
{
    QJsonObject object;
    object.insert("taskId", m_taskId);
    object.insert("machineName", System::getLoginDisplayName());
    object.insert("ip", System::getLanIP());
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpDownloadStartCmd::analysisResponsed(const QByteArray& data)
{
    DownloadStartResponse* response = new DownloadStartResponse;
    response->cmdType = m_cmdType;
    response->rawData = data;

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
                QJsonObject obj = jsObj["data"].toObject();
                response->taskId = m_taskId;
                response->downloadRequestId = obj["downloadRequestId"].toString();
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


QByteArray HttpDownloadCompleteCmd::serializeParams()
{
    QJsonObject object;
    object.insert("taskId", m_taskId);
    object.insert("downloadRequestId", m_requestId);
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpDownloadCompleteCmd::analysisResponsed(const QByteArray& data)
{
    DownloadCompleteResponse* response = new DownloadCompleteResponse;
    response->cmdType = m_cmdType;
    response->rawData = data;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if (!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if (jsDoc.isObject()) {
            QJsonObject jsObj = jsDoc.object();
            response->version = jsObj["version"].toString();
            response->result = jsObj["result"].toBool();
            response->msg = jsObj["message"].toString();
            response->code = jsObj["code"].toInt();

            response->serverTime = jsObj["serverTime"].toVariant().toLongLong();
        } else {
            qDebug() << "parse json error, document is not a jsonobject ";
        }
    } else {
        qDebug() << "parse json error, document is NULL or jsonerror is " << jsonError.errorString();
    }
    emit sendResponseFormat(QSharedPointer<ResponseHead>(response));
}


QByteArray HttpDownloadCountCmd::serializeParams()
{
    QJsonObject object;
    QJsonArray jsArray;
    foreach(qint64 taskId, m_taskIds) {
        jsArray.append(taskId);
    }
    object.insert("taskIds", jsArray);

    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpDownloadCountCmd::analysisResponsed(const QByteArray& data)
{
    DownloadCountResponse* response = new DownloadCountResponse;
    response->cmdType = m_cmdType;
    response->rawData = data;

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
                QJsonObject obj = jsObj["data"].toObject();
                response->pageCount = obj["pageCount"].toInt();
                response->pageNum   = obj["pageNum"].toInt();
                response->total     = obj["total"].toInt();
                response->size      = obj["size"].toInt();

                if (obj["items"].isArray()) {
                    QJsonArray items = obj["items"].toArray();
                    for (int i = 0; i < items.count(); i++) {
                        QJsonObject obj = items[i].toObject();
                        
                        DownloadCountItem item;
                        item.taskId        = obj["taskId"].toVariant().toLongLong(0);
                        item.downloadCount = obj["downloadCount"].toInt();
                        item.downloading   = obj["downloading"].toInt();
                        item.creatTime     = obj["creatTime"].toVariant().toLongLong();
                        item.updateTime    = obj["updateTime"].toVariant().toLongLong();

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


QByteArray HttpDownloadInfoCmd::serializeParams()
{
    QJsonObject object;
    object.insert("taskId", m_taskId);
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpDownloadInfoCmd::analysisResponsed(const QByteArray& data)
{
    DownloadInfoResponse* response = new DownloadInfoResponse;
    response->cmdType = m_cmdType;
    response->rawData = data;

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
                QJsonObject obj = jsObj["data"].toObject();
                response->pageCount = obj["pageCount"].toInt();
                response->pageNum = obj["pageNum"].toInt();
                response->total = obj["total"].toInt();
                response->size = obj["size"].toInt();

                if (obj["items"].isArray()) {
                    QJsonArray items = obj["items"].toArray();
                    for (int i = 0; i < items.count(); i++) {
                        QJsonObject obj = items[i].toObject();

                        DownloadInfoItem item;

                        item.downloadStatus = obj["downloadStatus"].toInt();
                        item.machineName    = obj["machineName"].toString();
                        item.ip             = obj["ip"].toString();
                        item.startTime      = obj["startTime"].toVariant().toLongLong();
                        item.endTime        = obj["endTime"].toVariant().toLongLong();
                        item.updateTime     = obj["updateTime"].toVariant().toLongLong();

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

