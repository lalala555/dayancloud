#ifndef HTTPTASKRESUBFAILFRAMECMD_H
#define HTTPTASKRESUBFAILFRAMECMD_H

#include <QObject>
#include "HttpCommand/HttpCmdRequest.h"

class HttpTaskResubFailFrameCmd : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpTaskResubFailFrameCmd(const QList<int>& resubStatus, const QList<qint64>& taskIds, QObject *parent = 0);
    HttpTaskResubFailFrameCmd(int resubStatus, const qint64 taskId, QObject *parent = 0);
    ~HttpTaskResubFailFrameCmd() {};
    QByteArray serializeParams();
	void setSurveyPointOption(const QString& filePath);

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    QList<qint64> m_taskIds;
    QList<int> m_resubStatus;

	QString m_option;  // rebuild
	int m_atType;      // 0使用GPS/POS做刚体变换
				           // 1使用控制点做平差
	QString m_surveyPointFile;
};

class HttpUploadJsonFileCmd : public HttpCmdRequest
{
    Q_OBJECT
public:
    HttpUploadJsonFileCmd(qint64 taskId, const QString& filePath, QObject *parent = 0);
    ~HttpUploadJsonFileCmd() {}
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    qint64 m_taskId;
    QString m_filePath;
};

#endif // HTTPTASKRESUBFAILFRAMECMD_H
