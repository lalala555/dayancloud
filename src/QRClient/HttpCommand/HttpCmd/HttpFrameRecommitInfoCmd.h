#pragma once

#include <QObject>
#include "HttpCommand/HttpCmdRequest.h"

class HttpFrameRecommitInfoCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpFrameRecommitInfoCmd(qint64 frameid, int pageNum, qint64 pageSize, QObject *parent = 0);
    ~HttpFrameRecommitInfoCmd();
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    qint64 m_frameId;
    qint64 m_pageSize;
    int m_pageNum;  
};
