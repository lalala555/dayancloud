#include "stdafx.h"
#include "DownloadListProxyModel.h"
#include "HttpCommand/HttpCmdManager.h"
#include "DownloadState.h"

DownloadListProxyModel::DownloadListProxyModel(QWidget *parent)
    : QSortFilterProxyModel(parent)
{
}

DownloadListProxyModel::~DownloadListProxyModel()
{
}

void DownloadListProxyModel::refreshList()
{
    beginResetModel();
    endResetModel();
}

bool DownloadListProxyModel::lessThan(const QModelIndex & source_left, const QModelIndex & source_right) const
{
    if (!source_left.isValid() || !source_right.isValid())
        return false;

    QVariant leftData = sourceModel()->data(source_left, Qt::UserRole);
    QVariant rightData = sourceModel()->data(source_right, Qt::UserRole);

    if (leftData.canConvert<CDownloadState*>() && rightData.canConvert<CDownloadState*>()) {
        CDownloadState* leftState = leftData.value<CDownloadState*>();
        CDownloadState* rightState = rightData.value<CDownloadState*>();
        if (leftState != nullptr && rightState != nullptr) {
            return leftState->getStartDateTime() < rightState->getStartDateTime();
        }       
    }

    return QSortFilterProxyModel::lessThan(source_left, source_right);
}

bool DownloadListProxyModel::filterAcceptsRow(int source_row, const QModelIndex & source_parent) const
{
    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

bool DownloadListProxyModel::hasChildren(const QModelIndex & parent) const
{
    const QModelIndex sourceIndex = mapToSource(parent);
    return sourceModel()->hasChildren(sourceIndex);
}

void DownloadListProxyModel::updateRowData(const QModelIndex& begin, const QModelIndex& end)
{
    emit dataChanged(begin, end);
}