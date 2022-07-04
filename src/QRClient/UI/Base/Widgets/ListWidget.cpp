#include "stdafx.h"
#include "ListWidget.h"

ListWidget::ListWidget(QWidget *parent)
    : QListWidget(parent)
{
    this->horizontalScrollBar()->hide();
    this->verticalScrollBar()->hide();
}

ListWidget::~ListWidget()
{

}

void ListWidget::enterEvent(QEvent * event)
{
    this->horizontalScrollBar()->show();
    this->verticalScrollBar()->show();

    QListWidget::enterEvent(event);
}

void ListWidget::leaveEvent(QEvent * event)
{
    this->horizontalScrollBar()->hide();
    this->verticalScrollBar()->hide();

    QListWidget::leaveEvent(event);
}
