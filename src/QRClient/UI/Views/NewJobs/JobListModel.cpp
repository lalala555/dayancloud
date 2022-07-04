#include "stdafx.h"
#include "JobListModel.h"

JobListModel::JobListModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_itemCount(0)
    , m_currItemCount(0)
    , m_rootItem(NULL)
{
}

JobListModel::~JobListModel()
{
}

QModelIndex JobListModel::addState(JobState * pState)
{
    if (pState == 0) return QModelIndex();
    if (m_jobs.contains(pState)) {
        return indexOfJobstate(pState);
    } 
    
    m_jobs.append(pState);
    int row = m_jobs.count() - 1;

    //QModelIndex index = this->createIndex(row, 0);
    //m_indexList.append(index);
    return QModelIndex();
}

QModelIndexList JobListModel::addStates(const QList<JobState*>& jobs)
{
#if 0
    int row = this->rowCount();
    m_jobs.append(jobs);
    this->insertRows(row, jobs.count());
#else
    QModelIndexList indexs;
    foreach(JobState* job, jobs) {
        indexs.append(addState(job));
    }
    return indexs;
#endif
}

void JobListModel::setRootItem(JobState* pState)
{
    m_rootItem = pState;
}

void JobListModel::removeState(JobState * pState)
{
    if (pState == 0) return;
    m_jobs.removeOne(pState);
    m_rootItem->removeChildState(pState);
}

void JobListModel::removeAll()
{
    // m_jobs.clear();
    m_indexList.clear();
}

void JobListModel::refreshModel()
{
    beginResetModel();
    endResetModel();
}

QList<JobState*> JobListModel::getModelDatas()
{
    return m_jobs;
}

qint32 JobListModel::getRowCount()
{
    return m_jobs.count();
}

QModelIndexList JobListModel::getModelIndexList()
{
    return persistentIndexList();
}

Qt::ItemFlags JobListModel::flags(const QModelIndex & index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant JobListModel::data(const QModelIndex & index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int col = index.column();
    int row = index.row();
    // JobState *jobState = (row >= m_jobs.count() ? NULL : m_jobs.at(row));
    JobState *jobState = static_cast<JobState*>(index.internalPointer());
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

int JobListModel::rowCount(const QModelIndex & parent) const
{
#if 1
    if (m_rootItem == NULL)
        return 0;

    JobState *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<JobState*>(parent.internalPointer());

    return parentItem->childCount();
#else
    return m_jobs.count();
#endif 
}

int JobListModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QModelIndex JobListModel::index(int row, int column, const QModelIndex &parent) const
{
#if 1
    if (!hasIndex(row, column, parent) || m_rootItem == NULL)
        return QModelIndex();

    JobState *parentItem = NULL;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<JobState*>(parent.internalPointer());

    JobState *childItem = parentItem->childItem(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
#else
    return QAbstractListModel::index(row, column, parent);
#endif
 
}

QModelIndex JobListModel::parent(const QModelIndex &index) const
{
    if (!index.isValid() || m_rootItem == NULL)
        return QModelIndex();

    JobState *childItem = static_cast<JobState*>(index.internalPointer());
    JobState *parentItem = childItem->getParentItem();

    if (parentItem == m_rootItem || parentItem == NULL)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

QModelIndex JobListModel::indexOfJobstate(JobState* job)
{
    int row = m_jobs.indexOf(job);
    if (row < 0 || row > m_jobs.size()) return QModelIndex();
    m_jobs[row] = job;
    QModelIndex index = this->index(row);
    return index;
}
