#ifndef HTTPTASKPROCESSIMG_H
#define HTTPTASKPROCESSIMG_H

#include <QObject>
#include "HttpCommand/HttpCmdRequest.h"

class HttpTaskProcessImg : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpTaskProcessImg(int taskid, int frameType, QObject *parent = 0);
    ~HttpTaskProcessImg();
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    int     m_frameType;
    qint64  m_taskId;
};

#endif // HTTPTASKPROCESSIMG_H
