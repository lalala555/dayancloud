#include "stdafx.h"
#include "JobListView.h"
#include "JobListModel.h"
#include "JobListItemDelegate.h"
#include "TaskItemWidget.h"
#include "JobListProxyModel.h"
#include <QGLWidget>
#include <QGLFormat>

JobListView::JobListView(QWidget *parent)
    : QListView(parent)
{
    m_busyIcon = new QProgressIndicator(this);
    m_busyIcon->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_busyIcon->show();
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    // this->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));

    m_jobListModel = new JobListModel(this);
    m_listDelegate = new JobListItemDelegate();
    m_proxyModel = new JobListProxyModel(this);
    m_proxyModel->setSourceModel(m_jobListModel);

    this->setModel(m_proxyModel);
    this->setItemDelegate(m_listDelegate);
    m_listDelegate->setView(this);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setUniformItemSizes(true);

    // 滚动加载
    m_vscrollBar = verticalScrollBar();
    m_vscrollBar->setSingleStep(12);
    this->setAutoScroll(true);
    connect(m_vscrollBar, SIGNAL(valueChanged(int)), this, SLOT(onSliderChanged(int)));
    connect(this, &JobListView::clicked, this, &JobListView::onItemClicked);
}

JobListView::~JobListView()
{
}

void JobListView::addState(JobState *pState)
{
}

void JobListView::addStates(const QList<JobState*>& jobs)
{
}

void JobListView::setRootItem(JobState *pState)
{
    m_jobListModel->setRootItem(pState);
}

void JobListView::removeState(JobState *pState)
{
    m_jobListModel->removeState(pState);
}

void JobListView::clearAllItems()
{
}

QList<JobState*> JobListView::getSelectTaskInfos()
{
    QList<JobState *> jobList;
    QModelIndexList indexList = selectionModel()->selectedRows();
    foreach(QModelIndex index, indexList) {
        JobState *job = index.model()->data(index, Qt::UserRole).value<JobState*>();
        jobList.push_back(job);
    }
    return jobList;
}

JobState* JobListView::getSelectTaskInfo()
{
    QModelIndex modelIndex = currentIndex();
    if (!modelIndex.isValid()) return 0;

    return model()->data(modelIndex, Qt::UserRole).value<JobState*>();
}

void JobListView::updateView()
{
    m_jobListModel->refreshModel();
    m_proxyModel->resetProxySort();
    updateMainUiRenderBtnState();
}

void JobListView::setProxySortKey(int key, int sortType)
{
    m_proxyModel->setProxySortKey(key, sortType);
}

void JobListView::setProxyFilterKey(int key)
{
    m_proxyModel->setFilterKeyColumn(key);
}

void JobListView::onSliderChanged(int p)
{
    if (p > 0 && p == m_vscrollBar->maximum()) {
        qDebug() << "You reached the bottom of the vertical scroll bar!";
        emit reachedBottom();
    } else if (p == m_vscrollBar->minimum()) {
        qDebug() << "You reached the top of the vertical scroll bar!";
        //emit reachedTop();
    }
}

void JobListView::mousePressEvent(QMouseEvent *e)
{
    QListView::mousePressEvent(e);
}

void JobListView::mouseReleaseEvent(QMouseEvent *e)
{
    QListView::mouseReleaseEvent(e);
}

void JobListView::showEvent(QShowEvent *e)
{
    QListView::showEvent(e);
}

void JobListView::resizeEvent(QResizeEvent *e)
{
    QListView::resizeEvent(e);
    m_proxyModel->resetProxySort();

    int sx = (this->width() - m_busyIcon->width()) / 2;
    int sy = (this->height() - m_busyIcon->height()) / 2;
    m_busyIcon->move(sx, sy);
}

void JobListView::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QListView::paintEvent(e);

    if (this->model()->rowCount() == 0) {
        QListView::paintEvent(e);
        QPainter painter(viewport());
        painter.save();
        QRect rect = painter.viewport();

        int gx = 0;
        int gy = 0;

        if (!m_emptyHint.isEmpty()) {
            //如果是文件
            QFileInfo info(m_emptyHint);
            if (info.isFile()) {
                QPixmap pix(m_emptyHint);

                gx = (rect.width() - pix.width()) / 2;
                gy = (rect.height() - pix.height() - 50) / 2;
                painter.drawPixmap(gx, gy, pix.width(), pix.height(), pix);
                return;
            } else {
                QFont font;
                font.setFamily("Microsoft YaHei");
                font.setBold(true);
                font.setPointSize(15);
                QFontMetrics fm(font);
                QRect fontRect = fm.boundingRect(m_emptyHint);
                painter.setFont(font);

                gx = (rect.width() - fontRect.width()) / 2;
                gy = (rect.height() - fontRect.height() - 50) / 2;
                painter.drawText(gx, gy, fontRect.width(), fontRect.height(), Qt::AlignCenter, m_emptyHint);

                return;
            }
        }
    }   
}

void JobListView::onItemClicked(const QModelIndex& index)
{
    if (!index.isValid()) return;
}

void JobListView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    QModelIndexList selectList = selected.indexes();
    QModelIndexList deselectList = deselected.indexes();
   
    foreach (QModelIndex index, deselectList) {
        if (!index.isValid()) continue;

        TaskItemWidget* widget = (TaskItemWidget*)this->indexWidget(index);
        if (widget) {
            widget->setSelected(false);
        }
    }

    foreach(QModelIndex index, selectList) {
        if (!index.isValid()) continue;

        TaskItemWidget* widget = (TaskItemWidget*)this->indexWidget(index);
        if (widget) {
            widget->setSelected(true);
        }
    }

    updateMainUiRenderBtnState();
}

void JobListView::updateMainUiRenderBtnState()
{
    QList<JobState* > selStates = this->getSelectTaskInfos();

    MainUiRenderBtnState *btnState = new MainUiRenderBtnState();
    int canStartCount = 0;
    int canStopCount = 0;
    int canResubCount = 0;
    int canDelCount = 0;
    int canDownloadCount = 0;
    int canPaymentVoucher = 0;

    foreach(JobState *state, selStates) {
        // 可否使用免单券
        if (state->IsCanUseVoucher()) {
            canPaymentVoucher++;
        }
    }

    // 使用免单券
    if (canPaymentVoucher == 1 && MyConfig.voucherSet.hasEffectiveVoucher()) {
        btnState->exchangeBtn = true;
    }

    CGlobalHelp::Response(UM_UPDATE_MAIN_UI_BTN_STATE, 0, (intptr_t)btnState);
}

void JobListView::showLoading(bool bShow)
{
    if (bShow) {
        m_busyIcon->show();
        m_busyIcon->startAnimation();
    }
    else {
        m_busyIcon->stopAnimation();
        m_busyIcon->hide();
    }
}

void JobListView::setEmptyHint(const QString& hint)
{
    m_emptyHint = hint;
}

void JobListView::updateRow(int row)
{
    if (m_jobListModel->hasIndex(row, 0)) {
        QModelIndex begin = m_jobListModel->index(row);
        QModelIndex end = m_jobListModel->index(row);
        if (begin.isValid() && end.isValid()) {
            m_proxyModel->updateRowData(begin, end);
            // m_proxyModel->invalidate();
        }
    } else {
        // m_proxyModel->invalidate();
    }
}

JobState* JobListView::getCurrentTaskInfo(const QModelIndex &index)
{
    if (!index.isValid()) return 0;

    return model()->data(index, Qt::UserRole).value<JobState*>();
}

void JobListView::setItemSelected(JobState *pState)
{
    if (pState == nullptr) return;

}