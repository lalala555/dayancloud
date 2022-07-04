#pragma once

#include <QObject>
#include "DownloadListView.h"

class DownloadListItemDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    DownloadListItemDelegate(QObject *parent = 0);
    ~DownloadListItemDelegate();
    void setView(DownloadListView *listView) { m_listView = listView; }
public:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    DownloadListView* m_listView;
};
