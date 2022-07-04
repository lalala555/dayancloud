#include "stdafx.h"
#include "HttpBalanceCmd.h"
#include "HttpCommand/HttpCommon.h"

HttpBalanceCmd::HttpBalanceCmd(QObject *parent)
    : HttpCmdRequest(RESTAPI_USER_BALANCE, parent)
{
}

QByteArray HttpBalanceCmd::serializeParams()
{
    QJsonObject object = qstringToJsonObject("{}");
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpBalanceCmd::analysisResponsed(const QByteArray& data)
{
    BalanceResponse* response = new BalanceResponse();
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
                response->user_id               = obj["id"].toVariant().toLongLong();
                response->rmbbalance            = QString::number(obj["rmbbalance"].toDouble(), 'f', 3);
                response->usdbalance            = QString::number(obj["usdbalance"].toDouble(), 'f', 3);
                response->coupon                = QString::number(obj["coupon"].toDouble(), 'f', 3);
                response->vipLevel              = obj["level"].toInt();
                response->downloadDisable       = obj["downloadDisable"].toInt();
                response->isPicture             = obj["businessType"].toInt() == 0 ? true : false;
                response->isPicturePro          = obj["pictureLever"].toInt() ? true : false;
                response->pictureProExpiredDate = obj["pictureProExpiredDate"].toVariant().toLongLong();
                response->coupon                = QString::number(obj["commonCoupon"].toDouble(), 'f', 3);
                response->couponCount           = obj["commonCouponCount"].toInt();
                response->qyCoupon              = QString::number(obj["qyCoupon"].toDouble(), 'f', 3);
                response->qyCouponCount         = obj["qyCouponCount"].toInt();
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
