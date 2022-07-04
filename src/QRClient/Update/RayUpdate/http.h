#pragma once

/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslError>
#include <QStringList>
#include <QTimer>
#include <QUrl>
#include <QUrlQuery>
#include <QTemporaryDir>

#include <stdio.h>

QT_BEGIN_NAMESPACE
class QSslError;
QT_END_NAMESPACE

QT_USE_NAMESPACE

#include <QObject>
#include <QTimer>
#include <QNetworkReply>

#define HTTP_REQUEST_TIMEOUT 1000 * 60

class UpdateRequestTimer : public QObject {
    Q_OBJECT

public:
    UpdateRequestTimer(QObject* parent = 0);
    ~UpdateRequestTimer();

    void setTimeout(QNetworkReply* reply, int delayTime = HTTP_REQUEST_TIMEOUT);
signals:
    void requestTimeout(QNetworkReply*);
    private slots:
    void onRequestTimeout();
private:
    QNetworkReply* m_reply;
    QScopedPointer<QTimer> m_timer;
};

class Http : public QObject {
    Q_OBJECT
    QNetworkAccessManager http_;
    QList<QNetworkReply*> currentDownloads;

public:
    Http();
    ~Http();
    void get(const QUrl& url);
    void doDownload(const QUrl& url);
    QString saveFileName(const QUrl& url);
    bool saveToDisk(const QString& filename, QIODevice* data);
    void stop();
private:
    void DownloadFinished(QNetworkReply* reply);
    void GetCheckFinished(QNetworkReply* reply);

public slots:
    //void execute();
    void NetworkFinished(QNetworkReply* reply);
    void sslErrors(const QList<QSslError>& errors);
    void updateDataReadProgress(qint64, qint64);
    void onReplyTimeout(QNetworkReply* reply);
signals:
    void downloadDone(const int code, const QUrlQuery query);
    void getCheckDone(const int code, const QString filename);
    void speedReport(QString data);
public:
    QString download_path_;
private:
    double download_speed_;
    QTime time_;
    volatile bool is_stoped_;
    UpdateRequestTimer reply_time_;
};





//int main(int argc, char **argv) {
//    QCoreApplication app(argc, argv);
//
//    DownloadManager manager;
//    QTimer::singleShot(0, &manager, SLOT(execute()));
//
//    app.exec();
//}

