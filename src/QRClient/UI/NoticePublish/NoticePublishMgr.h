#ifndef NOTICEPUBLISHMGR_H
#define NOTICEPUBLISHMGR_H

#include <QObject>
#include <QTimer>
#include "Common/QHttpManager.h"
#include "HttpCommand/HttpCommon.h"

enum MsgErrCode {
    Legal_Data = 0, // 合法数据
    Illegal_Data,   // 数据不合法
};

class NoticePublishMgr : public QObject
{
    Q_OBJECT

public:
    NoticePublishMgr(QObject *parent = 0);
    ~NoticePublishMgr();
    void startGetPublishMsg(qint32 delay = 0);
    void startGetProNotice(qint32 delay = 0);
    void stopGetPublishMsg();
    void setCurLoginState(qint32 state)
    {
        m_curLoginState = state;
    }
    void setCurLangId(int langId);
    QList<NoticeContent> checkContentIsChanged(const QList<NoticeContent> contents);// 检查收到的内容是否发生变化

signals:
    void notifyObserver(const QList<NoticeContent>);

private slots:
    void downNoticeContent();
    QList<NoticeContent> parseRollingContents(QSharedPointer<ResponseHead> response);
    void onRecvResponse(QSharedPointer<ResponseHead> response);
    void notifySysNoticeContent(QSharedPointer<ResponseHead> response);
    void notifyProNoticeContent(QSharedPointer<ResponseHead> response);
    void getProNoticeContent();
private:
    QHttpManager m_httpManager;
    QTimer*  m_noticeTimer;
    qint32   m_curLoginState;
    QTimer*  m_newsTimer;
    QList<NoticeContent>  m_oldContent;
};

#endif // NOTICEPUBLISHMGR_H
