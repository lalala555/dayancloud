#ifndef HTTPQUERYTASKINFO_H
#define HTTPQUERYTASKINFO_H

#include "HttpCommand/HttpCmdRequest.h"

class HttpQueryTaskInfo : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpQueryTaskInfo(QList<qint64> taskIds, bool needFresh = true, bool freshView = false, QObject *parent = 0);
    ~HttpQueryTaskInfo();
    QByteArray serializeParams();
    QList<MaxTaskItem *> analysisRespList(const QJsonArray& jsonArr);

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    QList<qint64> m_taskIds;
    bool m_needFresh;
    bool m_freshView;
};

#endif // HTTPQUERYTASKINFO_H
