#pragma once
#include "Common/MainFrame.h"
#include "Common/Pool/Pool.h"
#include "Common/SyncObject.h"
#include <QThread>
#include <QHash>
#include <QNetworkAccessManager>
#include <QMutex>

class QDownloadImage;
class QDownloadImageMgr;
class QMutex;
class QMutexLocker;

class CSmallpicMgr : public IQueueSink<stSmallPicInfo>, public TSingleton<CSmallpicMgr>
{
public:
    CSmallpicMgr();
    virtual ~CSmallpicMgr();

    QString LookupPic(qint64 frameId, int nIndex);
    int LookupUrl(qint64 frameId, const QString& strUrl);
    void SetPicfiles(qint64 frameId, int nIndex, const QString& strPicFiles);
    int AddUrlPath(qint64 frameId, const QString& strFiles, bool bUrl = true, const QString& framename = "");
    QList<QString> LookPicFilePath(qint64 frameId);
    void clearAllContainer(qint64 frameId);
    void addSmallPicDownloadMsg(stSmallPicInfo* callBack);
    QList<QString> LookPicUrls(qint64 frameId);
    QString LookupUrlPath(qint64 frameId, int nIndex);
private slots:
    void onDownloadFinished(qint64, int, const QString&);

signals:
    void startDownload();

protected:
    int InstallUrlPath(qint64 frameId, const QString& strUrl);
    virtual void on_queue_data(stSmallPicInfo* pData);

public:
    std::vector<QString>                m_vtUrl;
    std::vector<QString>                m_vtPic;
    QThread                             m_thread;
    QDownloadImageMgr                   *m_downMgr;
    QHash<int, qint64>                  m_picIdInTask;

    QMutex                              m_picFileLocker;
    QMutex                              m_picUrlLocker;
    QHash<qint64, QList<QString>>       m_picUrlList;
    QHash<qint64, QList<QString>>       m_curTaskPicList;
    IQueue<stSmallPicInfo>              m_msgQueue;
};
