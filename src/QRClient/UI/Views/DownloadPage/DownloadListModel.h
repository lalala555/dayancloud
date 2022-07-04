#pragma once

#include <QObject>
#include "DownloadState.h"

class DownloadListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    DownloadListModel(QObject *parent = 0);
    ~DownloadListModel();
    QModelIndex addState(CDownloadState* pState);
    QModelIndexList addStates(const QList<CDownloadState*>& jobs);
    void removeState(CDownloadState* pState);
    void setRootItem(CDownloadState* pState);
    void removeAll();
    void refreshModel();
    qint32 getRowCount();
    QList<CDownloadState *> getModelDatas();
    QModelIndex indexOfJobstate(CDownloadState* job);
    QModelIndexList getModelIndexList();

protected:
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &index) const;
public:
    virtual QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const;

private:
    QList<CDownloadState*> m_jobs;
    int m_itemCount;
    int m_currItemCount;
    QModelIndexList m_indexList;
    CDownloadState* m_rootItem;
};
