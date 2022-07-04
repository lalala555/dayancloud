#pragma once
#ifndef HTTPCREATEPUBLISH_H
#define HTTPCREATEPUBLISH_H

#include <QObject>
#include "HttpCommand/HttpCmdRequest.h"

class HttpCreatePublish : public HttpCmdRequest
{
	Q_OBJECT

public:
	HttpCreatePublish(const PublishItem &item,QObject *parent = 0);
	~HttpCreatePublish();
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
	int m_expiredDay;//失效天数
};
#endif // HTTQUERYPUBLISHLIST_H
