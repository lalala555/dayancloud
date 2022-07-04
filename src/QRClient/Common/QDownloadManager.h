/***********************************************************************
* Module:  QDownloadManager.h
* Author:  hqr
* Created: 2018/07/17 11:51:14
* Modifier: hqr
* Modified: 2018/07/17 11:51:14
* Purpose: Declaration of the class
***********************************************************************/
#pragma once
#ifndef QDOWNLOADMANAGER_H
#define QDOWNLOADMANAGER_H

#include <QtNetwork>

class QDownloadManager: public QObject
{
    Q_OBJECT
    QNetworkAccessManager m_manager;

public:
    QDownloadManager(QObject* parent = 0);

    void downloadAutoUpdater(const QString& updater); // ���ظ��³���
    void downloadCoordSystemFile(const QString& filename); // ���ؿռ�����ϵͳ�ļ�

private:
    void doDownload(const QUrl &url, const QString &saveFilePath);
    bool saveToDisk(const QString &filename, QIODevice *data);

signals:
    void resultReady(int, const QString& data = "");

public slots:
    void downloadFinished(QNetworkReply *reply);
};

#endif // QDOWNLOADMANAGER_H
