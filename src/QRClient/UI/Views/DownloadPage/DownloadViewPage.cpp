#include "stdafx.h"
#include "DownloadViewPage.h"
#include "DownloadStateMgr.h"
#include "DownloadState.h"
#include "TaskCenterManager.h"

DownloadViewPage::DownloadViewPage(QWidget *parent)
    : LayerWidget(parent, Qt::FramelessWindowHint)
    , m_listView(nullptr)
    , m_currentPage(1)
{
    ui.setupUi(this);
    
    m_width = this->width();
    m_height = this->height();

    // auto shadowEffect = new QGraphicsDropShadowEffect(this);
    // shadowEffect->setOffset(0, 0);
    // shadowEffect->setColor(QColor(0, 0, 0, 51));
    // shadowEffect->setBlurRadius(15);
    // ui.widgetBody->setGraphicsEffect(shadowEffect); 

    connect(ui.widget_page, &PaginationCtrl::pageChanged, this, &DownloadViewPage::onPageChanged);
}

DownloadViewPage::~DownloadViewPage()
{
}

void DownloadViewPage::retranslateUi()
{
}

void DownloadViewPage::initView()
{
    if (m_listView == nullptr) {
        m_listView = ui.listview_download;
        connect(m_listView, &DownloadListView::deleteItem, this, &DownloadViewPage::onDeleteItem);
        connect(m_listView, &DownloadListView::taskOpition, this, &DownloadViewPage::onTaskOpition);
    }  
    m_listView->setRootItem(CDownloadStateMgr::Inst()->resetRootItem());
    CDownloadStateMgr::Inst()->LoadlocalHistory();
    CDownloadStateMgr::Inst()->SetListBox(this);
    updateTasklistUi(true);
    this->refreshView();

    ui.widget_page->setPageInfo(CDownloadStateMgr::Inst()->getTotalDownloadCount());
}

void DownloadViewPage::addTaskState(CDownloadState * downloadstate)
{
    if (downloadstate == nullptr || m_listView == nullptr)
        return;

    CDownloadState *rootItem = CDownloadStateMgr::Inst()->resetRootItem();
    rootItem->addChildDownloadState(downloadstate);
}

bool DownloadViewPage::removeDownloadState(t_taskid dwTaskid)
{
    if (m_listView == nullptr)
        return false;
    CDownloadState *pDownloadState = CDownloadStateMgr::Inst()->LookupTaskstate(dwTaskid);
    m_listView->removeState(pDownloadState);
    return true;
}

void DownloadViewPage::updateTasklistUi(bool bClean)
{
    if (m_listView == nullptr)
        return;

    if (bClean == true) {
        PaginationCtrl* pageCtrl = ui.widget_page;
        int pageNum = pageCtrl->getCurrentPage() - 1;
        pageNum = pageNum < 0 ? 0 : pageNum;
        int rowCount = pageCtrl->getRowCount();

        std::map<t_taskid, CDownloadState*> lists = getDownloadStateList(pageNum, rowCount);
        CDownloadState *rootItem = CDownloadStateMgr::Inst()->resetRootItem();
        rootItem->clearAllChildren();

        auto it = lists.begin();
        while (it != lists.end()) {
            CDownloadState *jobState = it->second;
            if (jobState) {
                jobState->setTransState(jobState->GetTransState());
                jobState->setParentItem(rootItem);
                addTaskState(jobState);
            }
            ++it;
        }

        m_listView->setRootItem(rootItem);
        this->refreshView();

    } else {
        this->refreshView();
    }
}

void DownloadViewPage::refreshView()
{
    m_listView->refreshModel();
}

void DownloadViewPage::updateWidgetGeometry(QObject* obj)
{
    if (!obj)
        return;

    QWidget *parentWidget = static_cast<QWidget*>(obj);
    int x = parentWidget->x() + parentWidget->width() - m_width;
    int y = parentWidget->y() - 45;

    int w_bar = 0;
    int h_bar = 0;

    this->setFixedSize(m_width, parentWidget->height());
    this->move(x, y);

    this->updateGeometry();
}

void DownloadViewPage::resizeEvent(QResizeEvent *event)
{
    LayerWidget::resizeEvent(event);
    updateWidgetGeometry(parentWidget());
}

void DownloadViewPage::showEvent(QShowEvent* event)
{
    LayerWidget::showEvent(event);
    updateWidgetGeometry(parentWidget()); 
}

void DownloadViewPage::paintEvent(QPaintEvent *p)
{
    QStylePainter painter(this);
    //用style画背景 (会使用setstylesheet中的内容)
    QStyleOption opt;
    opt.initFrom(this);
    opt.rect = rect();
    painter.drawPrimitive(QStyle::PE_Widget, opt);

    LayerWidget::paintEvent(p);
}

void DownloadViewPage::onPageChanged(int pageNum, int rowCount)
{
    updateTasklistUi(true);
}

void DownloadViewPage::onDisplayItemCountChanged(int count)
{

}

std::map<t_taskid, CDownloadState*> DownloadViewPage::getDownloadStateList(int pageNum, int rowCount)
{
    std::map<t_taskid, CDownloadState*> lists = CDownloadStateMgr::Inst()->getAllDownloadTask();
    QList<t_taskid> jobIds = CDownloadStateMgr::Inst()->getAllOrderedJobIdList();
    std::map<t_taskid, CDownloadState*> curlist;
    
    if (lists.empty() || jobIds.isEmpty()) {
        return curlist;
    }

    QList<t_taskid> showList = jobIds.mid(pageNum * rowCount, rowCount);
    foreach(t_taskid taskId, showList) {
        auto it = lists.find(taskId);
        if (it == lists.end()) continue;
        curlist.insert(std::make_pair(taskId, it->second));
    }

    return curlist;
}

void DownloadViewPage::onDeleteItem(t_taskid taskId)
{
    CDownloadStateMgr::Inst()->removeDownloadState(taskId);
    CGlobalHelp::Response(UM_CHANGE_STATE_VIEW, VIEW_REFRESH_DOWNLOAD_PAGE_ALL, 0);
}

void DownloadViewPage::onTaskOpition(t_taskid taskId, const QString& type)
{
    if (type == "start") {
        CTaskCenterManager::Inst()->manualStartDownload(taskId, false);
    } else if (type == "stop") {
        CTaskCenterManager::Inst()->manualStopDownload(taskId, true);
    } else if (type == "retry") {
        CTaskCenterManager::Inst()->manualStartDownload(taskId, false);
    }
    this->refreshView();
}

void DownloadViewPage::updateRow(int row)
{
    m_listView->updateRow(row);
}