#include "stdafx.h"
#include "HttpClientSysNotice.h"
#include "HttpCommand/HttpCommon.h"

HttpClientSysNotice::HttpClientSysNotice(int channel, int langFlag, int noticeType, QObject *parent)
    : HttpCmdRequest(RESTAPI_USER_SYS_NOTICE, parent)
    , m_channel(channel)
    , m_langFlag(langFlag)
    , m_noticeType(noticeType)
{
}

HttpClientSysNotice::~HttpClientSysNotice()
{
}

QByteArray HttpClientSysNotice::serializeParams()
{
    QJsonObject object;
    object.insert("channel", m_channel);
    object.insert("lanFlag", m_langFlag);
    if(m_noticeType > -1)
        object.insert("noticeType", m_noticeType);
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpClientSysNotice::analysisResponsed(const QByteArray& data)
{
    ClientSysNoticeResponse* response = new ClientSysNoticeResponse();
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
                QJsonArray datas = jsObj["data"].toArray();
                for (int i = 0; i < datas.count(); i++) {
                    QJsonObject obj = datas[i].toObject();
                    SysNoticeContent* content = new SysNoticeContent;

                    content->id         = obj["id"].toInt();
                    content->adminId    = obj["adminId"].toInt();
                    content->noticeType = obj["noticeType"].toInt();
                    content->channel    = obj["channel"].toInt();
                    content->isDelete   = obj["isDelete"].toInt();
                    content->contentZh  = obj["contentZh"].toString();
                    content->contentJp  = obj["contentJp"].toString();
                    content->contentEn  = obj["contentEn"].toString();
                    content->startTime  = obj["startTime"].toString();
                    content->endTime    = obj["endTime"].toString();
                    content->createTime = obj["createTime"].toString();
                    content->updateTime = obj["updateTime"].toString();

                    if(obj["platform"].isArray()) {
                        QJsonArray platform = obj["platform"].toArray();
                        for(int j = 0; j < platform.count(); j++) {
                            content->platform.append(platform[j].toInt());
                        }
                    }

                    response->noticeContents.append(content);
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