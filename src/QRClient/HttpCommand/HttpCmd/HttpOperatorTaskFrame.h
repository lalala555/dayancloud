#ifndef HTTPOPERATORTASKFRAME_H
#define HTTPOPERATORTASKFRAME_H

#include <QObject>
#include "HttpCommand/HttpCmdRequest.h"

class HttpOperatorTaskFrame : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpOperatorTaskFrame(qint64 taskId, qint64 frameId, int selectAll, int optType, QObject *parent = nullptr);
    HttpOperatorTaskFrame(qint64 taskId, QList<qint64> frameIds, int selectAll, int optType, QObject *parent = nullptr);
    ~HttpOperatorTaskFrame();
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    int     m_optType;
    qint64  m_taskId;
    int     m_selectAll;
    QList<qint64>  m_frameIds; 
};

#endif // HTTPOPERATORTASKFRAME_H
