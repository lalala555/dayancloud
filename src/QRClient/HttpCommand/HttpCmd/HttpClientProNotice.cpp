#include "stdafx.h"
#include "HttpClientProNotice.h"
#include "HttpCommand/HttpCommon.h"
#include "Common/SystemUtil.h"

HttpClientProNotice::HttpClientProNotice(int channel, int langFlag, QObject *parent)
    : HttpCmdRequest(RESTAPI_USER_PRO_NOTICE, parent)
    , m_channel(channel)
    , m_langFlag(langFlag)
{
}

HttpClientProNotice::~HttpClientProNotice()
{
}

QByteArray HttpClientProNotice::serializeParams()
{
    QJsonObject object;
    object.insert("channel",m_channel);
    object.insert("lanFlag",m_langFlag);
    object.insert("fingerprint", System::getMacAddress());
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpClientProNotice::analysisResponsed(const QByteArray& data)
{
    ClientProNoticeResponse* response = new ClientProNoticeResponse();
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
                    ProNoticeContent* content = new ProNoticeContent;

                    content->id         = obj["id"].toInt();
                    content->adminId    = obj["adminId"].toInt();
                    content->channel    = obj["channel"].toInt();
                    content->noticeType = obj["noticeType"].toInt();
                    content->isDelete   = obj["isDelete"].toInt();
                    content->noticeUrl  = obj["noticeUrl"].toString();
                    content->startTime  = obj["startTime"].toString();
                    content->endTime    = obj["endTime"].toString();
                    content->createTime = obj["createTime"].toString();
                    content->updateTime = obj["updateTime"].toString();

                    if(obj["noticeLanFlag"].isArray()) {
                        QJsonArray flag = obj["noticeLanFlag"].toArray();
                        for(int j = 0; j < flag.count(); j++) {
                            content->noticeLanFlag.append(flag[j].toInt());
                        }
                    }

                    if(obj["noticeContents"].isArray()) {
                        QJsonArray notice = obj["noticeContents"].toArray();
                        for (int k = 0; k < notice.count(); k++) {
                            QJsonObject objContent = notice[k].toObject();
                            ProContent procontent;
                            procontent.contentId     = objContent["contentId"].toInt();
                            procontent.noticeId      = objContent["noticeId"].toInt();
                            procontent.noticeLanFlag = objContent["noticeLanFlag"].toInt();
                            procontent.noticeTitle   = objContent["noticeTitle"].toString();
                            procontent.noticeText    = objContent["noticeText"].toString();

                            content->contents.append(procontent);
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
