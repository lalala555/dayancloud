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
	int m_task_id; //��id
	QString m_publishName; //��������
	QString m_coverImgUrl;//����ͼƬ
	int m_modelType;//����
	QString m_desc;//����
	int m_isNeedkey; //�Ƿ���Ҫ�ܳ�
	QString m_secretKey; //��Կ
	int m_concurrentCount;//���Ӵ򿪲�����
	int m_expiredDay;//ʧЧ����
};
#endif // HTTQUERYPUBLISHLIST_H
