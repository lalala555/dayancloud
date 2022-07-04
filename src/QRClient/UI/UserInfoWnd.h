#ifndef USERINFOWND_H
#define USERINFOWND_H

#include <QWidget>
#include "ui_UserInfoWnd.h"

class MoneyInfoItem;

class UserInfoWnd : public QWidget
{
    Q_OBJECT

public:
    UserInfoWnd(QWidget *parent = 0);
    ~UserInfoWnd();
    void setUserName(const QString& userName);
    void setVipLevel(int level, qint64 picMemberValidity = 0);
    void setBalance(float balance);
    void setCoupon(float coupon, int couponCount = 0);
    void setHeadImage(const QString& img);
    void initMoneyItem();
    void setQCoupon(float qcoupon, int couponCount = 0);
    void setVoucher(int count);

private slots:
    void onChangePasswdClicked();
    void onBillingClicked();
    void onLogoutClicked();
    void onRechargeClicked();
    void onUserinfoClicked();
    void onVipLabelClicked();
    void onAssetsClicked();
    void onUserAuthClicked();

protected:
    virtual void focusOutEvent(QFocusEvent *e);
    virtual void paintEvent(QPaintEvent* e);

private:
    Ui::UserInfoWnd ui;
    MoneyInfoItem* m_balanceItem;
    MoneyInfoItem* m_couponItem;
    MoneyInfoItem* m_qcouponItem;
    MoneyInfoItem* m_voucherItem;
    int m_level;
    qint64 m_picMemberValidity;
};

#endif // USERINFOWND_H
