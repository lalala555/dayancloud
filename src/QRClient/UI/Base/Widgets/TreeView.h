/***********************************************************************
* Module:  TreeView.h
* Author:  hqr
* Created: 2018/12/03 11:07:58
* Modifier: hqr
* Modified: 2018/12/03 11:07:58
* Purpose: Declaration of the class
***********************************************************************/
#pragma once
#ifndef CTREEVIEW_H
#define CTREEVIEW_H

#include <QTreeView>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QShowEvent>
#include <QResizeEvent>
#include <QSortFilterProxyModel>
#include "QProgressIndicator.h"

class CTreeView : public QTreeView
{
    Q_OBJECT

public:
    CTreeView(QWidget *parent);
    ~CTreeView();

    void showLoading(bool bShow);
    void setEmptyHint(const QString& hint);

    int getSelectedCount();
    void setFilterRegExp(const QString & pattern);

    QRect headerSectionRect(int logicIndex);
    int getScrollBarCurValue();
protected:
    virtual void setHorizontalHeaders(const QStringList &headers) = 0;

protected:
    virtual void paintEvent(QPaintEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void showEvent(QShowEvent *e);
    virtual void resizeEvent(QResizeEvent *e);
    virtual void keyPressEvent(QKeyEvent* e);
    virtual void enterEvent(QEvent *e);
    virtual void leaveEvent(QEvent *e);

protected:
    QString m_emptyHint;
    bool m_isShowLoading;
    QProgressIndicator* m_busyIcon;

    QSortFilterProxyModel* m_proxyModel;
};

#endif // CTREEVIEW_H
