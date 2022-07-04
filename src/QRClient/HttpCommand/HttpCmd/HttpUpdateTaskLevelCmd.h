#ifndef HTTPUPDATETASKLEVELCMD_H
#define HTTPUPDATETASKLEVELCMD_H

#include "HttpCommand/HttpCmdRequest.h"

class HttpUpdateTaskLevelCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpUpdateTaskLevelCmd(qint64 taskId, const QString& level, QObject *parent = 0);
    ~HttpUpdateTaskLevelCmd();
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    qint64  m_taskId;
    QString m_level;
};

#endif // HTTPUPDATETASKLEVELCMD_H
