/***********************************************************************
* Module:  ToastWidget.h
* Author:  yanglong
* Created: 2018/11/02 11:03:11
* Modifier: hqr
* Modified: 2018/11/02 11:03:11
* Purpose: Declaration of the class
***********************************************************************/
#ifndef TOASTWIDGET_H
#define TOASTWIDGET_H

#include <QWidget>
#include <QGraphicsOpacityEffect>

class ToastWidget : public QWidget
{
    Q_OBJECT

public:
    ToastWidget(QWidget *parent, const QString& msg, const QColor& color = QColor("#555555"), int displayTime = 2000);
    ~ToastWidget();

private slots:
    void hideToast();

private:
    QGraphicsOpacityEffect* m_widgetOpacity;
};

class TipsToastWidget : public QWidget
{
    Q_OBJECT

public:
    TipsToastWidget(QWidget *parent, const QString& msg, int fontSize = 16, const QColor& color = QColor("#555555"));
    ~TipsToastWidget();

public slots:
    void hideToast();
    void updateMessage(const QString& msg);

private:
    QGraphicsOpacityEffect* m_widgetOpacity;
    QLabel* m_labelMsg;
};

#endif // TOASTWIDGET_H
