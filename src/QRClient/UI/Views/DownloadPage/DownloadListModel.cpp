#include "stdafx.h"
#include "DownloadListModel.h"

DownloadListModel::DownloadListModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_itemCount(0)
    , m_currItemCount(0)
    , m_rootItem(NULL)
{
}

DownloadListModel::~DownloadListModel()
{
}

QModelIndex DownloadListModel::addState(CDownloadState * pState)
{
    if (pState == 0) return QModelIndex();
    if (m_jobs.contains(pState)) {
        return indexOfJobstate(pState);
    }
    m_jobs.append(pState);
    return QModelIndex();
}

QModelIndexList DownloadListModel::addStates(const QList<CDownloadState*>& jobs)
{
    QModelIndexList indexs;
    foreach(CDownloadState* job, jobs) {
        indexs.append(addState(job));
    }
    return indexs;
}

void DownloadListModel::removeState(CDownloadState * pState)
{
    if (pState == 0) return;
    m_jobs.removeOne(pState);
    m_rootItem->removeChildState(pState);
}

void DownloadListModel::setRootItem(CDownloadState * pState)
{
    m_rootItem = pState;
}

void DownloadListModel::removeAll()
{
    m_indexList.clear();
}

void DownloadListModel::refreshModel()
{
    beginResetModel();
    endResetModel();
}

qint32 DownloadListModel::getRowCount()
{
    return m_jobs.count();
}

QList<CDownloadState*> DownloadListModel::getModelDatas()
{
    return m_jobs;
}

QModelIndex DownloadListModel::indexOfJobstate(CDownloadState * job)
{
    int row = m_jobs.indexOf(job);
    m_jobs[row] = job;
    QModelIndex index = this->index(row);
    return index;
}

QModelIndexList DownloadListModel::getModelIndexList()
{
    return m_indexList;
}

Qt::ItemFlags DownloadListModel::flags(const QModelIndex & index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant DownloadListModel::data(const QModelIndex & index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int col = index.column();
    int row = index.row();
    CDownloadState *jobState = static_cast<CDownloadState*>(index.internalPointer());
    if (jobState == NULL)
        return QVariant();
    jobState->setRowNum(row);

    if ((role == Qt::UserRole || role == Qt::DisplayRole) && col == 0) {
        QVariant v;
        v.setValue(jobState);
        return v;
    }
    return QVariant();
}

int DownloadListModel::rowCount(const QModelIndex & parent) const
{
    if (m_rootItem == NULL)
        return 0;

    CDownloadState *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<CDownloadState*>(parent.internalPointer());

    return parentItem->childCount();
}

int DownloadListModel::columnCount(const QModelIndex & parent) const
{
    return 1;
}

QModelIndex DownloadListModel::parent(const QModelIndex & index) const
{
    if (!index.isValid() || m_rootItem == NULL)
        return QModelIndex();

    CDownloadState *childItem = static_cast<CDownloadState*>(index.internalPointer());
    CDownloadState *parentItem = childItem->getParentItem();

    if (parentItem == m_rootItem || parentItem == NULL)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

QModelIndex DownloadListModel::index(int row, int column, const QModelIndex & parent) const
{
    if (!hasIndex(row, column, parent) || m_rootItem == NULL)
        return QModelIndex();

    CDownloadState *parentItem = NULL;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<CDownloadState*>(parent.internalPointer());

    CDownloadState *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}
