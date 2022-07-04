#include "stdafx.h"
#include "JobListProxyModel.h"
#include "HttpCommand/HttpCmdManager.h"

JobListProxyModel::JobListProxyModel(QWidget *parent)
    : QSortFilterProxyModel(parent)
    , m_sortKey(SORT_TASK_ID)
    , m_sortType(SORT_DESC)
{
}

JobListProxyModel::~JobListProxyModel()
{
}

void JobListProxyModel::refreshList()
{
    beginResetModel();
    m_itemCount = 0;
    endResetModel();
}

void JobListProxyModel::setProxySortKey(int key, int sortType)
{
    m_sortKey = key;
    m_sortType = sortType;
    this->sort(0, m_sortType == SORT_ASCE ? Qt::AscendingOrder : Qt::DescendingOrder);
    refreshList();
}

void JobListProxyModel::resetProxySort()
{
    this->setProxySortKey(m_sortKey, m_sortType);
}

bool JobListProxyModel::lessThan(const QModelIndex & source_left, const QModelIndex & source_right) const
{
    if (!source_left.isValid() || !source_right.isValid())
        return false;

    QVariant leftData = sourceModel()->data(source_left, Qt::UserRole);
    QVariant rightData = sourceModel()->data(source_right, Qt::UserRole);

    if (m_sortKey == SORT_TASK_ID) {
        if (leftData.canConvert<JobState*>() && rightData.canConvert<JobState*>()) {
            JobState* leftState = leftData.value<JobState*>();
            JobState* rightState = rightData.value<JobState*>();
            return leftState->getTaskId() < rightState->getTaskId();
        }
        return false;
    
    } else if (m_sortKey == SORT_TIME_COST) {
        if (leftData.canConvert<JobState*>() && rightData.canConvert<JobState*>()) {
            JobState* leftState = leftData.value<JobState*>();
            JobState* rightState = rightData.value<JobState*>();
            return leftState->getRenderTimeCost() < rightState->getRenderTimeCost();
        }
        return false;
    } else if (m_sortKey == SORT_TASK_FEE) {
        if (leftData.canConvert<JobState*>() && rightData.canConvert<JobState*>()) {
            JobState* leftState = leftData.value<JobState*>();
            JobState* rightState = rightData.value<JobState*>();
            return leftState->getTotalConsume() < rightState->getTotalConsume();
        }
        return false;
    } else if (m_sortKey == SORT_SUBMIT_TIME) {
        if (leftData.canConvert<JobState*>() && rightData.canConvert<JobState*>()) {
            JobState* leftState = leftData.value<JobState*>();
            JobState* rightState = rightData.value<JobState*>();
            return leftState->getSubmitDate() < rightState->getSubmitDate();
        }
        return false;
    } else if (m_sortKey == SORT_FINISHED_TIME) {
        if (leftData.canConvert<JobState*>() && rightData.canConvert<JobState*>()) {
            JobState* leftState = leftData.value<JobState*>();
            JobState* rightState = rightData.value<JobState*>();
            return leftState->getCompelteDate() < rightState->getCompelteDate();
        }
        return false;
    }

    return QSortFilterProxyModel::lessThan(source_left, source_right);
}

int JobListProxyModel::stateToShowType(int state) const
{
    int showType = -1;
    switch (state) {
    case enTransUploading:
        showType = TYPE_SHOW_UPLOADING;
        break;
    case enUploadUnfinished:
        showType = TYPE_SHOW_UPLOADUNFINIFSHED;
        break;
    case enUploadStop:
        showType = TYPE_SHOW_UPLOADSTOPED;
        break;
    case enUploadQueued:
    case enUploadRetrying:
    case enUploadWaiting:
        showType = TYPE_SHOW_UPLOADWAITING;
        break;
    case enUploadError:
        showType = TYPE_SHOW_UPLOADFAIL;
        break;
    case enUploadCheckCompleted:
    case enUploadCompleted:
        break;
    case enTransDownloading:
        showType = TYPE_SHOW_DOWNLOADING;
        break;
    case enDownloadCompleted:
        showType = TYPE_SHOW_DOWNLOADED;
        break;
    default:
        break;
    }
    return showType;
}

bool JobListProxyModel::filterAcceptsRow(int source_row, const QModelIndex & source_parent) const
{
    QString searchWord = HttpCmdManager::getInstance()->filterContent.searchWord;

    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
    JobState *job = sourceModel()->data(index).value<JobState *>();
    if (job == NULL) return false;

    // 先检查搜索内容是否包含, 不包含直接返回false
    if (!searchWord.isEmpty()) {
        bool exist = false;
        exist = QString::number(job->getTaskId()).contains(searchWord.toLower()) ||
                job->getTaskIdAlias().toLower().contains(searchWord.toLower()) ||
                job->getScene().toLower().contains(searchWord.toLower()) || 
                job->getProjectName().toLower().contains(searchWord.toLower());
        if (exist == false) return false;
    }

    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

bool JobListProxyModel::dateInRange(const QString& srcDate) const
{
    int type = HttpCmdManager::getInstance()->filterContent.filterType;
    QString content = HttpCmdManager::getInstance()->filterContent.content;

    if (type != TIME_STATE_FILTER)
        return true;

    QString subTime = srcDate;
    QDateTime subDate = QDateTime::fromString(subTime, "yyyy-MM-dd hh:mm:ss");

    QStringList timeRound = content.split("::");
    QDateTime startDate = QDateTime::fromString(timeRound.at(0), "yyyy-MM-dd hh:mm:ss");
    QDateTime endDate = QDateTime::fromString(timeRound.at(timeRound.size() - 1), "yyyy-MM-dd hh:mm:ss");

    if (subDate.toTime_t() >= startDate.toTime_t()
        && subDate.toTime_t() <= endDate.toTime_t()) {
        return true;
    }
    return false;
}

bool JobListProxyModel::hasChildren(const QModelIndex & parent) const
{
    const QModelIndex sourceIndex = mapToSource(parent);
    return sourceModel()->hasChildren(sourceIndex);
}

#if 0
bool JobListProxyModel::canFetchMore(const QModelIndex & parent) const
{
    if (!parent.isValid()) return false;
    
    if (m_itemCount < sourceModel()->rowCount()) {
        return true;
    }
    return false;
}

void JobListProxyModel::fetchMore(const QModelIndex & parent)
{
    if (!parent.isValid()) return;
    
    int remainder = sourceModel()->rowCount() - m_itemCount;
    int itemsToFetch = qMin(25, remainder);
    
    beginInsertRows(QModelIndex(), m_itemCount, m_itemCount + itemsToFetch - 1);
    
    m_itemCount += itemsToFetch;
    
    endInsertRows();
    
    m_currItemCount = m_itemCount < m_currItemCount ? m_currItemCount : m_itemCount;
    if (m_currItemCount > sourceModel()->rowCount())
    m_currItemCount = itemsToFetch;
}

int JobListProxyModel::rowCount(const QModelIndex & parent) const
{
    return m_currItemCount;
}
#endif

void JobListProxyModel::updateRowData(const QModelIndex& begin, const QModelIndex& end)
{
    emit dataChanged(begin, end);
}