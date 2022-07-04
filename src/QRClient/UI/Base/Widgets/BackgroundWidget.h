#ifndef BACKGROUNDWIDGET_H
#define BACKGROUNDWIDGET_H

#include <QWidget>
#include <QColor>
#include <QPaintEvent>
#include <QPainter>

class BackgroundWidget : public QWidget
{
    Q_OBJECT

public:
    BackgroundWidget(QWidget *parent = 0);
    ~BackgroundWidget();
    void setBackColor(const QColor& color, double opacity = 0.2);
    void setProgress(float progress);

protected:
    virtual void paintEvent(QPaintEvent *);

private:
    float m_progress;
    QColor m_backColor;
    double m_opacity;
};

#endif // BACKGROUNDWIDGET_H
