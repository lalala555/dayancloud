/***********************************************************************
* Module:  TabWidget.h
* Author:  hqr
* Created: 2018/11/20 10:18:03
* Modifier: hqr
* Modified: 2018/11/20 10:18:03
* Purpose: Declaration of the class
***********************************************************************/
#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabWidget>
#include <QLabel>

class BadgeLabel : public QLabel
{
    Q_OBJECT

public:
    BadgeLabel(QWidget* parent = 0);
    void setNumber(const QString& number);
};

class CTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    CTabWidget(QWidget *parent = 0);
    ~CTabWidget();

    int addTab(QWidget * page, const QString & label);
    void setTabTextNumber(int index, const QString& text, int number);
    void setBadgeNumber(int index, int number);
    void setNotify(int index, bool notify);
private:

private:
    QTabBar* m_tabBar;
};

#endif // TABWIDGET_H
