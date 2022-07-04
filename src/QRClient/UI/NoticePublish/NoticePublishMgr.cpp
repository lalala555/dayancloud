#include "stdafx.h"
#include "NoticePublishMgr.h"
#include "Common/QHttpManager.h"
#include "HttpCommand/HttpCmdManager.h"

NoticePublishMgr::NoticePublishMgr(QObject *parent)
    : QObject(parent)
    , m_noticeTimer(nullptr)
    , m_newsTimer(nullptr)
    , m_curLoginState(BeforeLogin)
{
    // 注册元类型
    qRegisterMetaType<NoticeContent>("NoticeContent");
    qRegisterMetaType<QList<NoticeContent>>("QList<NoticeContent>");
    qRegisterMetaType<NewsContent>("NewsContent");
    qRegisterMetaType<QString*>("QString*");

    m_oldContent.clear();
}

NoticePublishMgr::~NoticePublishMgr()
{
    delete m_newsTimer;
    delete m_noticeTimer;
}

void NoticePublishMgr::startGetPublishMsg(qint32 delay)
{
    if(delay == 0) {
        QTimer::singleShot(0, this, SLOT(downNoticeContent()));
    } else {
        if(m_noticeTimer == nullptr) {
            m_noticeTimer = new QTimer;
            connect(m_noticeTimer, SIGNAL(timeout()), this, SLOT(downNoticeContent()));
        }
        m_noticeTimer->start(delay);
    }
}

void NoticePublishMgr::stopGetPublishMsg()
{
    if(m_noticeTimer != NULL && m_noticeTimer->isActive()) {
        m_noticeTimer->stop();
        disconnect(m_noticeTimer, SIGNAL(timeout()), this, SLOT(downNoticeContent()));
    }
}

void NoticePublishMgr::downNoticeContent()
{
#ifdef FOXRENDERFARM
    HttpCmdManager::getInstance()->qureyClientSysNotice(CLIENT_FOXRENDER_DAYAN, LocalSetting::getInstance()->getLanguageId(), -1, this);
#else
    HttpCmdManager::getInstance()->qureyClientSysNotice(CLIENT_RENDERBUS_DAYAN, LocalSetting::getInstance()->getLanguageId(), -1, this);
#endif // FOXRENDERFARM
}

QList<NoticeContent> NoticePublishMgr::checkContentIsChanged(const QList<NoticeContent> contents)
{
    if(contents.isEmpty())
        return contents;

    QList<NoticeContent> list;
    foreach(NoticeContent newcontent, contents) {
        bool isExist = false;
        foreach(NoticeContent oldcontent, m_oldContent) {
            if(newcontent == oldcontent) {
                isExist = true;
            }
        }
        if(!isExist)
            list.append(newcontent);
    }
    if(list.isEmpty() && m_oldContent.isEmpty())
        list = contents;

    m_oldContent = contents;

    return list;
}

void NoticePublishMgr::setCurLangId(int langId)
{

}

QList<NoticeContent> NoticePublishMgr::parseRollingContents(QSharedPointer<ResponseHead> response)
{
    QList<NoticeContent> contents;
    QSharedPointer<ClientSysNoticeResponse> sysNotice = qSharedPointerCast<ClientSysNoticeResponse>(response);
    if(sysNotice->noticeContents.isEmpty())
        return contents;

    foreach(SysNoticeContent* content, sysNotice->noticeContents) {
        NoticeContent notice;

        notice.isBlockLogin = false;
        notice.notifyMessageEn = content->contentEn;
        notice.notifyMessageJp = content->contentJp;
        notice.notifyMessageZh = content->contentZh;
        notice.notifyMode      = m_curLoginState;
        notice.notifyType      = content->noticeType;

        contents.append(notice);
    }

    return contents;
}

void NoticePublishMgr::onRecvResponse(QSharedPointer<ResponseHead> response)
{
    if(response->cmdType == RESTAPI_USER_PRO_NOTICE) {
        this->notifyProNoticeContent(response);

    } else if(response->cmdType == RESTAPI_USER_SYS_NOTICE) {
        this->notifySysNoticeContent(response);
    }
}

void NoticePublishMgr::notifySysNoticeContent(QSharedPointer<ResponseHead> response)
{
    QList<NoticeContent> list = parseRollingContents(response);
    emit notifyObserver(list);
}

void NoticePublishMgr::notifyProNoticeContent(QSharedPointer<ResponseHead> response)
{
    QSharedPointer<ClientProNoticeResponse> proNotice = qSharedPointerCast<ClientProNoticeResponse>(response);

    if(proNotice->noticeContents.isEmpty())
        return;

    int curLang = LocalSetting::getInstance()->getLanguageId();
    foreach(ProNoticeContent* content, proNotice->noticeContents) {
        foreach(ProContent notic, content->contents) {
            if(curLang == notic.noticeLanFlag) {
                NewsContent *news = new NewsContent;
                news->newsid = notic.noticeId;
                news->title  = notic.noticeTitle;
                news->content = notic.noticeText;
                news->hyperlink = content->noticeUrl;
                CGlobalHelp::Response(UM_NEWS_CONTENT_RET, 0, (intptr_t)news);
            }
        }
    }
}

void NoticePublishMgr::startGetProNotice(qint32 delay)
{
    if(delay == 0) {
        QTimer::singleShot(0, this, SLOT(getProNoticeContent()));
    } else {
        if(m_newsTimer == nullptr) {
            m_newsTimer = new QTimer;
            connect(m_newsTimer, SIGNAL(timeout()), this, SLOT(getProNoticeContent()));
        }
        m_newsTimer->start(delay);
    }
}

void NoticePublishMgr::getProNoticeContent()
{
#ifdef FOXRENDERFARM
    HttpCmdManager::getInstance()->qureyClientProNotice(CLIENT_FOXRENDER_DAYAN, LocalSetting::getInstance()->getLanguageId(), this);
#else
    HttpCmdManager::getInstance()->qureyClientProNotice(CLIENT_RENDERBUS_DAYAN, LocalSetting::getInstance()->getLanguageId(), this);
#endif
}
