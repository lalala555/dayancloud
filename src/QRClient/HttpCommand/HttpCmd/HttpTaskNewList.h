#ifndef HttpTaskNewList_H
#define HttpTaskNewList_H

#include <QObject>
#include <QThread>
#include "HttpCommand/HttpCmdRequest.h"

class HttpTaskNewList : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpTaskNewList(const QString& timeFilter, int showType, int pageNum = 1, int pageSize = PAGE_SIZE, int renderingFlag = 1, QObject *parent = 0); // 时间筛选

    ~HttpTaskNewList() {};
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

#endif // HttpTaskNewList_H
