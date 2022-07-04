#include "stdafx.h"
#include "SSOUserLoginCmd.h"
#include "HttpCommand/HttpCommon.h"
#include "Common/SystemUtil.h"

SSOUserLoginCmd::SSOUserLoginCmd(const QString& username, const QString passwd, QObject *parent)
    : HttpCmdRequest(SSOAPI_SIGNIN, parent)
    , m_userName(username)
    , m_passWord(passwd)
    , m_rememberPassword(false)
{
}

QByteArray SSOUserLoginCmd::serializeParams()
{
    QJsonObject object;
    object.insert("loginName", m_userName);
    object.insert("password", passwordEncrypt(m_passWord));
    object.insert("fingerprint", System::getMacAddress());
    QJsonObject extendData;
    extendData.insert("rememberPassword", false);
    extendData.insert("version", STRFILEVERSION);
    extendData.insert("osVersion", System::getSysProductName());
    extendData.insert("machineName", System::getLoginDisplayName());
    extendData.insert("zone", 1);
    object.insert("extendData", extendData);
    object.insert("redirect", "");

    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

QString SSOUserLoginCmd::passwordEncrypt(const QString& dest)
{
    if(dest.isEmpty() || dest.isNull())
        return "";

    int saltLength = 3;
    int splitLength = dest.length() / saltLength;
    QString tempSource;

    for (int i = 0; i < saltLength; i++) {
        if(i == (saltLength-1)) {
            tempSource.append(dest.mid(splitLength * i, dest.length()));
            tempSource.append(INFO_SALT[i]);
        } else {
            tempSource.append(dest.mid(splitLength * i, splitLength));
            tempSource.append(INFO_SALT[i]);
        }
    }

    QByteArray byte_array;
    byte_array.append(tempSource);
    QByteArray hash_byte_array = QCryptographicHash::hash(byte_array, QCryptographicHash::Sha1);
    QString sha1 = hash_byte_array.toHex();
    return sha1;
}

void SSOUserLoginCmd::analysisResponsed(const QByteArray& data)
{
    SSOResponse* response = new SSOResponse;
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
            response->scode   = jsObj["code"].toString();
            response->code    = querySSOErrorCode(response->scode);;
            if(response->scode == "Common.Success" && jsObj["data"].isObject()) {
                QJsonObject obj = jsObj["data"].toObject();
                response->httpCode = obj["code"].toInt();
                response->redirect = obj["redirect"].toString();
                response->rsAuthToken = obj["rsAuthToken"].toString();
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