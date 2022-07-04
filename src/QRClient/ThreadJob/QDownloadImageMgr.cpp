#include "stdafx.h"
#include "QDownloadImageMgr.h"
#include "QDownloadImage.h"
#include "SmallPicMgr.h"
#include <QDateTime>

QDownloadImageMgr::QDownloadImageMgr(QObject *parent)
    : QObject(parent)
{
    m_threadpool.setMaxThreadCount(15);
}

QDownloadImageMgr::~QDownloadImageMgr()
{
}

void QDownloadImageMgr::addDownloadTask(qint64 frameId, int index, const QString& strUrl, const QString& strLocal)
{
    QDownloadImage *pack = new QDownloadImage(frameId, index, strUrl, strLocal);
    m_threadpool.start(pack);
    // connect(pack, SIGNAL(onFinished(qint64, int, const QString&)), this , SIGNAL(finished(qint64, int, const QString&)));
    connect(pack, SIGNAL(onFinished(qint64, int, const QString&)), this, SLOT(onFinished(qint64, int, const QString&)));
}

void QDownloadImageMgr::onFinished(qint64 frameId, int index, const QString& strLocal)
{
    // qDebug() << strLocal;
    // emit finished(frameId, index, strLocal);
    stSmallPicInfo* info = new stSmallPicInfo;
    info->frameId = frameId;
    info->index = index;
    info->strLocal = strLocal;

    CSmallpicMgr::Inst()->addSmallPicDownloadMsg(info);
}