#include "stdafx.h"
#include "UserInfoWnd.h"
#include "UI/Base/Controls/MoneyInfoItem.h"
#include <QWidgetAction>
#include <QGraphicsDropShadowEffect>
#include "Common/StringUtil.h"
#include "HttpCommand/HttpRestApi.h"

UserInfoWnd::UserInfoWnd(QWidget *parent)
    : QWidget(parent)
    , m_balanceItem(nullptr)
    , m_couponItem(nullptr)
    , m_qcouponItem(nullptr)
    , m_voucherItem(nullptr)
{
    ui.setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setAttribute(Qt::WA_DeleteOnClose);
    connect(ui.widget_logout, SIGNAL(clicked()), this, SLOT(onLogoutClicked()));
    connect(ui.widget_billing, SIGNAL(clicked()), this, SLOT(onBillingClicked()));
    connect(ui.widget_changePasswd, SIGNAL(clicked()), this, SLOT(onChangePasswdClicked()));
    connect(ui.label_header, SIGNAL(clicked()), this, SLOT(onUserinfoClicked()));
    connect(ui.label_username, SIGNAL(clicked()), this, SLOT(onUserinfoClicked()));
    connect(ui.label_member, SIGNAL(clicked()), this, SLOT(onVipLabelClicked()));
    connect(ui.widget_assets, SIGNAL(clicked()), this, SLOT(onAssetsClicked()));
    connect(ui.label_auth, SIGNAL(clicked()), this, SLOT(onUserAuthClicked()));

    auto shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setOffset(0, 0);
    shadowEffect->setColor(QColor(4, 4, 4));
    shadowEffect->setBlurRadius(15);
    ui.mainWidget->setGraphicsEffect(shadowEffect);

    // 添加到窗口中
    this->layout()->setContentsMargins(4, 4, 4, 4);

    // 初始化有关钱的item
    this->initMoneyItem();

    ui.mainWidget->setWidgetHeight(350);
    if(MyConfig.userSet.isChildAccount() && MyConfig.accountSet.hideBalance) {
        ui.frameUserInfo->hide();
        ui.widget_money->hide();
        ui.mainWidget->setWidgetHeight(200);
    }

    // ui.widget_vip_info->hide();
    // ui.widget_billing->hide();
    // ui.widget_money->hide();

    this->setFocus();
    this->hide();

    QString status = "";
    if (MyConfig.accountSet.authStatus == AUTHSTATUS_AUTH_NONE) {
        status = QString(tr("未实名"));
    } else if (MyConfig.accountSet.authStatus == AUTHSTATUS_AUTH_DONE ||
        MyConfig.accountSet.authStatus == AUTHSTATUS_AUTH_PERSONAL_OK ||
        MyConfig.accountSet.authStatus == AUTHSTATUS_AUTH_ENTERPRISE_OK) {
        status = QString(tr("已实名"));
    } else if (MyConfig.accountSet.authStatus == AUTHSTATUS_AUTH_AUTHING) {
        status = QString(tr("审核中"));
    } else if (MyConfig.accountSet.authStatus == AUTHSTATUS_AUTH_FAILED) {
        status = QString(tr("认证失败"));
    } else if (MyConfig.accountSet.authStatus == AUTHSTATUS_AUTH_WAITING) {
        status = QString(tr("待审核"));
    } else if (MyConfig.accountSet.authStatus == AUTHSTATUS_AUTH_CHANGING) {
        status = QString(tr("变更中"));
    } else {
        ui.label_auth->hide();
    }

    QString lab = QString("<a style='color:#6b6cff; font-size:14px;'>%1</a>").arg(status);
    ui.label_auth->setText(lab);

#ifdef FOXRENDERFARM
	ui.label_auth->hide();
	ui.label_member->hide();
	ui.label_username->setFixedWidth(200);
	ui.frameUserInfo->setFixedWidth(100);
	ui.frameUserInfo->setFixedWidth(100);
#endif

}

UserInfoWnd::~UserInfoWnd()
{
}

void UserInfoWnd::setUserName(const QString& userName)
{
    ui.label_username->setText(String::getElidedText(ui.label_username, userName, 100));
    ui.label_username->setToolTip(userName);
}

void UserInfoWnd::setVipLevel(int level, qint64 picMemberValidity)
{
    if (MyConfig.userSet.accountType == MAIN_ACCOUNT) {
        QString levelDisplay = RBHelper::getVipDisplay(level);
        if (!levelDisplay.isEmpty()) {
            ui.widget_vip_info->show();

            levelDisplay = QString("<a style='color:#675ffe; font-size:10px;'>%1</a>").arg(levelDisplay);
            ui.label_member->setText(levelDisplay);

            if (picMemberValidity > 0) {
                QDateTime timedate = QDateTime::fromMSecsSinceEpoch(picMemberValidity);
                QString time = QObject::tr("%1到期").arg(timedate.toString("yyyy-MM-dd"));               
                time = QString("<a style='color:#675ffe; font-size:10px;'>%1</a>").arg(time);
               
                ui.label_time->setText(time);
                ui.label_time->show();
            }
        }
    } else if (MyConfig.userSet.isChildAccount()) {
        ui.widget_vip_info->show();
        ui.label_member->setText(QObject::tr("子账户"));
        ui.label_time->hide();
    }
}

void UserInfoWnd::setBalance(float balance)
{
#ifdef FOXRENDERFARM
    QString ba = "$ ";
#else
    QString ba = "￥ ";
#endif
    ba += QString::number(balance, 'f', 3); 
    if (m_balanceItem) m_balanceItem->setMoneyText(ba);
}

