#ifndef HTTPTASKOPERATERCMD_H
#define HTTPTASKOPERATERCMD_H

#include <QObject>
#include "HttpCommand/HttpCmdRequest.h"

class HttpTaskOperaterCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpTaskOperaterCmd(int optType, QList<qint64> taskIds, const QString& option = "render", QObject *parent = 0);
    HttpTaskOperaterCmd(int optType, qint64 taskId, const QString& option = "render", QObject *parent = 0);
    ~HttpTaskOperaterCmd() { m_taskIds.clear(); };
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    QList<qint64> m_taskIds;
    qint64 m_taskId;
    QString m_option;
};

#endif // HTTPTASKOPERATERCMD_H
