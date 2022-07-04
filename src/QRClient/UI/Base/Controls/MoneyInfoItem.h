#ifndef MONEYINFOITEM_H
#define MONEYINFOITEM_H

#include <QWidget>
#include "ui_MoneyInfoItem.h"

class MoneyInfoItem : public QWidget
{
    Q_OBJECT

public:
    MoneyInfoItem(const QString& icon, const QString& name, const QString& btnTxt = "", 
                  const QString& url = "", const QString& money = "", QWidget *parent = 0);
    ~MoneyInfoItem();

    void setIconPath(const QString& path);
    void setNameText(const QString& name);
    void setBtnText(const QString txt);
    void setBtnUrl(const QString& url);
    void setMoneyText(const QString& money);

    void hideUrlBtn();
    void hideMoneyLabel();
signals:
    void clicked();
private slots:
    void onBtnClicked();
    void onVipLabelClicked();

private:
    Ui::MoneyInfoItem ui;
    QString m_url;
};

#endif // MONEYINFOITEM_H
