#ifndef NOTICEPUBLISHWND_H
#define NOTICEPUBLISHWND_H

#include "Dialog.h"
#include "kernel/msg.h"
#include "ui_NoticePublishWnd.h"

class NoticePublishWnd : public Dialog
{
    Q_OBJECT

public:
    NoticePublishWnd(QWidget *parent = 0);
    ~NoticePublishWnd();

    int execNotice();
    void setNoticeContent(const NoticeContent& content);

signals:
    void sendDialogRet(int);

private:
    void initUi();

private slots:
    void openLabelLink(const QString&);
    void btnOkClicked();
    void btnCancelClicked();

protected:
    virtual void changeEvent(QEvent *event);

private:
    Ui::NoticePublishWnd ui;
    qint32 m_noticeType;
    NoticeContent m_content;
    QHash<int, QString> m_msg;
};

#endif // NOTICEPUBLISHWND_H
