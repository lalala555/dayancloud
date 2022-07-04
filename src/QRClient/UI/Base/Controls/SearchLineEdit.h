/***********************************************************************
* Module:  SearchLineEdit.h
* Author:  hqr
* Created: 2020/04/28 18:26:00
* Modifier: hqr
* Modified: 2020/04/28 18:26:00
* Purpose: Declaration of the class
***********************************************************************/
#ifndef SEARCHLINEEDIT_H
#define SEARCHLINEEDIT_H

#include <QtWidgets>
#include <QLineEdit>
#include "LineEdit.h"
#include "BasePopup.h"

class SearchTooltip : public QWidget
{
public:
    SearchTooltip(QWidget* parent);
    ~SearchTooltip();

    void setToolTip(const QString & text);
    QString tooltip();

protected:
    virtual void paintEvent(QPaintEvent *p);
  
private:
    QLabel* m_tooltipLabel;
    QString m_tooltipText;
};

class SearchLineEdit : public LineEdit
{
    Q_OBJECT

public:
    SearchLineEdit(QWidget *parent = 0);
    ~SearchLineEdit();
    
    void setToolTip(const QString & text);

protected:
    virtual void showEvent(QShowEvent * event);
    virtual void enterEvent(QEvent * event);
    virtual void leaveEvent(QEvent * event);
    virtual void focusOutEvent(QFocusEvent *event);

signals:
    void lostFocus();

private:
    SearchTooltip* m_tooltip;
};

#endif // SEARCHLINEEDIT_H
