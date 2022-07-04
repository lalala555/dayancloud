/***********************************************************************
* Module:  QDownloadImage.h
* Author:  yanglong
* Created: 2017/10/09 15:26:22
* Modifier: yanglong
* Modified: 2017/10/09 15:26:22
* Purpose: Declaration of the class
***********************************************************************/
#ifndef QDOWNLOADIMAGE_H
#define QDOWNLOADIMAGE_H

#include <QObject>
#include <QRunnable>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>

class QDownloadImage : public QObject, public QRunnable
{
    Q_OBJECT

public:
    QDownloadImage(qint64 frameId, int index, const QString& strUrl, const QString& strLocal, bool onlyPixelData = false, QObject *parent = nullptr);
    ~QDownloadImage();
    virtual void run();

private slots:
    void downloadFinished(QNetworkReply *reply);
    bool handleRedirect(QNetworkReply *reply);

signals:
    void onFinished(qint64, int, const QString&);
    void onFinished(qint64, int, const QByteArray&);

private:
    QString   m_strLocal;
    QString   m_strUrl;
    int       m_index;
    qint64    m_frameId;
    int       m_retryTimes;
    bool      m_onlyPixelData;
    QNetworkAccessManager *m_netMgr;
};

#endif // QDOWNLOADIMAGE_H
