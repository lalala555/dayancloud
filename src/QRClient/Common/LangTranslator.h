/***********************************************************************
* Module:  LangTranslator.h
* Author:  hqr
* Modified: 2016/11/14 15:08:16
* Purpose: Declaration of the class
***********************************************************************/
#pragma once
#ifndef LANGTRANSLATOR_H_
#define LANGTRANSLATOR_H_

#include <QTranslator>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextStream>
#include "HttpCommand/HttpRestApi.h"

class LangTranslator
{
public:
    static void translateLanguage(int index)
    {
        if(translator) {
            qApp->removeTranslator(translator);
            delete translator;
            translator = Q_NULLPTR;
        }

        translator = new QTranslator();

        QString appPath = QApplication::applicationDirPath();

        switch(index) {
        case LANG_EN: {
//            translator->load(appPath + "/resource/qrenderbus_en.qm");
			translator->load(appPath + "/resource/dayancloud_en.qm");
            InitRequestErrorI18nFile(appPath + "/resource/error_en.json");
        }
        break;
        case LANG_CN: {
            translator->load(appPath + "/resource/qrenderbus_zh.qm");
            InitRequestErrorI18nFile(appPath + "/resource/error_zh.json");
        }
        break;
        break;
        default: {
//            translator->load(appPath + "/resource/qrenderbus_en.qm");
			translator->load(appPath + "/resource/dayancloud_en.qm");
            InitRequestErrorI18nFile(appPath + "/resource/error_en.json");
        }
        break;
        }

        qApp->installTranslator(translator);

        LocalSetting::getInstance()->setLanguageId(index);
    }

    static QString RequstErrorI18n(const QString& key)
    {
        QString lang(key);
        if(!g_translatorJson.isEmpty()) {
            lang = g_translatorJson[key].toString();
        }
        return lang.isEmpty() ? key : lang;
    }

    static bool InitRequestErrorI18nFile(const QString& filePath)
    {
        QFile json_file(filePath);

        if(!json_file.open(QIODevice::ReadOnly)) {
            qDebug() << "File open failed :" << filePath;
            return false;
        }
        QTextStream in_stream(&json_file);
        in_stream.setCodec("UTF-8");
        QString content = in_stream.readAll();

        QJsonParseError error;
        QJsonDocument jsonDocument(QJsonDocument::fromJson(content.toUtf8(), &error));
        if(jsonDocument.isNull() || jsonDocument.isEmpty() || error.error != QJsonParseError::NoError) {
            qDebug() << "i18n file parse failed : " << filePath;
            return false;
        }
        json_file.close();

        g_translatorJson = jsonDocument.object();
        if(g_translatorJson.isEmpty())
            return false;

        return true;
    }

    static void destory()
    {
        delete translator, translator = 0;
    }
    static QTranslator*  translator;
    static QJsonObject   g_translatorJson;
};

#endif // LANGTRANSLATOR_H_