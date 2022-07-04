#ifndef HTTPSUBTASKCMD_H
#define HTTPSUBTASKCMD_H

#include <QObject>
#include "HttpCommand/HttpCmdRequest.h"

class HttpSubTaskCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpSubTaskCmd(qint64 taskId, const QString& artist, const QString& sceneName , QObject *parent = 0);
    ~HttpSubTaskCmd() {};
    void setATOption(int type);
    void setRebuildOption(const RebuildOptionParams& params);
    void setSurveyPointOption(const QString& filePath);
    QByteArray serializeParams();
    qint64 getTaskId()
    {
        return m_taskId;
    }
    bool isTimeout(int timeVal)
    {
        QDateTime currentTime = QDateTime::currentDateTime();
        int secs = m_submitTime.secsTo(currentTime);
        return secs > timeVal;
    };

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    qint64 m_taskId;
    QString m_artist;
    QString m_sceneName;
    QDateTime m_submitTime;
    QString m_option; // AT or rebuild

    int m_atType;  // 0使用GPS/POS做刚体变换
                   // 1使用控制点做平差

    RebuildOptionParams m_params;

    QString m_surveyPointFile;
};

//////////////////////////////////////////////////////////////////////////
class HttpRecommitCCAT : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpRecommitCCAT(qint64 taskId, int type, QObject *parent = 0);
    ~HttpRecommitCCAT() {};
    QByteArray serializeParams();
    qint64 getTaskId()
    {
        return m_taskId;
    }

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    qint64 m_taskId;
    int m_atType;  // 0使用GPS/POS做刚体变换
                   // 1使用控制点做平差
};

//////////////////////////////////////////////////////////////////////////
class HttpCloneTask : public HttpCmdRequest
{
    Q_OBJECT
public:
    HttpCloneTask(qint64 taskId, const QString& artist, const QString& projectName, const RebuildOptionParams& params, QObject *parent = 0);
    ~HttpCloneTask() {}
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    qint64 m_taskId;
    QString m_artist;
    QString m_projectName;
    RebuildOptionParams m_params;
};

#endif // HTTPSUBTASKCMD_H
