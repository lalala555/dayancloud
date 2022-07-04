#pragma once

#include <QObject>

class MaxTaskItem;
class PublishItem;
class PubTask;

namespace HttpResponseParse
{
    MaxTaskItem *parseTaskJson(const QJsonObject& obj);

	//���ط����б���
	PublishItem *parsePublishJson(const QJsonObject& obj);

	//���ط����������б���
	PubTask *parsePubTask(const QJsonObject& obj);
}
