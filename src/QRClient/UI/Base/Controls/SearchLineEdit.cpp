#include "stdafx.h"
#include "SearchLineEdit.h"
#include <QPainter>
#include <QStyleOption>
#include <QStylePainter>
#include <QAction>

SearchTooltip::SearchTooltip(QWidget* parent)
    : QWidget(parent)
    , m_tooltipLabel(new QLabel(this))
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setFixedHeight(50);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(10);
    layout->setContentsMargins(10, 20, 10, 10);
    this->setLayout(layout);
    layout->addWidget(m_tooltipLabel);

    auto shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setOffset(0, 0);
    shadowEffect->setColor(QColor(16, 16, 16));
    shadowEffect->setBlurRadius(10);
    this->setGraphicsEffect(shadowEffect);
}
SearchTooltip::~SearchTooltip() {}

void SearchTooltip::setToolTip(const QString & text)
{
    m_tooltipText = text;

    QFontMetrics metrics(font());
    int tw = metrics.width(m_tooltipText);
    this->setMinimumWidth(tw);

    m_tooltipLabel->setText(m_tooltipText);
}

QString SearchTooltip::tooltip()
{
    return m_tooltipText;
}

void SearchTooltip::paintEvent(QPaintEvent *p)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::transparent, 2));
    painter.setBrush(QColor(68, 68, 68));

    /* Polygon
    3
    /\
    1------2/  \4------5
    |                  |
    |                  |
    7----------------  6
    */
    QPointF p1(0, 15);
    QPointF p2(width() / 2 - 10, 15);
    QPointF p3(width() / 2, 5);
    QPointF p4(width() / 2 + 10, 15);
    QPointF p5(width(), 15);
    QPointF p6(width(), height());
    QPointF p7(0, height());

    QPolygonF pw;
    pw.append(p1);
    pw.append(p2);
    pw.append(p3);
    pw.append(p4);
    pw.append(p5);
    pw.append(p6);
    pw.append(p7);

    painter.drawPolygon(pw);

    QWidget::paintEvent(p);
}


SearchLineEdit::SearchLineEdit(QWidget *parent)
    : LineEdit(parent)
    , m_tooltip(new SearchTooltip(this))
{
    this->setAttribute(Qt::WA_Hover);
    this->setMouseTracking(true);
}

SearchLineEdit::~SearchLineEdit()
{
}

void SearchLineEdit::setToolTip(const QString & text)
{
    m_tooltip->setToolTip(text);
}

void SearchLineEdit::showEvent(QShowEvent * event)
{
    m_tooltip->setMinimumWidth(qMax(m_tooltip->minimumWidth(), this->width()));
    LineEdit::showEvent(event);
}

void SearchLineEdit::enterEvent(QEvent * event)
{
    LineEdit::enterEvent(event);

    if(m_tooltip->tooltip().isEmpty())
        return;

    int px = -(m_tooltip->minimumWidth() - this->width()) / 2;
    QPoint pos = mapToGlobal(QPoint(px, this->height()));
    m_tooltip->move(pos);

    m_tooltip->show();
}

void SearchLineEdit::leaveEvent(QEvent * event)
{
    LineEdit::leaveEvent(event);

    m_tooltip->hide();
}

void SearchLineEdit::focusOutEvent(QFocusEvent *event)
{
    emit lostFocus();
}

