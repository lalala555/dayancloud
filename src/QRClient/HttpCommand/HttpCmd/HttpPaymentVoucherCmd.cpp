#include "stdafx.h"
#include "HttpPaymentVoucherCmd.h"

HttpPaymentVoucherCmd::HttpPaymentVoucherCmd(qint64 taskId, QObject *parent)
    : HttpCmdRequest(RESTAPI_PAYMENT_USE_VOUCHER, parent)
    , m_taskId(taskId)
{
}

HttpPaymentVoucherCmd::~HttpPaymentVoucherCmd()
{
}

QByteArray HttpPaymentVoucherCmd::serializeParams()
{
    QJsonObject object;
    object.insert("taskId", m_taskId);
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

void HttpPaymentVoucherCmd::analysisResponsed(const QByteArray& data)
{
    PaymentUseVoucherResponse* response = new PaymentUseVoucherResponse;
    response->cmdType = m_cmdType;
    response->rawData = data;
    response->taskId  = m_taskId;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if (!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if (jsDoc.isObject()) {
            QJsonObject jsObj = jsDoc.object();
            response->version = jsObj["version"].toString();
            response->result  = jsObj["result"].toBool();
            response->msg     = jsObj["message"].toString();
            response->data    = jsObj["data"].toString();
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

////////////////////////////////////////////////////////////////////////////////////////////////////
HttpPaymentQueryVoucherCmd::HttpPaymentQueryVoucherCmd(QObject *parent)
    : HttpCmdRequest(RESTAPI_PAYMENT_QUERY_VOUCHER, parent)
{

}

QByteArray HttpPaymentQueryVoucherCmd::serializeParams()
{
    QJsonObject object;
    object = qstringToJsonObject("{}");
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

VoucherInfo parseVoucherInfo(const QJsonObject& effective)
{
    VoucherInfo info;
    info.id            = effective["id"].toVariant().toLongLong();
    info.couponNo      = effective["couponNo"].toString();
    info.deduckAmount  = effective["deduckAmount"].toVariant().toLongLong();
    info.deductTask    = effective["deductTask"].toVariant().toLongLong();
    info.effectiveDate = effective["effectiveDate"].toVariant().toLongLong();
    info.expireDate    = effective["expireDate"].toVariant().toLongLong();
    info.getWay        = effective["getWay"].toInt();
    info.remark        = effective["remark"].toString();
    info.updateDate    = effective["updateDate"].toVariant().toLongLong();
    info.used          = effective["used"].toInt();
    info.useDate       = effective["useDate"].toVariant().toLongLong();
    info.userId        = effective["userId"].toVariant().toLongLong();

    return info;
}

void HttpPaymentQueryVoucherCmd::analysisResponsed(const QByteArray& data)
{
    PaymentQueryVoucherResponse* response = new PaymentQueryVoucherResponse;
    response->cmdType = m_cmdType;
    response->rawData = data;

    QJsonParseError jsonError;
    QJsonDocument jsDoc = QJsonDocument::fromJson(data, &jsonError);
    if (!jsDoc.isNull() && jsonError.error == QJsonParseError::NoError) {
        if (jsDoc.isObject()) {
            QJsonObject jsObj = jsDoc.object();
            response->version = jsObj["version"].toString();
            response->result  = jsObj["result"].toBool();
            response->msg     = jsObj["message"].toString();
            
            if (jsObj["data"].isObject()) {
                QJsonObject jsData = jsObj["data"].toObject();
                // ø…”√»Ø
                if (jsData["effectiveVoucher"].isArray()) {
                    QJsonArray effectives = jsData["effectiveVoucher"].toArray();
                    for (int j = 0; j < effectives.count(); j++) {
                        QJsonObject voucher = effectives[j].toObject();
                        response->effectiveVouchers.append(parseVoucherInfo(voucher));
                    }
                }
                //  ß–ß»Ø
                if (jsData["invalidVoucher"].isArray()) {
                    QJsonArray effectives = jsData["invalidVoucher"].toArray();
                    for (int j = 0; j < effectives.count(); j++) {
                        QJsonObject voucher = effectives[j].toObject();
                        response->invalidVouchers.append(parseVoucherInfo(voucher));
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
