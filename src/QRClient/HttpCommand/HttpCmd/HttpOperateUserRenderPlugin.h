/************************************************************************/
/* desc: ±‡º≠”√ªß‰÷»æ∆˜≤Âº˛≈‰÷√
*  api: /api/rendering/task/common/editUserRenderPlugin
/************************************************************************/

#ifndef HTTPEDITUSERRENDERPLUGIN_H
#define HTTPEDITUSERRENDERPLUGIN_H

#include "HttpCommand/HttpCmdRequest.h"

class HttpOperateUserRenderPlugin : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpOperateUserRenderPlugin(int optType, OperateUserRenderPlugin* opt, QObject *parent = 0);
    ~HttpOperateUserRenderPlugin();
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    OperateUserRenderPlugin* m_opt;
};

#endif // HTTPEDITUSERRENDERPLUGIN_H