void UserInfoWnd::setCoupon(float coupon, int couponCount)
{
    QString cou = "C ";
    cou += QString::number(coupon, 'f', 3);
    /*if(MyConfig.userSet.isChildAccount()) {
        cou = "— —";
    }*/
   
    QString btnTxt = QObject::tr("%1张").arg(couponCount);
    if (m_couponItem != nullptr) {
        m_couponItem->setMoneyText(cou);
        m_couponItem->setBtnText(btnTxt);
    } 
}

void UserInfoWnd::setVoucher(int count)
{
    QString btnTxt = QObject::tr("%1张").arg(count);
    if (m_voucherItem != nullptr) m_voucherItem->setBtnText(btnTxt);
}

void UserInfoWnd::setHeadImage(const QString& img)
{
    ui.label_header->setPixmap(QPixmap(img));
}

void UserInfoWnd::onChangePasswdClicked()
{
    // QString ex = QString("&from=%1").arg(WSConfig::getInstance()->get_link_address("path_from"));// .arg(LocalConfig::getInstance()->defaultProduct.toLower());
    // QUrl url = RBHelper::getLinkUrl("path_change_password", ex);
    // QDesktopServices::openUrl(url);
    QUrl url = RBHelper::getLinkUrl("path_user_center");
    QDesktopServices::openUrl(url);
}

void UserInfoWnd::onBillingClicked()
{
    QString ex = QString("&from=%1").arg(WSConfig::getInstance()->get_link_address("path_from"));//.arg(LocalConfig::getInstance()->defaultProduct.toLower());
    QUrl url = RBHelper::getLinkUrl("path_consumption", ex);
    QDesktopServices::openUrl(url);
}

void UserInfoWnd::onLogoutClicked()
{
    this->close();
    CGlobalHelp::Response(UM_LOGOUT, 0, 0);
}

void UserInfoWnd::onRechargeClicked()
{
    QUrl url;
    // 子账号 跳转至消费清单
    if(MyConfig.userSet.isChildAccount()) {
        url = RBHelper::getLinkUrl("path_account_child_recharge");
    } else {
        url = RBHelper::getLinkUrl("path_account_parent_recharge");
    }
    QDesktopServices::openUrl(url);
}

void UserInfoWnd::onUserinfoClicked()
{
    return;
    QUrl url = RBHelper::getLinkUrl("path_user_profile");
    QDesktopServices::openUrl(url);
}

void UserInfoWnd::onVipLabelClicked()
{
    onRechargeClicked();
}

void UserInfoWnd::focusOutEvent(QFocusEvent *e)
{
    this->close();
}

void UserInfoWnd::initMoneyItem()
{
    QUrl balanceUrl;
    // 子账号 跳转至消费清单
    if (MyConfig.userSet.isChildAccount()) {
        balanceUrl = RBHelper::getLinkUrl("path_account_child_recharge");
    } else {
        balanceUrl = RBHelper::getLinkUrl("path_account_parent_recharge");
    }

    m_balanceItem = new MoneyInfoItem(":/images/userInfo/icon_balance_n.png", QObject::tr("余额"), QObject::tr("充值"), balanceUrl.toString());

    // 代金券
    QUrl couponUrl = RBHelper::getLinkUrl("path_user_coupon");
    m_couponItem = new MoneyInfoItem(":/images/userInfo/icon_renderingcoupon_n.png", QObject::tr("大雁券"), QObject::tr("0张"), couponUrl.toString());

    // 青云券
    QUrl qcouponUrl = RBHelper::getLinkUrl("path_user_coupon");
    m_qcouponItem = new MoneyInfoItem(":/images/userInfo/icon_qingyuncoupon_n.png", QObject::tr("青云券"), QObject::tr("0张"), qcouponUrl.toString());
    // 免单券
    QUrl voucherUrl = RBHelper::getLinkUrl("path_user_voucher");
    m_voucherItem = new MoneyInfoItem(":/images/userInfo/icon_renderingcoupon_n.png", QObject::tr("免单券"), QObject::tr("0张"), voucherUrl.toString());

    // 添加到界面
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(ui.widget_money->layout());
    if(layout == nullptr) return;
    layout->addWidget(m_balanceItem);
    layout->addWidget(m_couponItem);
    // layout->addWidget(m_voucherItem);

    ui.mainWidget->setWidgetHeight(450);
    ui.widget_money->setLayout(layout);
}

void UserInfoWnd::setQCoupon(float qcoupon, int couponCount)
{
#ifdef FOXRENDERFARM
    QString ba = "$ ";
#else
    QString ba = "C ";
#endif

    ba += QString::number(qcoupon, 'f', 3);
    m_qcouponItem->setMoneyText(ba);

    QString btnTxt = QObject::tr("%1张").arg(couponCount);
    m_qcouponItem->setBtnText(btnTxt);
}

void UserInfoWnd::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    QPainter painter(this);
}

void UserInfoWnd::onAssetsClicked()
{
    QString ex = QString("&from=%1").arg(WSConfig::getInstance()->get_link_address("path_from"));// .arg(LocalConfig::getInstance()->defaultProduct.toLower());
    QUrl url = RBHelper::getLinkUrl("path_asset_manage", ex);
    QDesktopServices::openUrl(url);
}

void UserInfoWnd::onUserAuthClicked()
{
    QUrl url = RBHelper::getLinkUrl("path_user_auth");
    QDesktopServices::openUrl(url);
}