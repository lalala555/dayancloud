#ifndef HTTPFILTERTASKLISTCMD_H
#define HTTPFILTERTASKLISTCMD_H

#include <QObject>
#include "HttpCommand/HttpCmdRequest.h"

class HttpFilterTaskListCmd : public HttpCmdRequest
{
public:
    HttpFilterTaskListCmd(const QString& timeRange, int showType, int pageNum = 1, 
        int pageSize = PAGE_SIZE, int renderingFlag = 1, QObject *parent = 0);
    ~HttpFilterTaskListCmd();
    QByteArray serializeParams();
    QList<MaxTaskItem *> analysisRespList(const QJsonArray& jsonArr);

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    int m_pageSize;
    int m_pageNum;
    int m_renderingTaskFlag;
    int m_showType;
    QString m_timeRange;
};

#endif // HTTPFILTERTASKLISTCMD_H
