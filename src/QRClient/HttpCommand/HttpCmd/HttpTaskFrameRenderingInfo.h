#ifndef HTTPTASKFRAMERENDERINGINFO_H
#define HTTPTASKFRAMERENDERINGINFO_H

#include "HttpCommand/HttpCmdRequest.h"

class HttpTaskFrameRenderingInfo : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpTaskFrameRenderingInfo(qint64 taskId, qint32 pageNum, qint32 pageSize, const QString& searchKey, const QList<int>& stateList = QList<int>(), QObject *parent = 0);
    ~HttpTaskFrameRenderingInfo();
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    qint32  m_pageSize;
    qint32  m_pageNum;
    qint64  m_taskId;
    QString m_searchKey;
    QList<int> m_stateList;
};

#endif // HTTPTASKFRAMERENDERINGINFO_H
