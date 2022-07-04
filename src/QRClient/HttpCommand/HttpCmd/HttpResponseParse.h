#pragma once

#include <QObject>

class MaxTaskItem;
class PublishItem;
class PubTask;

namespace HttpResponseParse
{
    MaxTaskItem *parseTaskJson(const QJsonObject& obj);

	//返回发布列表类
	PublishItem *parsePublishJson(const QJsonObject& obj);

	//返回发布中任务列表类
	PubTask *parsePubTask(const QJsonObject& obj);
}
