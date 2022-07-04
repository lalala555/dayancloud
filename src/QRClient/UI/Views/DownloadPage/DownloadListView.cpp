#include "stdafx.h"
#include "DownloadListView.h"
#include "DownloadListModel.h"
#include "DownloadListItemDelegate.h"
#include "DownloadListProxyModel.h"

DownloadListView::DownloadListView(QWidget *parent)
    : QListView(parent)
    , m_downloadListModel(NULL)
{
    m_busyIcon = new QProgressIndicator(this);
    m_busyIcon->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_busyIcon->show();
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setAttribute(Qt::WA_TranslucentBackground, true);

    m_downloadListModel = new DownloadListModel(this);
    m_proxyModel = new DownloadListProxyModel(this);
    m_proxyModel->setSourceModel(m_downloadListModel);
    m_listDelegate = new DownloadListItemDelegate();

    this->setModel(m_proxyModel);
    this->setItemDelegate(m_listDelegate);
    m_listDelegate->setView(this);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setUniformItemSizes(true);
}

DownloadListView::~DownloadListView()
{
}

void DownloadListView::addState(CDownloadState * pState)
{
}

void DownloadListView::addStates(const QList<CDownloadState*>& jobs)
{
}

void DownloadListView::removeState(CDownloadState * pState)
{
    m_downloadListModel->removeState(pState);
}

void DownloadListView::setRootItem(CDownloadState * pState)
{
    m_downloadListModel->setRootItem(pState);
}

void DownloadListView::showEvent(QShowEvent *e)
{
    QListView::showEvent(e);
}

void DownloadListView::resizeEvent(QResizeEvent *e)
{
    QListView::resizeEvent(e);

    int sx = (this->width() - m_busyIcon->width()) / 2;
    int sy = (this->height() - m_busyIcon->height()) / 2;
    m_busyIcon->move(sx, sy);
}

void DownloadListView::showLoading(bool bShow)
{
    if (bShow) {
        m_busyIcon->show();
        m_busyIcon->startAnimation();
    } else {
        m_busyIcon->stopAnimation();
        m_busyIcon->hide();
    }
}

void DownloadListView::refreshModel()
{
    // m_downloadListModel->refreshModel();
    m_proxyModel->sort(0);
    m_proxyModel->refreshList();
}

void DownloadListView::updateRow(int row)
{
    if (m_downloadListModel->hasIndex(row, 0)) {
        QModelIndex begin = m_downloadListModel->index(row);
        QModelIndex end = m_downloadListModel->index(row);
        if (begin.isValid() && end.isValid()) {
            m_proxyModel->updateRowData(begin, end);
            // m_proxyModel->invalidate();
        }
    }
    else {
        // m_proxyModel->invalidate();
    }
}