/************************************************************************/
/* desc: º”‘ÿ»ŒŒÒÀı¬‘Õº
*  api: /api/rendering/task/renderingTask/loadingTaskThumbnail
/************************************************************************/

#ifndef HTTPLOADTASKTHUMBNAIL_H
#define HTTPLOADTASKTHUMBNAIL_H

#include "HttpCommand/HttpCmdRequest.h"

class HttpLoadTaskThumbnail : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpLoadTaskThumbnail(qint64 frameId, int frameStatus, QObject *parent = 0);
    ~HttpLoadTaskThumbnail();
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    qint64 m_frameId;
    int m_frameStatus;
};

#endif // HTTPLOADTASKTHUMBNAIL_H
