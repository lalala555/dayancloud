#ifndef HTTPUPDATETASKREMARKCMD_H
#define HTTPUPDATETASKREMARKCMD_H

#include "HttpCommand/HttpCmdRequest.h"

class HttpUpdateTaskRemarkCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpUpdateTaskRemarkCmd(qint64 taskId, const QString& remark, QObject *parent = 0);
    ~HttpUpdateTaskRemarkCmd();
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    qint64  m_taskId;
    QString m_remark;
};

#endif // HTTPUPDATETASKREMARKCMD_H
