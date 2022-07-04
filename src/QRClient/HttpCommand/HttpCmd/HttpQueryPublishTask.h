#pragma once
#ifndef HTTPQUERYPUBLISHTASK_H
#define HTTPQUERYPUBLISHTASK_H

#include <QObject>
#include "HttpCommand/HttpCmdRequest.h"

class HttpQueryPublishTask : public HttpCmdRequest
{
	Q_OBJECT

public:
	HttpQueryPublishTask(int id,QObject *parent = 0);
	~HttpQueryPublishTask();
	QByteArray serializeParams();
private slots:
	void analysisResponsed(const QByteArray& data);
private:
	int m_task_id; //¿Çid
};
#endif // HTTQUERYPUBLISHLIST_H
