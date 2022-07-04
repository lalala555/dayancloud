#ifndef HTTPMAXSUBMITCMD_H
#define HTTPMAXSUBMITCMD_H

#include "HttpCommand/HttpCmdRequest.h"

class HttpMaxSubmitCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpMaxSubmitCmd(QObject *parent = 0);
    ~HttpMaxSubmitCmd() {};
    QByteArray serializeParams();
    void setExternalParams(const QString& params);

private slots:
    void analysisResponsed(const QByteArray& data);
private:
    MaxCommitParams m_params;
};

#endif // HTTPMAXSUBMITCMD_H
