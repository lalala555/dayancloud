#ifndef CPROGRESSBAR_H
#define CPROGRESSBAR_H

#include <QProgressBar>
#include <QPen>
#include <QFont>

class CProgressBar : public QProgressBar
{
    Q_OBJECT

public:
    CProgressBar(QWidget *parent = 0);
    ~CProgressBar();

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor &backgroundColor);

    QColor foregroundColor() const;
    void setForegroundColor(const QColor &foregroundColor);

    QColor textColor() const;
    void setTextColor(const QColor &testColor);

    void setColorAt(qreal progress, const QColor& color);
    void setText(const QString& txt);

protected:
    virtual void paintEvent(QPaintEvent *e);

private:
    QPen   m_pen;
    QFont  m_font;
    QColor m_backgroundColor;
    QColor m_foregroundColor;
    QColor m_textColor;
    QLinearGradient* m_gradient;
    QString m_text;
};

#endif // CPROGRESSBAR_H
