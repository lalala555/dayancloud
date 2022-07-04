#include "stdafx.h"
#include "AccountLineEdit.h"
#include <QPainter>
#include <QStyleOption>
#include <QStylePainter>
#include <QAction>

AccountLineEdit::AccountLineEdit(QWidget *parent)
    : QLineEdit(parent)
    , m_accountPopup(new AccountList(this))
    , m_supportIM(false)
{
    qRegisterMetaType<UserAccount>("UserAccount");

    m_accountPopup->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);

    connect(m_accountPopup, &AccountList::hideSignal, this, &AccountLineEdit::onIconChanged);
    connect(m_accountPopup, &AccountList::showSignal, this, &AccountLineEdit::onIconChanged);
    connect(m_accountPopup, &AccountList::selected, m_accountPopup, &AccountList::hide);
    connect(m_accountPopup, &AccountList::selected, this, &AccountLineEdit::selected);
    connect(m_accountPopup, &AccountList::deleted, this, &AccountLineEdit::deleted);
}

AccountLineEdit::~AccountLineEdit()
{
}

void AccountLineEdit::setIcon(const QPixmap& pixmap)
{
    m_pixmap = pixmap;
    this->setTextMargins(pixmap.width() + 12, 1, 1, 1);
}

void AccountLineEdit::setDropdownIcon()
{
    m_dropdownAction = this->addAction(QIcon(":/icon/images/icon/down.png"), QLineEdit::TrailingPosition);
    connect(m_dropdownAction, SIGNAL(triggered()), this, SLOT(onRevealTriggered()));
}

void AccountLineEdit::paintEvent(QPaintEvent *p)
{
    QStylePainter style(this);
    // 用style画背景 (会使用setstylesheet中的内容)
    QStyleOption option;
    option.initFrom(this);
    option.rect = rect();
    style.drawPrimitive(QStyle::PE_Widget, option);
    QLineEdit::paintEvent(p);

    if(!m_pixmap.isNull()) {
        // 绘制icon
        QPainter painter(this);
        QRect rect(10, (this->height() - m_pixmap.height()) / 2, m_pixmap.width(), m_pixmap.height());
        painter.drawPixmap(rect, m_pixmap);
    }
}

void AccountLineEdit::onRevealTriggered()
{
    m_accountPopup->move(this->mapToGlobal(QPoint(0, this->height())));
    m_accountPopup->setFixedWidth(this->width());
    m_accountPopup->showList();
}

void AccountLineEdit::onIconChanged()
{
    if(m_accountPopup->isVisible()) {
        m_dropdownAction->setIcon(QIcon(":/icon/images/icon/up.png"));
    } else {
        m_dropdownAction->setIcon(QIcon(":/icon/images/icon/down.png"));
    }
}

void AccountLineEdit::focusInEvent(QFocusEvent *event)
{
    QLineEdit::focusInEvent(event);
    emit(focussed(true));
}

void AccountLineEdit::focusOutEvent(QFocusEvent *event)
{
    if (m_accountPopup->isVisible() && event->reason() == Qt::MouseFocusReason) {
        m_accountPopup->hide();
    }
    QLineEdit::focusOutEvent(event);
    emit(focussed(false));
}

QVariant AccountLineEdit::inputMethodQuery(Qt::InputMethodQuery query) const
{
#if 1
    if (m_supportIM == false && Qt::ImEnabled == query)
        return false;
#endif
    return QLineEdit::inputMethodQuery(query);
}
