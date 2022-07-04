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
	int m_task_id; //��id
	QString m_publishName; //��������
	QString m_coverImgUrl;//����ͼƬ
	int m_modelType;//����
	QString m_desc;//����
	int m_isNeedkey; //�Ƿ���Ҫ�ܳ�
	QString m_secretKey; //��Կ
	int m_concurrentCount;//���Ӵ򿪲�����
	qint64 m_expiredDay;//ʧЧ����
};
#endif // HTTQUERYPUBLISHLIST_H
