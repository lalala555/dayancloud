#pragma once
#ifndef HTTQUERYPUBLISHLIST_H
#define HTTQUERYPUBLISHLIST_H

#include <QObject>
#include "HttpCommand/HttpCmdRequest.h"

class HttpQueryPublishList : public HttpCmdRequest
{
	Q_OBJECT

public:
	HttpQueryPublishList(QString publishName, QString publishId, int status,QObject *parent = 0);
	~HttpQueryPublishList();
	QByteArray serializeParams();
private slots:
	void analysisResponsed(const QByteArray& data);
private:
	QString m_publishId;
	QString m_publishName;
	int m_status;
};
#endif // HTTQUERYPUBLISHLIST_H
