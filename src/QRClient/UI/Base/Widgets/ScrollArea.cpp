#include "stdafx.h"
#include "ScrollArea.h"

ScrollArea::ScrollArea(QWidget *parent)
    :QScrollArea(parent)
{
    QScrollBar *pVbar = this->verticalScrollBar();
    pVbar->hide();
    QScrollBar *pHbar = this->horizontalScrollBar();
    pHbar->hide();
}

ScrollArea::~ScrollArea()
{

}

void ScrollArea::enterEvent( QEvent *e )
{
    QScrollBar *pVbar = this->verticalScrollBar();
    pVbar->show();
    QScrollBar *pHbar = this->horizontalScrollBar();
    pHbar->show();
}

void ScrollArea::leaveEvent( QEvent *e )
{
    QScrollBar *pVbar = this->verticalScrollBar();
    pVbar->hide();
    QScrollBar *pHbar = this->horizontalScrollBar();
    pHbar->hide();
}
