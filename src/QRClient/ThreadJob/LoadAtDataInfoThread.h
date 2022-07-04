#pragma once

#include "HttpCommand/HttpCommon.h"

#include <QThread>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class QRequestTimer;

class LoadAtDataInfoThread : public QThread
{
    Q_OBJECT

public:
    LoadAtDataInfoThread(qint64 taskId, QObject *parent = 0);
    ~LoadAtDataInfoThread();
	at_view_info::at_view_data* getAtViewInfo();
    void loadAtViewDataUrl();
	void setImagePosJsonMd5(const QString &_imagePosJson_md5);
    const QString getHash(QString _filePath);
protected:
    virtual void run();

private:
    bool downloadAtViewInfo(const QString & url, const QString & _filePath);
    bool parseAtViewDataInfo(const QString& path);
    void setAtViewFileSavePath();

	

signals:
    void loadAtPointCloudInfoFinished(int);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private slots:
    void onRecvResponse(QSharedPointer<ResponseHead> response);
    bool onDownloadAtDataFinished(QNetworkReply * reply);
    bool handleRedirect(QNetworkReply *reply);

private:
    QString m_filePath;
	QString m_url;
    qint64 m_taskId;
	QString m_imagePosMd5;

    int m_retryTimes;
    QScopedPointer<QNetworkAccessManager> m_netMgr;
    QScopedPointer<QRequestTimer> m_netRequestTimer;
	at_view_info::at_view_data* m_at_view_data;



	QString m_filePath_at_data;
	QString m_filePath_image_pos;
	QString m_url_at_data;
	QString m_url_image_pos;
};
