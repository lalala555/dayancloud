#include "stdafx.h"
#include "TabWidget.h"

BadgeLabel::BadgeLabel(QWidget* parent)
    : QLabel(parent)
{
    this->setAlignment(Qt::AlignCenter);
    this->setMargin(0);
    this->setStyleSheet("QLabel{font-weight:bold;color:#fff;background-color:#e25c59; border-radius: 8px;}");
}

void BadgeLabel::setNumber(const QString& number)
{
    if(number.isEmpty() || number == "0") {
        this->setFixedSize(0, 0);
        return;
    }

    QFontMetrics fm(this->font());
    int w = fm.width(number);
    int h = fm.height();
    this->setFixedSize(w + 10, h);
    this->setText(number);
}

CTabWidget::CTabWidget(QWidget *parent)
    : QTabWidget(parent)
{
    m_tabBar = tabBar();
}

CTabWidget::~CTabWidget()
{
}

int CTabWidget::addTab(QWidget * page, const QString & label)
{
    return QTabWidget::addTab(page, label);
}

void CTabWidget::setTabTextNumber(int index, const QString& text, int number)
{
    QString tabTextExt;
    if(number > 0) {
        tabTextExt = QString("(%1)").arg(number);
    }

    QTabWidget::setTabText(index, text + tabTextExt);
}

void CTabWidget::setBadgeNumber(int index, int number)
{
    BadgeLabel* badge = new BadgeLabel(this);
    badge->setNumber(QString::number(number));
    m_tabBar->setTabButton(index, QTabBar::RightSide, badge);
}

void CTabWidget::setNotify(int index, bool notify)
{

}
