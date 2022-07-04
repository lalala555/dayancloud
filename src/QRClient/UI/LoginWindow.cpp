#include "stdafx.h"
#include "LoginWindow.h"
#include "ui_LoginWindow.h"
#include "UI/MessageBox.h"
#include "UI/PwdMismatchWindow.h"
#include "UI/Base/Controls/AccountLineEdit.h"
#include "UI/Base/Controls/PlatformItem.h"
#include "Common/LangTranslator.h"
#include "Update/UpdateManager.h"
#include "Update/UpdateForm.h"
#include "NoticePublish/NoticePublishWnd.h"
#include "HttpCommand/HttpCmdManager.h"

LoginWindow::LoginWindow(QWidget *parent)
    : Window(parent)
    , ui(new Ui::LoginWindowClass)
    , m_loginChannelId(ID_LOGIN_RENDERBUS)
    , m_isServerInitSuccess(false)
    , m_snsTimer(new QTimer(this))
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setWindowTitle(WINDOW_TITLE);
#ifdef FOXRENDERFARM
	ui->btnRegister->setFixedWidth(120);
	ui->btnPassword->setFixedWidth(120);
	ui->checkChangePlatform->hide();
	ui->btn_tip1->hide();

	QStringList list;
	list.append("color:white");                         // 前景色
	list.append("background-color:rgb(85,170,255)");    // 背景色
	list.append("border-style:outset");                 // 边框风格
	list.append("border-width:0px");                    // 边框宽度
	list.append("border-color:rgb(10,45,110)");         // 边框颜色
	list.append("border-radius:4px");                   // 边框倒角
	list.append("padding:4px");                         // 内边距
	list.append("QPushButton:hover");

	ui->btnThirdLogin->setStyleSheet(list.join(';'));
	ui->btnThirdLogin->setIcon(QIcon(":/images/google.png"));
	ui->btnThirdLogin->setIconSize(QSize(24, 24));

	// 按钮按下时修改样式
	list.replace(1, "background-color:rgb(66,134,243)");
	connect(ui->btnThirdLogin, &QPushButton::pressed, [=]() {
		ui->btnThirdLogin->setStyleSheet(list.join(';'));
	});
	
	// 按钮弹起时恢复样式
	list.replace(1, "background-color:rgb(85,170,255)");
	connect(ui->btnThirdLogin, &QPushButton::released, [=]() {
		ui->btnThirdLogin->setStyleSheet(list.join(';'));
	});

	ui->btnThirdLogin->hide();

#else
	ui->labelLogo->setPixmap(QPixmap(LOGO_LARGE_ICON));
	ui->btnThirdLogin->hide();
#endif
    
    ui->editPassword->supportInputMethod(false);
    ui->editUsername->supportInputMethod(false);

    m_busyIcon = new QProgressIndicator(ui->btnLogin);
    m_serverBusyIcon = new QProgressIndicator(ui->comboServer);
	ui->btn_tip1->setStyleSheet("border-image:url(:/view/images/view/icon.png)");
	//ui->btn_tip1->setStyleSheet("QPushButton{border:none;background-color:transparent;}""QPushButton:hover{ color: white; background-color:transparent;Qt::CursorShape}");
    this->initAdBackground(LOGO_LARGE_BG);
    this->initAccount();
    this->initPlatform();
    this->initRefreshBtn();

    ui->btnLogin->setFocus();
    ui->btnLogin->setDefault(true);
    ui->widget_server->hide();
    ui->labelTips->hide();

    connect(&m_httpManager, SIGNAL(responseServerList(const QString&)), this, SLOT(recvDownLoadServerList(const QString&)));

    // 推送消息相关
    connect(&m_noticeMgr, SIGNAL(notifyObserver(const QList<NoticeContent>)), this, SLOT(onRecvNotifyMsg(const QList<NoticeContent>)));
    m_noticeMgr.setCurLoginState(BeforeLogin);

    connect(ui->btnRegister, &QPushButton::clicked, [=]() {
        QDesktopServices::openUrl(QUrl(m_registerLink));
    });
    connect(ui->btnPassword, &QPushButton::clicked, [=]() {
        QDesktopServices::openUrl(QUrl(m_resetPwdLink));
    });
    connect(ui->checkRemember, &QCheckBox::stateChanged, [=](int state) {
        if(Qt::Unchecked == state) {
            ui->checkAutolog->setCheckState(Qt::Unchecked);
        }
        LocalSetting::getInstance()->setRememberPassword(state == Qt::Checked);
    });
    connect(ui->checkAutolog, &QCheckBox::stateChanged, [=](int state) {
        if(Qt::Checked == state) {
            ui->checkRemember->setCheckState(Qt::Checked);
        }
        LocalSetting::getInstance()->setAutoLogin(state == Qt::Checked);
    });

