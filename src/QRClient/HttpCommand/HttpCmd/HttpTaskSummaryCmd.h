#ifndef HTTPTASKSUMMARYCMD_H
#define HTTPTASKSUMMARYCMD_H

#include <QObject>
#include "HttpCommand/HttpCmdRequest.h"

class HttpTaskSummaryCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpTaskSummaryCmd(qint32 userId, QObject *parent = 0);
    ~HttpTaskSummaryCmd() {};
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    qint32 m_userId;
};

#endif // HTTPTASKSUMMARYCMD_H
