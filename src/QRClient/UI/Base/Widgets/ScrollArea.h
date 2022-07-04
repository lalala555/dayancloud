#ifndef SCROLLAREA_H
#define SCROLLAREA_H

#include <QScrollArea>

class ScrollArea : public QScrollArea
{
    Q_OBJECT

public:
    ScrollArea( QWidget *parent = 0);
    ~ScrollArea();
protected:
    virtual void enterEvent(QEvent *e);
    virtual void leaveEvent(QEvent *e);
private:

};

#endif // SCROLLAREA_H