#ifdef FOXRENDERFARM
	ui->editUsername->setPlaceholderText("Enter your email or username");
	ui->label_user_prompt->setText("Email or Username");
	ui->label_pw_hint->setText("Password");
#endif
    QToolHelper::bringToFront(this);
    QToolHelper::centerScreen(this);

	connect(ui->checkChangePlatform, SIGNAL(stateChanged(int)), this, SLOT(onStateChanged(int)));
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::closeEvent(QCloseEvent *event)
{
    done(enDialogExit);
}

void LoginWindow::showEvent(QShowEvent *event)
{
    onChangeLayout();
}

void LoginWindow::resizeEvent(QResizeEvent * event)
{
    onChangeLayout();
}

void LoginWindow::initAdBackground(const QString& imagePath)
{
    ui->widgetLeft->setStyleSheet(QString("QWidget#widgetLeft {background-image:url(%1);}").arg(imagePath));
}

void LoginWindow::initAccount()
{
    ui->editPassword->setEye(QIcon(":/icon/images/icon/eye_off.svg"));

    QString userName = LocalSetting::getInstance()->getUserName();
    ui->editUsername->setText(userName);
    QString password = LocalSetting::getInstance()->getPassword();
    bool bCheckRemberPwd = LocalSetting::getInstance()->getRememberPassword();
    ui->editPassword->setText(bCheckRemberPwd ? password : "");

    Qt::CheckState checkRememberState = bCheckRemberPwd && !password.isEmpty() ? Qt::Checked : Qt::Unchecked;
    ui->checkRemember->setCheckState(checkRememberState);
    Qt::CheckState checkAutologState = LocalSetting::getInstance()->getAutoLogin() ? Qt::Checked : Qt::Unchecked;
    ui->checkAutolog->setCheckState(checkAutologState);

    // 限制中文输入
    QRegExp reg("^[0-9a-zA-Z_@.-]{1,}$");
    QRegExpValidator *regVal = new QRegExpValidator(reg);
    ui->editUsername->setValidator(regVal);
    ui->editUsername->setDropdownIcon();

    // 添加下拉用户列表
    QList<UserAccount> userAccounts;
    auto accounts = CProfile::Inst()->get_accounts();
    QMapIterator<QString, AccountItem> it(accounts);
    while(it.hasNext()) {
        AccountItem account = it.next().value();
        if(account.name.empty()) continue;
        userAccounts.append({ QString::fromStdString(account.name),
                            QString::fromStdString(account.password),
                            account.save_password_flag,
                            account.auto_login_flag });
    }
    ui->editUsername->addAccounts(userAccounts);

    connect(ui->editUsername, &AccountLineEdit::selected, [=](const UserAccount & account) {
        ui->editUsername->setText(account.name);
        ui->editPassword->setText(account.password);
        ui->checkRemember->setChecked(account.remember);
        ui->checkAutolog->setChecked(account.autologin);
    });
    connect(ui->editUsername, &AccountLineEdit::deleted, [=](const UserAccount & account) {
        auto name = ui->editUsername->text();
        if(account.name == name) {
            ui->editUsername->clear();
            ui->editPassword->clear();
            ui->checkRemember->setChecked(false);
            ui->checkAutolog->setChecked(false);
        }

        auto saved_name = LocalSetting::getInstance()->getUserName();
        if(account.name == saved_name) {
            LocalSetting::getInstance()->setUserName(QString());
            LocalSetting::getInstance()->setPassword(QString());
            LocalSetting::getInstance()->setRememberPassword(false);
            LocalSetting::getInstance()->setAutoLogin(false);
        }

        CProfile::Inst()->remove_account(account.name);
    });
}

