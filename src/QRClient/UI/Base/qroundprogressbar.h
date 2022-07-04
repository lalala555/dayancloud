/***********************************************************************
* Module:  qroundprogressbar.h
* Author:  hqr
* Modified: 2021/06/11 14:42:34
* Purpose: Declaration of the class
***********************************************************************/

#ifndef QROUNDPROGRESSBAR_H
#define QROUNDPROGRESSBAR_H

#include <QWidget>

class QRoundProgressBar : public QWidget
{
    Q_OBJECT

public:
    explicit QRoundProgressBar(QWidget *parent = 0) {};
    ~QRoundProgressBar() {};

protected:
    void paintEvent(QPaintEvent *);
    void drawBackground(QPainter *painter);
    void drawOuterCircle(QPainter *painter);
    void drawInnerCircle(QPainter *painter);
    void drawValue(QPainter *painter);

public Q_SLOTS:
    void setValue(double value);

private:
    const int radius = 100;
    double minValue = 0;
    double maxValue = 100;
    double currentValue = 0;

    int startAngle = 90;
    int ringWidth = 15;

    QColor bgColor = ("#0d0c17");
    QColor textColor = ("#13cb75");
    QColor ringColor = ("#13cb75");
    QColor ringBgColor = ("#1f1f27");
    QColor circleColor = ("#0d0c17");

Q_SIGNALS:
    void valueChanged(int value);
};

#endif // QROUNDPROGRESSBAR_H