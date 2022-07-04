#pragma once
#ifndef HttpDeletePublish_H
#define HttpDeletePublish_H

#include <QObject>
#include "HttpCommand/HttpCmdRequest.h"

class HttpDeletePublish : public HttpCmdRequest
{
	Q_OBJECT

public:
	HttpDeletePublish(int id,QObject *parent = 0);
	~HttpDeletePublish();
	QByteArray serializeParams();
private slots:
	void analysisResponsed(const QByteArray& data);
private:
	int m_publishid; //¿Çid
};
#endif // HTTQUERYPUBLISHLIST_H
