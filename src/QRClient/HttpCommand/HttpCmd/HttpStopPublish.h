#pragma once
#ifndef HttpStopPublish_H
#define HttpStopPublish_H

#include <QObject>
#include "HttpCommand/HttpCmdRequest.h"

class HttpStopPublish : public HttpCmdRequest
{
	Q_OBJECT

public:
	HttpStopPublish(int id,QObject *parent = 0);
	~HttpStopPublish();
	QByteArray serializeParams();
private slots:
	void analysisResponsed(const QByteArray& data);
private:
	int m_publishJobId; //jobid
};
#endif // HTTQUERYPUBLISHLIST_H
