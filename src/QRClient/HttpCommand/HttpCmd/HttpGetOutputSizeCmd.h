#pragma once

#include "HttpCommand/HttpCmdRequest.h"

class HttpGetOutputSizeCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpGetOutputSizeCmd(qint64 taskid, int fromPage, QObject *parent = 0);
    HttpGetOutputSizeCmd(const QList<qint64>& taskids, int fromPage, QObject *parent = 0);
    ~HttpGetOutputSizeCmd();
    QByteArray serializeParams();

 private slots:
    void analysisResponsed(const QByteArray& data);

private:
    QList<qint64> m_taskIds;
    int m_fromPage;
};