bool LoginWindow::initPlatform()
{
    if (m_isServerInitSuccess)
        return false;

    m_serverBusyIcon->startAnimation();
    m_httpManager.downloadServerListAsync(DESKTOP_CLIENT_LANG);

    return true;
}

bool LoginWindow::initServers()
{
    if (m_isServerInitSuccess)
        return false;

    site_info& site = WSConfig::getInstance()->site;
    if(site.platforms.size() <= 0) return false;

    QSignalBlocker blocker(ui->comboServer);
    ui->comboServer->clear();
    // QComboBox Server Init
    QListView *view = new QListView(this);
    QStandardItemModel *model = new QStandardItemModel(this);

    int selIdx = 0;
    int currPlatformId = LocalSetting::getInstance()->getDefaultsite();

    QMap<int, QStandardItem*> enableItems;
    int defaultIdx = -1;
    for(int i = 0; i < site.platforms.size(); i++) {
        auto platform = site.platforms[i];

        QString stateText = RBHelper::getPlatformStatusText(platform.system_status);
        QIcon stateIcon = RBHelper::getServerLoadIcon(platform.system_status);

        QString platform_name = platform.alias;
        QStandardItem *item = new QStandardItem(stateIcon, platform_name/* + "\t" + stateText*/);
        item->setEnabled(platform.isPlatformEnable());
        item->setToolTip(platform_name);
        item->setData(platform.id, Qt::UserRole);
        model->appendRow(item);

        if(platform.isPlatformEnable()) enableItems.insert(i, item);

        if(platform.is_default && platform.isPlatformEnable())
            selIdx = i;
        if(platform.id == currPlatformId && platform.isPlatformEnable())
            defaultIdx = i;
    }
    if (defaultIdx != -1) {
        selIdx = defaultIdx;
    }

    view->setIconSize(QSize(30, 30));
    ui->comboServer->setView(view);
    ui->comboServer->setModel(model);
    ui->comboServer->setCurrentIndex(selIdx);

    if(selIdx > -1){
        m_defaultSiteName = site.platforms[selIdx].alias;
        m_defaultPlatformHost = site.host;
        m_defaultPlatformId = site.platforms[selIdx].id;
    }

    return enableItems.size() > 0;  // 只有有可用平台，初始化才算成功
}

void LoginWindow::initLinks()
{
    QString host = HttpCmdManager::getInstance()->getRequestHost();
    QString product = WSConfig::getInstance()->get_link_address("path_from");

    m_registerLink = QString("%1/sso/register?&lang=%2&from=%3").arg(host).arg(RBHelper::getLocaleName()).arg(product);
    m_resetPwdLink = QString("%1/sso/forget-password?&lang=%2&from=%3").arg(host).arg(RBHelper::getLocaleName()).arg(product);
}

void LoginWindow::showErrorMsgBox(const QString& message)
{
    this->enableLoginItem();

    Util::ShowMessageBoxError(message);
}

void LoginWindow::enableLoginItem()
{
    m_snsTimer->stop();
    m_busyIcon->stopAnimation();
    ui->comboServer->setEnabled(true);
    ui->stackedWidget->setEnabled(true);
    ui->widgetSNS->setEnabled(true);
}

void LoginWindow::disableLoginItem()
{
    m_snsTimer->stop();
    m_busyIcon->startAnimation();
    ui->comboServer->setEnabled(false);
    ui->stackedWidget->setEnabled(false);
    ui->widgetSNS->setEnabled(false);
}

