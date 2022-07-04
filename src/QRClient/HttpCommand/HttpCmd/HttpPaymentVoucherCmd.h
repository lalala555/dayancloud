#pragma once

#include <QObject>
#include "HttpCommand/HttpCmdRequest.h"

// ʹ�õֿ�ȯ
class HttpPaymentVoucherCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpPaymentVoucherCmd(qint64 taskId, QObject *parent = 0);
    ~HttpPaymentVoucherCmd();
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    qint64 m_taskId;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// ��ѯ�ֿ�ȯ
class HttpPaymentQueryVoucherCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpPaymentQueryVoucherCmd(QObject *parent = 0);
    ~HttpPaymentQueryVoucherCmd() {};
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);
};
