#include "stdafx.h"
#include "qroundprogressbar.h"
#include <QPainter>

void QRoundProgressBar::setValue(double value)
{
    currentValue = value;

    update();
}

void QRoundProgressBar::paintEvent(QPaintEvent *)
{
    int width = this->width();
    int height = this->height();
    int side = qMin(width, height);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.translate(width / 2, height / 2);
    painter.scale(side / 200.0, side / 200.0);

    drawBackground(&painter);
    drawOuterCircle(&painter);
    drawInnerCircle(&painter);
    drawValue(&painter);
}

void QRoundProgressBar::drawBackground(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(ringBgColor);
    painter->drawEllipse(-radius, -radius, radius * 2, radius * 2);
    painter->restore();
}

void QRoundProgressBar::drawOuterCircle(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(ringColor);

    QRectF rect(-radius, -radius, radius * 2, radius * 2);

    double angleAll = 360.0;
    double angleCurrent = angleAll * ((currentValue - minValue) / (maxValue - minValue));
    double angleOther = angleAll - angleCurrent;

    painter->setBrush(ringColor);
    painter->drawPie(rect, (startAngle - angleCurrent) * 16, angleCurrent * 16);

    painter->setBrush(ringBgColor);
    painter->drawPie(rect, (startAngle - angleCurrent - angleOther) * 16, angleOther * 16);

    painter->restore();
}

void QRoundProgressBar::drawInnerCircle(QPainter *painter)
{
    int innerRadius = radius - ringWidth;
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(circleColor);
    painter->drawEllipse(-innerRadius, -innerRadius, innerRadius * 2, innerRadius * 2);
    painter->restore();
}

void QRoundProgressBar::drawValue(QPainter *painter)
{
    int innerRadius = radius - ringWidth;
    painter->save();
    painter->setPen(textColor);

    QFont font;
    font.setPixelSize(30);
    painter->setFont(font);

    QRectF textRect(-innerRadius, -innerRadius, innerRadius * 2, innerRadius * 2);
    double percent = (currentValue * 100) / (maxValue - minValue);
    QString strValue = QString("%1%").arg(percent, 0, 'f', 0);

    painter->drawText(textRect, Qt::AlignCenter, strValue);

    painter->restore();
}