void LoginWindow::checkAppUpdate()
{
    QString updater = QString("%1/%2%3").arg(QCoreApplication::applicationDirPath()).arg("RayUpdate").arg(APP_SUFFIX);
    UpdateManagerInstance.init(updater, STRFILEVERSION);
    QObject::connect(&UpdateManagerInstance, SIGNAL(resultReady(int)), this, SLOT(onUpdateCheckReturn(int)));
    UpdateManagerInstance.checkUpdate(UPDATE_MANUAL_TRIGGER);
}

void LoginWindow::onUpdateCheckReturn(int updateCode)
{
    QObject::disconnect(&UpdateManagerInstance, SIGNAL(resultReady(int)), this, SLOT(onUpdateCheckReturn(int)));
    bool needUpdate = true;

    int updateRetcode = enDialogYes;
    switch(updateCode) {
    case server_is_unusable: {
        this->enableLoginItem();
        return;
    }
    break;
    case forced_to_update: {
        this->enableLoginItem();
        QPointer<UpdateForm> updateWnd(new UpdateForm());
        updateWnd->exec();
        return;
    }
    break;
    case need_to_update: {
        this->enableLoginItem();
        if (LocalSetting::getInstance()->getEnableAutoUpdate() == 1) {
            QPointer<UpdateForm> updateWnd(new UpdateForm());
            updateRetcode = updateWnd->exec();
        }
    }
    break;
    case update_process_is_exist:
    case need_not_to_update:
    default:
        break;
    }

    if(updateRetcode == enDialogYes) {
        this->doLogin();
    }
}

void LoginWindow::onAuthSuccess(const QString& accessId)
{
    HttpRequestHeader header = HttpCmdManager::getInstance()->getRequestHeader();
    header.userKey = accessId;
    HttpCmdManager::getInstance()->setRequestHeader(header);

    ui->btnLogin->clicked();
}

void LoginWindow::onChangeLayout()
{
    int w = (ui->btnLogin->width() - m_busyIcon->width()) / 2;
    int h = (ui->btnLogin->height() - m_busyIcon->height()) / 2;
    m_busyIcon->move(w, h);

    // 服务器列表进度条位置
    int sx = (ui->comboServer->width() - m_serverBusyIcon->width()) / 2;
    int sy = (ui->comboServer->height() - m_serverBusyIcon->height()) / 2;
    m_serverBusyIcon->move(sx, sy);

    // refreshBtn
    int bx = (ui->comboServer->width() - m_refreshBtn->width()) / 2;
    int by = (ui->comboServer->height() - m_refreshBtn->height()) / 2;
    m_refreshBtn->move(bx, by);
}

void LoginWindow::on_btnLogin_clicked()
{
    m_loginChannelId = ID_LOGIN_RENDERBUS;
    MyConfig.userSet.loginType = "normal";
    // 针对自动登录的情况，如果服务器列表没有加载完毕则不能进行登录
    if(!m_isServerInitSuccess)
        return;

    m_strUser = ui->editUsername->text().trimmed();
    m_strPass = ui->editPassword->text().trimmed(); 
    if(m_strUser.isEmpty() || m_strPass.isEmpty()) {
        this->showErrorMsgBox(QObject::tr("用户名/密码不能为空!"));
        return;
    }

    this->disableLoginItem();

    this->checkAppUpdate();
}

void LoginWindow::on_btn_tip1_clicked()
{
	Util::ShowMessageBoxInfo(tr("平台升级，新老平台作业数据不通用，查看老平台的作业请勾选此选项。如有疑问，请联系官网客服。"), tr("老平台登录提示"));
}

