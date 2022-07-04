#ifndef CLICKWIDGET_H
#define CLICKWIDGET_H

#include <QWidget>

class ClickWidget : public QWidget
{
    Q_OBJECT

public:
    ClickWidget(QWidget *parent = Q_NULLPTR);
    ~ClickWidget();

signals:
    void clicked();
protected:
    virtual void mousePressEvent(QMouseEvent * e); 
    virtual void paintEvent(QPaintEvent *p);
};

#endif // CLICKWIDGET_H
