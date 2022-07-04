#include "stdafx.h"
#include "IconLineEdit.h"
#include <QPainter>
#include <QStyleOption>
#include <QStylePainter>
#include <QAction>

IconLineEdit::IconLineEdit(QWidget *parent)
    : QLineEdit(parent)
    , m_supportIM(false)
    , m_revealAction(new QAction())
{
    connect(this, &QLineEdit::editingFinished, [=]() {
        //if (this->echoMode() == QLineEdit::Password) {
            QTimer::singleShot(2000, this, [=] {
                if (this->echoMode() == QLineEdit::Password) {
                    m_revealAction->setVisible(false);
                }              
            });
        //}
    });

    connect(this, &QLineEdit::textEdited, [=](const QString &text) {
        //if (this->echoMode() == QLineEdit::Password) {
        m_revealAction->setVisible(true);
        //}
    });

    this->setStyleSheet(
        "QLineEdit[echoMode='2'] { lineedit-password-character: 9679;}"
        "QLineEdit[echoMode='2'] { lineedit-password-mask-delay: 0; }" // 远程或录屏时防止密码泄露
    );
}

IconLineEdit::~IconLineEdit()
{

}

void IconLineEdit::setIcon(const QPixmap& pixmap)
{
    m_pixmap = pixmap;
    this->setTextMargins(pixmap.width() + 12, 1, 1, 1);
}

void IconLineEdit::setEye(const QIcon& icon)
{
    if(this->echoMode() == QLineEdit::Password) {
        m_revealAction = this->addAction(icon, QLineEdit::TrailingPosition);
        connect(m_revealAction, SIGNAL(triggered()), this, SLOT(onRevealTriggered()));
        m_revealAction->setVisible(false);
    }
}

void IconLineEdit::focusInEvent(QFocusEvent * event)
{
    m_revealAction->setVisible(true);
    QLineEdit::focusInEvent(event);
    emit(focussed(true));
}

void IconLineEdit::focusOutEvent(QFocusEvent *event)
{
    QLineEdit::focusOutEvent(event);
    emit(focussed(false));
}

void IconLineEdit::paintEvent(QPaintEvent *event)
{
    QStylePainter style(this);
    // 用style画背景 (会使用setstylesheet中的内容)
    QStyleOption option;
    option.initFrom(this);
    option.rect=rect();
    style.drawPrimitive(QStyle::PE_Widget, option);
    QLineEdit::paintEvent(event);

    if(!m_pixmap.isNull()) {
        // 绘制icon
        QPainter painter(this);
        QRect rect(10, (this->height() - m_pixmap.height()) / 2, m_pixmap.width(), m_pixmap.height());
        painter.drawPixmap(rect, m_pixmap);
    }
}

void IconLineEdit::onRevealTriggered()
{
    if (this->echoMode() == QLineEdit::Password) {
        this->setEchoMode(QLineEdit::Normal);
        m_revealAction->setIcon(QIcon(":/icon/images/icon/eye_on.svg"));
    } else if (this->echoMode() == QLineEdit::Normal) {
        this->setEchoMode(QLineEdit::Password);
        m_revealAction->setIcon(QIcon(":/icon/images/icon/eye_off.svg"));
    }
}

QVariant IconLineEdit::inputMethodQuery(Qt::InputMethodQuery query) const
{
    if (m_supportIM == false && Qt::ImEnabled == query)
        return false;

    return QLineEdit::inputMethodQuery(query);
}