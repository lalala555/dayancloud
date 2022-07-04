#pragma once

#include "HttpCommand/HttpCommon.h"

#include <QThread>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class QRequestTimer;

class LoadAtReportInfoThread : public QThread
{
    Q_OBJECT

public:
    LoadAtReportInfoThread(qint64 taskId, QObject *parent = 0);
    ~LoadAtReportInfoThread();
    at_report_data* getAtReportInfo();
    void loadAtReportDataUrl();

protected:
    virtual void run();

private:
    void downloadReportData(const QString& url);
    bool parseReportDataInfo(const QString& path);
    QString getReportFileSavePath();

signals:
    void loadAtReportInfoFinished(int);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private slots:
    void onRecvResponse(QSharedPointer<ResponseHead> response);
    void onDownloadReportDataFinished(QNetworkReply * reply);
    bool handleRedirect(QNetworkReply *reply);

private:
    QString m_filePath;
    qint64 m_taskId;
    int m_retryTimes;
    QScopedPointer<QNetworkAccessManager> m_netMgr;
    QScopedPointer<QRequestTimer> m_netRequestTimer;
    at_report_data* m_at_report_data;
    QString m_url;
};
