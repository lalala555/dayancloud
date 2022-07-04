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
    int m_type; // �������ͣ�1:��Ʒ���� 2���������
    int m_channel; // �ͻ��˰汾0��������ͻ��ˣ�1��Ч��ͼ�ͻ��ˣ�2��renderbus��ҳ�棬3��foxrenderfarm��ҳ�棬4��3d66��5��oumo��6��linecg��7������
    QString m_version; // �ͻ��˰汾
};

#endif // HTTPUSERFEEDBACKCMD_H
