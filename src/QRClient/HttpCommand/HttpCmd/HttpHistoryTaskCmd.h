#ifndef HTTPHISTORYTASKCMD_H
#define HTTPHISTORYTASKCMD_H

#include "HttpCommand/HttpCmdRequest.h"

class HttpHistoryTaskCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpHistoryTaskCmd(QObject *parent = 0);
    ~HttpHistoryTaskCmd() {};
    QByteArray serializeParams();
    void setExternalParams(const QString& params);
private slots:
    void analysisResponsed(const QByteArray& data);
private:
    int m_pageSize;
    int m_pageNum;
    int m_historyTaskFlag;
};

#endif // HTTPHISTORYTASKCMD_H
