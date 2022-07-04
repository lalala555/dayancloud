/***********************************************************************
* Module:  ScriptDownload.h
* Author:  hqr
* Created: 2018/10/29 15:44:57
* Modifier: hqr
* Modified: 2018/10/29 15:44:57
* Purpose: Declaration of the class
***********************************************************************/
#pragma once
#ifndef SCRIPTDOWNLOADER_H
#define SCRIPTDOWNLOADER_H

#include <QtNetwork>

class ScriptDownloader : public QObject
{
    Q_OBJECT
    QNetworkAccessManager m_manager;

public:
    ScriptDownloader(QObject* parent = 0);
    void downloadScript(const QStringList& scripts);
    void downloadScript(const QString& script);
    void downloadPlugins();

private:
    void doDownload(const QUrl &url, const QString &saveFilePath);
    bool saveToDisk(const QString &filename, QIODevice *data);
    bool unzipToDisk(const QString &filename);

signals:
    void resultReady(int, const QString& data = "");

public slots:
    void downloadFinished(QNetworkReply *reply);
};

#endif // SCRIPTDOWNLOADER_H
