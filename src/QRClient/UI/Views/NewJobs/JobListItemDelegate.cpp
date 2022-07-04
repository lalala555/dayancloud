#include "stdafx.h"
#include "JobListItemDelegate.h"
#include "TaskItemWidget.h"

JobListItemDelegate::JobListItemDelegate(QObject *parent)
    : QItemDelegate(parent)
    , m_listView(NULL)
{
}

JobListItemDelegate::~JobListItemDelegate()
{
}

void JobListItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QItemDelegate::paint(painter, option, index);

    if (index.isValid()) {
        JobState *job = index.model()->data(index, Qt::UserRole).value<JobState*>();
        if (m_listView != NULL && job != NULL) {
            TaskItemWidget* widget = static_cast<TaskItemWidget*>(m_listView->indexWidget(index));
            if (widget != NULL) {
                widget->updateView(job);
                if (widget->isHidden()) {
                    widget->show();
                }     
            } else {
                widget = new TaskItemWidget(job, m_listView);
                // connect(m_listView, &JobListView::viewResize, widget, &TaskItemWidget::onViewResize);    
                connect(widget, &TaskItemWidget::itemClicked, m_listView, &JobListView::setCurrentIndex);
                m_listView->setIndexWidget(index, widget);
                widget->setModelIndex(index);
                widget->hide();
            }
        } 
    } 
}

QSize JobListItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)
    return QSize(option.rect.width(), 100);
}