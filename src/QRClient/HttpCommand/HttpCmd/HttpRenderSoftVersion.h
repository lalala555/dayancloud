/************************************************************************/
/* 获取渲染器插件版本信息
*  /api/rendering/task/common/getRenderSoftVersion
/************************************************************************/

#ifndef HTTPRENDERSOFTVERSION_H
#define HTTPRENDERSOFTVERSION_H

#include "HttpCommand/HttpCmdRequest.h"

class HttpRenderSoftVersion : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpRenderSoftVersion(qint64 cgId, const QString& osName, QObject *parent = 0);
    ~HttpRenderSoftVersion();
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    qint64  m_cgId;
    QString m_osName;
};

#endif // HTTPRENDERSOFTVERSION_H
