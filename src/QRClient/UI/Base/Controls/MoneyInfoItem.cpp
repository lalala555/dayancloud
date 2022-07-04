#include "stdafx.h"
#include "MoneyInfoItem.h"

MoneyInfoItem::MoneyInfoItem(const QString& icon, const QString& name, const QString& btnTxt, 
                             const QString& url, const QString& money, QWidget *parent)
    : QWidget(parent)
    , m_url(url)
{
    ui.setupUi(this);

    this->setIconPath(icon);
    this->setNameText(name);
    this->setBtnText(btnTxt);
    this->setMoneyText(money);
    ui.btn_item->setFocusPolicy(Qt::NoFocus);

    connect(ui.btn_item, SIGNAL(clicked()), this, SLOT(onBtnClicked()));
    connect(ui.label_value, SIGNAL(clicked()), this, SLOT(onVipLabelClicked()));
#ifdef FOXRENDERFARM
	ui.btn_item->setFixedWidth(70);
#endif
}

MoneyInfoItem::~MoneyInfoItem()
{

}

void MoneyInfoItem::onBtnClicked()
{
    emit clicked();

    QDesktopServices::openUrl(QUrl(m_url));
}

void MoneyInfoItem::setIconPath(const QString& path)
{
    ui.label_icon->setPixmap(QPixmap(path));
}

void MoneyInfoItem::setNameText(const QString& name)
{
    ui.label_name->setText(name);
}

void MoneyInfoItem::setBtnText(const QString txt)
{
    ui.btn_item->setText(txt);
}

void MoneyInfoItem::setBtnUrl(const QString& url)
{
    m_url = url;
}

void MoneyInfoItem::setMoneyText(const QString& money)
{
    ui.label_value->setText(money);
}

void MoneyInfoItem::hideUrlBtn()
{
    ui.btn_item->hide();
}

void MoneyInfoItem::hideMoneyLabel()
{
    ui.label_value->hide();
}

void MoneyInfoItem::onVipLabelClicked()
{
    this->onBtnClicked();
}