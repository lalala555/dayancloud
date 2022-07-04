#include "stdafx.h"
#include "ThreadJob/SmallPicMgr.h"
#include "ThreadJob/QDownloadImage.h"
#include "ThreadJob/QDownloadImageMgr.h"
#include "Common/FileUtil.h"

CSmallpicMgr::CSmallpicMgr(void)
{
    m_thread.start();
    m_downMgr = new QDownloadImageMgr();
    m_downMgr->moveToThread(&m_thread);
    // connect(m_downMgr, SIGNAL(finished(qint64, int, const QString&)), this, SLOT(onDownloadFinished(qint64, int, const QString&)));

    m_msgQueue.init(this);
    m_msgQueue.start();
}

CSmallpicMgr::~CSmallpicMgr(void)
{
    m_msgQueue.stop();
}

int CSmallpicMgr::AddUrlPath(qint64 frameId, const QString& strFiles, bool bUrl, const QString& framename)
{
    int nIndex = LookupUrl(frameId, strFiles);

    if(nIndex == -1) {
        // 没有查找到.添加一个新的URL下载地址
        nIndex = InstallUrlPath(frameId, strFiles);
    }

    if(!this->LookupPic(frameId, nIndex).isEmpty()) {
        CGlobalHelp::Response(UM_SMALLPIC_RET, (intptr_t)frameId, (intptr_t)nIndex);
        return nIndex;
    }

    if(bUrl) {
        QString strFileName = File::getFileBaseName(strFiles);

        QString tempPath = RBHelper::makePathFromTempPath("Small");
        QString localPath = QString("%1/%2/%3")
                            .arg(tempPath)
                            .arg(frameId)
                            .arg(strFileName);

        QString dir = QString("/Small/%1").arg(frameId);

        QString smallPicDir = RBHelper::makePathFromTempPath(dir);
        RBHelper::makeDirectory(smallPicDir);

        // 开始下载
        m_downMgr->addDownloadTask(frameId, nIndex, strFiles, localPath);
    } else {
        onDownloadFinished(frameId, nIndex, strFiles);
    }
    return nIndex;
}

int CSmallpicMgr::LookupUrl(qint64 frameId, const QString& strUrl)
{
    QMutexLocker locker(&m_picFileLocker);
    auto urlList = m_picUrlList;

    auto it = urlList.find(frameId);
    if(it == urlList.end())
        return -1;

    QList<QString> url = it.value();
    for(int i = 0; i < url.size(); i++) {
        if(strUrl == url[i]) {
            return i;
        }
    }

    return -1;
}

QString CSmallpicMgr::LookupPic(qint64 frameId, int nIndex)
{
    QMutexLocker locker(&m_picFileLocker);
    auto picList = m_curTaskPicList;

    auto it = picList.find(frameId);
    if(it == picList.end() || nIndex < 0)
        return "";
    QList<QString> picPath = it.value();

    if(nIndex > (picPath.size() - 1)) {
        return "";
    }

    return picPath.at(nIndex);
}

void CSmallpicMgr::SetPicfiles(qint64 frameId, int nIndex, const QString& strPicFiles)
{
    QMutexLocker locker(&m_picFileLocker);
    auto it = m_curTaskPicList.find(frameId);
    if(it != m_curTaskPicList.end()) {
        if(m_curTaskPicList[frameId].indexOf(strPicFiles) == -1) // 去重
            m_curTaskPicList[frameId].append(strPicFiles);

    } else {
        m_curTaskPicList.insert(frameId, QList<QString>() << strPicFiles);
    }
}

int CSmallpicMgr::InstallUrlPath(qint64 frameId, const QString& strUrl)
{
    QMutexLocker locker(&m_picFileLocker);
    auto it = m_picUrlList.find(frameId);
    if(it != m_picUrlList.end()) {
        if(m_picUrlList[frameId].indexOf(strUrl) == -1) // 去重
            m_picUrlList[frameId].append(strUrl);

    } else {
        m_picUrlList.insert(frameId, QList<QString>() << strUrl);
    }

    return (m_picUrlList[frameId].size() - 1);
}

void CSmallpicMgr::onDownloadFinished(qint64 frameId, int picId, const QString& strFile)
{
    SetPicfiles(frameId, picId, strFile);
    // 通知主窗口此ID下载好了.
    CGlobalHelp::Response(UM_SMALLPIC_RET, (intptr_t)frameId, (intptr_t)picId);
}

QList<QString> CSmallpicMgr::LookPicFilePath(qint64 frameId)
{
    QMutexLocker locker(&m_picFileLocker);
    auto picList = m_curTaskPicList;
    QList<QString> list;
    if(picList.find(frameId) != picList.end()) {
        list = picList.value(frameId);
    }
    return list;
}

void CSmallpicMgr::clearAllContainer(qint64 frameId)
{
    QMutexLocker locker(&m_picFileLocker);
    auto url = m_picUrlList.find(frameId);
    if(url != m_picUrlList.end()) {
        m_picUrlList[frameId].clear();
    }

    auto file = m_curTaskPicList.find(frameId);
    if(url != m_curTaskPicList.end()) {
        m_curTaskPicList[frameId].clear();
    }
}

void CSmallpicMgr::addSmallPicDownloadMsg(stSmallPicInfo* callBack)
{
    m_msgQueue.post(callBack);
}

void CSmallpicMgr::on_queue_data(stSmallPicInfo* pData)
{
    if (pData == nullptr)
        return;

    stSmallPicInfo* picMsg = pData;
    this->onDownloadFinished(picMsg->frameId, picMsg->index, picMsg->strLocal);
    delete picMsg;
    picMsg = NULL;
}

QList<QString> CSmallpicMgr::LookPicUrls(qint64 frameId)
{
    QMutexLocker locker(&m_picFileLocker);
    auto picList = m_picUrlList;
    QList<QString> list;
    if (picList.find(frameId) != picList.end()) {
        list = picList.value(frameId);
    }
    return list;
}

QString CSmallpicMgr::LookupUrlPath(qint64 frameId, int nIndex)
{
    QList<QString> list = LookPicUrls(frameId);
    if (list.isEmpty()) return "";

    QString url = list.at(0);
    if(nIndex < list.size() - 1)
        url = list.at(nIndex);

    return url;
}