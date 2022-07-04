#pragma once
#ifndef HttpUpdatePub_H
#define HttpUpdatePub_H

#include <QObject>
#include "HttpCommand/HttpCmdRequest.h"

class HttpUpdatePub : public HttpCmdRequest
{
	Q_OBJECT

public:
	HttpUpdatePub(const PublishItem &item,QObject *parent = 0);
	~HttpUpdatePub();
	QByteArray serializeParams();
private slots:
	void analysisResponsed(const QByteArray& data);
private:
	int m_task_id; //壳id
	QString m_publishName; //任务名称
	QString m_coverImgUrl;//封面图片
	int m_modelType;//类型
	QString m_desc;//描述
	int m_isNeedkey; //是否需要密匙
	QString m_secretKey; //密钥
	int m_concurrentCount;//链接打开并发数
	qint64 m_expiredDay;//失效天数
};
#endif // HTTQUERYPUBLISHLIST_H
