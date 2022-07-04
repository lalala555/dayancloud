#include "stdafx.h"
#include "HttpLoadProducerCmd.h"

HttpLoadProducerCmd::HttpLoadProducerCmd(QObject *parent)
    : HttpCmdRequest(RESTAPI_JOB_LOAD_PRODUCER, parent)
{
}

HttpLoadProducerCmd::~HttpLoadProducerCmd()
{
}

QByteArray HttpLoadProducerCmd::serializeParams()
{   
    QJsonObject object;
    object = qstringToJsonObject("{}");
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpLoadProducerCmd::analysisResponsed(const QByteArray& data)
{
    LoadProducerResponse* response = new LoadProducerResponse;
    response->cmdType   = m_cmdType;
    response->rawData   = data;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if(!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if(jsDoc.isObject()) {
            QJsonObject jsObj = jsDoc.object();
            response->version = jsObj["version"].toString();
            response->result  = jsObj["result"].toBool();
            response->msg     = jsObj["message"].toString();
            if(jsObj["data"].isObject()){
                QJsonObject datas = jsObj["data"].toObject();

                if(datas["producerList"].isArray()){
                    QJsonArray list = datas["producerList"].toArray();
                    for(int i = 0; i < list.count(); i++){
                        QString name   = list[i].toString();
                        response->producers.append(name);
                    }
                }
            }
            response->code    = jsObj["code"].toInt();
            response->serverTime = jsObj["serverTime"].toVariant().toLongLong();
        } else {
            qDebug() << "parse json error, document is not a jsonobject ";
        }
    } else {
        qDebug() << "parse json error, document is NULL or jsonerror is " << jsonError.errorString();
    }
    emit sendResponseFormat(QSharedPointer<ResponseHead>(response));
}