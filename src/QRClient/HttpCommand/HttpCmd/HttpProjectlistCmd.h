#ifndef HTTPPROJECTLISTCMD_H
#define HTTPPROJECTLISTCMD_H

#include <QObject>
#include "HttpCommand/HttpCmdRequest.h"

class HttpProjectlistCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpProjectlistCmd(qint64 operateType, QObject *parent = 0);
    HttpProjectlistCmd(qint64 operateType, QString proName, QObject *parent = 0);
    ~HttpProjectlistCmd() {};
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    qint64  m_operateType;
    QString m_projectName;
};

// ��ȡ���е���Ŀ�����������˺ŵ� ,ֻ���ɸѡʱʹ��
class HttpProjectNamesCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpProjectNamesCmd(QObject *parent = 0);
    ~HttpProjectNamesCmd() {};
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);
};

#endif // HTTPPROJECTLISTCMD_H
