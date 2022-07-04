#pragma once

#include <QObject>
#include "JobListView.h"

class JobListItemDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    JobListItemDelegate(QObject *parent = 0);
    ~JobListItemDelegate();
    void setView(JobListView *listView) { m_listView = listView; }
public:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:
    void menuRequested(const QPoint &pos);

private:
    JobListView *m_listView;
};
