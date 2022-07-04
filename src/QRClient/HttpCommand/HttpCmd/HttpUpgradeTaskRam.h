/************************************************************************/
/* 用户升级任务内存
*  /api/rendering/task/renderingTask/upgradeTaskRam
/************************************************************************/

#ifndef HTTPUPGRADETASKRAM_H
#define HTTPUPGRADETASKRAM_H

#include "HttpCommand/HttpCmdRequest.h"

class HttpUpgradeTaskRam : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpUpgradeTaskRam(qint64 taskId, int ram, QObject *parent = 0);
    HttpUpgradeTaskRam(const QList<qint64>& taskIds, int ram, QObject *parent = 0);
    ~HttpUpgradeTaskRam();
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    qint64 m_taskId;
    int    m_ram;
    QList<qint64> m_taskIds;
};

#endif // HTTPUPGRADETASKRAM_H
