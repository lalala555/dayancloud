#ifndef PROMOTENEWSWND_H
#define PROMOTENEWSWND_H

#include "Notification.h"
#include "ui_PromoteNewsWnd.h"
#include "kernel/msg.h"

class PromoteNewsWnd : public Notification
{
    Q_OBJECT

public:
    PromoteNewsWnd(QWidget *parent = 0);
    ~PromoteNewsWnd();
    void showPromoteNews(const NewsContent& newsInfo);
    QString getContentMd5();

public slots:
    void labelLinkClicked(const QString &);

protected:
    virtual void closeEvent(QCloseEvent * event);
    virtual void changeEvent(QEvent *event);

private:
    Ui::PromoteNewsWnd ui;
    QDesktopWidget m_desktop;
    NewsContent m_content;
};

#endif // PROMOTENEWSWND_H
