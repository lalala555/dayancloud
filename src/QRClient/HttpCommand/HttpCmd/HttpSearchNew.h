#ifndef HttpSearchNew_H
#define HttpSearchNew_H

#include <QObject>
#include "kernel/msg.h"
#include "HttpCommand/HttpCmdRequest.h"

class HttpSearchNew : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpSearchNew(const QString& searchKey = "", int pageNum = 1, int pageSize = PAGE_SIZE, int renderingFlag = 1, QObject *parent = 0);
    HttpSearchNew(const FilterContent& content, QObject *parent = 0);
    ~HttpSearchNew() {};
    QByteArray serializeParams();
    QList<MaxTaskItem *> analysisRespList(const QJsonArray& jsonArr);
    void setRepeat(bool isRepeat) { m_isRepeat = isRepeat; }

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    int m_pageSize;
    int m_pageNum;
    int m_renderingTaskFlag;
    QString m_searchKey;
    FilterContent m_content;
    bool m_isRepeat;// 是否是循环请求
};


#endif // HttpSearchNew_H
