#include "stdafx.h"
#include "HttpResponseParse.h"
#include "HttpCommand/HttpCommon.h"

MaxTaskItem *HttpResponseParse::parseTaskJson(const QJsonObject& obj)
{
    MaxTaskItem *item = new MaxTaskItem;

    item->abortFrames           = obj["abortFrames"].toInt();
    item->cgId                  = obj["cgId"].toInt();
    item->cloneOriginalId       = obj["cloneOriginalId"].toInt();
    item->cloneParentId         = obj["cloneParentId"].toInt();
    item->doneFrames            = obj["doneFrames"].toInt();
    item->executingFrames       = obj["executingFrames"].toInt();
    item->failedFrames          = obj["failedFrames"].toInt();
    item->isOpen                = obj["isOpen"].toInt();
    item->layerParentId         = obj["layerParentId"].toVariant().toLongLong();
    item->taskLevel             = obj["taskLevel"].toInt();
    item->taskLimit             = obj["taskLimit"].toInt();
    item->taskStatus            = obj["taskStatus"].toInt();
    item->totalFrames           = obj["totalFrames"].toInt();
    item->preTaskStatus         = obj["preTaskStatus"].toInt(25);
    item->isDelete              = obj["isDelete"].toInt();
    item->taskUserLevel         = obj["taskUserLevel"].toInt();
    item->taskRam               = obj["taskRam"].toInt();
    item->shareMainCapital      = obj["shareMainCapital"].toInt();
    item->stopType              = obj["stopType"].toInt();
    item->operable              = obj["operable"].toInt(1);
    item->isOverTime            = obj["isOverTime"].toInt(0);

    item->id                    = obj["id"].toVariant().toLongLong();
    item->completedDate         = obj["completedDate"].toVariant().toLongLong();
    item->renderDuration        = obj["renderDuration"].toVariant().toLongLong();
    item->startTime             = obj["startTime"].toVariant().toLongLong();
    item->submitDate            = obj["submitDate"].toVariant().toLongLong();
    item->taskOverTime          = obj["taskOverTime"].toVariant().toLongLong();
    item->overTimeStop          = obj["overTimeStop"].toVariant().toLongLong();
    item->userId                = obj["userId"].toVariant().toLongLong();

    item->framesRange           = obj["framesRange"].toString();
    item->keyValue              = obj["keyValue"].toString();
    item->layerName             = obj["layerName"].toString();
    item->munuTaskId            = obj["munuTaskId"].toString();
    item->outputFileName        = obj["outputFileName"].toString();
    item->producer              = obj["producer"].toString();
    item->projectName           = obj["projectName"].toString();
    item->renderCamera          = obj["renderCamera"].toString();
    item->sceneName             = obj["sceneName"].toString();
    item->statusText            = obj["statusText"].toString();
    item->taskTypeText          = obj["taskTypeText"].toString();
    item->userName              = obj["userName"].toString();
    item->taskAlias             = obj["taskAlias"].toString();
    item->preStatusText         = obj["preStatusText"].toString();
    item->locationOutput        = obj["locationOutput"].toString();
    item->taskType              = obj["taskType"].toString();
    item->channel               = obj["channel"].toInt();
    item->sceneLocalPath        = obj["sceneLocalPath"].toString();

    item->userAccountConsume    = QString::number(obj["userAccountConsume"].toDouble(), 'f', 3).toDouble();
    item->renderConsume         = QString::number(obj["renderConsume"].toDouble(), 'f', 3).toDouble();
    item->couponConsume         = QString::number(obj["couponConsume"].toDouble(), 'f', 3).toDouble();
    item->taskArrears           = QString::number(obj["taskArrears"].toDouble(), 'f', 3).toDouble();
    item->qyCouponConsume       = QString::number(obj["qyCouponConsume"].toDouble(), 'f', 3).toDouble();
    // item->differenceOfRenderConsume = QString::number(obj["differenceOfRenderConsume"].toDouble(), 'f', 3);
  

    if (obj["taskKeyValueVo"].isObject()) {
        QJsonObject taskKeyValueVo = obj["taskKeyValueVo"].toObject();
        item->tiles = taskKeyValueVo["tiles"].toString();
        item->ctrlPointCount = taskKeyValueVo["ctrlPointCount"].toString();
        item->picCount = taskKeyValueVo["picCount"].toString();
        item->jobType = taskKeyValueVo["jobType"].toString().toInt();
        item->progress = taskKeyValueVo["progress"].toString().toDouble();
        item->renderNum = taskKeyValueVo["renderNum"].toString().toInt();
        item->totalPixel = taskKeyValueVo["totalPixel"].toString();
        if (taskKeyValueVo["outputType"].isArray()) {
            QJsonArray types = taskKeyValueVo["outputType"].toArray();
            for (int i = 0; i < types.size(); i++) {
                item->outputType.append(types[i].toString() + ".zip");
            }
        }
    }

    return item;
}

PublishItem * HttpResponseParse::parsePublishJson(const QJsonObject & obj)
{
	PublishItem *item = new PublishItem;

	item->id = obj["id"].toInt();
	item->userId = obj["userId"].toInt();
	item->taskId = obj["taskId"].toInt();
	item->platform = obj["platform"].toInt();
	item->channel = obj["channel"].toInt();
	item->zone = obj["zone"].toInt();
	item->publishName = obj["publishName"].toString();
	item->coverImgUrl = obj["coverImgUrl"].toString();
	item->des = obj["des"].toString();
	item->isNeedKey = obj["isNeedKey"].toInt();
	item->secretKey = obj["secretKey"].toString();
	item->concurrentCount = obj["concurrentCount"].toInt();
	item->publishTime = obj["publishTime"].toVariant().toLongLong();
	item->expiredTime = obj["expiredTime"].toVariant().toLongLong();
	item->isDelete = obj["isDelete"].toInt();
	item->url = obj["url"].toString();
	item->status = obj["status"].toInt();
	item->type = enum PublishItem::modelType(obj["modelType"].toInt());
	return item;
}

PubTask * HttpResponseParse::parsePubTask(const QJsonObject & obj)
{
	//PubTask *task = new PubTask;
	PubTask *task = new PubTask();
	task->publishId = obj["publishId"].toInt();
	task->publishJobId = obj["publishJobId"].toInt();
	task->publishJobMunuId = obj["publishJobMunuId"].toString();
	task->startTime = obj["startTime"].toVariant().toLongLong();
	task->submitTime = obj["submitTime"].toVariant().toLongLong();
	task->completedTime = obj["completedTime"].toVariant().toLongLong();
	task->status = obj["status"].toInt();
	task->duration = obj["duration"].toInt();
	task->consume = obj["consume"].toDouble();
	task->couponConsume = obj["couponConsume"].toDouble();
	task->unitPrice = obj["unitPrice"].toDouble();
	return task;
}
