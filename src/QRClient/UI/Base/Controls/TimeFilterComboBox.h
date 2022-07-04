/***********************************************************************
* Module:  TimeFilterComboBox.h
* Author:  yanglong
* Created: 2018/01/02 9:44:28
* Modifier: hqr
* Modified: 2018/01/02 9:44:28
* Purpose: Declaration of the class
***********************************************************************/
#ifndef TIMEFILTERCOMBOBOX_H
#define TIMEFILTERCOMBOBOX_H

#include <QComboBox>
#include <QStyledItemDelegate>

class TimeFilterDelegate;

class TimeFilterComboBox : public QComboBox
{
    Q_OBJECT

public:
    TimeFilterComboBox(QWidget *parent);
    ~TimeFilterComboBox();

protected:
    virtual void showPopup();
    virtual bool eventFilter(QObject *o, QEvent *e);

private:
    TimeFilterDelegate *m_filterDelegate;
};

class TimeFilterDelegate: public QStyledItemDelegate
{
    Q_OBJECT
public:
    TimeFilterDelegate(QObject* parent = 0);
    ~TimeFilterDelegate(void) {};
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
public slots:
    void currentIndex(int index);
private:
    int m_curIndex;
};

#endif // TIMEFILTERCOMBOBOX_H