void LoginWindow::doLogin()
{

    // 取得当前选择服务器,并初始化
    int serverSite = ui->comboServer->currentIndex();
    if(serverSite == -1) return;
    LocalSetting::getInstance()->setLoginChoiceMode(m_loginChannelId);

    LOGFMTI("[Login] Start to login...");

    QString platform_name = ui->comboServer->currentText();
    int platform_id = ui->comboServer->currentData().toInt();

    MyConfig.client.platform = platform_id;
    MyConfig.client.platformName = platform_name.split("\t")[0];
    LocalSetting::getInstance()->setDefaultSite(platform_id);
/*
	if (ui->checkChangePlatform->checkState() == Qt::Checked)
	{
		m_isServerInitSuccess = false;
		m_httpManager.changePlatform(true);
		this->initPlatform();

		QString plat = WSConfig::getInstance()->get_host_url();
	}
	*/
	QString plat = WSConfig::getInstance()->get_host_url();

    HttpCmdManager::getInstance()->initRequestHeader();

    // 可接受客户端最低版本检查
    QString accept_version = WSConfig::getInstance()->get_accept_version();
    if(!accept_version.isEmpty()) {
        if(RBHelper::versionCompare(STRFILEVERSION, accept_version) < 0) {
            this->showErrorMsgBox(QObject::tr("软件版本过低，请升级客户端!"));
            return;
        }
    }

    // 开始登陆
    LOGFMTI("[Login] Start to send login request...");
    HttpCmdManager::getInstance()->ssoLogin(m_strUser, m_strPass, this);
}

void LoginWindow::onServerChanged(int index)
{
    QComboBox *comboServer = qobject_cast<QComboBox *>(sender());
    int idx = comboServer->currentIndex();
    if(idx == -1) return;

    site_info& site = WSConfig::getInstance()->site;
    int platform_id = site.platforms[idx].id;
    LocalSetting::getInstance()->setDefaultSite(platform_id);

    this->initLinks();
}

void LoginWindow::tryAutoLogin()
{
    bool autoLogin = LocalSetting::getInstance()->getAutoLogin();
    if(autoLogin) on_btnLogin_clicked();
}

int LoginWindow::showNotifyMsg(const NoticeContent& content)
{
    //显示消息
    QPointer<NoticePublishWnd> noticeWnd(new NoticePublishWnd(this));
    noticeWnd->setNoticeContent(content);
    return noticeWnd->execNotice();
}

void LoginWindow::recvDownLoadServerList(const QString& content)
{
    if(content.isEmpty() || !WSConfig::getInstance()->parse_site(content)) {
        new ToastWidget(this, QObject::tr("初始化服务器列表失败,请检查网络或联系客服!"), QColor("#e25c59"), 10000);
        m_serverBusyIcon->stopAnimation();
        m_refreshBtn->show();
        return;
    }

    QString hostUrl = WSConfig::getInstance()->get_host_url();
    CConfigDb::Inst()->setHost(hostUrl);

    if(this->initServers()) {
        m_serverBusyIcon->stopAnimation();
        m_isServerInitSuccess = true;

        this->initLinks();
        this->enableLoginItem();
        this->tryAutoLogin();
    } else {
        this->disableLoginItem();
        m_busyIcon->stopAnimation();
    }
}

void LoginWindow::onRecvNotifyMsg(const QList<NoticeContent> list)
{
    bool isCanLogin = true;
    // step 1 根据通知判断是否可以登录
    if(!list.isEmpty()) {
        foreach(NoticeContent content, list) {
            if(content.notifyMode == BeforeLogin && content.notifyType == Confirm) {
                this->showNotifyMsg(content);
                isCanLogin = false;
            }
        }
    }

    if(isCanLogin) {
        // 通知没问题 进入主界面
        done(enDialogLogin);
        return;
    }

    this->enableLoginItem();
}

