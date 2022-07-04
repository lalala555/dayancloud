#include "stdafx.h"
#include "PolygonWidget.h"
#include <QPolygon>

PolygonWidget::PolygonWidget(QWidget *parent)
    : QWidget(parent)
    , m_height(300)
    , m_width(260)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
}

PolygonWidget::~PolygonWidget()
{

}

void PolygonWidget::paintEvent(QPaintEvent *p)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::transparent, 2));
    painter.setBrush(QColor(31, 31, 39));
    QPolygonF pw;
    pw.append(QPointF(0, 20));
    pw.append(QPointF(70, 20));
    pw.append(QPointF(80, 10));
    pw.append(QPointF(90, 20));
    pw.append(QPointF(m_width, 20));
#if 0
    if(MyConfig.userSet.isChildAccount() && MyConfig.accountSet.hideBalance) {
        pw.append(QPointF(240, 180));
        pw.append(QPointF(0, 180));
    } else {
        pw.append(QPointF(240, 260));
        pw.append(QPointF(0, 260));
    }
#else
    pw.append(QPointF(m_width, m_height));
    pw.append(QPointF(0, m_height));
#endif

    // painter.drawPolygon(pw);
    painter.drawRoundedRect(QRect(0, 0, m_width, m_height), 10, 10);
}