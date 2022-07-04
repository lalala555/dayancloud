#pragma once

#include <QObject>

class JobListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    JobListModel(QObject *parent = 0);
    ~JobListModel();

    QModelIndex addState(JobState* pState);
    QModelIndexList addStates(const QList<JobState*>& jobs);
    void removeState(JobState* pState);
    void setRootItem(JobState* pState);
    void removeAll();
    void refreshModel();
    qint32 getRowCount();
    QList<JobState *> getModelDatas();
    QModelIndex indexOfJobstate(JobState* job);
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
    // QMap<qint64, JobState*> m_jobs;
    QList<JobState*> m_jobs;
    int m_itemCount;
    int m_currItemCount;
    QModelIndexList m_indexList;
    JobState* m_rootItem;
};