void LoginWindow::onRecvResponse(QSharedPointer<ResponseHead> response)
{
    int errorCode = response->code;
    if(errorCode == HTTP_ERROR_UNKNOWN) {
        this->enableLoginItem();
        return;
    } else if(errorCode < SSO_ERROR_END) {
        if(errorCode == SSO_ERROR_ACCOUNT_THIRDPARTY_SIGNIN_FAILED) {
            // continue
            return;
        } else if(errorCode != SSO_ERROR_COMMON_SUCCESS) {
            this->enableLoginItem();
        }
    } else if(errorCode != HTTP_ERROR_SUCCESS) {
        this->enableLoginItem();
    }

    int type = response->cmdType;
    if(type == RESTAPI_USER_LOGIN || type == RESTAPI_THIRD_USER_LOGIN) {
        auto userlogin = qSharedPointerCast<UserLoginResponse>(response);
        parseLoginData(userlogin);
    } else if(type == SSOAPI_SIGNIN) {
        auto userlogin = qSharedPointerCast<SSOResponse>(response);
        parseSSOLoginData(userlogin);
    }
}

void LoginWindow::onStateChanged(int state)
{
	if (state == Qt::Checked)
	{
		m_isServerInitSuccess = false;
		m_httpManager.changePlatform(true);
		this->initPlatform();
	}
	else
	{
		m_isServerInitSuccess = false;
		m_httpManager.changePlatform(false);
		this->initPlatform();
	}
}

void LoginWindow::parseSSOLoginData(QSharedPointer<SSOResponse> login)
{
    switch(login->code) {
    case SSO_ERROR_COMMON_SUCCESS:
    {
        QString accessId = QString::fromStdString(CConfigDb::Inst()->getAccessId(m_strUser.toStdString()));
        HttpCmdManager::getInstance()->login(login->rsAuthToken, accessId, this);
        break;
    }

    case SSO_ERROR_ACCOUNT_SIGNIN_FAILED:
    {
        QScopedPointer<PwdMismatchWindow> pwdMisWnd(new PwdMismatchWindow);
        pwdMisWnd->exec();
        break;
    }

    case SSO_ERROR_ACCOUNT_UNACTIVATED:
        this->showErrorMsgBox(QObject::tr("账号未激活"));
        break;
    case SSO_ERROR_ACCOUNT_LOCKED:
        this->showErrorMsgBox(QObject::tr("账号已被锁定"));
        break;
    case SSO_ERROR_ACCOUNT_DISABLED:
        this->showErrorMsgBox(QObject::tr("账号已被停用"));
        break;
    case SSO_ERROR_ACCOUNT_SIGN_GRAPHICVERIFICATIONREQUIRED:
    case SSO_ERROR_ACCOUNT_VALIDATE_INTELLIGENTFAILED: {
        QString url = QObject::tr("<a href = '%1'><span style='color:#675ffe;text-decoration:none;'>网页端验证</span></a>")
            .arg(RBHelper::getSimpleLinkUrl("path_login").toString());
        QString msg = QObject::tr("<p>密码输错次数达到3次！</p> <p>请前往%1后重新登录</p>").arg(url);

        QPointer<CMessageBox> msgBox(new CMessageBox);
        msgBox->setMessage(msg, CMessageBox::MB_ERROR);
        connect(msgBox.data(), &CMessageBox::linkClicked, [&msgBox](const QString& v) {
            QDesktopServices::openUrl(QUrl(v));
            msgBox->accept();
        });
        msgBox->exec();
    }
        break;

    default:
        this->showErrorMsgBox(QString("%1 [%2]").arg(QObject::tr("登录失败,请联系客服。错误码")).arg(login->scode));
        break;
    }
}

