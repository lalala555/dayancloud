#include "stdafx.h"
#include "DownloadListItemDelegate.h"
#include "DownloadViewItem.h"
#include "DownloadViewPage.h"

DownloadListItemDelegate::DownloadListItemDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}

DownloadListItemDelegate::~DownloadListItemDelegate()
{
}

void DownloadListItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    QItemDelegate::paint(painter, option, index);

    if (index.isValid()) {
        CDownloadState *job = index.model()->data(index, Qt::UserRole).value<CDownloadState*>();
        if (m_listView != NULL && job != NULL) {
            DownloadViewItem* widget = static_cast<DownloadViewItem*>(m_listView->indexWidget(index));
            if (widget != NULL) {
                widget->updateView(job);
                if (widget->isHidden()) {
                    widget->show();
                }
            } else {
                widget = new DownloadViewItem(job, m_listView);
                // connect(m_listView, &JobListView::viewResize, widget, &TaskItemWidget::onViewResize);
                connect(widget, &DownloadViewItem::deleteItem, m_listView, &DownloadListView::deleteItem);
                connect(widget, &DownloadViewItem::taskOpition, m_listView, &DownloadListView::taskOpition);
                m_listView->setIndexWidget(index, widget);
                widget->hide();
            }
        }
    }
}

QSize DownloadListItemDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    Q_UNUSED(index)
    return QSize(option.rect.width(), 60);
}
