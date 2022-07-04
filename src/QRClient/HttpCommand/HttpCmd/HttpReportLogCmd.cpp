#include "stdafx.h"
#include "HttpReportLogCmd.h"

HttpReportLogCmd::HttpReportLogCmd(qint64 taskId, const QString& filePath, QObject *parent)
    : HttpCmdRequest(RESTAPI_UPLOAD_FILE, parent)
    , m_taskId(taskId)
    , m_filePath(filePath)
{
}

HttpReportLogCmd::~HttpReportLogCmd()
{
}

QByteArray HttpReportLogCmd::serializeParams()
{
    return m_filePath.toLocal8Bit();
}

void HttpReportLogCmd::analysisResponsed(const QByteArray& data)
{
    ReportLogResponse* response = new ReportLogResponse();
    response->cmdType = m_cmdType;
    response->rawData = data;
    response->taskId  = m_taskId;
    response->filePath = m_filePath;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if (!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if (jsDoc.isObject()) {
            QJsonObject jsObj = jsDoc.object();
            response->version = jsObj["version"].toString();
            response->result = jsObj["result"].toBool();
            response->msg = jsObj["message"].toString();
            response->code = jsObj["code"].toInt();
            response->data = jsObj["data"].toInt();
            response->serverTime = jsObj["serverTime"].toVariant().toLongLong();
        }
        else {
            qDebug() << "parse json error, document is not a jsonobject ";
        }
    }
    else {
        qDebug() << "parse json error, document is NULL or jsonerror is " << jsonError.errorString();
    }
    emit sendResponseFormat(QSharedPointer<ResponseHead>(response));
}