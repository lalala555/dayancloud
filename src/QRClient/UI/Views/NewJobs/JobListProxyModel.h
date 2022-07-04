#pragma once

#include <QObject>

class JobListProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    JobListProxyModel(QWidget *parent = 0);
    ~JobListProxyModel();
    void refreshList();
    void setProxySortKey(int key, int sortType);
    bool dateInRange(const QString& srcDate) const;
    int stateToShowType(int state) const;
    void resetProxySort();
    void updateRowData(const QModelIndex& begin, const QModelIndex& end);

protected:
    virtual bool lessThan(const QModelIndex & source_left, const QModelIndex & source_right) const;
    virtual bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const;
    virtual bool hasChildren(const QModelIndex &parent) const;
#if 0
    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
#endif

private:
    qint64 m_itemCount;
    qint64 m_currItemCount;
    int m_sortKey;
    int m_sortType;
};
