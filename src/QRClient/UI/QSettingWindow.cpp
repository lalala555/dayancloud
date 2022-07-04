#include "stdafx.h"
#include "QSettingWindow.h"
#include "MainWindow.h"
#include "SettingPage/CommonSetting.h"
#include "SettingPage/NetSetting.h"

QSettingWindow::QSettingWindow(QWidget *parent)
    : Dialog(parent)
    , m_signFlag(false)
{
    ui.setupUi(this);
    ui.label_logo->setPixmap(QPixmap(LOGO_ICON));
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowStaysOnTopHint);

    setupEventListeners();

    QGlobalHelp::setDefaultDisabled(this);

    ui.scrollArea->hide();
}

QSettingWindow::~QSettingWindow()
{
}

void QSettingWindow::initUI()
{
    m_commonSetting = new CommonSetting(this);
    m_netSetting = new NetSetting(this);

#if USE_SCROLL_PAGE
    QVBoxLayout* content = (QVBoxLayout*)ui.scrollAreaWidgetContents->layout();
    content->addWidget(m_commonSetting);
    content->addWidget(m_netSetting);

    m_commonSetting->setAccessibleDescription(QString::number(0));
    m_netSetting->setAccessibleDescription(QString::number(1));

    // 获取监听事件
    connect(m_commonSetting, SIGNAL(noticeWidgetOpt()), this, SLOT(onPartWidgetOpted()));
    connect(m_netSetting, SIGNAL(noticeWidgetOpt()), this, SLOT(onPartWidgetOpted()));
#else
    QVBoxLayout* content = (QVBoxLayout*)ui.pageCommon->layout();
    content->addWidget(m_commonSetting);

    content = (QVBoxLayout*)ui.pageNetwork->layout();
    content->addWidget(m_netSetting);

    m_commonSetting->setAccessibleDescription(QString::number(0));
    m_netSetting->setAccessibleDescription(QString::number(1));
#endif

    ui.listWidget_base->setCurrentRow(0);
}

void QSettingWindow::slotCurrentChanged(int index)
{
#if USE_SCROLL_PAGE
    m_signFlag = true;
    QPoint widgetPos;
    this->changeGroupboxStyle(index);
    if (index == 0) {
        widgetPos = m_commonSetting->pos();
        ui.scrollArea->verticalScrollBar()->setValue(widgetPos.y());

    } else if (index == 1) {
        widgetPos = m_netSetting->pos();
        ui.scrollArea->verticalScrollBar()->setValue(widgetPos.y());

    }
#else
    ui.stackedWidget->setCurrentIndex(index);
#endif
}

void QSettingWindow::initData()
{
    m_commonSetting->initData();
    m_netSetting->initDialog();
    ui.listWidget_base->setCurrentRow(0);
}

void QSettingWindow::setupEventListeners()
{
    QScrollBar* scrollBar = ui.scrollArea->verticalScrollBar();
    connect(scrollBar, SIGNAL(valueChanged(int)), this, SLOT(onSlotValueChanged(int)));
    connect(ui.listWidget_base, SIGNAL(currentRowChanged(int)), this, SLOT(slotCurrentChanged(int)));
    connect(&m_guiUpdater, SIGNAL(updateNetworkSignal(int, int)), this, SLOT(slotUpdateNetwork(int, int)));
    connect(&m_guiUpdater, SIGNAL(finishNetworkSignal(int, int)), this, SLOT(slotFinishPingIP(int, int)));
}

void QSettingWindow::changeEvent(QEvent *event)
{
    switch(event->type()) {
    case QEvent::LanguageChange: {
        ui.retranslateUi(this);
    }
    break;
    default:
        Dialog::changeEvent(event);
        break;
    }
}

bool QSettingWindow::Response(int type, intptr_t wParam, intptr_t lParam)
{
    switch(type) {
    case UM_UPDATE_PLUGINLIST:
        m_guiUpdater.showPluginList(wParam, lParam);
        break;
    case UM_UPDATE_CONFIGLIST:
        m_guiUpdater.showConfigList(wParam, lParam);
        break;
    case UM_SET_DELETE_ACTION:
        m_guiUpdater.deleteActionSettingPage(wParam, lParam);
        break;
    case UM_UPDATE_NETWORK:
        m_guiUpdater.updateNetworkData(wParam, lParam);
        break;
    case UM_FINISH_NETWORK:
        m_guiUpdater.finishNetwork(wParam, lParam);
        break;
    case UM_OPERATING_PLUGINCONFIG:
        m_guiUpdater.operatingPluginConfig(wParam, lParam);
        break;
    default:
        break;
    }
    return true;
}

void QSettingWindow::slotUpdateNetwork(int wParam, int lParam)
{
    m_netSetting->updateNetworkData();
}

void QSettingWindow::slotFinishPingIP(int wParam, int lParam)
{
    m_netSetting->finishPingIP();
}

void QSettingWindow::onSlotValueChanged(int v)
{
    QListWidgetItem* commonItem = ui.listWidget_base->item(0);
    QListWidgetItem* netItem    = ui.listWidget_base->item(1);

    if(!m_signFlag) {
        if(commonItem && netItem) {
            if(!m_commonSetting->visibleRegion().isEmpty()) {
                commonItem->setSelected(true);
                this->changeGroupboxStyle(0);
                return;
            } else {
                commonItem->setSelected(false);
                m_commonSetting->setSelectStyle(false);
            }

            if(!m_netSetting->visibleRegion().isEmpty()) {
                netItem->setSelected(true);
                this->changeGroupboxStyle(1);
                return;
            } else {
                netItem->setSelected(false);
                m_netSetting->setSelectStyle(false);
            }
        }
    }
    m_signFlag = false;
}

void QSettingWindow::changeGroupboxStyle(int index)
{
    if(index == 0) {
        m_commonSetting->setSelectStyle(true);
        m_netSetting->setSelectStyle(false);
    } else if(index == 1) {
        m_commonSetting->setSelectStyle(false);
        m_netSetting->setSelectStyle(true);
    }
}

void QSettingWindow::onPartWidgetOpted()
{
    QObject *obj = sender();
    if(obj == NULL)
        return;
    QWidget* w = qobject_cast<QWidget*>(obj);
    int accessId = w->accessibleDescription().toUInt();
    ui.listWidget_base->setCurrentRow(accessId);
    this->slotCurrentChanged(accessId);
}

void QSettingWindow::ShowWnd()
{
    ui.listWidget_base->setCurrentRow(0);
    this->show();
}

void QSettingWindow::switchToProjectSetPage(int cgType)
{
    if (cgType == CG_ID_3DS_MAX || cgType == CG_ID_MAYA) {
        ui.listWidget_base->setCurrentRow(PAGE_PROJECT);
        //m_projectSettingPage->switchToSoftware(cgType);
    } else if (cgType == PAGE_NETWORK) {
        ui.listWidget_base->setCurrentRow(PAGE_NETWORK);
    }
}

void QSettingWindow::showEvent(QShowEvent *e)
{   
    Dialog::showEvent(e);
}
