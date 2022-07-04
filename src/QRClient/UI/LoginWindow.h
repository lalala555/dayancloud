/***********************************************************************
* Module:  LoginWindow.h
* Author:  hqr
* Modified: 2016/11/14 15:00:39
* Purpose: Declaration of the class
***********************************************************************/
#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include "Window.h"
#include "HttpCommand/HttpCommon.h"
#include "Common/QHttpManager.h"
#include "NoticePublish/NoticePublishMgr.h"

namespace Ui
{
class LoginWindowClass;
}

class QHttpManager;
class NoticePublishMgr;
class UserLoginResponse;
class QProgressIndicator;

class LoginWindow : public Window
{
    Q_OBJECT

public:
    LoginWindow(QWidget *parent = 0);
    ~LoginWindow();

    virtual void closeEvent(QCloseEvent *event);
    virtual void showEvent(QShowEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
private:
    void initAccount();
    bool initServers();
    void initLinks();
    void checkAppUpdate();
    void tryAutoLogin();
    void doLogin();
    void showErrorMsgBox(const QString& message);
    void enableLoginItem();
    void disableLoginItem();
    void parseLoginData(QSharedPointer<UserLoginResponse> login);
    void parseSSOLoginData(QSharedPointer<SSOResponse> login);
    void initRefreshBtn();
private slots:
    void initAdBackground(const QString& imagePath);
    bool initPlatform();
    void onServerChanged(int index);
    void onUpdateCheckReturn(int updateCode);
    void onAuthSuccess(const QString& accessId);
    void onChangeLayout();
    // 推送消息相关
    int  showNotifyMsg(const NoticeContent&);
    void recvDownLoadServerList(const QString& content);

    // 获取客户端通知
    void onRecvNotifyMsg(const QList<NoticeContent>);
    void onRecvResponse(QSharedPointer<ResponseHead> response);

	void onStateChanged(int state);

    void on_btnLogin_clicked();
    //提示按钮
	void on_btn_tip1_clicked();
private:
    Ui::LoginWindowClass* ui;
    QProgressIndicator* m_busyIcon;
    QProgressIndicator* m_serverBusyIcon;
    QHttpManager m_httpManager;
    NoticePublishMgr m_noticeMgr;

    QString m_strUser;
    QString m_strPass;
    QString m_userKey;
    QString m_registerLink;
    QString m_resetPwdLink;
    int  m_loginChannelId;
    bool m_isServerInitSuccess;
    qint32 m_defaultPlatformId;
    QString m_defaultPlatformHost;
    QString m_defaultSiteName;
    QPushButton* m_refreshBtn;
    QTimer* m_snsTimer;
};

#endif // LOGINWINDOW_H
