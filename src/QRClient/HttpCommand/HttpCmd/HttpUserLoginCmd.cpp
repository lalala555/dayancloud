#include "stdafx.h"
#include "HttpUserLoginCmd.h"
#include "HttpCommand/HttpCommon.h"
#include "Common/SystemUtil.h"

HttpUserLoginCmd::HttpUserLoginCmd(const QString username, const QString passwd, const QString& accessId, QObject *parent)
    : HttpCmdRequest(RESTAPI_USER_LOGIN, parent)
    , m_userName(username)
    , m_passWord(passwd)
    , m_accessId(accessId)
    , m_rememberPassword(false)
{
}

QByteArray HttpUserLoginCmd::serializeParams()
{
    QJsonObject object;
    object.insert("userName", m_userName);
    object.insert("password", passwordEncrypt(m_passWord));
    object.insert("accessId", m_accessId);
    object.insert("rememberPassword", false);

    object.insert("version", STRFILEVERSION);
    object.insert("osVersion", System::getSysProductName());
    object.insert("machineName", System::getLoginDisplayName());
    object.insert("fingerprint", System::getMacAddress());
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

QString HttpUserLoginCmd::passwordEncrypt(const QString& dest)
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

void HttpUserLoginCmd::analysisResponsed(const QByteArray& data)
{
    UserLoginResponse* response = new UserLoginResponse;
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
                QJsonObject obj = jsObj["data"].toObject();
                response->accountType      = obj["accountType"].toInt();
                response->channel          = obj["channel"].toInt();
                response->email            = obj["email"].toString();
                response->loginStatus      = obj["loginStatus"].toInt();
                response->loginTime        = QString::number(obj["loginTime"].toVariant().toLongLong());
                response->phone            = obj["phone"].toString();
                response->platform         = obj["platform"].toInt();
                response->raySyncUserKey   = obj["raySyncUserKey"].toString();
                response->shareMainCapital = obj["shareMainCapital"].toInt();
                response->signature        = obj["signature"].toString();
                response->subDeleteTask    = obj["subDeleteTask"].toInt();
                response->useMainBalance   = obj["useMainBalance"].toInt();
                response->userId           = obj["id"].toInt();
                response->userKey          = obj["userKey"].toString();
                response->_version         = obj["version"].toString();
                response->zone             = obj["zone"].toInt();
                response->userName         = obj["userName"].toString(m_userName);
                response->accessId         = obj["accessId"].toString("null");
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

//////////////////////////////////////////////////////////////

HttpSigninCmd::HttpSigninCmd(const QString& userKey, const QString& accessId, QObject *parent)
    : HttpCmdRequest(RESTAPI_USER_LOGIN, parent)
    , m_userKey(userKey)
    , m_accessId(accessId)
{
}

QByteArray HttpSigninCmd::serializeParams()
{
    QJsonObject object;
    object.insert("userName", "");
    object.insert("password", "");
    object.insert("rememberPassword", false);
    object.insert("loginType", "sso");
    object.insert("rsAuthToken", m_userKey);

    object.insert("version", STRFILEVERSION);
    object.insert("osVersion", System::getSysProductName());
    object.insert("machineName", System::getLoginDisplayName());
    object.insert("fingerprint", System::getMacAddress());
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpSigninCmd::analysisResponsed(const QByteArray& data)
{
    UserLoginResponse* response = new UserLoginResponse;
    response->cmdType = m_cmdType;
    response->rawData = data;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if(!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if(jsDoc.isObject()) {
            QJsonObject jsObj = jsDoc.object();
            response->version = jsObj["version"].toString();
            response->result = jsObj["result"].toBool();
            response->msg = jsObj["message"].toString();
            response->code = jsObj["code"].toInt();
            if(jsObj["data"].isObject()) {
                QJsonObject obj = jsObj["data"].toObject();
                response->accountType      = obj["accountType"].toInt();
                response->channel          = obj["channel"].toInt();
                response->email            = obj["email"].toString();
                response->loginStatus      = obj["loginStatus"].toInt();
                response->loginTime        = QString::number(obj["loginTime"].toVariant().toLongLong());
                response->phone            = obj["phone"].toString();
                response->platform         = obj["platform"].toInt();
                response->raySyncUserKey   = obj["raySyncUserKey"].toString();
                response->shareMainCapital = obj["shareMainCapital"].toInt();
                response->signature        = obj["signature"].toString();
                response->subDeleteTask    = obj["subDeleteTask"].toInt();
                response->useMainBalance   = obj["useMainBalance"].toInt();
                response->userId           = obj["id"].toInt();
                response->userKey          = obj["userKey"].toString();
                response->_version         = obj["version"].toString();
                response->zone             = obj["zone"].toInt();
                response->userName         = obj["userName"].toString();
                response->accessId         = obj["accessId"].toString("null");
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


//////////////////////////////////////////////////////////////

HttpThirdSigninCmd::HttpThirdSigninCmd(const QString& userKey, int type, QObject *parent)
    : HttpCmdRequest(RESTAPI_THIRD_USER_LOGIN, parent)
    , m_userKey(userKey)
    , m_type(type)
{
}

QByteArray HttpThirdSigninCmd::serializeParams()
{
    QJsonObject object;
    object.insert("loginType", "sso");
    object.insert("rsAuthToken", m_userKey);
    object.insert("thirdType", m_type);

    object.insert("version", STRFILEVERSION);
    object.insert("osVersion", System::getSysProductName());
    object.insert("machineName", System::getLoginDisplayName());
    object.insert("fingerprint", System::getMacAddress());
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpThirdSigninCmd::analysisResponsed(const QByteArray& data)
{
    UserLoginResponse* response = new UserLoginResponse;
    response->cmdType = m_cmdType;
    response->rawData = data;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if(!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if(jsDoc.isObject()) {
            QJsonObject jsObj = jsDoc.object();
            response->version = jsObj["version"].toString();
            response->result = jsObj["result"].toBool();
            response->msg = jsObj["message"].toString();
            response->code = jsObj["code"].toInt();
            if(jsObj["data"].isObject()) {
                QJsonObject obj = jsObj["data"].toObject();
                response->accountType      = obj["accountType"].toInt();
                response->channel          = obj["channel"].toInt();
                response->email            = obj["email"].toString();
                response->loginStatus      = obj["loginStatus"].toInt();
                response->loginTime        = QString::number(obj["loginTime"].toVariant().toLongLong());
                response->phone            = obj["phone"].toString();
                response->platform         = obj["platform"].toInt();
                response->raySyncUserKey   = obj["raySyncUserKey"].toString();
                response->shareMainCapital = obj["shareMainCapital"].toInt();
                response->signature        = obj["signature"].toString();
                response->subDeleteTask    = obj["subDeleteTask"].toInt();
                response->useMainBalance   = obj["useMainBalance"].toInt();
                response->userId           = obj["id"].toInt();
                response->userKey          = obj["userKey"].toString();
                response->_version         = obj["version"].toString();
                response->zone             = obj["zone"].toInt();
                response->userName         = obj["userName"].toString();
                response->accessId         = obj["accessId"].toString("null");
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