#include "stdafx.h"
#include "JobListWidget.h"
#include "TaskItemWidget.h"
#include "JobListModel.h"
#include "JobListItemDelegate.h"
#include "TaskItemWidget.h"
#include "JobListProxyModel.h"
#include <QListWidgetItem>

JobListWidget::JobListWidget(QWidget *parent)
    : QListWidget(parent)
{
    m_jobListModel = new JobListModel(this);
    m_listDelegate = new JobListItemDelegate();
    m_proxyModel = new JobListProxyModel(this);
    m_proxyModel->setSourceModel(m_jobListModel);

    // this->setModel(new QStandardItemModel());
    this->setItemDelegate(m_listDelegate);
    // m_listDelegate->setView(this);
    this->setContextMenuPolicy(Qt::CustomContextMenu);

    // ¹ö¶¯¼ÓÔØ
    m_vscrollBar = verticalScrollBar();
    m_vscrollBar->setSingleStep(12);
    this->setAutoScroll(true);
    connect(m_vscrollBar, SIGNAL(valueChanged(int)), this, SLOT(onSliderChanged(int)));

    // connect(m_listDelegate, &JobListItemDelegate::downloadSignal, this, &JobListView::downloadSignal);
    // connect(m_listDelegate, &JobListItemDelegate::openFileDlgSignal, this, &JobListView::openFileDlgSignal);

    // connect(this, &JobListView::clicked, this, &JobListView::onItemClicked);
}

JobListWidget::~JobListWidget()
{
}

void JobListWidget::addJobState(JobState* job, TaskItemWidget* widget)
{
    if (job == NULL) return;
    if (widget == NULL) return;
    QListWidgetItem* item = new QListWidgetItem(this);
    widget->initView(job);
    connect(widget, &TaskItemWidget::downloadSignal, this, &JobListWidget::downloadSignal);
    connect(widget, &TaskItemWidget::openFileDlgSignal, this, &JobListWidget::openFileDlgSignal);

    item->setSizeHint(widget->size());
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    this->setItemWidget(item, widget);
    this->addItem(item);
}

void JobListWidget::removeState(JobState * pState)
{
}

void JobListWidget::setRootItem(JobState * pState)
{
    m_jobListModel->setRootItem(pState);
}

QList<JobState*> JobListWidget::getSelectTaskInfos()
{
    return QList<JobState*>();
}

JobState * JobListWidget::getSelectTaskInfo()
{
    return nullptr;
}

void JobListWidget::updateView()
{
    m_proxyModel->refreshList();
}

void JobListWidget::clearAllItems()
{
}

void JobListWidget::setTaskItem(const QModelIndex & index, TaskItemWidget * widget)
{
}

TaskItemWidget * JobListWidget::getTaskItem(const QModelIndex & index) const
{
    return nullptr;
}

void JobListWidget::setProxySortKey(int key, int sortType)
{
    m_proxyModel->setProxySortKey(key, sortType);
}

void JobListWidget::setProxyFilterKey(int key)
{
    m_proxyModel->setFilterKeyColumn(key);
}

void JobListWidget::updateMainUiRenderBtnState()
{
}

void JobListWidget::paintEvent(QPaintEvent *e)
{
    QListWidget::paintEvent(e);
}

void JobListWidget::onSliderChanged(int p)
{
    if (p > 0 && p == m_vscrollBar->maximum()) {
        qDebug() << "You reached the bottom of the vertical scroll bar!";
        emit reachedBottom();
    } else if (p == m_vscrollBar->minimum()) {
        qDebug() << "You reached the top of the vertical scroll bar!";
        //emit reachedTop();
    }
}

void JobListWidget::onItemClicked(const QModelIndex&)
{

}