void LoginWindow::parseLoginData(QSharedPointer<UserLoginResponse> login)
{
    int errorCode = login->code;
    switch(errorCode) {
    case HTTP_ERROR_SUCCESS: {
        // 登录成功 设置相关的值
        HttpRequestHeader header = HttpCmdManager::getInstance()->getRequestHeader();
        header.userKey = login->userKey;
        header.platform = LocalSetting::getInstance()->getDefaultsite();
        HttpCmdManager::getInstance()->setRequestHeader(header);

        MyConfig.userSet.id          = login->userId;
        MyConfig.userSet.userName    = login->userName;
        MyConfig.userSet.userKey     = login->userKey;
        MyConfig.userSet.accountType = login->accountType;
        MyConfig.accountSet.userId   = login->userId;
        MyConfig.accountSet.platform = LocalSetting::getInstance()->getDefaultsite();

        if(login->cmdType == RESTAPI_USER_LOGIN) {
            MyConfig.accountSet.password = m_strPass;

            // 记住用户名密码
            LocalSetting::getInstance()->setUserName(m_strUser);
            bool rememberPassword = ui->checkRemember->checkState() == Qt::Checked;
            if(rememberPassword) {
                LocalSetting::getInstance()->setPassword(m_strPass);
            }
            LocalSetting::getInstance()->setRememberPassword(rememberPassword);
            bool autoLogin = ui->checkAutolog->checkState() == Qt::Checked;

            // 存入数据库
            AccountItem account;
            account.name = m_strUser.toStdString();
            account.password = rememberPassword ? m_strPass.toStdString() : "";
            account.save_password_flag = rememberPassword;
            account.auto_login_flag = autoLogin;
            if (!account.name.empty()) {
                CProfile::Inst()->insert_account(account);
            }           
        }
        
        // step 3 开始获取客户端通知
        m_noticeMgr.startGetPublishMsg();
    }
    break;
    case HTTP_ERROR_ACCOUNT_USERNAME_PASSWORD_FAIL: {
        QScopedPointer<PwdMismatchWindow> pwdMisWnd(new PwdMismatchWindow);
        pwdMisWnd->exec();
    }
    break;
    case HTTP_ERROR_INTERNAL_HTTP_ERROR: {
        this->showErrorMsgBox(QObject::tr("服务器处理请求失败,请稍后重试"));
    }
    break;
    case HTTP_ERROR_FORBIDDEN: {
        this->showErrorMsgBox(QObject::tr("没有权限,请联系客服!"));
    }
    break;
    case HTTP_ERROR_FAIL: {
        this->showErrorMsgBox(QObject::tr("网络出现异常,请稍后重试"));
    }
    break;
    case HTTP_ERROR_ACCOUNT_BINDING_USER_NULL: {
        this->showErrorMsgBox(QObject::tr("用户名不存在!"));
    }
    break;
    case  HTTP_ERROR_ACCOUNT_LOGIN_IPLIMITFAIL: {
        this->showErrorMsgBox(QObject::tr("登录地址受限,请联系客服!"));
    }
    break;
    case HTTP_ERROR_ACCOUNT_LOCKED: {
        this->showErrorMsgBox(QObject::tr("此帐户已被禁用，请联系客服!"));
    }
    break;
    case HTTP_ERROR_USER_LOGIN_NEED_VERIFICATION: {
        QString ex = QString("&from=%1").arg(WSConfig::getInstance()->get_link_address("path_from"));
        QString url = QObject::tr("<a href = '%1'><span style='color:#675ffe;text-decoration:none;'>网页端验证</span></a>")
            .arg(RBHelper::getLinkUrl("path_login", ex).toString());
        QString msg = QObject::tr("<p>密码输错次数达到3次！</p> <p>请前往%1后重新登录</p>").arg(url);

        QPointer<CMessageBox> msgBox(new CMessageBox);
        msgBox->setMessage(msg, CMessageBox::MB_ERROR);
        connect(msgBox.data(), &CMessageBox::linkClicked, [&msgBox](const QString& v) {
            QDesktopServices::openUrl(QUrl(v));
            msgBox->accept();
        });
        msgBox->exec();
    }
    break;
    case HTTP_ERROR_ACCOUNT_NOT_BINDING: {
        this->showErrorMsgBox(QObject::tr("账号未绑定,请绑定后重新登录"));

        QString accessId = QString::fromStdString(CConfigDb::Inst()->getAccessId(MyConfig.userSet.userName.toStdString()));
        QString url = QString("%1/device-binding.html?lang=%2&phone=%3&userName=%4&accessId=%5")
            .arg(CConfigDb::Inst()->getHost())
            .arg(RBHelper::getLocaleName())
            .arg(MyConfig.accountSet.phone)
            .arg(MyConfig.accountSet.userName)
            .arg(accessId);

        qDebug() << url;
        QDesktopServices::openUrl(QUrl(url));
    }
    break;
    case HTTP_ERROR_ACCOUNT_NOACTIVE: {
        this->showErrorMsgBox(QObject::tr("账号未激活,请联系客服"));
    }
    break;
    case HTTP_ERROR_ACCOUNT_DISENABLED: {
        this->showErrorMsgBox(QObject::tr("账号已被停用,请联系客服"));
    }
    break;
    case HTTP_ERROR_USER_AREA_ERROR:
        this->showErrorMsgBox(QObject::tr("客户国内外区域不匹配"));
        break;
    case HTTP_ERROR_NEED_REDIRECT:
        this->showErrorMsgBox(QObject::tr("平台数据未迁移，请联系客服"));
        break;
    case  HTTP_ERROR_INTELLIGENT_VERIFY_FAIL:
        this->showErrorMsgBox(QObject::tr("输入密码次数过多，请尝试找回密码！"));
        break;
    case  HTTP_ERROR_FOX_USER_FORBIDDEN: {
        QString url = QObject::tr("<a href = '%1'><span style='color:#675ffe;text-decoration:none;'>请注册大雁云账号</span></a>")
            .arg(m_registerLink);
        QString msg = QObject::tr("<p>不支持Fox用户登录！</p> <p>%1</p>").arg(url);

        QPointer<CMessageBox> msgBox(new CMessageBox);
        msgBox->setMessage(msg, CMessageBox::MB_ERROR);
        connect(msgBox.data(), &CMessageBox::linkClicked, this, [this](const QString& v) {
            QDesktopServices::openUrl(QUrl(v));
        });
        connect(msgBox.data(), SIGNAL(linkClicked(const QString&)), msgBox, SLOT(close()));
        msgBox->exec();
    }      
        break;
    case  HTTP_ERROR_RENDERBUS_USER_FORBIDDEN: {
        QString url = QObject::tr("<a href = '%1'><span style='color:#675ffe;text-decoration:none;'>请注册大雁云账号</span></a>")
            .arg(m_registerLink);
        QString msg = QObject::tr("<p>不支持RenderBus用户登录！</p> <p>%1</p>").arg(url);

        QPointer<CMessageBox> msgBox(new CMessageBox);
        msgBox->setMessage(msg, CMessageBox::MB_ERROR);
        connect(msgBox.data(), &CMessageBox::linkClicked, this, [this](const QString& v) {
            QDesktopServices::openUrl(QUrl(v));
        });
        connect(msgBox.data(), SIGNAL(linkClicked(const QString&)), msgBox, SLOT(close()));
        msgBox->exec();
    }
        break;
    default: {
        // 未知异常
        if(this->isVisible()) {
            QString eMsg = LangTranslator::RequstErrorI18n(login->msg);
            if(eMsg.isEmpty()) {
                eMsg = "未知错误";
                eMsg += "\r\n";
                eMsg += "code:";
                eMsg += login->msg;
            }
            this->showErrorMsgBox(eMsg);
        }
    }
    break;
    }
}

void LoginWindow::initRefreshBtn()
{
    m_refreshBtn = new QPushButton(ui->comboServer);
    m_refreshBtn->setStyleSheet("QPushButton{border-image:url(:/icon/images/icon/refresh_n.png); border:none;background:transparent;padding:0px;}"
                                "QPushButton:hover{border-image:url(:/icon/images/icon/refresh_hover.png);}");
    m_refreshBtn->setToolTip(QObject::tr("刷新平台列表"));
    m_refreshBtn->setFocusPolicy(Qt::NoFocus);
    m_refreshBtn->setMaximumSize(15, 15);
    m_refreshBtn->hide();

    connect(m_refreshBtn, &QPushButton::clicked, this, [this]() {
        this->initPlatform();
        ui->comboServer->clear();
        QPushButton* refresh = qobject_cast<QPushButton*>(sender());
        if(refresh != NULL){
            refresh->hide();
        }
    });
}
