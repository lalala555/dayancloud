#pragma once
#ifndef HTTPGETPUBBYID_H
#define HTTPGETPUBBYID_H

#include <QObject>
#include "HttpCommand/HttpCmdRequest.h"

class HttpGetPubById : public HttpCmdRequest
{
	Q_OBJECT

public:
	HttpGetPubById(int id,QObject *parent = 0);
	~HttpGetPubById();
	QByteArray serializeParams();
private slots:
	void analysisResponsed(const QByteArray& data);
private:
	int m_publishId;
};
#endif // HTTQUERYPUBLISHLIST_H
