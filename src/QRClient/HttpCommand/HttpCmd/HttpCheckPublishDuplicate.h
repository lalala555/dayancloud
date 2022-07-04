#pragma once
#ifndef HttpCheckPublishDuplicate_H
#define HttpCheckPublishDuplicate_H

#include <QObject>
#include "HttpCommand/HttpCmdRequest.h"

class HttpCheckPublishDuplicate : public HttpCmdRequest
{
	Q_OBJECT

public:
	HttpCheckPublishDuplicate(int id,QObject *parent = 0);
	~HttpCheckPublishDuplicate();
	QByteArray serializeParams();
private slots:
	void analysisResponsed(const QByteArray& data);
private:
	int m_publishJobId; //jobid
};
#endif // HTTQUERYPUBLISHLIST_H
