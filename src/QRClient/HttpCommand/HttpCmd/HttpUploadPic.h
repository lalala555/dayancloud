#pragma once
#ifndef HttpUploadPic_H
#define HttpUploadPic_H

#include <QObject>
#include "HttpCommand/HttpCmdRequest.h"
#include <QFile>

class HttpUploadPic : public HttpCmdRequest
{
	Q_OBJECT

public:
	HttpUploadPic(QString filepath,QObject *parent = 0);
	~HttpUploadPic();
	QByteArray serializeParams();
private slots:
	void analysisResponsed(const QByteArray& data);
public:
	QString m_filepath;
};
#endif // HTTQUERYPUBLISHLIST_H
