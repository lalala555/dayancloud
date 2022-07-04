#ifndef HTTPRENDERLOG_H
#define HTTPRENDERLOG_H

#include "HttpCommand/HttpCmdRequest.h"

class HttpRenderLog : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpRenderLog(qint64 frameId, int pageNum, const QString& renderingType, qint32 pageSize, QObject *parent = 0);
    ~HttpRenderLog();

    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);
    
private:
    qint64  m_frameId;
    int     m_pageNum;
    qint32  m_pageSize;
    QString m_renderingType;
};

//////////////////////////////////////////////////////////////////////////
class HttpAtPointCloudDataUrl : public HttpCmdRequest
{
        Q_OBJECT

public:
	HttpAtPointCloudDataUrl(qint64 userId, qint64 taskId, QObject *parent = 0);
    ~HttpAtPointCloudDataUrl();

    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);
    
private:
    qint64 m_taskId;
    qint64 m_userId;
};

//////////////////////////////////////////////////////////////////////////
class HttpShareResultLink : public HttpCmdRequest
{
    Q_OBJECT

public:
    HttpShareResultLink(qint64 taskId, int days = 7, QObject *parent = 0);
    ~HttpShareResultLink(){}
    QByteArray serializeParams();

private slots:
    void analysisResponsed(const QByteArray& data);

private:
    qint64 m_taskId;
    int m_days;
};


//////////////////////////////////////////////////////////////////////////
class HttpAtReportUrl : public HttpCmdRequest
{
	Q_OBJECT

public:
	HttpAtReportUrl(qint64 userId, qint64 taskId, QObject *parent = 0);
	~HttpAtReportUrl();

	QByteArray serializeParams();

	private slots:
	void analysisResponsed(const QByteArray& data);

private:
	qint64 m_taskId;
	qint64 m_userId;
};

//////////////////////////////////////////////////////////////////////////
class HttpCheckImagePosFileMd5 : public HttpCmdRequest
{
	Q_OBJECT

public:
	HttpCheckImagePosFileMd5(qint64 userId, qint64 taskId,QString fileMd5, QObject *parent = 0);
	~HttpCheckImagePosFileMd5();

	QByteArray serializeParams();

	private slots:
	void analysisResponsed(const QByteArray& data);

private:
	qint64 m_taskId;
	qint64 m_userId;
	QString m_fileMd5;
};



#endif // HTTPRENDERLOG_H

