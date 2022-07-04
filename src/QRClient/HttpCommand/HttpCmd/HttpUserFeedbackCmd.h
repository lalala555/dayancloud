#ifndef HTTPUSERFEEDBACKCMD_H
#define HTTPUSERFEEDBACKCMD_H

#include <QObject>
#include "HttpCommand/HttpCmdRequest.h"

class HttpUserFeedbackCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpUserFeedbackCmd(const QString& userName, const QString& detail,
                        const QString& contactWay, int type, int channel, const QString& version, QObject *parent = 0);
    ~HttpUserFeedbackCmd();
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);
private:
    QString m_userName;
    QString m_detail;
    QString m_contactWay;
    int m_type; // 反馈类型（1:产品建议 2：程序错误）
    int m_channel; // 客户端版本0：动画版客户端，1：效果图客户端，2：renderbus网页版，3：foxrenderfarm网页版，4：3d66，5：oumo，6：linecg，7：云舞
    QString m_version; // 客户端版本
};

#endif // HTTPUSERFEEDBACKCMD_H
