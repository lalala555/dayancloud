#include "stdafx.h"
#include "HttpErrorCodeCmd.h"
#include "HttpCommand/HttpCommon.h"

HttpErrorCodeCmd::HttpErrorCodeCmd(int lang, QList<int> codes, QObject *parent)
    : HttpCmdRequest(RESTAPI_ERROR_LIST_CODE, parent)
    , m_lang(lang)
{
}

QByteArray HttpErrorCodeCmd::serializeParams()
{
    QJsonObject object;
    object.insert("language", m_lang); // //0 ÖÐÎÄ  1Ó¢ÎÄ

    QJsonArray jsArray;
    foreach (auto code, m_codes) {
        jsArray.append(QString::number(code));
    }
    object.insert("codes", jsArray);

    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpErrorCodeCmd::analysisResponsed(const QByteArray& data)
{
    ErrorListResponse* response = new ErrorListResponse();
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
                QJsonArray errorCodes = jsObj["data"].toArray();
                for(int i = 0; i < errorCodes.count(); i++) {
                    QJsonObject obj = errorCodes[i].toObject();

                    ErrorCodeItem codeItem;
                    codeItem.id              = obj["id"].toInt();
                    codeItem.code            = obj["code"].toString();
                    codeItem.type            = obj["type"].toInt();
                    codeItem.languageFlag    = obj["languageFlag"].toInt();
                    codeItem.desDescription  = obj["desDescription"].toString();
                    codeItem.desSolution     = obj["desSolution"].toString();
                    codeItem.solutionPath    = obj["solutionPath"].toString();
                    codeItem.isRepair        = obj["isRepair"].toBool();
                    codeItem.isDelete        = obj["isDelete"].toBool();
                    codeItem.isOpen          = obj["isOpen"].toBool();
                    codeItem.lastModifyAdmin = obj["lastModifyAdmin"].toString();
                    codeItem.updateTime      = obj["updateTime"].toVariant().toLongLong();

                    response->errorCodes.append(codeItem);
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
