#ifndef POLYGONWIDGET_H
#define POLYGONWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QPoint>

class PolygonWidget : public QWidget
{
    Q_OBJECT

public:
    PolygonWidget(QWidget *parent = Q_NULLPTR);
    ~PolygonWidget();
    void setWidgetWidth(int width) { m_width = width; }
    void setWidgetHeight(int height) { m_height = height; }

protected:
    virtual void paintEvent(QPaintEvent *p);

private:
    int m_height;
    int m_width;
};

#endif // POLYGONWIDGET_H
