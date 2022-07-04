#ifndef HTTPCREATEJOBID_H
#define HTTPCREATEJOBID_H

#include "HttpCommand/HttpCmdRequest.h"
class CommandServer;

class HttpCreateJobId : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpCreateJobId(t_taskid oldJobId, QObject *parent = 0);
    HttpCreateJobId(int count, int createType, QObject *parent = 0);
    HttpCreateJobId(socket_id sid, CommandServer* commandserver, int count, int createType, QObject *parent = 0);
    ~HttpCreateJobId() {};
    QByteArray serializeParams();
    void setCreateIdCount(int count)
    {
        m_idCount = count;
    }

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    t_taskid m_oldJobId;
    int m_idCount;
    int m_createType;
    socket_id m_sid;
    CommandServer* m_commandserver;
};

#endif // HTTPCREATEJOBID_H
