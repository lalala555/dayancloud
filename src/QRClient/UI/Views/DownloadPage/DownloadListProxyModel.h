#pragma once

#include <QObject>
#include <QSortFilterProxyModel>

class DownloadListProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    DownloadListProxyModel(QWidget *parent = 0);
    ~DownloadListProxyModel();
    void refreshList();
    void updateRowData(const QModelIndex& begin, const QModelIndex& end);

protected:
    virtual bool lessThan(const QModelIndex & source_left, const QModelIndex & source_right) const;
    virtual bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const;
    virtual bool hasChildren(const QModelIndex &parent) const;
};
