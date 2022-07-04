/***********************************************************************
* Module:  ListWidget.h
* Author:  hqr
* Modified: 2016/09/14 11:33:06
* Purpose: Declaration of the class
***********************************************************************/
#ifndef LISTWIDGET_H
#define LISTWIDGET_H

#include <QListWidget>

class ListWidget : public QListWidget
{
    Q_OBJECT

public:
    ListWidget(QWidget *parent = 0);
    ~ListWidget();

protected:
    virtual void enterEvent(QEvent * event);
    virtual void leaveEvent(QEvent * event);

private:

};

#endif // LISTWIDGET_H
