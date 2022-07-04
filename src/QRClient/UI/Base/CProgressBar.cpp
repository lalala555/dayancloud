#include "stdafx.h"
#include "CProgressBar.h"

CProgressBar::CProgressBar(QWidget *parent)
    : QProgressBar(parent)
    , m_text("")
{
    setMinimum(0);
    setMaximum(100);
    setValue(0);
}

CProgressBar::~CProgressBar()
{

}

QColor CProgressBar::backgroundColor() const
{
    return m_backgroundColor;
}

void CProgressBar::setBackgroundColor(const QColor &backgroundColor)
{
    m_backgroundColor = backgroundColor;
}

QColor CProgressBar::foregroundColor() const
{
    return m_foregroundColor;
}

void CProgressBar::setForegroundColor(const QColor &foregroundColor)
{
    m_foregroundColor = foregroundColor;
}

QColor CProgressBar::textColor() const
{
    return m_textColor;
}

void CProgressBar::setTextColor(const QColor &textColor)
{
    m_textColor = textColor;
}

void CProgressBar::setColorAt(qreal progress, const QColor& color)
{
    m_gradient->setColorAt(progress, color);
}

void CProgressBar::paintEvent(QPaintEvent *paint)
{
    Q_UNUSED(paint);

    qreal width = this->width();
    qreal height = this->height();
    qreal radius = height / 2;
    qreal t = maximum() - minimum();
    if (t <= 0) t = 1;
    qreal percentage = (double)(value() - minimum()) / t;
    QPainter painter(this);
    QRectF rect(2, 2, width - 4, height - 4);
    QRectF rectForeground(2, 2, height + ((width - height) * percentage)- 4, height - 4);
    QRectF txtForeground(0, 2, this->width(), height - 4);

    painter.setRenderHint(QPainter::Antialiasing, true);		//	±ﬂ‘µ∆Ωª¨..

    // ªÊ÷∆±≥æ∞
    m_pen.setColor(foregroundColor());
    m_pen.setWidth(1);
    painter.setPen(m_pen);
    painter.setBrush(backgroundColor());
    painter.drawRoundedRect(rect, radius, radius);

    // ªÊ÷∆«∞æ∞
    QBrush brush(*m_gradient);
    painter.setBrush(brush);
    if(percentage == 0){
        rectForeground = QRectF(2, 2, 0, 0);
        painter.drawRoundedRect(rectForeground, 0, 0);
    }else{
        painter.drawRoundedRect(rectForeground, radius, radius);
    }

    m_pen.setColor(textColor());
    painter.setPen(m_pen);
    painter.setFont(m_font);
    if(m_text.isEmpty()){
        painter.drawText(txtForeground, Qt::AlignHCenter | Qt::AlignVCenter,
            QString::number(int(percentage * 100)) + "% ");
    }else{
        painter.drawText(txtForeground, Qt::AlignHCenter | Qt::AlignVCenter, m_text);
    }
    
}

void CProgressBar::setText(const QString& txt)
{
    m_text = txt;
}