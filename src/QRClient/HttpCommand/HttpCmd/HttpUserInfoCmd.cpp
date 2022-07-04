#include "stdafx.h"
#include "HttpUserInfoCmd.h"

HttpUserInfoCmd::HttpUserInfoCmd(QObject *parent)
    : HttpCmdRequest(RESTAPI_USER_INFO, parent)
{
}

QByteArray HttpUserInfoCmd::serializeParams()
{
    QJsonObject object;
    object = qstringToJsonObject("{}");
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpUserInfoCmd::analysisResponsed(const QByteArray& data)
{
    UserInfoResponse* response = new UserInfoResponse();
    response->cmdType = m_cmdType;
    response->rawData = data;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if(!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if(jsDoc.isObject()) {
            QJsonObject jsObj = jsDoc.object();
            response->version    = jsObj["version"].toString();
            response->result     = jsObj["result"].toBool();
            response->msg        = jsObj["message"].toString();
            response->code       = jsObj["code"].toInt();
            response->serverTime = jsObj["serverTime"].toVariant().toLongLong();

            if(jsObj["data"].isObject()) {
                QJsonObject obj = jsObj["data"].toObject();
                response->authStatus          = obj["authStatus"].toInt();
                response->realnameStatus      = obj["userAuthStatus"].toInt();
                response->delayAuthTime       = obj["delayAuthTime"].toVariant().toULongLong();
                response->accountType         = obj["accountType"].toInt();
                response->businessType        = obj["businessType"].toInt();
                response->userId              = obj["userId"].toInt();
                response->mainUserId          = obj["mainUserId"].toInt();
                response->softType            = obj["softType"].toInt();
                response->status              = obj["status"].toInt();
                response->zone                = obj["zone"].toInt();
                response->level               = obj["level"].toInt();
                response->userType            = obj["userType"].toInt();
                response->blenderFlag         = obj["blenderFlag"].toInt();
                response->c4dFlag             = obj["c4dFlag"].toInt();
                response->houdiniFlag         = obj["houdiniFlag"].toInt();
                response->keyshotFlag         = obj["keyshotFlag"].toInt();
                response->displaySubaccount   = obj["displaySubaccount"].toInt();
                response->hideJobCharge       = obj["hideJobCharge"].toInt();
                response->infoStatus          = obj["infoStatus"].toInt();
                response->softStatus          = obj["softStatus"].toInt();
                response->subDeleteTask       = obj["subDeleteTask"].toInt();
                response->subDeleteCapital    = obj["subDeleteCapital"].toInt();
                response->useLevelDirectory   = obj["useLevelDirectory"].toInt();
                response->useMainBalance      = obj["useMainBalance"].toInt();
                response->hideBalance         = obj["hideBalance"].toInt();
                response->studentEndTime      = obj["studentEndTime"].toVariant().toLongLong();
                response->platform            = obj["platform"].toInt();
                response->enableNodeDetails   = obj["enableNodeDetails"].toInt();
                response->taskNodeLimitPermission = obj["taskNodeLimitPermission"].toInt();
                response->groupTaskLevel      = obj["groupTaskLevel"].toInt();
                response->openRenderRam       = obj["openRenderRam"].toInt();
                
                response->picMemberValidity   = obj["picMemberValidity"].toVariant().toLongLong();
                response->pictureLever        = obj["pictureLever"].toInt();
                response->picturePro          = obj["picturePro"].toBool();
                response->downloadDisable     = obj["downloadDisable"].toInt();
               
                response->pictureProExpire    = obj["pictureProExpire"].toString();
                response->communicationNumber = obj["communicationNumber"].toString();
                response->address             = obj["address"].toString();
                response->trueName            = obj["name"].toString();
                response->userName            = obj["userName"].toString();
                response->phone               = obj["phone"].toString();
                response->job                 = obj["job"].toString();
                response->email               = obj["email"].toString();
                response->description         = obj["description"].toString();
                response->city                = obj["city"].toString();
                response->country             = obj["country"].toString();
                response->company             = obj["company"].toString();

                response->usdbalance          = QString::number(obj["usdbalance"].toDouble(), 'f', 3).toDouble();
                response->rmbbalance          = QString::number(obj["rmbbalance"].toDouble(), 'f', 3).toDouble();
                response->coupon              = QString::number(obj["coupon"].toDouble(), 'f', 3).toDouble();
                response->rmbCumulative       = QString::number(obj["rmbCumulative"].toDouble(), 'f', 3).toDouble();
                response->usdCumulative       = QString::number(obj["usdCumulative"].toDouble(), 'f', 3).toDouble();
                response->cpuPrice            = QString::number(obj["cpuPrice"].toDouble(), 'f', 3).toDouble();
                response->gpuPrice            = QString::number(obj["gpuPrice"].toDouble(), 'f', 3).toDouble();
                response->credit              = QString::number(obj["credit"].toDouble(), 'f', 3).toDouble();
                response->commonCoupon        = QString::number(obj["commonCoupon"].toDouble(), 'f', 3).toDouble();
                response->commonCouponCount   = obj["commonCouponCount"].toInt();
                response->qyCoupon            = QString::number(obj["qyCoupon"].toDouble(), 'f', 3).toDouble();
                response->qyCouponCount       = obj["qyCouponCount"].toInt();
                response->gpuSingleDiscount   = QString::number(obj["gpuSingleDiscount"].toDouble(), 'f', 3).toDouble();

                // balance
                response->balance.user_id               = obj["id"].toVariant().toLongLong();
                response->balance.rmbbalance            = QString::number(obj["rmbbalance"].toDouble(), 'f', 3);
                response->balance.usdbalance            = QString::number(obj["usdbalance"].toDouble(), 'f', 3);
                response->balance.coupon                = QString::number(obj["coupon"].toDouble(), 'f', 3);
                response->balance.vipLevel              = obj["level"].toInt();
                response->balance.downloadDisable       = obj["downloadDisable"].toInt();
                response->balance.isPicture             = obj["businessType"].toInt() == 0 ? true : false;
                response->balance.isPicturePro          = obj["pictureLever"].toInt() ? true : false;
                response->balance.pictureProExpiredDate = obj["pictureProExpiredDate"].toVariant().toLongLong();
                response->balance.coupon                = QString::number(obj["commonCoupon"].toDouble(), 'f', 3);
                response->balance.couponCount           = obj["commonCouponCount"].toInt();
                response->balance.qyCoupon              = QString::number(obj["qyCoupon"].toDouble(), 'f', 3);
                response->balance.qyCouponCount         = obj["qyCouponCount"].toInt();

                // memberHistory
                if (obj["memberHistoryVOS"].isArray()) {
                    QJsonArray vos = obj["memberHistoryVOS"].toArray();
                    for (int i = 0; i < vos.size(); i++) {
                        QJsonObject history = vos[i].toObject();

                        memberHistory mem;
                        mem.userId = history["userId"].toVariant().toLongLong();
                        mem.startTime = history["startTime"].toVariant().toLongLong();
                        mem.endTime = history["endTime"].toVariant().toLongLong();

                        response->memberHistoryVOS.append(mem);
                    }
                }
            }
        } else {
            qDebug() << "parse json error, document is not a jsonobject ";
        }
    } else {
        qDebug() << "parse json error, document is NULL or jsonerror is " << jsonError.errorString();
    }
    emit sendResponseFormat(QSharedPointer<ResponseHead>(response));
}

//////////////////////////////////////////////////////////////////////////
HttpQueryPicturePrice::HttpQueryPicturePrice(QObject *parent)
    : HttpCmdRequest(RESTAPI_QUERY_PICTURE_PRICE, parent)
{

}

QByteArray HttpQueryPicturePrice::serializeParams()
{
    QJsonObject object;
    object = qstringToJsonObject("{}");
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpQueryPicturePrice::analysisResponsed(const QByteArray& data)
{
    PicturePriceResponse* response = new PicturePriceResponse();
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

            if (jsObj["data"].isObject()) {
                QJsonObject obj = jsObj["data"].toObject();

                response->memberRate = obj["memberRate"].toVariant().toDouble();
                response->commonRate = obj["commonRate"].toVariant().toDouble();
                response->maxMemberPrice = obj["maxMemberPrice"].toVariant().toDouble();
                response->maxCommonPrice = obj["maxCommonPrice"].toVariant().toDouble();
            }
        } else {
            qDebug() << "parse json error, document is not a jsonobject ";
        }
    } else {
        qDebug() << "parse json error, document is NULL or jsonerror is " << jsonError.errorString();
    }
    emit sendResponseFormat(QSharedPointer<ResponseHead>(response));
}

////////////////////////////////////////////////////////////////////////////////

HttpCreateAuthKeyCmd::HttpCreateAuthKeyCmd(QObject *parent)
    : HttpCmdRequest(RESTAPI_GENERATE_REALNAME_AUTH, parent)
{
}

QByteArray HttpCreateAuthKeyCmd::serializeParams()
{
    QJsonObject object;
    object = qstringToJsonObject("{}");
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpCreateAuthKeyCmd::analysisResponsed(const QByteArray& data)
{
    UserAuthKeyResponse* response = new UserAuthKeyResponse();
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

            if (jsObj["data"].isString()) {
                response->data = jsObj["data"].toString();
            }
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
