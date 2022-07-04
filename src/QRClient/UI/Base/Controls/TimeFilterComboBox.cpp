#include "stdafx.h"
#include "TimeFilterComboBox.h"

TimeFilterComboBox::TimeFilterComboBox(QWidget *parent)
    : QComboBox(parent)
{
    m_filterDelegate = new TimeFilterDelegate(this);
    this->setItemDelegate(m_filterDelegate);
    connect(this, SIGNAL(currentIndexChanged(int)), m_filterDelegate, SLOT(currentIndex(int)));

    this->view()->installEventFilter(this);
}

TimeFilterComboBox::~TimeFilterComboBox()
{
    delete m_filterDelegate;
}

void TimeFilterComboBox::showPopup()
{
    bool oldAnimationEffects = qApp->isEffectEnabled(Qt::UI_AnimateCombo);
    qApp->setEffectEnabled(Qt::UI_AnimateCombo, false);

    QComboBox::showPopup();
    qApp->setEffectEnabled(Qt::UI_AnimateCombo, oldAnimationEffects);
}

bool TimeFilterComboBox::eventFilter(QObject *o, QEvent *e)
{
    bool handled = false;
    if(e->type() == QEvent::Show) {
        if(o == view()) {
            QWidget* popup = findChild<QFrame*>();
            popup->move(popup->x(), mapToGlobal(this->geometry().topLeft()).y() - popup->height() - 6);
        }
    }

    if(!handled)
        handled = QComboBox::eventFilter(o, e);

    return handled;
}

TimeFilterDelegate::TimeFilterDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
    , m_curIndex(enFilter1Month)
{
}

void TimeFilterDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem ostyle = option;
    initStyleOption(&ostyle, index);
    if(ostyle.state.testFlag(QStyle::State_HasFocus))
        ostyle.state = ostyle.state ^ QStyle::State_HasFocus;
    QStyledItemDelegate::paint(painter, ostyle, index);

    if(index.row() == m_curIndex) {
        int height = (ostyle.rect.height() - 20) / 2;
        QPixmap pix = QPixmap(":/images/success.png");
        QRect rect = QRect(ostyle.rect.left() + ostyle.rect.width() - 30, ostyle.rect.top() + height, 20, 20);
        painter->drawPixmap(rect, pix);
    }
}

void TimeFilterDelegate::currentIndex(int index)
{
    m_curIndex = index;
}