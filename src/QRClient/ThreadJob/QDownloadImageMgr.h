/***********************************************************************
* Module:  QDownloadImageMgr.h
* Author:  yanglong
* Created: 2017/10/09 15:26:53
* Modifier: yanglong
* Modified: 2017/10/09 15:26:53
* Purpose: Declaration of the class
***********************************************************************/
#ifndef QDOWNLOADIMAGEMGR_H
#define QDOWNLOADIMAGEMGR_H
#include <QThreadPool>

class QDownloadImage;

class QDownloadImageMgr : public QObject
{
    Q_OBJECT

public:
    QDownloadImageMgr(QObject *parent = 0);
    ~QDownloadImageMgr();
    void addDownloadTask(qint64 frameId, int index, const QString& strUrl, const QString& strLocal);

signals:
    void finished(qint64, int, const QString&);

private slots:
    void onFinished(qint64, int, const QString&);

private:
    QThreadPool m_threadpool;
};

#endif // DOWNLOADIMAGEMGR_H
