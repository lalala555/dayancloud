#include "stdafx.h"
#include "ClickWidget.h"

ClickWidget::ClickWidget(QWidget *parent)
    : QWidget(parent)
{

}

ClickWidget::~ClickWidget()
{

}

void ClickWidget::mousePressEvent(QMouseEvent * e)
{
    emit clicked();
}

void ClickWidget::paintEvent(QPaintEvent *p)
{
    QStylePainter painter(this);
    //用style画背景 (会使用setstylesheet中的内容)
    QStyleOption opt;
    opt.initFrom(this);
    opt.rect=rect();
    painter.drawPrimitive(QStyle::PE_Widget, opt);

    QWidget::paintEvent(p);
}