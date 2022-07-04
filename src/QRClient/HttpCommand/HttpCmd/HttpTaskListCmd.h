#ifndef HTTPTASKLISTCMD_H
#define HTTPTASKLISTCMD_H

#include <QObject>
#include <QThread>
#include "HttpCommand/HttpCmdRequest.h"

class HttpTaskListCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpTaskListCmd(const QString& timeFilter, int showType, int pageNum = 1, int pageSize = PAGE_SIZE, int renderingFlag = 1, QObject *parent = 0); // 时间筛选

    ~HttpTaskListCmd() {};
    QByteArray serializeParams();
    QList<MaxTaskItem *> analysisRespList(const QJsonArray& jsonArr);
    void setRepeat(bool isRepeat) { m_isRepeat = isRepeat; }

private slots:
    void analysisResponsed(const QByteArray& data);
    void analysisThread(const QByteArray& data);

private:
    int     m_pageSize;
    int     m_pageNum;
    int     m_renderingFlag;
    int     m_showType;
    bool    m_isRepeat;// 是否是循环请求
    QString m_timeFilter;
};

#endif // HTTPTASKLISTCMD_H
