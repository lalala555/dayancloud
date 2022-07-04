#include "stdafx.h"
#include "NoticePublishWnd.h"
#include "NoticePublishMgr.h"

NoticePublishWnd::NoticePublishWnd(QWidget *parent)
    : Dialog(parent)
{
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowStaysOnTopHint);

    ui.setupUi(this);
    ui.label_logo->setPixmap(QPixmap(LOGO_ICON));

    this->resize(400, 280);
    connect(ui.btnOk, SIGNAL(clicked()), this, SLOT(btnOkClicked()));
    connect(ui.btnCancel, SIGNAL(clicked()), this, SLOT(btnCancelClicked()));
    connect(ui.msgLabel, SIGNAL(linkActivated(const QString &)), this, SLOT(openLabelLink(const QString&)));

    // 设置UI
    ui.btnCancel->setVisible(false);

    QToolHelper::centerScreen(this);
    QToolHelper::bringToFront(this);
}

NoticePublishWnd::~NoticePublishWnd()
{

}

void NoticePublishWnd::initUi()
{
    int langid = LocalSetting::getInstance()->getLanguageId();
    m_noticeType = m_content.notifyMode;
    QString msg = m_msg[langid];
    // 设置文字
    ui.msgLabel->setText(msg);
    ui.msgLabel->setWordWrap(true);
    QString title = tr("公告");

    // 根据类型判断窗口的位置及按钮定制
    if(m_noticeType == Confirm) {
        ui.msgTypeLab->setText(title);
        // 如果是阻止登录，则只显示确认
        if(!m_content.isBlockLogin) {
            ui.btnCancel->setVisible(true);
        }
    } else if(m_noticeType == Warning) {
        ui.msgTypeLab->setText(title);
    } else if(m_noticeType == Publish) {
        ui.msgTypeLab->setText(title);
    }
}

int NoticePublishWnd::execNotice()
{
    initUi();
    return this->exec();
}

void NoticePublishWnd::changeEvent(QEvent *event)
{
    switch(event->type()) {
    case QEvent::LanguageChange: {
        initUi();
        ui.retranslateUi(this);
    }
    break;
    default:
        Dialog::changeEvent(event);
        break;
    }
}

void NoticePublishWnd::openLabelLink(const QString& url)
{
    QDesktopServices::openUrl(QUrl(url));
}

void NoticePublishWnd::btnOkClicked()
{
    done(QDialog::Accepted);
}

void NoticePublishWnd::btnCancelClicked()
{
    done(QDialog::Rejected);
}

void NoticePublishWnd::setNoticeContent(const NoticeContent& content)
{
    m_content = content;

    // 分离消息内容
    m_msg[LANG_EN] = content.notifyMessageEn;
    m_msg[LANG_CN] = content.notifyMessageZh;
    m_msg[LANG_JP] = content.notifyMessageJp;
}