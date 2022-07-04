#include "stdafx.h"
#include "AppInstance.h"
#include "MainWindow.h"
#include "SearchBox.h"
#include "AboutWindow.h"
#include "UserInfoWnd.h"
#include "NewJobs/JobStateMgr.h"
#include "Kernel/TaskCenterManager.h"
#include "PluginConfig/ProjectMgr.h"
#include "NoticePublish/NoticePublishMgr.h"
#include "NoticePublish/NoticePublishWnd.h"
#include "NoticePublish/PromoteNewsWnd.h"
#include "HttpCommand/HttpCmdManager.h"
#include "Common/QDownloadManager.h"
#include "Common/LangTranslator.h"
#include "QSettingWindow.h"
#include "Update/UpdateForm.h"
#include "Update/UpdateNotify.h"
#include "Common/StringUtil.h"
#include "UI/MessageBox.h"
#include "QSettingWindow.h"
#include "Scripts/ScriptDownload.h"
#include "Views/NewJobs/JobListItemDelegate.h"
#include "Views/NewJobs/TaskItemWidget.h"
#include "Views/NewJobs/taskatkmlblock.h"
#include "Views/NewJobs/JobListViewPage.h"
#include "Views/NewJobs/DetailViewPage.h"
#include "UI/Base/Controls/TabBarWidget.h"
#include "UI/Base/Controls/HeaderView.h"
#include "UI/Base/Controls/TableItemDelegate.h"
#include "ThreadJob/LoadPictureInfoThread.h"
#include "CoordEditWindow.h"
#include "PicturesViewer.h"
#include "SRS/srswindow.h"
#include "Base/Controls/BlockInfoWidget.h"
#include "Views/DownloadPage/DownloadViewPage.h"
#include "ThreadJob/LoadConfigDataThread.h"
#include "ThreadJob/LoadProjectDataThread.h"
#include "ThreadJob/LoadAtReportInfoThread.h"
#include "ThreadJob/LoadAtDataInfoThread.h"
#include "Config/LoadCoordSystemConfig.h"
#include "Config/CameraDB.h"
#include "SummaryBox.h"
#include "ThreadJob/LoadBlockFileThread.h"
#include "DownloadPage/DownloadStateMgr.h"
#include "FloatLogPanel.h"
#include "UI/Base/Controls/ATGroupBox.h"
#include "PublishWindow.h"
#include "ResultsShareWindow.h"
#include "NewJobs/TaskCloneWindow.h"
#include "BlockGuideWindow.h"
#include "SurveyWidget.h" // 刺相控点
#include "KrGcp.h"
#include "PublishItemWidget.h"
#include <time.h>
#include <QDate>
#include <sys/timeb.h>
#include <qdatetime.h>
#include <memory>

#include "MessageBoxATBlock.h"


CMainWindow::CMainWindow(QWidget *parent)
    : Window(parent, true)
    , ui(new Ui::CMainWindow)
    , m_settingWindow(0)
    , m_trayIcon(0)
    , m_trayiconMenu(0)
    , m_updateTrigger(UPDATE_TIMER_TRIGGER)
    , m_noticeMgr(0)
    , m_isLogout(false)
    , m_downloadViewPage(nullptr)
    , m_brCornerWidget(nullptr)
    , m_rebuildParamWidget(nullptr)
    , m_projectType(enBlockProject)
	, m_blockCheck(true)
	, m_surveysPoints(nullptr)
	, m_atBlock(false)
	, m_atBlockMerage(true)
{
    CGlobalHelp::g_mainSink = this;
    this->initStyle();
#if !_DEBUG
    ui->btnTest->hide();
#endif

    QTimer::singleShot(5000, this, [=] {
        if (MyConfig.accountSet.authStatus == AUTHSTATUS_AUTH_NONE) {
            // 与网页端未达成一致，目前跳转到个人中心
            // HttpCmdManager::getInstance()->realnameAuthKey(this);

#ifndef FOXRENDERFARM //海外版本不需要实名认证
            onUserAuthRequested();
#endif
        }
    });
	
	// 限制瓦片大小输入范围
	QDoubleValidator *validator = new QDoubleValidator(ui->lineEdit_tile_size_3);
	validator->setRange(50, 150, 2);
	ui->lineEdit_tile_size_3->setValidator(validator);

	ui->lineEdit_x_coord_3->setValidator(new QDoubleValidator(-999999999.999999, 999999999.999999, 6, ui->lineEdit_x_coord_3));
	ui->lineEdit_y_coord_3->setValidator(new QDoubleValidator(-999999999.999999, 999999999.999999, 6, ui->lineEdit_y_coord_3));
	ui->lineEdit_z_coord_3->setValidator(new QDoubleValidator(-999999999.999999, 999999999.999999, 6, ui->lineEdit_z_coord_3));

	ui->lineEdit_x_offset->setValidator(new QDoubleValidator(-999999999.999999, 999999999.999999, 6, ui->lineEdit_x_offset));
	ui->lineEdit_y_offset->setValidator(new QDoubleValidator(-999999999.999999, 999999999.999999, 6, ui->lineEdit_y_offset));
	ui->lineEdit_z_offset->setValidator(new QDoubleValidator(-999999999.999999, 999999999.999999, 6, ui->lineEdit_z_offset));

    ui->btnTest->hide();
    ui->btnAddCtrlPoint2->hide();
}

CMainWindow::~CMainWindow()
{
	delete m_surveysPoints;
    delete m_settingWindow;
    delete m_updateWnd;
    delete m_updateNotify;
    delete m_noticeMgr;
    delete ui;

}

void CMainWindow::init()
{
    this->initUI();
    this->initViews();
    this->initWindow();
    this->initTrayIcon();
    this->retranslateUi();
    this->addEventListeners();
    this->updateComponents();
    this->DownloadAutoUpdateApp();
    this->flushUserInfo();
}

void CMainWindow::initStyle()
{
    QApplication::setQuitOnLastWindowClosed(false);
    this->setAttribute(Qt::WA_DeleteOnClose);
#ifdef Q_OS_WIN
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
#else
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
#endif
    ui->setupUi(this);

    ui->widget_title->installEventFilter(this);
    ui->widget_page->installEventFilter(this);
}

void CMainWindow::initUI()
{
    LOGFMTI("CMainWindow::initUI() in");
#ifdef FOXRENDERFARMSIZE
	this->resize(QSize(1660,900));
	this->setMinimumWidth(1500);
#endif
	this->setWindowIcon(QIcon(APP_ICON));

    ui->btnRestore->hide();
    ui->labelLogo->setPixmap(QPixmap(LOGO_LARGE_ICON));
    ui->widget_select_group->hide();
    ui->widget_range_file->hide();
    // 默认滚动通知栏不显示
    ui->widget_rollNews->hide();
    // 显示项目详情页上一步
    ui->btnBackAssets->show();
    
    ui->progressBar_upload->setStyleSheet(
        "QProgressBar{border: none; text-align: center; background: #0d0c17; border-radius:4px;}"
        "QProgressBar::chunk {background: #675ffe; border-radius:4px;}");

    m_moreMenu      = this->createMoreMenu();
    m_trayiconMenu  = this->createTrayMenu();
    m_csMenu        = this->createCSMenu();
    m_selPicMenu    = this->createSelPicMenu();
    m_settingWindow = new QSettingWindow(this);
    m_updateWnd     = new UpdateForm(this);
    m_updateNotify  = new UpdateNotify(this);
    m_noticeMgr     = new NoticePublishMgr();
    m_busyIcon      = new QProgressIndicator(ui->widget_page);
    m_downloadViewPage = new DownloadViewPage(ui->widget_main);
    m_downloadViewPage->initView();
    m_downloadViewPage->hide();

    m_detailViewPage = new DetailViewPage(ui->widget_main);
    m_detailViewPage->initView();
    m_detailViewPage->hide();

    m_busyIcon->setAttribute(Qt::WA_TransparentForMouseEvents);

    ui->tableWidget_pic_group->viewport()->setMouseTracking(true);
    ui->tableWidget_pic_group->setItemDelegate(new TableColorItemDelegate(this));

    ui->tableWidget_camera->viewport()->setMouseTracking(true);
    ui->tableWidget_camera->setItemDelegate(new TableColorItemDelegate(this));

    ui->tableWidget_pictures->viewport()->setMouseTracking(true);
    ui->tableWidget_pictures->setItemDelegate(new TableColorItemDelegate(this));

    ui->tableWidgetCoord->viewport()->setMouseTracking(true);
    ui->tableWidgetCoord->setItemDelegate(new TableColorItemDelegate(this));

    ui->tableWidgetCoord_pos_view->viewport()->setMouseTracking(true);
    ui->tableWidgetCoord_pos_view->setItemDelegate(new TableColorItemDelegate(this));

    ui->tableWidgetCoord_3->viewport()->setMouseTracking(true);
    ui->tableWidgetCoord_3->setItemDelegate(new TableColorItemDelegate(this));
    ui->tableWidgetCoord_4->setItemDelegate(new TableColorItemDelegate(this));

    ui->tableWidget_camera->setItemDelegateForColumn(4, new TableItemNumberDelegate(this));
    ui->tableWidget_camera->setItemDelegateForColumn(5, new TableItemNumberDelegate(this));

    ui->tableWidget_camera->setHorizontalHeader(new CameraHeaderView(Qt::Horizontal, ui->tableWidget_camera));
    ui->tableWidget_camera->horizontalHeader()->setDefaultSectionSize(120);
    ui->tableWidget_camera->horizontalHeader()->setStretchLastSection(true);

    m_taskSearchBox = ui->widgetSearchBox;

    QToolHelper::centerScreen(this);
    QToolHelper::bringToFront(this);

    ui->widget_coord_edit_2->initCtrl(COORD_REF_CTRL_POINT_KEY);
    ui->widget_coord_edit_3->initCtrl(COORD_REF_POS_INFO_KEY);
    ui->widget_coord_edit->initCtrl(COORD_REF_PARAM_SET_KEY);
	ui->widget_coord_edit_5->initCtrl(COORD_REF_PARAM_SET_KEY);

    if (RBHelper::isDeveloper()) {
        ui->comboProjectType->clear();
        ui->comboProjectType->addItem(QObject::tr("影像数据"), enImageProject);
        ui->comboProjectType->addItem(QObject::tr("区块数据"), enBlockProject);
    } else {
        ui->comboProjectType->clear();
		ui->comboProjectType->addItem(QObject::tr("影像数据"), enImageProject);   //增加空三模块20220312
        ui->comboProjectType->addItem(QObject::tr("区块数据"), enBlockProject);
    }

    initTurnPageCtrl();

	ui->checkBox_xyz_offset->setChecked(false);
	ui->checkBox_enable_custom_3->setChecked(false);
	ui->checkBox_xyz_offset->setChecked(false);
	ui->checkBox_lockRange_3->setChecked(true);

	ui->checkBox_blockAT->setChecked(false);
	ui->radioBtn_blockAT->setChecked(true);
	ui->widget_BlockAT_radioBtn->setVisible(0);

	ui->lineEdit_x_coord_3->clear();
	ui->lineEdit_y_coord_3->clear();
	ui->lineEdit_z_coord_3->clear();
	ui->lineEdit_x_offset->clear();
	ui->lineEdit_y_offset->clear();
	ui->lineEdit_z_offset->clear();
	ui->comboBox_tile_mode_3->setCurrentIndex(0);

	//初始化重建参数
	// 初始化stackedWidget中的数据
	ui->comboOutputType_3->clear();
	for (int i = 0; i < OUTPUT_TYPE_TOTAL; i++) {
//		if(i == OUTPUT_TYPE_TIFF) continue; // 不显示TIFF
		ui->comboOutputType_3->addItem(RBHelper::getOutputTypeDisplay((OutputType)i), RBHelper::getOutputTypeString((OutputType)i));
	}
	ui->comboOutputType_3->setCurrentText(QStringList() << "OSGB");
	// 初始化坐标系
	ui->widget_coord_edit->initCtrl(COORD_REF_PARAM_SET_KEY);
	// 隐藏范围
	if (ui->checkBox_lockRange_3->isChecked()) {
		ui->widget_range_file_3->hide();
	}
	// 自定义原点
	ui->widget_coord_set_3->setEnabled(ui->checkBox_enable_custom_3->isChecked());
	// 坐标偏移
	ui->widget_xyz_offset->setEnabled(ui->checkBox_xyz_offset->isChecked());
	// 隐藏瓦片大小框
	ui->widget_tile_size_3->hide();

	initATBlockListView();

	//初始化发布列表
	InitPubList();
	//初始化发布界面
	InitCloudPublish();
	//初始化详情task
	InitPubTask();


    LOGFMTI("CMainWindow::initUI() out");
	ui->label_24->adjustSize();
	ui->label_24->setWordWrap(true);
#ifdef FOXRENDERFARM
	ui->widget_tabbar->setFixedWidth(90);
	ui->widget_sidebar->setFixedWidth(90);
#endif

#ifdef FOXRENDERFARM
	ui->label_upload_block->setFixedWidth(130);
//	ui->label_22->setFixedWidth(20);
	ui->btnGuide->setFixedWidth(50);
	ui->label_24->setFixedWidth(480);
	ui->btnCtrlPointView->setFixedWidth(190);
	ui->label_cp_info->setMidLineWidth(150);
	ui->label_file_sep->setMidLineWidth(150);
	ui->label_word_order->setMidLineWidth(150);
	ui->btnBacktoProject1->setMinimumWidth(200);
	ui->btnCtrlPointView->setMinimumWidth(200);
#endif
}

void CMainWindow::initViews()
{
	ui->widget_blockAT_options->hide();
	QString plat = WSConfig::getInstance()->get_host_url();
	if (plat == "https://task-beta.dayancloud.com")
	{
		new ToastWidget(ui->widget_title, QObject::tr("老平台作业只能查看和下载，不支持其他相关操作。"), QColor("#FF0000"), 180000);
		ui->btnCreateJob->setEnabled(false);
		ui->btnPublish->setEnabled(false);
		ui->btnRebuildSubmit->setEnabled(false);
		ui->btnATSubmit->setEnabled(false);
	}
	else
	{
		ui->btnCancelJob->setEnabled(true);
		ui->btnPublish->setEnabled(true);
		ui->btnRebuildSubmit->setEnabled(true);
		ui->btnATSubmit->setEnabled(true);
	}
    LOGFMTI("CMainWindow::initViews() in");
    this->setWindowTitle(WINDOW_TITLE);

    m_floatLogPanel = new FloatLogPanel(ui->widget_main);
    m_floatLogPanel->hide();

    m_jobListViewPage = new JobListViewPage(ui->listView, m_floatLogPanel);
    m_jobListViewPage->initView();

    // settings
    m_settingWindow->initUI();
    // tabbar
    this->initSideBar();
    // cornerwidget
    {
        QWidget* cornerWidget = new QWidget();

		QPushButton*  buttonATResult = new QPushButton(tr("下载空三"));
		buttonATResult->setObjectName("btnDownloadATResult");
		connect(buttonATResult, &QPushButton::clicked, this, &CMainWindow::on_btnDownloadATResult_clicked);

		m_buttonSurveyPoint = new QPushButton(tr("刺相控点"));
		m_buttonSurveyPoint->setObjectName("btnSurveyPoint");
#if FOXRENDERFARM
		m_buttonSurveyPoint->setFixedWidth(250);
		buttonATResult->setFixedWidth(250);
#else
		m_buttonSurveyPoint->setFixedWidth(150);
		buttonATResult->setFixedWidth(150);
#endif

        connect(m_buttonSurveyPoint, &QPushButton::clicked, this, &CMainWindow::on_btnSurveyPoint_clicked);
        QHBoxLayout* layout = new QHBoxLayout(cornerWidget);
        layout->setSpacing(20);
		layout->setContentsMargins(0, 0, 18, 24);

		layout->addWidget(buttonATResult);
        layout->addWidget(m_buttonSurveyPoint);
        cornerWidget->setLayout(layout);
        // TODO
         ui->tabWidget_AT->setCornerWidget(cornerWidget);
    }
    {
        QWidget* cornerWidget = new QWidget();

		m_buttonATResult = new QPushButton(tr("下载空三结果"));
		m_buttonATResult->setObjectName("btnDownloadATResult");
		connect(m_buttonATResult, &QPushButton::clicked, this, &CMainWindow::on_btnDownloadATResult_clicked);

        m_buttonResult = new QPushButton(tr("下载重建结果"));
        m_buttonResult->setObjectName("btnDownloadResult");		
        connect(m_buttonResult, &QPushButton::clicked, this, &CMainWindow::on_btnDownloadResult_clicked);

        QHBoxLayout* layout = new QHBoxLayout(cornerWidget);
        layout->setSpacing(20);
		layout->setContentsMargins(0, 0, 18, 24);
        layout->addWidget(m_buttonATResult);
        layout->addWidget(m_buttonResult);
        cornerWidget->setLayout(layout);

        ui->tabWidget_3DView->setCornerWidget(cornerWidget);	
    }

    {
        m_brCornerWidget = new QWidget(ui->widget_3dview); // 右下角
        m_brCornerWidget->setGeometry(0, 0, 52 * 2 + 20, 52 + 10);
        m_buttonShareResult = new QPushButton(m_brCornerWidget);
        m_buttonShareResult->setObjectName("buttonShareResult");
        m_buttonShareResult->setFixedSize(52, 52);
        m_buttonShareResult->setStyleSheet("border:none; border-image:url(:/images/share.png);width:52px;height:52px;");
        connect(m_buttonShareResult, &QPushButton::clicked, this, &CMainWindow::on_buttonShareResult_clicked);

#ifdef FOXRENDERFARM
//		m_buttonShareResult->hide();
		m_buttonATResult->setFixedWidth(250);
		m_buttonResult->setFixedWidth(250);
#endif
        m_buttonResetView = new QPushButton(m_brCornerWidget);
        m_buttonResetView->setObjectName("buttonResetView");
        m_buttonResetView->setFixedSize(52, 52);
        m_buttonResetView->setStyleSheet("border:none; border-image:url(:/images/reset.png);width:52px;height:52px;");
        connect(m_buttonResetView, &QPushButton::clicked, this, &CMainWindow::on_buttonResetView_clicked);
        QHBoxLayout* layout = new QHBoxLayout(m_brCornerWidget);
        layout->setSpacing(20);
        layout->setMargin(5);
        layout->addWidget(m_buttonShareResult);
        layout->addWidget(m_buttonResetView);
        m_brCornerWidget->setLayout(layout);      
        m_brCornerWidget->show();
    }

    {
        ui->widget_params->hide();

        m_rebuildParamWidget = new TaskCloneWindow(0, this);
        m_rebuildParamWidget->initUi(false);
        QLayout* layout = ui->widget_rebuild_params->layout();
        layout->addWidget(m_rebuildParamWidget->getParamWidget());
    }

    // 项目名限制字符输入
//    QRegExp reg("^[0-9a-zA-Z_.]{1,20}$");
	QRegExp reg("^[0-9a-zA-Z_]{1,20}$");
    QRegExpValidator *regVal = new QRegExpValidator(reg);
    ui->lineEditProject->setValidator(regVal);   

    ui->stackedWidget_task->setCurrentIndex(TASK_PAGE_HOME);
    ui->btnAddCtrlPoint->setProperty("nextPage", PROJECT_PAGE_CP_ADD);
    ui->btnAddPos->setProperty("nextPage", PROJECT_PAGE_POS_ADD);

    int upgradeTimeInterval = LocalConfig::getInstance()->upgradeTimeInterval;
    int refreshTimeInterval = LocalConfig::getInstance()->refreshTimeInterval;
    m_refreshTimer          = QObject::startTimer(refreshTimeInterval * 60 * 1000);
    m_upgradeCheckTimer     = QObject::startTimer(upgradeTimeInterval * 60 * 1000);
    m_runAllUploadTimer     = QObject::startTimer(30 * 1000);         // 30s
    m_scheduleTimer         = QObject::startTimer(30 * 1000);         // 30s
    m_downloadTimer         = QObject::startTimer(2 * 60 * 1000);     // 120s

    m_atRenderTimer = new QTimer(this);
    connect(m_atRenderTimer, &QTimer::timeout, [=]() {
        QList<qint64> taskIds;
        taskIds.append(m_currTaskId);
        HttpCmdManager::getInstance()->queryTaskInfo(taskIds, false, this);
    });

    m_rebuildRenderTimer = new QTimer(this);
    connect(m_rebuildRenderTimer, &QTimer::timeout, [=]() {
        QList<qint64> taskIds;
        taskIds.append(m_currTaskId);
        HttpCmdManager::getInstance()->queryTaskInfo(taskIds, false, this);
    });
	

    LOGFMTI("CMainWindow::initViews() out");
}

void CMainWindow::initWindow()
{
    LOGFMTI("CMainWindow::initWindow() in");
    CConfigDb::Inst()->setShowTaskType(TYPE_SHOW_ALLD);     // 默认显示为所有任务（不含delete）

    // 开始请求消息
    m_noticeMgr->startGetPublishMsg(1000 * 60 * 5); // 5min请求一次
    m_noticeMgr->startGetProNotice(1000 * 60 * 5); // 5min请求一次
    m_noticeMgr->setCurLoginState(AfterLogin);

    QString appdata_path = RBHelper::getProgramDataLocalPath();
    CGlobalHelp::DestoryEnv(appdata_path);
    CGlobalHelp::InitEnv(appdata_path);

    // 获取坐标信息
    LoadCoordSystemConfig::getInstance()->initConfig();
    // 获取传感器尺寸数据库
    CameraDB::getInstance()->init();

    LOGFMTI("CMainWindow::initWindow() out.");
}

void CMainWindow::initTrayIcon()
{
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(QIcon(APP_ICON));
    m_trayIcon->setContextMenu(m_trayiconMenu);
#ifdef FOXRENDERFARM
	m_trayIcon->setToolTip(tr("AvicaCloud"));
#else
	m_trayIcon->setToolTip(tr("大雁云"));
#endif    
    m_trayIcon->show();
}

void CMainWindow::addEventListeners()
{
    QObject::connect(m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
        this, SLOT(onSystemTrayIconActivated(QSystemTrayIcon::ActivationReason)));
    QObject::connect(m_taskSearchBox, SIGNAL(searchTriggered(QString, QString)), this, SLOT(onSearchContent(QString, QString)));
    QObject::connect(m_moreMenu, SIGNAL(triggered(QAction *)), this, SLOT(onMoreMenuTriggered(QAction *)));
    QObject::connect(m_logoutAction, SIGNAL(triggered()), this, SLOT(onTrayMenuActionSlot()));
    QObject::connect(m_quitAction, SIGNAL(triggered()), this, SLOT(onTrayMenuActionSlot()));
    QObject::connect(m_aboutAction, SIGNAL(triggered()), this, SLOT(onTrayMenuActionSlot()));
    QObject::connect(m_translogShowAction, SIGNAL(triggered()), this, SLOT(onTrayMenuActionSlot()));
    QObject::connect(ui->label_head, SIGNAL(clicked()), this, SLOT(onPopupUserMenu()));
    QObject::connect(&m_guiUpdater, SIGNAL(logoutSignal(intptr_t, intptr_t)), this, SLOT(onLogout(intptr_t, intptr_t)));
    QObject::connect(&m_guiUpdater, SIGNAL(commandTypeSignal(intptr_t, intptr_t)), this, SLOT(OnCommandType(intptr_t, intptr_t)));
    QObject::connect(&m_guiUpdater, SIGNAL(updateTasklistUiSignal(intptr_t, intptr_t)), this, SLOT(OnUpdateTasklistUi(intptr_t, intptr_t)));
    QObject::connect(&m_guiUpdater, SIGNAL(showMsgBoxSignal(intptr_t, intptr_t)), this, SLOT(OnShowMsgBox(intptr_t, intptr_t)));
    QObject::connect(&m_guiUpdater, SIGNAL(showMsgErrorSignal(intptr_t, intptr_t)), this, SLOT(OnShowMsgError(intptr_t, intptr_t)));
    QObject::connect(&m_guiUpdater, SIGNAL(searchtoTaskListSignal(intptr_t, intptr_t)), this, SLOT(OnSearchtoTaskList(intptr_t, intptr_t)));
    QObject::connect(&m_guiUpdater, SIGNAL(startUploadTaskSignal(intptr_t, intptr_t)), this, SLOT(OnStartUploadTask(intptr_t, intptr_t)));
    QObject::connect(&m_guiUpdater, SIGNAL(submitTaskRetSignal(intptr_t, intptr_t)), this, SLOT(OnSubmitTaskRet(intptr_t, intptr_t)));
    QObject::connect(&m_guiUpdater, SIGNAL(viewStateChangedSignal(intptr_t, intptr_t)), this, SLOT(OnViewStateChanged(intptr_t, intptr_t)));
    QObject::connect(&m_guiUpdater, SIGNAL(speedLimitSignal(intptr_t, intptr_t)), this, SLOT(OnSpeedLimitChanged(intptr_t, intptr_t)));
    QObject::connect(&m_guiUpdater, SIGNAL(deleteTaskUiSignal(intptr_t, intptr_t)), this, SLOT(OnDeleteTaskUi(intptr_t, intptr_t)));
    QObject::connect(&m_guiUpdater, SIGNAL(appendLogInfoSignal(intptr_t, intptr_t)), this, SLOT(OnAppendLogInfo(intptr_t, intptr_t)));
    QObject::connect(&m_guiUpdater, SIGNAL(updateProjectLocalPathSignal(intptr_t, intptr_t)), this, SLOT(OnLocalProjectPathChanged(intptr_t, intptr_t)));
    QObject::connect(&m_guiUpdater, SIGNAL(loadFileFinishedSignal(intptr_t, intptr_t)), this, SLOT(OnLoadFileFinished(intptr_t, intptr_t)));
    QObject::connect(&m_guiUpdater, SIGNAL(messageCompletedSignal(intptr_t, intptr_t)), this, SLOT(OnMessageCompleted(intptr_t, intptr_t)));
    QObject::connect(&m_guiUpdater, SIGNAL(relaunchSignal(intptr_t, intptr_t)), this, SLOT(OnRelaunch(intptr_t, intptr_t)));
    QObject::connect(&m_guiUpdater, SIGNAL(updateClientSignal(intptr_t, intptr_t)), this, SLOT(OnUpdateClient(intptr_t, intptr_t)));
    QObject::connect(&m_guiUpdater, SIGNAL(quit4UpdateSignal(intptr_t, intptr_t)), this, SLOT(OnQuit4Update(intptr_t, intptr_t)));
    QObject::connect(&m_guiUpdater, SIGNAL(transModeChangedSignal(intptr_t, intptr_t)), this, SLOT(OnChangeTransMode(intptr_t, intptr_t)));
    QObject::connect(&m_guiUpdater, SIGNAL(transServerChangedSignal(intptr_t, intptr_t)), this, SLOT(OnChangeTransServer(intptr_t, intptr_t)));
    QObject::connect(&m_guiUpdater, SIGNAL(newsContentResponseSignal(intptr_t, intptr_t)), this, SLOT(OnNewsContentResponse(intptr_t, intptr_t)));
    QObject::connect(m_noticeMgr, SIGNAL(notifyObserver(QList<NoticeContent>)), this, SLOT(onRecvNotifyMsg(QList<NoticeContent>)));
    QObject::connect(&m_guiUpdater, SIGNAL(needUserLoginSignal(intptr_t, intptr_t)), this, SLOT(OnNeedUserLogin(intptr_t, intptr_t)));
    QObject::connect(&m_guiUpdater, SIGNAL(accountUsernamePasswordFailSignal(intptr_t, intptr_t)), this, SLOT(onLoginException(intptr_t, intptr_t)));
    QObject::connect(&m_guiUpdater, SIGNAL(uploadAndDownLoadErrorSignal(intptr_t, intptr_t)), this, SLOT(OnUpLoadAndDownLoadError(intptr_t, intptr_t)));
    QObject::connect(&m_guiUpdater, SIGNAL(showProcessViewSignal(intptr_t, intptr_t)), this, SLOT(OnShowProcessView(intptr_t, intptr_t)));
    QObject::connect(&m_guiUpdater, SIGNAL(locateProjectPageSignal(intptr_t, intptr_t)), this, SLOT(OnLocateProjectPage(intptr_t, intptr_t)));
    QObject::connect(&m_guiUpdater, SIGNAL(infolistUiSignal(intptr_t, intptr_t)), this, SLOT(OnInfolistUi(intptr_t, intptr_t)));
    QObject::connect(m_csMenu, SIGNAL(triggered(QAction *)), this, SLOT(onCSMenuTriggered(QAction *)));
    QObject::connect(m_selPicMenu, SIGNAL(triggered(QAction *)), this, SLOT(onSelPicTriggered(QAction *)));
    QObject::connect(ui->listWidget_pg, SIGNAL(itemSelectionChanged()), this, SLOT(onItemSelectionChanged()));
    QObject::connect(ui->tableWidgetCoord, SIGNAL(cellClicked(int, int)), this, SLOT(onCellClicked(int, int)));
    QObject::connect(m_jobListViewPage, &JobListViewPage::taskItemDoubleClicked, this, &CMainWindow::onTaskItemDoubleClicked);
    QObject::connect(this, &CMainWindow::makeTaskCfgFinished, this, &CMainWindow::onMakeTaskCfgFinished);
    QObject::connect(m_jobListViewPage, &JobListViewPage::hideDownloadView, this, &CMainWindow::onHideDownloadView);
    QObject::connect(ui->tabWidget_AT, &QTabWidget::currentChanged, this, &CMainWindow::onTabWidgetATChanged);
    QObject::connect(&m_guiUpdater, SIGNAL(resubmitTaskRetSignal(intptr_t, intptr_t)), this, SLOT(OnResubmitTaskRet(intptr_t, intptr_t)));
	
}

void CMainWindow::removeEventListeners()
{
    m_guiUpdater.disconnect();
    QObject::disconnect(m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
        this, SLOT(onSystemTrayIconActivated(QSystemTrayIcon::ActivationReason)));
    QObject::disconnect(m_taskSearchBox, SIGNAL(searchTriggered(QString, QString)), this, SLOT(onSearchContent(QString, QString)));
    QObject::disconnect(m_logoutAction, SIGNAL(triggered()), this, SLOT(onTrayMenuActionSlot()));
    QObject::disconnect(m_quitAction, SIGNAL(triggered()), this, SLOT(onTrayMenuActionSlot()));
    QObject::disconnect(m_moreMenu, SIGNAL(triggered(QAction *)), this, SLOT(onMoreMenuTriggered(QAction *)));
    QObject::disconnect(m_aboutAction, SIGNAL(triggered()), this, SLOT(onTrayMenuActionSlot()));
    QObject::disconnect(m_translogShowAction, SIGNAL(triggered()), this, SLOT(onTrayMenuActionSlot()));
    QObject::disconnect(ui->label_head, SIGNAL(clicked()), this, SLOT(onPopupUserMenu()));
    QObject::disconnect(m_noticeMgr, SIGNAL(notifyObserver(const NoticeContent&, int)), this, SLOT(onRecvNotifyMsg(const NoticeContent&, int)));
    QObject::disconnect(m_csMenu, SIGNAL(triggered(QAction *)), this, SLOT(onCSMenuTriggered(QAction *)));
    QObject::disconnect(m_selPicMenu, SIGNAL(triggered(QAction *)), this, SLOT(onSelPicTriggered(QAction *)));
    QObject::disconnect(ui->listWidget_pg, SIGNAL(itemSelectionChanged()), this, SLOT(onItemSelectionChanged()));
    QObject::disconnect(ui->tableWidgetCoord, SIGNAL(cellClicked(int, int)), this, SLOT(onCellClicked(int, int)));
    QObject::disconnect(m_jobListViewPage, &JobListViewPage::taskItemDoubleClicked, this, &CMainWindow::onTaskItemDoubleClicked);
    QObject::disconnect(this, &CMainWindow::makeTaskCfgFinished, this, &CMainWindow::onMakeTaskCfgFinished);
    QObject::disconnect(m_jobListViewPage, &JobListViewPage::hideDownloadView, this, &CMainWindow::onHideDownloadView);
    QObject::disconnect(ui->tabWidget_AT, &QTabWidget::currentChanged, this, &CMainWindow::onTabWidgetATChanged);
}

void CMainWindow::retranslateUi()
{
    m_jobListViewPage->retranslateUi();

    m_setupAction->setText(tr("设置"));
    m_helpAction->setText(tr("帮助"));
    m_updateAction->setText(tr("检查更新"));
    m_aboutAction->setText(tr("关于我们"));
    m_diagMenu->setTitle(tr("故障诊断"));
    m_translogShowAction->setText(tr("传输日志"));
    m_logoutAction->setText(tr("注销 (&Q)"));
    m_quitAction->setText(tr("退出 (&X)"));

    m_csPhoneAction->setText(tr("客服电话"));
    m_csQQAction->setText(tr("客服QQ"));
    m_csHelpAction->setText(tr("帮助文档"));

    m_selFileAction->setText(tr("上传文件"));
    m_selDirAction->setText(tr("上传文件夹"));

    ui->labelLogo->setPixmap(QString(LOGO_LARGE_ICON));
    m_trayIcon->setToolTip(tr("大雁云"));
}

void CMainWindow::updateComponents()
{
    LOGFMTI("[MainWindow] start to updateComponents...");
    QPointer<ScriptDownloader> scriptDownloader(new ScriptDownloader(this));
    m_scripts << "modeling_analyze_v3.zip";
    scriptDownloader->downloadScript(m_scripts);
    QObject::connect(scriptDownloader, SIGNAL(resultReady(int, const QString&)),
        this, SLOT(updateComponentsFinished(int, const QString&)));

    if (LocalSetting::getInstance()->getEnableAutoUpdate() == 1) {
    runCheckUpdate(UPDATE_TIMER_TRIGGER);
}
}

void CMainWindow::flushUserInfo()
{
   int type = MyConfig.client.showType;
   JobStateMgr::Inst()->SendRefreshTasklist(-1, type);

   HttpCmdManager::getInstance()->userInfo(this);
   HttpCmdManager::getInstance()->queryAdvancedParamSetting(this);
   HttpCmdManager::getInstance()->getTaskStatusCount(this);
   HttpCmdManager::getInstance()->transformConfig();
   HttpCmdManager::getInstance()->queryUserRenderSetting(this);
   HttpCmdManager::getInstance()->taskTotalFrameInfo(this);
   HttpCmdManager::getInstance()->queryVoucher();
   HttpCmdManager::getInstance()->queryPicturePrice();
   //刷新当前任务task
   HttpCmdManager::getInstance()->getPublishTask(m_currentId,this);

   // 获取软件信息
   auto cgSoftwares = WSConfig::getInstance()->cg_support_softwares;
   QList<qint64> cgIds = cgSoftwares.keys();
   HttpCmdManager::getInstance()->queryUserPluginConfig(cgIds, this);
}

void CMainWindow::backToHome()
{
    m_currTaskId = 0;
    m_currTaskAlias.clear();
    ui->stackedWidget_task->setCurrentIndex(TASK_PAGE_HOME);
    resetWidgetConfig();
	ui->lineEditProject->clear();
}

void CMainWindow::backToProjectHome()
{
    updateStackRenderPage(RENDER_PAGE_PROJECT);
    ui->stackedWidget_project->setCurrentIndex(PROJECT_PAGE_PROJECT);
}

void CMainWindow::initATBlockListView()
{
	ui->listWidget_blockATKML->clear();
	TaskAtKmlBlock *cItemWidget = new TaskAtKmlBlock;
	cItemWidget->setinitView();
	QListWidgetItem *pItem = new QListWidgetItem;
	pItem->setTextAlignment(Qt::AlignCenter);//设置居中显示
	pItem->setSizeHint(QSize(700, 60));//设一项的大小
	
	ui->listWidget_blockATKML->addItem(pItem);
	ui->listWidget_blockATKML->setItemWidget(pItem, cItemWidget);
	connect(cItemWidget, &TaskAtKmlBlock::atXmlNumAdd, this, &CMainWindow::showATKmlBlock);
	connect(ui->listWidget_blockATKML, &QListWidget::itemChanged, this, &CMainWindow::showATKmlBlock);
}

void CMainWindow::RecommendedCoordinateSystem(const double & lon, const double & lat)
{
	m_rebuildParamWidget->resetView(lon,lat);
}

void CMainWindow::updateComponentsFinished(int errorCode, const QString& result)
{
    // plugin config
    QPointer<ScriptDownloader> pluginDownloader(new ScriptDownloader(this));
    pluginDownloader->downloadPlugins();
    QObject::connect(pluginDownloader, SIGNAL(resultReady(int, const QString&)),
        this, SLOT(getComponentsConfigFinished(int, const QString&)));

    // 下载新更新程序
    QPointer<QDownloadManager> updateDownloader(new QDownloadManager());
    updateDownloader->downloadAutoUpdater("update.exe");
}

void CMainWindow::onUploadResponse(QSharedPointer<ResponseHead> response)
{
	if (response == Q_NULLPTR)
		return;

	int type = response->cmdType;
	switch (type) {

	
	case RESTAPI_TASK_UPLOADPIC: {
		if (response->code == HTTP_ERROR_SUCCESS) {
			QSharedPointer<UploadPic> uploadpic = qSharedPointerCast<UploadPic>(response);
			m_serverPic = uploadpic->picurl;
		}
		break;
	}
}
}

void CMainWindow::getComponentsConfigFinished(int errorCode, const QString& result)
{
}

void CMainWindow::onSystemTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason) {
    case QSystemTrayIcon::Trigger :
        setWindowState(Qt::WindowActive);
        activateWindow();
        break;

    case QSystemTrayIcon::DoubleClick:
        if(this->isHidden()) {
            this->show();
            this->setWindowState(Qt::WindowActive);
            this->activateWindow();
        } else {
            this->hide();
        }
        break;

    default:
        break;
    }
}

void CMainWindow::onShowAbout()
{
    QPointer<AboutWindow> m_aboutWnd(new AboutWindow(this));
    m_aboutWnd->exec();
}

void CMainWindow::onTrayMenuActionSlot()
{
    QAction* action = qobject_cast<QAction *>(sender());

    if (action == m_aboutAction) {
        onShowAbout();
    } else if (action == m_logoutAction) {
        this->hide();
        onLogout(0, 0);
    } else if (action == m_quitAction) {
        this->hide();
        this->close();
    } else if (action == m_translogShowAction) {
        auto engine = CTaskCenterManager::Inst()->currentTransEngine();
        if (engine == nullptr)
            return;

        QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromStdString(engine->logpath())));
    }
}

bool CMainWindow::eventFilter(QObject * obj, QEvent * event)
{
#if 0  // 空三预览客户端功能鼠标事件调试
	if (event->type() == QEvent::MouseButtonPress) {
		if (obj == (QObject*)ui || obj == (QObject*)ui->widget_page)
		{
			qDebug() << "first event";
			qDebug() << obj;
			if (obj == (QObject*)ui /*&& obj == (QObject*)ui->widget_page*/)
			{
				qDebug() << "second event";
				qDebug() << obj;
				return QDialog::eventFilter(obj, event);
			}
		}
	}
#endif

    if(event->type() == QEvent::KeyPress) {
        QKeyEvent* ke = (QKeyEvent*)event;
        if(ke->matches(QKeySequence::Refresh) || ke->key() == Qt::Key_F5) {
            return true;
        }
    }

    if (obj == (QObject*)ui->widget_title) {
        if (event->type() == QEvent::MouseButtonDblClick) {
            this->clickTitlebar();
            return true;
        }
    }

	if (obj == (QObject*)ui->widget_page || obj == (QObject*)ui) {

		if (event->type() == QEvent::MouseButtonPress)
		{
			return QDialog::eventFilter(obj, event);
		}
	}

    return Window::eventFilter(obj, event);
}

void CMainWindow::changeEvent(QEvent * event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        this->retranslateUi();
    } else if (event->type() == QEvent::WindowStateChange) {
        if (this->isMaximized()) {
            ui->btnMax->hide();
            ui->btnRestore->show();
        } else {
            ui->btnMax->show();
            ui->btnRestore->hide();
        }
    }
    Window::changeEvent(event);
}

void CMainWindow::keyPressEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_Escape) {
        event->accept();
        return;
    }

    if (event->modifiers() == Qt::ControlModifier) {
        switch (event->key()) {
        case Qt::Key_F5:
            qDebug() << "Ctrl+F5 Refresh...";
            this->flushUserInfo();
            break;
        default:
            Window::keyPressEvent(event);
        }
    } else if(event->matches(QKeySequence::Refresh)) {
        qDebug() << "F5 Refresh...";
    }
}

void CMainWindow::resizeEvent(QResizeEvent *event)
{
    showShareResultButtons();

    qDebug() << "resize event...";
    Window::resizeEvent(event);
}

void CMainWindow::showTrayMessage(QString msg)
{
    if(msg.isEmpty()) {
        m_trayIcon->showMessage("", "", QSystemTrayIcon::Information, 0);
        return;
    }

    m_trayIcon->showMessage(WINDOW_TITLE, msg, QSystemTrayIcon::Information, 5000);
}

bool CMainWindow::Response(int type, intptr_t wParam, intptr_t lParam)
{
    switch (type) {
    case UM_COMMAND_TYPE:
        m_guiUpdater.handleCommandType(wParam, lParam);
        break;
    case UM_MESSAGE_FLOAT:
        m_guiUpdater.showMsgBox(wParam, lParam);
        break;
    case UM_MESSAGE_ERROR:
        m_guiUpdater.showMsgError(wParam, lParam);
        break;
    case UM_TASKLIST_UI:
        m_guiUpdater.updateTasklistUi(wParam, lParam);
        break;
    case UM_BATCH_SET_PARAM:
        m_guiUpdater.batchSetParam(wParam, lParam);
        break;
    case UM_BADGENUMBER:
        m_guiUpdater.badgeNumber(wParam, lParam);
        break;
    case UM_START_UPLOAD_TASK:
        m_guiUpdater.startUploadTask(wParam, lParam);
        break;
    case UM_CLONE_CFG_DOWNLOADED:
    case UM_CLONE_CFG_DOWNLOADFAIL:
        m_guiUpdater.cloneCfgRet(wParam, lParam);
        break;
    case UM_UPLOAD_DOWNLOAD_ERROR:
        m_guiUpdater.uploadAndDownLoadError(wParam, lParam);
        break;
    case UM_PRINT_LOGINFO:
        m_guiUpdater.appendLogInfo(wParam, lParam);
        break;
    case UM_WARN_SET_REDHOT:
        m_guiUpdater.setReddot(wParam, lParam);
        break;
    case UM_RELAUNCH:
        m_guiUpdater.relaunch(wParam, lParam);
        break;
    case UM_LOGIN_RET:
        m_guiUpdater.loginRet(wParam, lParam);
        break;
    case UM_UPDATE_PROJECT_LIST:
        m_guiUpdater.updateProjectList(wParam, lParam);
        break;
    case UM_SEARCH_TO_TASKLIST:
        m_guiUpdater.searchtoTaskList(wParam, lParam);
        break;
    case UM_CLONE_CREATETASK_RET:
        m_guiUpdater.cloneCreateTaskRet(wParam, lParam);
        break;
    case UM_SMALLPIC_RET:
        m_guiUpdater.smallpicRet(wParam, lParam);
        break;
    case UM_CHANGE_STATE_VIEW:
        m_guiUpdater.viewStateChanged(wParam, lParam);
        break;
    case UM_SHOW_LOADING:
        m_guiUpdater.showLoading(wParam, lParam);
        break;
    case UM_SUBMIT_TASK_RET:
        m_guiUpdater.submitTaskRet(wParam, lParam);
        break;
    case UM_TASKDELETE_UI:
        m_guiUpdater.deleteTaskUi(wParam, lParam);
        break;
    case UM_INFOLIST_UI:
        m_guiUpdater.infolistUi(wParam, lParam);
        break;
    case UM_LOGOUT:
        m_guiUpdater.logout(wParam, lParam);
        break;
    case UM_SPEED_LIMIT:
        m_guiUpdater.speedLimit(wParam, lParam);
        break;
    case UM_UPDATE_PROJECTLOCALPATH:
        m_guiUpdater.updateProjectLocalPath(wParam, lParam);
        break;
    case UM_SCRIPT_FINISHED:
        m_guiUpdater.execScriptFinished(wParam, lParam);
        break;
    case UM_LOADFILE_FINISHED:
        m_guiUpdater.loadFileFinished(wParam, lParam);
        break;
    case UM_EVENT_COMPLETED:
        m_guiUpdater.messageCompleted(wParam, lParam);
        break;
    case UM_UPDATE_CLIENT:
        m_guiUpdater.updateClient(wParam, lParam);
        break;
    case UM_UPDATE_SCRIPTS:
        m_guiUpdater.updateScripts(wParam, lParam);
        break;
    case UM_DEPENDTASK_COMPLETED:
        m_guiUpdater.dependTaskCompleted(wParam, lParam);
        break;
    case UM_TRIGGER_SCHEDULE:
        m_guiUpdater.triggerSchedule(wParam, lParam);
        break;
    case UM_QUIT4UPDATE:
        m_guiUpdater.quit4Update(wParam, lParam);
        break;
    case UM_CHANGE_TRANS_MODE:
        m_guiUpdater.transModeChanged(wParam, lParam);
        break;
    case UM_CHANGE_TRANS_SERVER:
        m_guiUpdater.transServerChanged(wParam, lParam);
        break;
    case UM_NEWS_CONTENT_RET:
        m_guiUpdater.newsContentResponse(wParam, lParam);
        break;
    case UM_UPDATE_USERNAME_LIST:
        m_guiUpdater.updateUserNameListResponse(wParam, lParam);
        break;
    case UM_NEED_USER_LOGIN:
        m_guiUpdater.needUserLogin(wParam, lParam);
        break;
    case UM_USERNAME_PASSWORD_FAIL:
        m_guiUpdater.accountUsernamePasswordFail(wParam, lParam);
        break;
    case UM_UPDATE_PLUGINLIST:
        m_guiUpdater.showPluginList(wParam, lParam);
        break;
    case UM_UPDATE_TRANS_SPEED:
        m_guiUpdater.updateTransSpeedDisplay(wParam, lParam);
        break;
    case UM_CHECK_CONNECTION_EXCEPTION:
        m_guiUpdater.fileCheckExceptionRespone(wParam, lParam);
        break;
    case UM_UPDATE_PLATFORM_STATUS:
        m_guiUpdater.updatePlatformStatus(wParam, lParam);
        break;
    case UM_DOWNLOADLIST_UI:
        m_guiUpdater.updateDownloadListUi(wParam, lParam);
        break;
    case UM_UPDATE_MAIN_UI:
        m_guiUpdater.updateMainUi(wParam, lParam);
        break;
    case UM_UPDATE_MAIN_UI_BTN_STATE:
        m_guiUpdater.updateMainUiBtnState(wParam, lParam);
        break;
    case UM_SHOW_PROCESS_VIEW:
        m_guiUpdater.showProcessView(wParam, lParam);
        break;
    case UM_LOCATE_PROJECT_PAGE:
        m_guiUpdater.locateProjectPage(wParam, lParam);
        break;
    case UM_RESUBMIT_TASK_FREAME_RET:
        // m_guiUpdater.resubmitTaskRet(wParam, lParam);
        this->OnResubmitTaskRet(wParam, lParam);
        break;
    default:
        break;
    }

    return true;
}


void CMainWindow::clickTitlebar()
{
    if (this->isMaximized())
        this->showNormal();
    else
        this->showMaximized();
}

void CMainWindow::onPopupUserMenu()
{
    UserInfoWnd* userInfoPopWnd = new UserInfoWnd();
    QPoint pos = ui->label_head->mapToGlobal(QPoint((-userInfoPopWnd->width() / 2) + 64, ui->label_head->height() + 5));

    userInfoPopWnd->move(pos);
#ifdef FOXRENDERFARM
    userInfoPopWnd->setBalance(MyConfig.accountSet.usdbalance);
#else
    userInfoPopWnd->setBalance(MyConfig.accountSet.rmbbalance);
#endif
    int couponCount = MyConfig.accountSet.couponCount + MyConfig.accountSet.qyCouponCount;
    userInfoPopWnd->setCoupon(MyConfig.accountSet.coupon, couponCount);
    userInfoPopWnd->setUserName(MyConfig.userSet.userName);
    userInfoPopWnd->setVipLevel(MyConfig.accountSet.level);
    userInfoPopWnd->setQCoupon(MyConfig.accountSet.qyCoupon, MyConfig.accountSet.qyCouponCount);
    userInfoPopWnd->setVoucher(MyConfig.voucherSet.voucherCount());
    userInfoPopWnd->show();
}

void CMainWindow::on_btnTest_clicked()
{
    QTime time;
    time.start();

    JobState *job = new JobState(1);
    for (int i = 0; i < 100; i++) {
        ui->listView->addState(job);
    }

    qDebug() << "add state use time " << time.elapsed();
}



void CMainWindow::onGuideWndClosed()
{

}

QPointer<QMenu> CMainWindow::createMoreMenu()
{
    QMenu* moreMenu = new QMenu(this);

    m_setupAction = new QAction(moreMenu);
    m_helpAction = new QAction(moreMenu);
    m_updateAction = new QAction(moreMenu);
    m_aboutAction = new QAction(moreMenu);

    moreMenu->addAction(m_setupAction);
    moreMenu->addSeparator();

    moreMenu->addAction(m_updateAction);
    moreMenu->addAction(m_aboutAction);

    return moreMenu;
}

QPointer<QMenu> CMainWindow::createTrayMenu()
{
    m_logoutAction = new QAction(this);
    m_quitAction = new QAction(this);

    m_diagMenu = new QMenu(this);
    m_translogShowAction = new QAction(m_diagMenu);
    m_diagMenu->addAction(m_translogShowAction);

    QMenu *trayiconMenu = new QMenu(this);
    trayiconMenu->addAction(m_aboutAction);
    trayiconMenu->addMenu(m_diagMenu);
    trayiconMenu->addAction(m_logoutAction);
    trayiconMenu->addAction(m_quitAction);
    trayiconMenu->addSeparator();

    return trayiconMenu;
}

void CMainWindow::closeEvent(QCloseEvent * event)
{
    // 关闭客户端时开始更新
    this->AutoUpdateAfterQuit();
    // 关闭客户端时清理分析缓存


    CGlobalHelp::g_mainSink = nullptr;
    this->removeEventListeners();

    killTimer(m_upgradeCheckTimer);
    killTimer(m_refreshTimer);
    killTimer(m_runAllUploadTimer);
    killTimer(m_scheduleTimer);
    killTimer(m_downloadTimer);

    m_settingWindow->hide();
    this->hide();
    delete m_trayIcon;

    CTaskCenterManager::Inst()->beepOff();
    LOGFMTI("[CMainWindow]CTaskCenterManager beepOff done.");
    CTaskCenterManager::Destroy();
    LOGFMTI("[CMainWindow]CTaskCenterManager Destroy done.");

    event->accept();

    LOGFMTI("Application about to exit...");
    QApplication::quit();
	if (m_isLogout)
	{
		LogoutAccountLogin();
	}

}

void CMainWindow::timerEvent(QTimerEvent * event)
{
    int timerIds = event->timerId();
    if (timerIds == m_upgradeCheckTimer) {
        if (LocalSetting::getInstance()->getEnableAutoUpdate() == 1) {
            runCheckUpdate(UPDATE_TIMER_TRIGGER);
        }        
    } else if (timerIds == m_refreshTimer) {
        this->flushUserInfo();
    } else if (timerIds == m_runAllUploadTimer) {
        CTaskCenterManager::Inst()->runAllUploadHistoryTask();
        CGlobalHelp::Response(UM_UPDATE_TRANS_SPEED, UM_UPDATE_TRANS_SPEED_UPLOAD, 0);
    } else if (timerIds == m_scheduleTimer) {
    } else if (timerIds == m_downloadTimer) {
        CDownloadStateMgr::Inst()->runAllHistoryDownload();
    }

    Window::timerEvent(event);
}

// Update Check
void CMainWindow::runCheckUpdate(int flag)
{
    m_updateTrigger = (UpdateFlag)flag;
    bool needCheck = false;

    if (flag == UPDATE_MANUAL_TRIGGER) {
        this->showTrayMessage(QObject::tr("正在检查版本..."));
        needCheck = true;
    } else if (m_updateTrigger == UPDATE_TIMER_TRIGGER) {
        needCheck = CConfigDb::Inst()->getAutoUpdate();
    }

    if (needCheck) {
        QObject::connect(&UpdateManagerInstance, SIGNAL(resultReady(int)), this, SLOT(onUpdateCheckReturn(int)));
        UpdateManagerInstance.checkUpdate(flag, this);
    }
}

void CMainWindow::onUpdateCheckReturn(int updateCode)
{
    QObject::disconnect(&UpdateManagerInstance, SIGNAL(resultReady(int)), this, SLOT(onUpdateCheckReturn(int)));

    int nUpdateRet = enDialogExit;

    if (updateCode == forced_to_update || updateCode == need_to_update) {
        if (m_updateTrigger == UPDATE_TIMER_TRIGGER) {
            if (!m_updateWnd->isVisible())
                m_updateNotify->showUpdate();
        } else {
            m_updateWnd->showUpdate();
        }
        this->showTrayMessage("");
    } else {
        if (m_updateTrigger == UPDATE_MANUAL_TRIGGER) {
            this->showTrayMessage(QObject::tr("无可用更新"));
        }
    }
}

void CMainWindow::onSearchContent(QString key, QString content)
{
    FilterContent &filterContent = HttpCmdManager::getInstance()->filterContent;
    // QString content = m_taskSearchBox->getSearchContent();
    filterContent.searchWord = content;

    m_jobListViewPage->showLoading(true);
    JobStateMgr::Inst()->SetClean(true);
    JobStateMgr::Inst()->m_vtPage.clear();

    m_jobListViewPage->SetShowType(TYPE_SHOW_ALLD);

    if(content.isEmpty()) {
        filterContent.filterType = NORMAL_FILTER;
    } else {
        filterContent.filterType = CONTENT_FILTER;
    }
    // 搜索时全部清空先前的筛选记录
    filterContent.stateList.clear();
    filterContent.producerList.clear();
    filterContent.projList.clear();
    filterContent.userList.clear();

    filterContent.content = content;
    filterContent.showType = TYPE_SHOW_ALLD;
    filterContent.pageNum = 1;
    filterContent.stateList.append(TYPE_SHOW_ALLD);
    HttpCmdManager::getInstance()->renderList(filterContent);

    this->Response(UM_SEARCH_TO_TASKLIST, 0, 0);
    MyConfig.client.showType = (SHOW_TASKTPE)TYPE_SHOW_ALLD;
}

void CMainWindow::onMoreMenuTriggered(QAction * action)
{
    if (action == m_setupAction) {
        m_settingWindow->initData();
        m_settingWindow->show();
    } else if (action == m_updateAction) {
        runCheckUpdate(UPDATE_MANUAL_TRIGGER);
    } else if (action == m_helpAction) {
        QString helpUrl = WSConfig::getInstance()->get_link_address("url_user_guide");
        QDesktopServices::openUrl(QUrl(helpUrl));
    }
}

void CMainWindow::OnCommandType(intptr_t wParam, intptr_t lParam)
{
    int cmd = (REND_CMD)wParam;

    LOGFMTI("Receive command: %d", cmd);

    switch (cmd) {
    case REND_CMD_UPLOAD_TASK: {
        stSubTask* st = (stSubTask *)lParam;
        qint64 taskId = st->jobId;
        StartTask(st->jobId, st->type, st);
        delete st;

        qApp->alert(this, 3000);
    }
        break;
        break;
    case REND_CMD_RELAUNCH_APP:
        this->Response(UM_RELAUNCH, 0, 0);
        break;
    case REND_CMD_UPDATE_APP:
        this->Response(UM_QUIT4UPDATE, 0, 0);
        break;
    default:
        LOGFMTE("[MainWindow] CommandType cmd undefined! %d", cmd);
        break;
    }
}

void CMainWindow::OnUpdateTasklistUi(intptr_t wParam, intptr_t lParam)
{
    int nCurPage = wParam;
    bool bClean = (lParam == 0) ? false : true;

    m_jobListViewPage->updateTasklistUi(bClean);

    m_jobListViewPage->showLoading(false);
}

void CMainWindow::OnShowMsgBox(intptr_t wParam, intptr_t lParam)
{
    MSGBOXPARAM *param = (MSGBOXPARAM *)lParam;
    int type = wParam;

    switch (type) {
    case MSGBOX_TYPE_PROJECT_NONE: {
        Util::ShowMessageBoxError(QObject::tr("未进行插件配置，请前往插件配置页进行配置"));
        break;
    }
    case MSGBOX_TYPE_PLUGIN_NONE: {
        SetErrorMessage(QObject::tr("未进行插件配置，请前往插件配置页进行配置"));
        break;
    }
    case MSGBOX_TYPE_INSTDIR_NONE: {
        SetErrorMessage(QObject::tr("软件安装目录未选择，请前往插件配置页进行配置"));
        break;
    }
    case  MSGBOX_TYPE_BATCH_DUPLICATE: {
        SetErrorMessage(param->content);
        delete param;
        param = nullptr;

        break;
    }
    case MSGBOX_TYPE_BATCH_TYPEMIS: {
        SetErrorMessage(QObject::tr("不支持此类型文档，请核实后再放入。"));
        break;

    }
    case MSGBOX_TYPE_BATCH_TYPE_BADFILE: {
        SetErrorMessage(QObject::tr("不支持的文件格式已自动跳过"));

        break;
    }
    case MSGBOX_TYPE_SUBMIT_ERROR: {
        SetErrorMessage(param->content);
        delete param;
        param = nullptr;

        break;
    }
    case MSGBOX_TYPE_ADDTASK_FAILED: {
        t_taskid jobId = lParam;
        SetErrorMessage(QObject::tr("任务 %1 相关配置文件加载失败,文件可能被损坏").arg(jobId));
        break;
    }
    case MSGBOX_TYPE_BALANCE_LIMITED: {
        SetErrorMessage(QObject::tr("您的余额不足,下载将停止。"));
        CTaskCenterManager::Inst()->manualStopDownload(lParam);

        break;
    }
    case MSGBOX_TYPE_DOWNLOAD_DISABLE: {
        SetErrorMessage(QObject::tr("禁止下载,请联系客服"));
        CTaskCenterManager::Inst()->manualStopDownload(lParam);

        break;
    }
    case MSGBOX_TYPE_PLUGIN_TIMEOUT: {
        SetErrorMessage(QObject::tr("请检查是否配置相应的插件信息，如有，插件信息获取超时，请检查网络是否流畅。"));

        break;
    }
    case MSGBOX_TYPE_START_TASK_FAILED: {
        SetErrorMessage(QObject::tr("您的余额不足，开始任务失败，请充值后重试!"));

        break;
    }
    case MSGBOX_TYPE_DELETETASK_FORBIDDEN: {
        SetErrorMessage(QObject::tr("该账号无删除作业权限，若需开通请联系主账号!"));

        break;
    }
    case MSGBOX_TYPE_STOP_TASK_ALL_COPY_PICTURE: {
        SetErrorMessage(QObject::tr("正在出图的任务不能停止，请等待出图完成后再做操作!"));

        break;
    }
    case MSGBOX_TYPE_TASK_EXPIRE_CANT_OPERATE: {
        SetErrorMessage(QObject::tr("任务已过期，不可操作!"));

        break;
    }
    case MSGBOX_TYPE_RENDER_COMMIT_PASS_ONE_MONTH: {
        SetErrorMessage(QObject::tr("作业已过期，请重新提交!"));

        break;
    }
    case MSGBOX_TYPE_ENGINE_BROKEN: {
        QString message = QObject::tr("网络已中断，请检查您的网络设置");
        m_jobListViewPage->appendLog(message, PAGE_TASK);
        break;
    }
    case MSGBOX_TYPE_SUBMIT_DISABLE:
        SetErrorMessage(QObject::tr("此平台已暂停提交作业，请前往其它平台提交，给您造成的不便敬请见谅!"));
        break;
    case MSGBOX_TYPE_VINSUFFICIENT_PERMISSIONS:
        SetErrorMessage(QObject::tr("权限不足"));
        break;
    case HTTP_ERROR_UNKNOWN: {
        MSGBOXPARAM *param = (MSGBOXPARAM*)lParam;
        QString message = LangTranslator::RequstErrorI18n(param->content);
#ifdef _DEBUG
        SetErrorMessage(message);
#endif   
    }
        break;
    case MSGBOX_TYPE_PLUGIN_CHECK_UPDATE: {
        QString msg = QObject::tr("插件发现新版本，请前往 设置页面->插件管理 更新插件版本！");
        this->showTrayMessage(msg);
    }
        break;
    case MSGBOX_TYPE_PATH_NOT_EXISTS: {
        MSGBOXPARAM *param = (MSGBOXPARAM*)lParam;
        SetErrorMessage(param->content);
    }
        break;
    case HTTP_ERROR_DO_NOT_HAVE_ANY_MORE_RECORD: {
        SetErrorMessage(QObject::tr("没有更多记录，操作任务失败!"));
    }
       break;
    case HTTP_ERROE_RENDER_WAIT_TASK_COMPLETE: {
        SetErrorMessage(QObject::tr("等待光子或者主图渲染完成!"));
    }
       break;
    case MSGBOX_TYPE_PAYMENT_USE_VOUCHER: {
        if (param->code) {
            SetErrorMessage(param->content);
        } else {
            SetErrorMessage(param->content);
        }
    }
        break;
    default:
        break;
    }
}

void CMainWindow::OnShowMsgError(intptr_t wParam, intptr_t lParam)
{
    int type = wParam;
    switch (type) {
    case MSGBOX_TYPE_OUTPUT_EMPTY: {
        SetToastMessage(QObject::tr("文件下载路径为空"));

        break;
    }
    case MSGBOX_TYPE_BUFFERTOOSMALL: {
        SetToastMessage(QObject::tr("选择文件数量太多"));

        break;
    }
    case HTTP_ERROR_UNKNOWN: {
        MSGBOXPARAM *param = (MSGBOXPARAM*)lParam;
        QString message = LangTranslator::RequstErrorI18n(param->content);
        SetToastMessage(message);
        break;
    }
    default:
        break;
    }
}

void CMainWindow::OnSearchtoTaskList(intptr_t wParam, intptr_t lParam)
{
    MyConfig.client.showType = TYPE_SHOW_ALLD;
}

void CMainWindow::OnLoginRet(intptr_t wParam, intptr_t lParam)
{
    enNetRet ret = (enNetRet)wParam;
    switch (ret) {
    case enLoginOk: {
        CTaskCenterManager::Inst()->resetTaskPacketBid();

        break;
    }
    case enUnamePassError: {
        onLoginException(wParam, lParam);
        break;
    }
    case enNetTimeout:
        break;
    }
}

void CMainWindow::OnDeleteTaskUi(intptr_t wParam, intptr_t lParam)
{
    t_taskid taskId = (t_taskid)wParam;
    m_jobListViewPage->deleteTaskUi(taskId);
}

void CMainWindow::OnChangeTransMode(intptr_t wParam, intptr_t lParam)
{
    QString transEngine = LocalSetting::getInstance()->getTransEngine();

    if (!CTaskCenterManager::Inst()->changeTransEngine(transEngine))
        LOGFMTE("[MainWindow] engine change to [%s] failed.", qPrintable(transEngine));
}

void CMainWindow::OnChangeTransServer(intptr_t wParam, intptr_t lParam)
{
    QString transEngine = LocalSetting::getInstance()->getTransEngine();
    QString hostName = LocalSetting::getInstance()->getNetWorkName();

    CTaskCenterManager::Inst()->changeTransServer(hostName);
}

void CMainWindow::OnAppendLogInfo(intptr_t wParam, intptr_t lParam)
{
    int iTabId = wParam;
    std::unique_ptr<MsgInfo> apMsg(reinterpret_cast<MsgInfo*>(lParam));

    QString strLog = QString::fromUtf8(apMsg->msg);
    // m_jobViewPage->appendLog(strLog, iTabId);
    m_jobListViewPage->appendLog(strLog, iTabId);
}

void CMainWindow::onLogout(intptr_t wParam, intptr_t lParam)
{
    LocalSetting::getInstance()->setAutoLogin(false);

    m_noticeMgr->stopGetPublishMsg();
    m_isLogout = true;
    HttpCmdManager::getInstance()->userLogout();

    this->close();
}

void CMainWindow::OnViewStateChanged(intptr_t wParam, intptr_t lParam)
{
    int freshModel = (int)wParam;
    qint64 rowNum = (qint64)lParam;

    if (freshModel == VIEW_REFRESH_UPLOAD_PAGE_ROWS) {
        m_jobListViewPage->updateRow(rowNum);

        this->updateUploadState();
    // 更新上传进度和状态
    } else if (freshModel == VIEW_REFRESH_DOWNLOAD_PAGE_ALL) {
        m_downloadViewPage->updateTasklistUi(true);   
    } else if (freshModel == VIEW_REFRESH_DOWNLOAD_PAGE_ROWS) {
        m_downloadViewPage->updateRow(rowNum);
}
}

void CMainWindow::OnStartUploadTask(intptr_t wParam, intptr_t lParam)
{
    int taskid = (int)wParam;

    if(!CTaskCenterManager::Inst()->startUpload(taskid)) {
        LOGFMTE(" %d>Start task failed!", taskid);
    }
}

void CMainWindow::OnRelaunch(intptr_t wParam, intptr_t lParam)
{
    if (!LocalSetting::getInstance()->getRememberPassword()) {
        LocalSetting::getInstance()->setAutoLogin(true);
        LocalSetting::getInstance()->setRememberPassword(true);
    }

    CAppInstance::GetApp()->Relaunch();

    this->close();
}

void CMainWindow::OnShowLoading(intptr_t wParam, intptr_t lParam)
{
    bool bShow = bool(lParam);
    m_jobListViewPage->showLoading(bShow);
    new ToastWidget(this, QObject::tr("刷新完成"), QColor("#e25c59"), 3000);
}

void CMainWindow::OnCheckUpdateRet(intptr_t wParam, intptr_t lParam)
{
    int dwErrorCode = wParam;
    int flag = (int)lParam;
    if (dwErrorCode == need_to_update) {
        if (flag == UPDATE_MANUAL_TRIGGER) {
            int ret = Util::ShowMessageBoxQuesion(QObject::tr("有可用更新下载"));
            if (ret == enIDOK) {
            }
        } else {
            this->showTrayMessage(QObject::tr("有可用更新下载"));
        }
    } else {
        if (flag == UPDATE_MANUAL_TRIGGER) {
            Util::ShowMessageBoxInfo(QObject::tr("暂无可用更新"));
        }
    }
}

void CMainWindow::OnQuit4Update(intptr_t wParam, intptr_t lParam)
{
    CAppInstance::GetApp()->Update();

    this->close();
}

void CMainWindow::OnDownloadUpdateRet(intptr_t wParam, intptr_t lParam)
{
    int dwErrorCode = wParam;
    int flag = (int)lParam;

    if (dwErrorCode == download_success) {
        if (flag == UPDATE_MANUAL_TRIGGER) {
            this->showTrayMessage(QObject::tr("升级文件已下载完成"));
            this->Response(UM_QUIT4UPDATE, 0, 0);
        }
    }
}

void CMainWindow::OnSubmitTaskRet(intptr_t wParam, intptr_t lParam)
{
    int code = SUBMIT_TASK_NETWORK_ERROR;
    MSGBOXPARAM *subRet = (MSGBOXPARAM*)lParam;
    if (subRet != nullptr) {
        code = subRet->code;
    }
    QString action = subRet->action;
    t_taskid taskId = (t_taskid)wParam;
    CTaskCenterManager::Inst()->submitTaskRet(taskId, code);

    if (code == SUBMIT_TASK_OK) {
        if (action == "clone") {
            this->Response(UM_EVENT_COMPLETED, UM_CLONE_SUCCESS, taskId);
        } else {
            this->Response(UM_EVENT_COMPLETED, UM_SUBMIT_SUCCESS, taskId);
#if 0
 		JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(taskId);
 		if (!getRebuildParams(m_settingsInfo.rebuildOption)) {
				return;
			}
			jobstate = jobstate->getRebuildJob();
			HttpCmdManager::getInstance()->taskSubmit(taskId, m_currProjectName);
			HttpCmdManager::getInstance()->taskSubmitRebuild(jobstate->getTaskId(), m_currProjectName, m_settingsInfo.rebuildOption, this);
#endif
        }
        

        // int nType = m_jobViewPage->GetShowType();
        int nType = m_jobListViewPage->GetShowType();
        JobStateMgr::Inst()->SendRefreshTasklist(-1, nType);

        if (subRet->action == "AT" || subRet->action == "render") {
            // 提交成功后自动跳转到空三页面
            resetUploadCtrl();
            int nShow = CConfigDb::Inst()->getShowTaskType();
            JobStateMgr::Inst()->SendRefreshTasklist(-1, nShow);

			// 如果当前任务与提交任务不匹配，则不切换页面
			if (taskId != m_currTaskId)
				return;

            JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(taskId);
            if (jobstate == nullptr) return;

            if (jobstate->getProjectType() == enImageProject) {
                updateStackRenderPage(RENDER_PAGE_AT);
                m_atRenderTimer->start(1000 * 5);
                ui->stackedWidget_AT->setCurrentIndex(AT_PAGE_RENDER);
            } else if (jobstate->getProjectType() == enBlockProject) {              
				updateStackRenderPage(RENDER_PAGE_RECONSTRUCTION);
				updateStackRenderPageNewProcess(RENDER_PAGE_RECONSTRUCTION);
//				updateStackRenderPage(RENDER_PAGE_RECONSTRUCTION);
            }
        
            resetCtrlPointViewSet();
            resetPositionViewSet();
            jobStateMachine();
        }

    } else {
        QString message;
        switch (code) {
        case SUBMIT_TASK_FAILED: {
            message = QObject::tr("提交作业失败(未知错误)");
            if (action == "clone") {
                message = QObject::tr("克隆作业失败(未知错误)");
            }
        }         
            break;
        case SUBMIT_TASK_INTERNAL_ERROR: {
            message = QObject::tr("提交作业失败(系统错误)");
            if (action == "clone") {
                message = QObject::tr("克隆作业失败(系统错误)");
            }
        }        
            break;
        case SUBMIT_TASK_MONEY_NOT_ENOUGH: {
            message = QObject::tr("提交作业失败(您的余额不足!)");
            if (action == "clone") {
                message = QObject::tr("克隆作业失败(您的余额不足!)");
            }
        }          
            break;
        case SUBMIT_TASK_SUBMIT_DENIED:
            message = QObject::tr("正在提交作业...");
            break;
        case SUBMIT_TASK_NETWORK_ERROR:
            message = QObject::tr("提交作业时发生网络错误,正在重试...").arg(code);
            break;
        case SUBMIT_TASK_FRAME_ANALYSE_FAILED:
            message = QObject::tr("提交作业失败(帧解析失败),请删除后重新提交");
            break;
        case HTTP_ERROR_TASK_JSON_IS_INVALID:
        case HTTP_ERROR_TASK_JSON_IS_EMPTY:
            message = QObject::tr("配置文件解析失败,请删除后重新提交");
            break;
        case HTTP_ERROR_RENDER_FRAME_ANALYZE_FAIL:
            message = QObject::tr("配置文件解析失败,帧解析失败");
            break;
        case HTTP_ERROR_ANALYSE_TOO_MANY_FRAMES:
            message = QObject::tr("渲染帧数超出限制");
            break;
        case HTTP_ERROR_TEST_FRAMES_ILLEGAL:
            message = QObject::tr("测试帧不合法");
            break;
        case HTTP_ERROR_RENDER_CAMARA_NOT_EXIST:
            message = QObject::tr("配置文件解析失败,渲染相机不存在");
            break;
        default: {
            if (subRet != nullptr) {
                QString errorText = LangTranslator::RequstErrorI18n(subRet->content);
                if (!errorText.isEmpty()) {
                    message = QObject::tr("提交作业失败,正在重试... (%1)").arg(errorText);
                    if (action == "clone") {
                        message = QObject::tr("克隆作业失败,正在重试... (%1)").arg(errorText);
                    }
                } else {
                    message = QObject::tr("提交作业失败 (错误码 %1)").arg(code);
                    if (action == "clone") {
                        message = QObject::tr("克隆作业失败 (错误码 %1)").arg(code);
                    }
                }
                    
            }
        }
           break;
        }

        QString msg = QString("[%1] %2").arg(taskId).arg(message);
        MSGBOXPARAM *param = new MSGBOXPARAM;
        param->content = msg;
        param->action = action;

        this->Response(UM_MESSAGE_FLOAT, MSGBOX_TYPE_SUBMIT_ERROR, (intptr_t)param);
    }

    if (subRet != nullptr) {
        delete subRet;
        subRet = nullptr;
    }
}

void CMainWindow::OnUpLoadAndDownLoadError(intptr_t wParam, intptr_t lParam)
{
    QString tip;
    int nErrorType = wParam;
    switch (nErrorType) {
    case UPLOAD_TASK_FILE_ERROR:
        tip = QObject::tr("上传任务出现错误的文件。任务号为 %1").arg(lParam);
        break;
    case DOWNLOAD_TASK_FILE_ERROR:
        tip = QObject::tr("下载任务出现错误的文件。任务号为 %1").arg(lParam);
        break;
    case UNABLE_OBTAIN_SERVER_ADDR:
        tip = QObject::tr("无法获取传输服务器地址...");
        break;
    case FAILED_CONNECT_SERVER:
        tip = QObject::tr("与传输接入服务器建立连接失败...");
        break;
    case DISCONNECT_SERVER:
        tip = QObject::tr("与传输服务器断开连接...");
        break;
    default:
        break;
    }

    SetErrorMessage(tip);
}

void CMainWindow::OnSpeedLimitChanged(intptr_t wParam, intptr_t lParam)
{
    CTaskCenterManager::Inst()->limitTransSpeed(wParam, lParam);
}

void CMainWindow::OnLocalProjectPathChanged(intptr_t wParam, intptr_t lParam)
{
    wchar_t * path = (wchar_t *)lParam;
    CTaskCenterManager::Inst()->downloadPathChanged(path);
}

void CMainWindow::OnLoadFileFinished(intptr_t wParam, intptr_t lParam)
{
    CTaskCenterManager::Inst()->loadFileFinished(wParam);
}

void CMainWindow::OnMessageCompleted(intptr_t wParam, intptr_t lParam)
{
    QString msg;
    t_taskid jobId = lParam;

    switch (wParam) {
    case UM_UPLOAD_COMPLETED:
        this->onUploadTaskFinished(jobId);
        break;
    case UM_DOWNLOAD_COMPLETED:
        msg = QObject::tr("作业 %1 下载全部完成").arg(jobId);
        m_jobListViewPage->refresh();
        break;
    case UM_DOWNLOAD_COMPLETED_LOST:
        msg = QObject::tr("作业 %1 下载完成, 但是有丢失帧").arg(jobId);
        break;
    case UM_RENDER_COMPLETED:
        break;
    case UM_SUBMIT_SUCCESS:
        msg = QObject::tr("作业 %1 提交成功").arg(jobId);
        m_jobListViewPage->refresh();
        SetToastMessage(msg);
        break;
    case UM_CLONE_SUCCESS:
        msg = QObject::tr("作业 %1 克隆成功").arg(jobId);
        m_jobListViewPage->refresh();
        SetToastMessage(msg);
        break;
    case UM_ANALYZE_COMPLETED:
        break;
    default:
        break;
    }

    this->showTrayMessage(msg);
}

void CMainWindow::OnUpdateClient(intptr_t wParam, intptr_t lParam)
{
    UpdateManagerInstance.performUpdate(UPDATE_MANUAL_TRIGGER, 0);
}

bool CMainWindow::StartTask(t_taskid taskId, char subFrom /*= 1*/, void *pData /*null*/)
{
    Q_UNUSED(pData)

    LOGFMTI("[MainWindow] Start Task: %d", taskId);
    JobStateMgr::Inst()->NewTaskEventRet(taskId);

    JobState* jobState = JobStateMgr::Inst()->LookupTaskstate(taskId);
    if (jobState == nullptr) {
        LOGFMTE("[MainWindow] Start Task %d failed, state is null.", taskId);
        return false;
    }

    TaskElementPtr task = CTaskCenterManager::Inst()->addNewTask(taskId);
    if (task == nullptr) {
        LOGFMTE("[MainWindow] Start Task addNewTask %d failed.", taskId);
        this->Response(UM_MESSAGE_FLOAT, MSGBOX_TYPE_ADDTASK_FAILED, (intptr_t)taskId);
        return false;
    }

    if (subFrom == CMD_SUBMIT_SCRIPT) {
        LOGFMTI("[MainWindow] Create upload Task: %d", taskId);
        CTaskCenterManager::Inst()->createUploadState(taskId);
        CTaskCenterManager::Inst()->uploadTask(MyConfig.userSet.id, taskId);
    } else {
        LOGFMTI("[MainWindow] Create upload Task: %d", taskId);
        CTaskCenterManager::Inst()->uploadTask(MyConfig.userSet.id, taskId);
    }

    return true;
}

void CMainWindow::SetErrorMessage(const QString &message)
{
    if(message.isEmpty())
        return;

    new ToastWidget(ui->widget_title, message, QColor("#e25c59"));
}

void CMainWindow::SetToastMessage(const QString & message)
{
    if (message.isEmpty())
        return;

    new ToastWidget(ui->widget_title, message, QColor("#13cb75"));
}

void CMainWindow::onRecvNotifyMsg(const QList<NoticeContent> contents)
{
    QList<NoticeContent> rollingMsg;
    QList<NoticeContent> warningMsg;
    foreach(NoticeContent content, contents) {
        if (content.notifyMode == AfterLogin && content.notifyType == Publish) {
            rollingMsg.append(content);
        } else if (content.notifyMode == AfterLogin && content.notifyType == Warning) {
            warningMsg.append(content);
        }
    }

    // 显示滚动消息
    this->showRollingMsg(rollingMsg);

    // 检查内容是否发生改变
    warningMsg = m_noticeMgr->checkContentIsChanged(warningMsg);
    // 显示警告消息
    foreach(NoticeContent content, warningMsg) {
        QPointer<NoticePublishWnd> noticeWnd(new NoticePublishWnd(this));
        connect(noticeWnd, SIGNAL(sendDialogRet(int)), this, SLOT(onRecvNoticeWndRet(int)));
        noticeWnd->setNoticeContent(content);
        noticeWnd->execNotice();
        noticeWnd->deleteLater();
    }
}

void CMainWindow::onRecvNoticeWndRet(int)
{

}

void CMainWindow::showRollingMsg(const QList<NoticeContent> msgs)
{
    // 滚动通知
    if (!msgs.isEmpty()) {
        ui->widget_rollNews->show();
        ui->widget_news->addMovingContents(msgs);
        return;
    }

    ui->widget_rollNews->hide();
    ui->widget_news->stopAnimation();
}

void CMainWindow::onLoginException(intptr_t wParam, intptr_t lParam)
{
    Q_UNUSED(wParam)
    Q_UNUSED(lParam)

    // 给出提示信息
    Util::ShowMessageBoxError(tr("账号异常，请重新登录"));
    // 注销登录
    onLogout(0, 0);
}
void CMainWindow::onRecvResponse(QSharedPointer<ResponseHead> response)
{
	if (response == Q_NULLPTR)
		return;

	int type = response->cmdType;
	switch (type) {

	case RESTAPI_CHECK_IMAGE_POS_MD5: {
		if (response->code == HTTP_ERROR_SUCCESS) {
			QSharedPointer<TaskOperateResponse> resp = qSharedPointerCast<TaskOperateResponse>(response);

		}
		break;


	}
	case RESTAPI_USER_BALANCE: {
		QSharedPointer<BalanceResponse> balance = qSharedPointerCast<BalanceResponse>(response);
		if (!JobStateMgr::Inst()->checkDownloadFlag(0)) {
			m_jobListViewPage->cancelAllDownloads();
		}
		break;
	}

	case RESTAPI_JOB_CREATE: {
		QSharedPointer<JobCreateResponse> jobs = qSharedPointerCast<JobCreateResponse>(response);
		if (jobs->createType == REND_CMD_TASK_ID) {
			if (jobs->code == HTTP_ERROR_SUCCESS) {
				if (jobs->taskids.count() > 0) {
					m_taskIds = jobs->taskids;
					m_taskIdsAlias = jobs->taskIdAlias;
					m_currTaskId = m_taskIds[0];
					m_currTaskAlias = m_taskIdsAlias[0];
					ui->stackedWidget_task->setCurrentIndex(TASK_PAGE_IMAGE);
					updateStackRenderPage(RENDER_PAGE_PROJECT);
					// 创建jobState
					JobStateMgr::Inst()->NewTaskEventRet(m_currTaskId);
				}
			}
			else if (jobs->code == HTTP_ERROR_USER_BALANCE_NOT_ENOUGH) {
				SetErrorMessage(QObject::tr("余额不足, 请充值后再试！"));
			}
			else if (jobs->code == HTTP_ERROR_USER_ACCOUNT_NOT_ACTIVE) {
				SetErrorMessage(QObject::tr("账户未激活，请联系客服！"));
			}
			else if (jobs->code == HTTP_ERROR_USER_INFO_NOT_EXIST) {
				SetErrorMessage(QObject::tr("用户信息不存在，请联系客服！"));
			}
			else if (jobs->code == HTTP_ERROR_USER_AUTH_STATUS_INVALID) {
				SetErrorMessage(QObject::tr("当前用户未完成实名认证,无法提交作业！"));
			}
			else {
				SetErrorMessage(QObject::tr("作业创建失败"));
			}

			showLoading(false);
		}
		break;
	}

	case RESTAPI_USER_INFO: {
		if (response->code == HTTP_ERROR_SUCCESS) {
			QSharedPointer<UserInfoResponse> userInfo = qSharedPointerCast<UserInfoResponse>(response);
			MyConfig.accountSet.authStatus = userInfo->authStatus;
			MyConfig.accountSet.realnameStatus = userInfo->realnameStatus;
			// 是否禁用下载
			if (!JobStateMgr::Inst()->checkDownloadFlag(0)) {
				m_jobListViewPage->cancelAllDownloads();
			}
		}
		break;
	}
	case RESTAPI_TASK_QUERYPUBLISH: {
		if (response->code == HTTP_ERROR_SUCCESS) {
			QSharedPointer<PublishItemsResponse> publishlist = qSharedPointerCast<PublishItemsResponse>(response);
			
			ui->stackedWidget_task->setCurrentIndex(TASK_PAGE_PUBLISH);
			ui->tableView->setStyleSheet("background-color:#0D0C17");
			ui->tableView->clear();
		//	ui->tableView->setStyleSheet("QLayout::setContentsMargins(0, 0, 0, 0)");
			for (int i = 0; i < publishlist->items.size(); i++)
			{
				int row = 0;
				int col = 0;
				PublistItemWidget *pWidget = new PublistItemWidget(publishlist->items.at(i), this);
				//QSharedPointer<PublistItemWidget> pWidget = QSharedPointer<PublistItemWidget>(new PublistItemWidget(publishlist->items.at(i)));// new PublistItemWidget(publishlist->items.at(i), ui->tableView);
				//pWidget->setStyleSheet(QString::fromUtf8("border:1px solid blue"));
				//pWidget->resize(248, 204);
				//pWidget->setMaximumSize(248, 204);
				pWidget->resize(248, 204);// setFixedSize(248.204);
				pWidget->setStyleSheet("background-color:#1F1F27;width:248px;height:204px;border-radius:6px");
				//pWidget->label_image
				connect(pWidget, SIGNAL(itemDetail(PublishItem *)), this, SLOT(onShowDetail(PublishItem *)));

				QListWidgetItem *pItem = new QListWidgetItem;
				if(i == 0)
					pItem->setData(Qt::UserRole, 0);
				  pItem->setTextAlignment(Qt::AlignLeft);//设置居中显示
				pItem->setSizeHint(QSize(248, 210));//设一项的大小
				ui->tableView->addItem(pItem);
				ui->tableView->setItemWidget(pItem, pWidget);
				//ui->tableView->sortItems(Qt::DescendingOrder);//自动排序
				ui->tableView->setResizeMode(QListWidget::Adjust);
				ui->tableView->setSpacing(18);
			//	pItem->setData(Qt::UserRole, 0);
				//ui->tableView->setModelColumn(4);
				//ui->tableView->setStyleSheet("margin-left:24px;margin-top:24px;margin-right:24px;margin-bottom:24px");
				ui->tableView->setViewMode(QListView::IconMode);
				//QListWidgetItem * item = new QListWidgetItem(ui->tableView);
				//ui->tableView->setItemWidget(item, pWidget);
			}
		}
			break;
	}
	case RESTAPI_TASK_CREATEPUBLISH: {
		if (response->code == HTTP_ERROR_SUCCESS) {
			QSharedPointer<CreatePublish> addpublish = qSharedPointerCast<CreatePublish>(response);

			InitCloudPublish();
			ui->stackedWidget->setCurrentIndex(PUBLISH_PAGE_SUCC);
			InitPublishSuccess(*addpublish);
		}
		break;
	}
	case RESTAPI_TASK_DETELEPUBLISH: {
		if (response->code == HTTP_ERROR_SUCCESS) {
			new ToastWidget(ui->widget_title, QObject::tr("取消发布成功"), QColor("#6B6CFF"),2000);
			//删除后重新请求列表
			HttpCmdManager::getInstance()->getPublishList(NULL, NULL, 0,this);		
		}
		break;
	}
	case RESTAPI_TASK_QUERYPUBTASK: {
		if (response->code == HTTP_ERROR_SUCCESS) {	
			QSharedPointer<PublishTaskResponse> taskpublish = qSharedPointerCast<PublishTaskResponse>(response);
			//ui->verticalStackedWidget->setCurrentIndex(PUBLISH_PAGE_SUCC);		
			if (m_detailTimer)
			{
				connect(m_detailTimer, &QTimer::timeout, this, &CMainWindow::onPageDetail);
				m_detailTimer->start(100);
				m_detailTimer->setSingleShot(true);
			}
			InitPublishTask(*taskpublish);	
		}
		break;
	}
	case RESTAPI_TASK_UPLOADPIC: {
		if (response->code == HTTP_ERROR_SUCCESS) {
			QSharedPointer<UploadPic> uploadpic = qSharedPointerCast<UploadPic>(response);
			m_serverPic = uploadpic->picurl;		
		}
		break;
	}
	case RESTAPI_TASK_STOPTASK: {
		if (response->code == HTTP_ERROR_SUCCESS) {
			QTime dieTime = QTime::currentTime().addMSecs(500);
			while (QTime::currentTime() < dieTime)
			{
				QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
			}			
			HttpCmdManager::getInstance()->getPublishTask(m_currentId, this);

			//QSharedPointer<UploadPic> uploadpic = qSharedPointerCast<UploadPic>(response);
			//m_serverPic = uploadpic->picurl;
		}
		break;
	}
	case RESTAPI_TASK_CHECKID: {
		if (response->code == HTTP_ERROR_SUCCESS) {
			QSharedPointer<CheckId> checkid = qSharedPointerCast<CheckId>(response);
			if (checkid->isDuplicate == 0)
			{
				ui->stackedWidget_task->setCurrentIndex(TASK_PAGE_RENDER);
				ui->stackedWidget_render->setCurrentIndex(RENDER_PAGE_PUBLISH);
				ui->stackedWidget->setCurrentIndex(PUBLISH_PAGE_SUM);
				updateStackRenderPage(RENDER_PAGE_PUBLISH, true);
			}
			else
			{
				new ToastWidget(ui->widget_title, tr("该任务已经发布，请重新选择"), QColor("#6B6CFF"), 3000);
				on_pushButton_4_clicked();
			}
		}
		break;
	}
	case RESTAPI_JOB_FRAME_RENDER_INFO: {
		if (response->code == HTTP_ERROR_SUCCESS) {
			QSharedPointer<TaskFrameRenderingInfoResponse> task = qSharedPointerCast<TaskFrameRenderingInfoResponse>(response);
			InitKsTask(*task);
			InitKsTask1(*task);
		}
		break;
	}
	case RESTAPI_JOB_SUBMIT: {
		QSharedPointer<TaskSubmitResponse> subResp = qSharedPointerCast<TaskSubmitResponse>(response);
		if (subResp->option == "AT") {
			if (response->code == HTTP_ERROR_SUCCESS) {
				ui->stackedWidget_AT->setCurrentIndex(AT_PAGE_RENDER);

				jobStateMachine();
				// TODO
				ui->progressBarAT->setValue(0);
				m_atRenderTimer->start(1000 * 5);
			}
		}
		else if (subResp->option == "rebuild") {
			if (response->code == HTTP_ERROR_SUCCESS) {
				ui->stackedWidget_rebuild->setCurrentIndex(REBUILD_PAGE_WAITING);

				jobStateMachine();
				// TODO
				ui->progressBarRebuild->setValue(0);
				m_rebuildRenderTimer->start(1000 * 5);
			}
		}

		break;
	}

	case RESTAPI_JOB_QUERY_TASK_INFO:
	{
		QSharedPointer<MaxTaskItemsResponse> tasklist = qSharedPointerCast<MaxTaskItemsResponse>(response);
		if (tasklist->code == HTTP_ERROR_SUCCESS) {
			for (int i = 0; i < tasklist->items.count(); i++) {
				const MaxTaskItem* info = tasklist->items[i];
				if (m_currTaskId != info->id)
					continue;
				jobStateMachine();
				renderPageSwitch();
			}
		}
		break;
	}

	case RESTAPI_JOB_TASK_STOP: {
		if (response->code == HTTP_ERROR_SUCCESS) {
			QSharedPointer<TaskOperateResponse> resp = qSharedPointerCast<TaskOperateResponse>(response);
			if (resp->option == "AT") {
				m_atRenderTimer->stop();
				ui->stackedWidget_AT->setCurrentIndex(AT_PAGE_HOME);
			}
			else if (resp->option == "rebuild" || resp->option == "render") {
				m_rebuildRenderTimer->stop();
				ui->stackedWidget_rebuild->setCurrentIndex(REBUILD_PAGE_HOME);
			}
		}
		break;
	}

	case RESTAPI_JOB_RESUBMIT_CCAT: {
		if (response->code == HTTP_ERROR_SUCCESS) {
			ui->stackedWidget_AT->setCurrentIndex(AT_PAGE_RENDER);
		}
		break;
	}
	}
}


void CMainWindow::OnNewsContentResponse(intptr_t wParam, intptr_t lParam)
{
    static PromoteNewsWnd* sNewsWnd = nullptr;
    NewsContent *content = (NewsContent*)lParam;
    if (content == nullptr || content->title.isEmpty())
        return;

    // 计算内容和标题的hash值
    QString qcontent = content->title + content->content;
    QString md5;
    QByteArray ba, bb;
    QCryptographicHash md(QCryptographicHash::Md5);
    ba.append(qcontent);
    md.addData(ba);
    bb = md.result();
    md5.append(bb.toHex());

    if (sNewsWnd != NULL && md5 == sNewsWnd->getContentMd5())
        return;

    QPointer<PromoteNewsWnd> newsWnd(new PromoteNewsWnd());
    newsWnd->showPromoteNews(*content);
    sNewsWnd = newsWnd;

    delete content;
    content = nullptr;
}

void CMainWindow::OnNeedUserLogin(intptr_t wParam, intptr_t lParam)
{
    Q_UNUSED(wParam)
    Q_UNUSED(lParam)

    Util::ShowMessageBoxError(QObject::tr("登录信息已过期，请重新登录客户端"));

    onLogout(0, 0);
}

void CMainWindow::AutoUpdateAfterQuit()
{
//     QString updater = QCoreApplication::applicationDirPath() + "/RayUpdate" + APP_SUFFIX;
//     updater.detach();
//     QString appPath = QCoreApplication::applicationDirPath() + "/";
//     appPath.detach();

	QString updater = QCoreApplication::applicationDirPath() + "/RayUpdate" + APP_SUFFIX;
	updater.detach();
	updater = "\"" + QDir::toNativeSeparators(updater) + "\"";
	QString appPath = QCoreApplication::applicationDirPath() + "/";
	appPath.detach();
	appPath = "\"" + QDir::toNativeSeparators(appPath) + "\"";

    if (!QFile::exists(updater) && m_isLogout) {
        CAppInstance::GetApp()->Logout();
        return;
    }

    QProcess *process = new QProcess;
    QStringList params;
 
//     params << appPath << QCoreApplication::applicationName() + APP_SUFFIX << STRFILEVERSION << QString::number(2) << "update" << LocalConfig::getInstance()->defaultProduct
//         << WSConfig::getInstance()->getWSVersion() << WSConfig::getInstance()->getWSHostUrl() + "desktop/qclient/" << (m_isLogout ? "restart" : "exit");

	params << appPath << QCoreApplication::applicationName() + APP_SUFFIX << STRFILEVERSION << QString::number(2) << "update" << LocalConfig::getInstance()->defaultProduct
		<< WSConfig::getInstance()->getWSVersion() << WSConfig::getInstance()->getWSHostUrl() + "desktop/dycloud/" << (m_isLogout ? "restart" : "exit");

    process->start(updater, params, QIODevice::ReadOnly);

    bool isStart = process->waitForStarted(1000);

    LOGFMTI("Update app lunch state is %s", isStart ? "true" : "false");
}

void CMainWindow::LogoutAccountLogin()
{
	QString appPath = QCoreApplication::applicationDirPath() + "/" + QCoreApplication::applicationName() + APP_SUFFIX;
	appPath.detach();
	appPath = "\"" + QDir::toNativeSeparators(appPath) + "\"";

	QProcess *process = new QProcess;
	process->start(appPath, QIODevice::ReadOnly);
	bool isStart = process->waitForStarted(1000);
	LOGFMTI("Log out of account and log in again %s", isStart ? "true" : "false");
}

void CMainWindow::DownloadAutoUpdateApp()
{
    QString appName = QString("%1%2").arg("RayUpdate").arg(APP_SUFFIX);
    QPointer<QDownloadManager> downmgr(new QDownloadManager);
    downmgr->downloadAutoUpdater(appName);
}

void CMainWindow::OnUserBalanceNotify(QSharedPointer<BalanceResponse> balance)
{
    if(balance == Q_NULLPTR)
        return;

    if(!JobStateMgr::Inst()->checkDownloadFlag(0)) {
        m_jobListViewPage->cancelAllDownloads();
    }
}

void CMainWindow::OnTaskOperatorSuccess(intptr_t wParam, intptr_t lParam)
{
    QString msg = QString("%1 %2").arg(wParam).arg(tr("任务操作成功"));
    new ToastWidget(ui->widget_title, msg, QColor("#e25c59"));
}

QPointer<QMenu> CMainWindow::createCSMenu()
{
    QMenu *csMenu = new QMenu(this);
    csMenu->setStyleSheet("QMenu::indicator:exclusive:checked{image: url(:/view/images/view/selected.png);}");

    m_csPhoneAction = new QAction(csMenu);
    m_csQQAction = new QAction(csMenu);
    m_csHelpAction = new QAction(csMenu);
    // csMenu->addAction(m_csPhoneAction);
    csMenu->addAction(m_csQQAction);
    csMenu->addAction(m_csHelpAction);

    return csMenu;
}

void CMainWindow::on_btnCancelJob_clicked()
{
    backToHome();
}

void CMainWindow::on_btnCreateJob_clicked()
{
    // 隐藏下载页面
    this->onHideDownloadView();
    // 隐藏详情页
    if (m_detailViewPage != nullptr) {
        m_detailViewPage->hide();
    }
    // 隐藏日志
    if (m_floatLogPanel != nullptr) {
        m_floatLogPanel->hide();
    }

    resetWidgetConfig();
	ui->lineEditProject->clear();
	ui->lineEdit_range_file_3->clear();
	ui->lineEdit_range_file->clear();
    ui->stackedWidget_task->setCurrentIndex(TASK_PAGE_PROJECT);
	
}

void CMainWindow::on_btnJobNext_clicked()
{
    if (ui->lineEditProject->text().trimmed().isEmpty()) {
        new ToastWidget(ui->widget_main, QObject::tr("请输入项目名称"), QColor("#e25c59"), 3000);
        return;
    }
    ui->label_project_name->setText(ui->lineEditProject->text());

    m_projectType = ui->comboProjectType->currentData().toInt();
    if (m_projectType < 0) {
        new ToastWidget(ui->widget_main, QObject::tr("项目类型选择有误，请重新选择"), QColor("#e25c59"), 3000);
        return;
    }

    if (m_projectType == enBlockProject) {
        ui->widget_add_block->show();
    } else if (m_projectType == enImageProject) {
        ui->widget_add_block->hide();
    }
  
    HttpCmdManager::getInstance()->createJobIds(1, REND_CMD_TASK_ID, this);

    showLoading(true);

}

void CMainWindow::on_btnPublish_clicked()
{
	HttpCmdManager::getInstance()->getPublishList(NULL, NULL, 0, this);
}

void CMainWindow::on_btn_sucpublish_clicked()
{
	ui->stackedWidget_task->setCurrentIndex(TASK_PAGE_PUBLISH);
	HttpCmdManager::getInstance()->getPublishList(NULL, NULL, 0, this);

}

void CMainWindow::on_btnReturn_clicked()
{
	//JobStateMgr::Inst()->SendRefreshTasklist(-1, 100);
	ui->stackedWidget_task->setCurrentIndex(TASK_PAGE_HOME);
}

void CMainWindow::on_btn_returnlist_clicked()
{
	ui->stackedWidget_task->setCurrentIndex(TASK_PAGE_PUBLISH);
}

void CMainWindow::on_pushButton_4_clicked()
{
	PublishWindow share;
	if (share.exec() == QDialog::Accepted)
	{
		int keId = share.getselectedId();
		//判断id是否已经发布过
		HttpCmdManager::getInstance()->checkId(keId, this);
		//renderPageSwitch(true, RENDER_PAGE_PUBLISH);
		
		m_keId = keId;
		// initSideBar();
		//onTaskItemDoubleClicked(87343430);
	}
}

void CMainWindow::on_btn_autobuild_clicked()
{
	QString strRand;
	int length = 4;
	QString strTmp = "1234567890QWERTYUIOPASDFGHJKLZXCVBNM";
	struct timeb timer;
	ftime(&timer);
	srand(timer.time * 1000 + timer.millitm);//毫秒种子
	for (int i = 0; i < length; i++)
	{
		int j = rand() % 35;
		strRand += strTmp.at(j);
	}
	ui->lineEdit_key->setText(strRand);
}

void CMainWindow::on_btn_addpic_clicked()
{

}

void CMainWindow::on_btn_publishok_clicked()
{
	PublishItem * item = new PublishItem;
	item->id = m_keId;
	item->publishName = ui->lineEdit->text();
	if (item->publishName.size() == 0 || item->publishName.size() > 50)
	{
		new ToastWidget(ui->widget_title, QObject::tr("发布名称字数须在1-50之间"), QColor("#6B6CFF"),2000);
		return;
	}
	item->type = enum PublishItem::modelType(ui->comboBox->currentIndex());
	item->des = ui->textEdit_des->toPlainText();
	item->isNeedKey = ui->checkBox_pwd->isChecked() ? 1 : 0;
	if (ui->checkBox_pwd->isChecked() && ui->lineEdit_key->text() != NULL)
	{
		item->secretKey = ui->lineEdit_key->text();
	}
	else
	{
		item->secretKey = "1111";
	}
	//item->expiredTime
	// ui->radioButton_3->setText("自定义");
	if (ui->radioButton_3->isChecked())
	{
		item->expiredTime = ui->spinBox_time->text().toInt();
		//item->expiredTime = ui->radioButton_3->text().toInt();
	}
	else if (ui->radioButton_1->isChecked())
	{
		item->expiredTime = 30;
	}
	else
	{
		item->expiredTime = 7;
	}
	item->coverImgUrl = m_serverPic;
	item->concurrentCount = ui->lineEdit_2->text().toInt();

	HttpCmdManager::getInstance()->createPublish(*item, this);
}

void CMainWindow::on_btn_publishcancel_clicked()
{
	if (Util::ShowMessageBoxQuesion(tr("是否保存发布数据")) == QDialog::Rejected)
	{
		InitCloudPublish();
	}
	ui->stackedWidget_task->setCurrentIndex(TASK_PAGE_PUBLISH);
	//updateStackRenderPage(RENDER_PAGE_HOME, true);
	//HttpCmdManager::getInstance()->getPublishList(NULL, NULL, 0, this);
}

void CMainWindow::on_btn_fmpic_clicked()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("上传封面文件"), QString(), "*.jpg *.png *.jpeg");
	if (fileName.isNull()) return;
	ui->btn_fmpic->setStyleSheet(QString("border-image: url(%1)").arg(fileName));
	//ui->btn_fmpic->setIcon(QIcon(fileName));
    ui->btn_fmpic->setIcon(QIcon(""));
	//ui->btn_fmpic->setIcon(QIcon(""));
	m_publishUrl = fileName;
	//ui->btn_fmpic->setStyleSheet(QString("background-image: url(%1)").arg("E:/img3.jpg"));
	HttpCmdManager::getInstance()->uploadPic(m_publishUrl, this);
	
	/*ui->btn_fmpic->setPixmap(QPixmap(item.localImgUrl));
	ui->label_85->setScaledContents(true);
	QPalette aa = ui->btn_fmpic->palette();
	QImage aaa(fileName);
	QImage bbb = aaa.scaled(ui->btn_fmpic->width(), ui->btn_fmpic->height(), Qt::IgnoreAspectRatio);
	aa.setBrush(QPalette::Window, QBrush(bbb));
	ui->btn_fmpic->setPalette(aa);*/

	//ui->btn_fmpic->setIcon(QIcon(QString("background-image: url(%1)").arg(fileName)));
	//ui->btn_fmpic->adjustSize();

}

void CMainWindow::onShowDetail(PublishItem *item)
{
	m_item = *item;
	m_detailTimer = new QTimer(this);
	//加载task任务
	HttpCmdManager::getInstance()->getPublishTask(item->id, this);
	//加载详情页
	InitPublishDetail(*item);
	//ui->stackedWidget_task->setCurrentIndex(TASK_PAGE_PUBDETAIL);
	m_currentId = item->id;
	//connect(m_detailTimer, &QTimer::timeout, [=]() {
	//ui->stackedWidget_task->setCurrentIndex(TASK_PAGE_PUBDETAIL);
	//	m_detailTimer->stop();
	//});
	
}

void CMainWindow::onPageDetail()
{
	ui->stackedWidget_task->setCurrentIndex(TASK_PAGE_PUBDETAIL);
	m_detailTimer->stop();
	m_detailTimer = NULL;
}

void CMainWindow::on_btn_pubcopy_clicked()
{
	QString link = ui->textEdit_lianjie->toPlainText();
	QString code = ui->label_81->text();
#ifdef FOXRENDERFARM
	QString copyStr = QObject::tr("Link：%1 \r\nPassword：%2\r\n--Share from AvicaCloud users").arg(link).arg(code);
#else
	QString copyStr = QObject::tr("链接：%1 \r\n提取码：%2\r\n--来自大雁云用户的分享").arg(link).arg(code);
#endif    

	QClipboard *copy = QGuiApplication::clipboard();
	if (copy != nullptr) {
		copy->setText(copyStr);
	}

	new ToastWidget(ui->widget_title, QObject::tr("复制链接成功"), QColor("#6B6CFF"));
}

void CMainWindow::on_btnCopy_clicked()
{
	QString link = ui->lineEdit_3->text();
	QString code = ui->label_68->text();
#ifdef FOXRENDERFARM
	QString copyStr = QObject::tr("Link：%1 \r\nPassword：%2\r\n--Share from AvicaCloud users").arg(link).arg(code);
#else
	QString copyStr = QObject::tr("链接：%1 \r\n提取码：%2\r\n--来自大雁云用户的分享").arg(link).arg(code);
#endif    

	QClipboard *copy = QGuiApplication::clipboard();
	if (copy != nullptr) {
		copy->setText(copyStr);
	}

	new ToastWidget(ui->widget_title, QObject::tr("复制链接成功"), QColor("#6B6CFF"));
}

void CMainWindow::on_btn_editdata_clicked()
{
	QString current = ui->label_83->text();
	QDate date = QDate::fromString(current, "yyyy-MM-dd");
	//获得当前当作最小时间
	//qint64 publishtime = m_item.publishTime;
	//QDateTime mintime = QDateTime::fromMSecsSinceEpoch(publishtime);
	//QString dd = QString("%1").arg(mintime.toString("yyyy-MM-dd"));
	//QDate mindate = QDate::fromString(dd, "yyyy-MM-dd");
	QDate currentdate = QDate::currentDate();
	QDate mindate = currentdate.addDays(1);
	QDate maxdate = date.addDays(30);
	if (!m_widget)
	{
		m_widget = new QCalendarWidget();		
	}
#ifdef FOXRENDERFARM
	m_widget->setLocale(QLocale::English);
#else
	m_widget->setLocale(QLocale::Chinese);
#endif    	//m_widget->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint | Qt::Tool | Qt::FramelessWindowHint);
	//m_widget->show();
	m_widget->setDateRange(mindate, maxdate);
	m_widget->setMaximumSize(QSize(270, 240));
//	m_widget->resize(300, 280);
	m_widget->setGeometry(QRect(QPoint(1000,600),QSize(300,280)));
	m_widget->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
	m_widget->setSelectedDate(date);
    m_widget->show();
//	m_widget->verticalHeaderFormat()->hide();
	connect(m_widget, SIGNAL(selectionChanged()), this, SLOT(onUpdateTime()));
}

void CMainWindow::on_btn_cancelpub_clicked()
{
	if (Util::ShowMessageBoxQuesion(tr("确定取消发布吗")) == QDialog::Rejected)
		return;
	HttpCmdManager::getInstance()->deletePublish(m_currentId, this);
}

void CMainWindow::on_pushButton_search_clicked()
{
	QString value = ui->lineEdit_publishname->text();
	int index = ui->comboBox_states->currentIndex();
	//QString states = QString(index);
	//if (index == 2)
	//{
	//	states = "";
	//}
	HttpCmdManager::getInstance()->getPublishList(value, NULL ,index , this);
}

void CMainWindow::onPublishStatusChanged(int index)
{
	//int index = ui->comboBox_states->currentIndex();
	/*if (index == 2)
	{
		HttpCmdManager::getInstance()->getPublishList(NULL, NULL, NULL, this);
	}
	else
	{*/
		//QString status = QString(index);
		HttpCmdManager::getInstance()->getPublishList(NULL, NULL, index, this);
	//}
	
}

void CMainWindow::onCheckSlot3(bool state)
{
	if (state)
	{
		ui->spinBox_time->show();
		ui->label_87->show();
	}
	else
	{
		ui->spinBox_time->hide();
		ui->label_87->hide();
	}
}

void CMainWindow::onSelectTime(int day)
{
	// ui->radioButton_3->setText(QString("%1").arg(day));
	if(ui->radioButton_3->isChecked())
		ui->radioButton_3->setText(QString("%1").arg(day));
	//m_validDate = day;
}

void CMainWindow::onStopTask()
{
	if (Util::ShowMessageBoxQuesion(tr("确定中止任务吗")) == QDialog::Rejected)
		return;
	//QPushButton *pushButton_ = dynamic_cast<QPushButton*>(this->sender());
	QPushButton *pushButton_ = (QPushButton *)sender();
	if (NULL == pushButton_)
		return;
	// 获取按钮的x坐标和y坐标
	int x = pushButton_->frameGeometry().x();
	int y = pushButton_->frameGeometry().y();
	// 根据按钮的x和y坐标来定位对应的单元格
	QModelIndex index = ui->tableWidget_task->indexAt(QPoint(x, y));
	// 获取该按钮所在表格的行号和列号
	int row = index.row();
	int column = index.column();
	//int text = ui->tableWidget_task->item(row, 1)->text().toInt();
	int text = ui->tableWidget_task->item(row, 1)->text().toInt();
	HttpCmdManager::getInstance()->stopPublishTask(text, this);
}

void CMainWindow::onUpdateDate(const QDateTime &time)
{
	//每次修改时间后updatepublish
	qint64 aa = time.toTime_t() ;
	qint64 cc = aa * 1000.0;
	QString bb = time.toString();
	//PublishItem item;
	m_item.expiredTime = cc;

	/*item.publishName = ui->label_74->text();
	item.id = m_currentId;
	item.coverImgUrl = m_serverPic;
	item.des = ui->label_78->text();
	item.concurrentCount = ui->label_87->text().toInt();*/
	//item.expiredTime = static_cast<qint64>(time);

	HttpCmdManager::getInstance()->updatePublish(m_item, this);

}

void CMainWindow::on_refreshTable(int col)
{
	if (m_sortUpDown)		//sortUpDown为自定义的一个bool变量，表示升序还是降序
	{
		ui->tableWidget_task->sortItems(col, Qt::AscendingOrder);
		m_sortUpDown = false;
	}
	else
	{
		ui->tableWidget_task->sortItems(col, Qt::DescendingOrder);
		m_sortUpDown = true;
	}
}

void CMainWindow::onUpdateTime()
{
	//if(m_widget)
	//delete m_widget;
	m_widget->close();
	//选择的日期
	QDate date = m_widget->selectedDate();
	QDateTime time = QDateTime::fromString(date.toString("yyyy-MM-dd"), "yyyy-MM-dd");
	//时间戳
	qint64 currentTime = time.toTime_t();
	qint64 aa = currentTime * 1000;

	QString cutOffDate = QString("%1").arg(time.toString("yyyy-MM-dd"));// = QString::number(QDateTime::fromString(date.toString("yyyy-MM-dd"), "yyyy-MM-dd").toTime_t());
	ui->label_83->setText(cutOffDate);

	emit updateDate(aa);
}

void CMainWindow::onUpdateRequest(qint64 time)
{
	//if (m_widget)
	//{
		//delete m_widget;
		//m_widget = NULL;
	//}
		
	m_item.expiredTime = time;

	HttpCmdManager::getInstance()->updatePublish(m_item, this);
}

void CMainWindow::onUpdateBf()
{
	int value = ui->lineEdit_bf->text().toInt();
	if (value > 0 && value < 1000)
	{
		m_item.concurrentCount = value;
		HttpCmdManager::getInstance()->updatePublish(m_item, this);
	}
	else
	{
		new ToastWidget(ui->widget_title, QObject::tr("并发限制数需要大于0且小于1000"), QColor("#6B6CFF"));
		ui->lineEdit_bf->setText(QString("%1").arg(m_item.concurrentCount));
	}
	
}

void CMainWindow::on_btnDes_clicked()
{
	//QString str = fontMetrics().elidedText(m_item.des, Qt::ElideRight, ui->label_78->width()*2.6);

	ui->label_78->setText(m_item.des);
	ui->btnDes->hide();
}

void CMainWindow::on_btnSucDes_clicked()
{
	ui->label_65->setText(m_item.des);
	ui->btnSucDes->hide();
}

void CMainWindow::on_btnParaPre_clicked()
{
	ui->stackedWidget_task->setCurrentIndex(TASK_PAGE_IMAGE);
}
void CMainWindow::on_btnParaNext_clicked()
{
	if (!assetsCheckEnable()) {
		return;
	}
	if (!checkRebulidCoordSys()) {
		Util::ShowMessageBoxError(QObject::tr("三维模型生产，不支持地理坐标系，请选择正确的坐标系！"));
		return;
	}

//	getRebuildParams(m_rebuildParam);

	ui->stackedWidget_task->setCurrentIndex(TASK_PAGE_RENDER);
	updateStackRenderPage(RENDER_PAGE_PROJECT);
	jobStateMachine();

	resetUploadCtrl();
	// 控制点view
	resetCtrlPointViewSet();
	// pos view
	resetPositionViewSet();
	// 侧边栏
	modifySideBarButtonVisible(m_projectType);

	// 如果是区块任务，则应用区块文件中的pos数据到照片
	if (m_projectType == enBlockProject) {
        JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
        if (jobstate == nullptr) return;

		applyBlockPosForPicture(jobstate);
	}
}

void CMainWindow::on_btnSubmitATBlockAT_clicked()
{
	ui->stackedWidget_project->setCurrentIndex(PROJECT_PAGE_PROJECT);

	JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
	if (jobstate == nullptr) return;

	quint64 totalPixel = 0;
	quint64 totalPics = 0;
	BaseTaskInfo *baseInfo = jobstate->getBaseTaskInfo();
	foreach(auto group, baseInfo->groups) {
		totalPixel += group->totalPixel;
		totalPics += group->pictures.count();
	}

	int state = ui->btnUpload->property("state").toInt();
	if (state == enCancelUpload) {
		if (Util::ShowMessageBoxQuesion(tr("确定取消上传影像数据？")) == QDialog::Rejected)
			return;

		ui->widget_upload_progress->hide();
		ui->widget_upload_state->setStyleSheet(".QWidget{background-color:transparent;}");
		ui->btnUpload->setText(QObject::tr("开始提交"));
		ui->btnUpload->setProperty("state", enStartUpload);

		CTaskCenterManager::Inst()->stopUploadTask(m_currTaskId);

		// 显示上一步
		ui->btnBackAssets->show();

		new ToastWidget(this, tr("项目 %1 照片上传已取消，请确认后重新提交").arg(jobstate->getProjectName()), QColor("#13cb75"));

		CGlobalHelp::Response(UM_UPDATE_TRANS_SPEED, UM_UPDATE_TRANS_SPEED_UPLOAD, 0);

	}
	else if (state == enStartUpload) {

		if (jobstate->getProjectType() == enImageProject)
		{
			quint64 ctrlPoint = baseInfo->ctrlPointInfo.itemDatas.count();
			QScopedPointer<SummaryBoxUpload> box(new SummaryBoxUpload(totalPics, totalPixel, ctrlPoint, this));
			if (box->exec() == QDialog::Rejected)
			{
				ui->widget_upload_progress->hide();
				ui->widget_upload_state->setStyleSheet(".QWidget{background-color:transparent;}");
				ui->btnUpload->setText(QObject::tr("开始提交"));
				ui->btnUpload->setProperty("state", enStartUpload);

				CTaskCenterManager::Inst()->stopUploadTask(m_currTaskId);

				// 显示上一步
				ui->btnBackAssets->show();

				new ToastWidget(this, tr("项目 %1 照片上传已取消，请确认后重新提交").arg(jobstate->getProjectName()), QColor("#13cb75"));

				CGlobalHelp::Response(UM_UPDATE_TRANS_SPEED, UM_UPDATE_TRANS_SPEED_UPLOAD, 0);
				return;
			}
		}
		else if (jobstate->getProjectType() == enBlockProject)
		{
			QScopedPointer<SummaryBoxRebuild> box(new SummaryBoxRebuild(ui->lineEditProject->text(),
				m_settingsInfo.rebuildOption.outputTypes, m_settingsInfo.rebuildOption.coordSysFull, totalPics, totalPixel, this));

			if (box->exec() == QDialog::Rejected)
			{
				ui->widget_upload_progress->hide();
				ui->widget_upload_state->setStyleSheet(".QWidget{background-color:transparent;}");
				ui->btnUpload->setText(QObject::tr("开始提交"));
				ui->btnUpload->setProperty("state", enStartUpload);

				CTaskCenterManager::Inst()->stopUploadTask(m_currTaskId);

				// 显示上一步
				ui->btnBackAssets->show();

				new ToastWidget(this, tr("项目 %1 照片上传已取消，请确认后重新提交").arg(jobstate->getProjectName()), QColor("#13cb75"));

				CGlobalHelp::Response(UM_UPDATE_TRANS_SPEED, UM_UPDATE_TRANS_SPEED_UPLOAD, 0);
				return;
			}
		}

		// 相机一致性检查
		if (baseInfo->groups.count() > 0) {
			QString defaultCamera = baseInfo->groups[0]->cameraProducter + baseInfo->groups[0]->cameraModel;
			QStringList cameraMode;
			QStringList cameraMaker;
			foreach(auto group, baseInfo->groups) {
				QString camera = group->cameraProducter + group->cameraModel;
				if (!cameraMode.contains(group->cameraModel)) {
					cameraMode.append(group->cameraModel);
				}
				if (!cameraMaker.contains(group->cameraProducter)) {
					cameraMaker.append(group->cameraProducter);
				}
			}
			if (cameraMode.size() > 1 || cameraMaker.size() > 1) {
				// Util::ShowMessageBoxError(tr("相机不一致，请检查！"));
				int res = Util::ShowMessageBoxQuesion(QObject::tr("相机不一致，请检查！"), QObject::tr("温馨提示"), QObject::tr("继续提交"), QObject::tr("放弃提交"));
				if (res == QDialog::Rejected)
					return;
			}
		}

		ui->widget_upload_progress->show();
		ui->widget_upload_state->setStyleSheet(".QWidget{background-color:#1f1f27;}");
		ui->btnUpload->setText(QObject::tr("取消上传"));
		ui->btnUpload->setProperty("state", enCancelUpload);

		m_taskIds.clear();
		m_taskIdsAlias.clear();
		m_taskIds.append(jobstate->getTaskId());
		m_taskIdsAlias.append(jobstate->getTaskIdAlias());

		// 隐藏上一步
		ui->btnBackAssets->hide();

		QtConcurrent::run([this]() {
			submitTask(this->m_taskIds, this->m_taskIdsAlias);
		});
	}
}


void CMainWindow::on_btnSubmitCancel_clicked()
{
    backToHome();
}

void CMainWindow::on_btnSubmitPreStep_clicked()
{
    resetWidgetConfig();
    ui->stackedWidget_task->setCurrentIndex(TASK_PAGE_PROJECT);
}


void CMainWindow::on_btnSubmitNextStep_clicked()
{
	if (!m_blockCheck)
	{
		showErrorMsg(CODE_ERROT_NOT_TIEPOINT_FILE);
		return;
	}

    if (m_projectType == enBlockProject) {
		JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
		BLOCK_TASK::BlockExchange *blockInfo = jobstate->getBlockExchangeInfo();
		if (blockInfo == nullptr)
		{
			new ToastWidget(this, QObject::tr("请上传区块数据"), QColor("#e25c59"), 2000);
			return;
		}
		if (blockInfo->block.photogroups.size()<1)
		{
			new ToastWidget(this, QObject::tr("区块文件中的照片组信息缺失，请检查对应区块文件"), QColor("#e25c59"), 2000);
			return;
		}
		if (ui->tableWidget_pic_group->rowCount() < 1) {
			new ToastWidget(this, QObject::tr("请添加照片组"), QColor("#e25c59"), 2000);
			return;
		}
		BLOCK_TASK::Photo * currgroup = blockInfo->block.photogroups.begin()->photos.at(0);
		if (currgroup->exifData.gps.Longitude != Null)
		{
			ui->widget_coord_edit->initCtrl(COORD_REF_PARAM_SET_KEY, currgroup->exifData.gps.Longitude.toDouble(), currgroup->exifData.gps.Latitude.toDouble());
			ui->widget_coord_edit_5->initCtrl(COORD_REF_PARAM_SET_KEY, currgroup->exifData.gps.Longitude.toDouble(), currgroup->exifData.gps.Latitude.toDouble());
		}
        ui->stackedWidget_task->setCurrentIndex(TASK_PAGE_PARA);
    } else if (m_projectType == enImageProject) {
        if (!assetsCheckEnable()) {
            return;
        }

        ui->stackedWidget_task->setCurrentIndex(TASK_PAGE_RENDER);
        updateStackRenderPage(RENDER_PAGE_PROJECT);
        jobStateMachine();
        resetUploadCtrl();
        // 控制点view
        resetCtrlPointViewSet();
        // pos view
        resetPositionViewSet();
        // 侧边栏
        modifySideBarButtonVisible(m_projectType);
    }	
}

void CMainWindow::on_btnATSubmit_clicked()
{
    int atType = 0;
    if(ui->radio_gps_pos->isChecked())
        atType = 0;
    else if(ui->radio_ctrl_point->isChecked())
        atType = 1;

    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if(jobstate == nullptr) return;

    jobstate = jobstate->getATJob();
    if(jobstate == nullptr) return;

    HttpCmdManager::getInstance()->taskSubmitAT(jobstate->getTaskId(), m_currProjectName, atType, this);


}

void CMainWindow::on_btnATCancel_clicked()
{
    if(Util::ShowMessageBoxQuesion(tr("确定取消空三计算？"), tr("警告")) == QDialog::Rejected)
        return;

    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if(jobstate == nullptr) return;

    JobState* atjobstate = jobstate->getATJob();
    if(atjobstate == nullptr) return;

    HttpCmdManager::getInstance()->taskOperatorStop(atjobstate->getTaskId(), "AT", this);
}

void CMainWindow::on_btnATShow_clicked()
{
    updateStackRenderPage(RENDER_PAGE_AT_RESULT);
}

void CMainWindow::on_btnATFailed_clicked()
{
    ui->stackedWidget_AT->setCurrentIndex(AT_PAGE_HOME);
}

void CMainWindow::on_btnRangeFile_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("选择范围文件"), QString(), "KML (*.kml)");
    if(file.isEmpty()) return;

    ui->lineEdit_range_file->setText(file);
}

void CMainWindow::on_btnRebuildSubmit_clicked()
{
	if (!checkTaskCloneCoordSys()) {
		Util::ShowMessageBoxError(QObject::tr("三维模型生产，不支持地理坐标系，请选择正确的坐标系！"));
		return;
	}

    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if(jobstate == nullptr) return;

    // 提交之前检查当前任务状态
    // 如果任务是 暂停或失败，则提示不能继续提交
    JobState *rebuildJob = jobstate->getRebuildJob();
    JobState *productJob = jobstate->getRenderJob();
    int blockSubmit = 0;
    if (rebuildJob != nullptr && (rebuildJob->IsRenderStop() || rebuildJob->getStatus() == JobState::enRenderTaskFailed)) {
        blockSubmit++;
    }
    if (productJob != nullptr && (productJob->IsRenderStop() || productJob->getStatus() == JobState::enRenderTaskFailed)) {
        blockSubmit++;
    }

    if (blockSubmit > 0) {
        Util::ShowMessageBoxInfo(tr("相关参数不支持再次提交！"), tr("提示"));
        return;
    }

#if 0
    QStringList outputTypes = ui->comboOutputType->currentText();
    QString coordSys = ui->widget_coord_edit->getCoordSystemValue();
    QString coordSysFull = ui->widget_coord_edit->getCoordSystem();
    bool lockRange = ui->checkBox_lockRange->isChecked();
    QString rangeFile = ui->lineEdit_range_file->text();
    CoordValue customOriginCoord; // 自定义原点
    if (ui->checkBox_enable_custom->isChecked()) {
        if (ui->lineEdit_x_coord->text().isEmpty()) {
            SetErrorMessage(QObject::tr("请输入自定义原点X坐标"));
            return;
        }
        if (ui->lineEdit_y_coord->text().isEmpty()) {
            SetErrorMessage(QObject::tr("请输入自定义原点Y坐标"));
            return;
        }
        if (ui->lineEdit_z_coord->text().isEmpty()) {
            SetErrorMessage(QObject::tr("请输入自定义原点Z坐标"));
            return;
        }
        customOriginCoord.coord_x = ui->lineEdit_x_coord->text().toDouble();
        customOriginCoord.coord_y = ui->lineEdit_y_coord->text().toDouble();
        customOriginCoord.coord_z = ui->lineEdit_z_coord->text().toDouble();
        customOriginCoord.enable = true;
    }

    if(outputTypes.isEmpty()) {
        SetErrorMessage(QObject::tr("请选择输出格式"));
        return;
    }

    if(coordSys.isEmpty()) {
        SetErrorMessage(QObject::tr("请选择坐标系"));
        return;
    }

    if(!lockRange && rangeFile.isEmpty()) {
        SetErrorMessage(QObject::tr("请选择范围文件"));
        return;
    }

    quint64 totalPixel = 0;
    quint64 totalPics = 0;
    BaseTaskInfo *baseInfo = jobstate->getBaseTaskInfo();
    foreach (auto group, baseInfo->groups) {
        totalPixel += group->totalPixel;
        totalPics += group->pictures.count();
    }

    // 用户自定义坐标内容
    QString userCoordContent = CProfile::Inst()->getUserCoordSystemContent(coordSys);
#else
    RebuildOptionParams rebuildParam;
    if (!m_rebuildParamWidget->getRebuildParams(rebuildParam)){
        return;
    }

    quint64 totalPixel = 0;
    quint64 totalPics = 0;
    BaseTaskInfo *baseInfo = jobstate->getBaseTaskInfo();
    foreach(auto group, baseInfo->groups) {
        totalPixel += group->totalPixel;
        totalPics += group->pictures.count();
    }

    if (jobstate->IsCloneTask()) {
        totalPics = jobstate->getPicCount().toULongLong();
        totalPixel = jobstate->getTotalPixel().toULongLong();
    }

#endif
    QScopedPointer<SummaryBoxRebuild> box(new SummaryBoxRebuild(jobstate->getProjectName(),
                                           rebuildParam.outputTypes, rebuildParam.coordSysFull, totalPics, totalPixel, this));

    if(box->exec() == QDialog::Accepted) {
        jobstate = jobstate->getRebuildJob();
        HttpCmdManager::getInstance()->taskSubmitRebuild(jobstate->getTaskId(), m_currProjectName, rebuildParam, this);
    }
}

void CMainWindow::on_btnRebuildCancel_clicked()
{
    if (ui->stackedWidget_rebuild->currentIndex() == REBUILD_PAGE_RENDER) {
        if (Util::ShowMessageBoxQuesion(tr("作业正在重建中，是否取消重建？"), tr("警告")) == QDialog::Rejected)
            return;
    } else {
        if (Util::ShowMessageBoxQuesion(tr("确定取消重建作业？"), tr("警告")) == QDialog::Rejected)
            return;
    }

    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if(jobstate == nullptr) return;
    HttpCmdManager::getInstance()->taskOperatorStop(jobstate->getTaskId(), "render", this);
    return;

    jobstate = jobstate->getRebuildJob();
    if(jobstate == nullptr) return;
    HttpCmdManager::getInstance()->taskOperatorStop(jobstate->getTaskId(), "rebuild", this);

    jobstate = jobstate->getRenderJob();
    if(jobstate == nullptr) return;
    HttpCmdManager::getInstance()->taskOperatorStop(jobstate->getTaskId(), "render", this);
}

void CMainWindow::on_btnRebuildCancel2_clicked()
{
    on_btnRebuildCancel_clicked();
}

void CMainWindow::on_btnRebuildShow_clicked()
{
    updateStackRenderPage(RENDER_PAGE_PRODUCTION);
}

void CMainWindow::on_btnRebuildFailed_clicked()
{
    // ui->stackedWidget_rebuild->setCurrentIndex(REBUILD_PAGE_HOME);
}

void CMainWindow::on_btnBacktoProject1_clicked()
{
    backToProjectHome();
}

void CMainWindow::on_btnBacktoProject2_clicked()
{
    backToProjectHome();
}

void CMainWindow::on_btnBacktoProject3_clicked()
{
    backToProjectHome();
}

void CMainWindow::on_btnBacktoProject4_clicked()
{
    backToProjectHome();
}

void CMainWindow::on_btnAddCtrlPoint_clicked()
{
    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if (jobstate == nullptr) return;

    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (btn == nullptr) return;
    int page = btn->property("nextPage").toInt();

    ui->stackedWidget_project->setCurrentIndex(page);

    if (page == PROJECT_PAGE_CP_SUBMIT) {
        this->loadContrlPointData();
    }
}

void CMainWindow::on_btnAddCtrlPoint2_clicked()
{
    ui->stackedWidget_project->setCurrentIndex(PROJECT_PAGE_CP_ADD);
}

void CMainWindow::on_btnCtrlPointView_clicked()
{
    if (this->loadContrlPointData()) {
        ui->stackedWidget_project->setCurrentIndex(PROJECT_PAGE_CP_VIEW);
    }
}

void CMainWindow::on_btnCtrlPointPreStep_clicked()
{
    this->showLoading(false);
    ui->stackedWidget_project->setCurrentIndex(PROJECT_PAGE_CP_ADD);
}

void CMainWindow::on_btnCtrlPointSubmit_clicked()
{
    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if (jobstate == nullptr) return;

    jobstate->setSubmitCtrlPoint(true);

    // 加载ctrl point文件
    if (this->loadContrlPointData()) {
        ui->stackedWidget_project->setCurrentIndex(PROJECT_PAGE_CP_SUBMIT);
    }
}

void CMainWindow::on_btnAddPos_clicked()
{
    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if (jobstate == nullptr) return;

    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (btn == nullptr) return;
    int page = btn->property("nextPage").toInt();

    ui->stackedWidget_project->setCurrentIndex(page);

    if (page == PROJECT_PAGE_POS_SUBMIT) {
        this->loadPositionInfoData();
    }
}

void CMainWindow::on_btnPosView_clicked()
{
    if (this->loadPositionInfoData()) {
        ui->stackedWidget_project->setCurrentIndex(PROJECT_PAGE_POS_VIEW);
    }    
}

void CMainWindow::on_btnPosPreStep_clicked()
{
    this->showLoading(false);

    ui->stackedWidget_project->setCurrentIndex(PROJECT_PAGE_POS_ADD);
}

void CMainWindow::on_btnPosSubmit_clicked()
{
    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if (jobstate == nullptr) return;

    // 检查pos数据与影像数据是否匹配
    QMap<QString, QStringList> notMatchs;
    if (!isPosInfoMatchedImageData(jobstate, notMatchs)) {
        Util::ShowMessageBoxError(QObject::tr("POS数据与影像信息不匹配!"), QObject::tr("温馨提示"));
        return;
    }

    jobstate->setSubmitPosInfo(true);

    // 加载pos文件
    if (this->loadPositionInfoData()) {
        ui->stackedWidget_project->setCurrentIndex(PROJECT_PAGE_POS_SUBMIT);
    }
}


void CMainWindow::on_btnUpload_clicked()
{
//	if (ui->checkBox_blockAT->isChecked())
//	{
//		ui->stackedWidget_project->setCurrentIndex(PROJECT_PAGE_KML_ADD);
//		return;
//	}

    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if(jobstate == nullptr) return;

	quint64 totalPixel = 0;
	quint64 totalPics = 0;
	BaseTaskInfo *baseInfo = jobstate->getBaseTaskInfo();
	foreach(auto group, baseInfo->groups) {
		totalPixel += group->totalPixel;
		totalPics += group->pictures.count();
	}
	

	if (jobstate->IsCloneTask()) {
		totalPics = jobstate->getPicCount().toULongLong();
		totalPixel = jobstate->getTotalPixel().toULongLong();
	}
	if (!getRebuildParams(m_settingsInfo.rebuildOption)) {
		return;
	}
	int state = ui->btnUpload->property("state").toInt();
    if (state == enCancelUpload) {
        if(Util::ShowMessageBoxQuesion(tr("确定取消上传影像数据？")) == QDialog::Rejected)
            return;

        ui->widget_upload_progress->hide();		
        ui->widget_upload_state->setStyleSheet(".QWidget{background-color:transparent;}");
        ui->btnUpload->setText(QObject::tr("开始提交"));
        ui->btnUpload->setProperty("state", enStartUpload);

        CTaskCenterManager::Inst()->stopUploadTask(m_currTaskId);

        // 显示上一步
        ui->btnBackAssets->show();

        new ToastWidget(this, tr("项目 %1 照片上传已取消，请确认后重新提交").arg(jobstate->getProjectName()), QColor("#13cb75"));

        CGlobalHelp::Response(UM_UPDATE_TRANS_SPEED, UM_UPDATE_TRANS_SPEED_UPLOAD, 0);

    } else if (state == enStartUpload) {

		if (jobstate->getProjectType() == enImageProject && totalPics > 20000) //大于20000张影像进行空三分块
		{
			//		QPointer<CMessageBoxATBlock> msgBox(new CMessageBoxATBlock(this));
			CMessageBoxATBlock* msgBox = new CMessageBoxATBlock(this);
			msgBox->setTitle(tr("分块选择"));
			msgBox->setWindowFlags(msgBox->windowFlags() ^ Qt::WindowStaysOnTopHint);
			msgBox->show();
			m_atBlock = true;
			if (msgBox->exec())
			{
				m_atBlockMerage = msgBox->getMerageOption();
				ui->stackedWidget_project->setCurrentIndex(PROJECT_PAGE_KML_ADD);
				return;
			}
			else
			{
				return;
			}

		}

		if (jobstate->getProjectType() == enImageProject)
		{
			quint64 ctrlPoint = baseInfo->ctrlPointInfo.itemDatas.count();
			QScopedPointer<SummaryBoxUpload> box(new SummaryBoxUpload(totalPics, totalPixel, ctrlPoint, this));
			if (box->exec() == QDialog::Rejected)
			{
				ui->widget_upload_progress->hide();
				ui->widget_upload_state->setStyleSheet(".QWidget{background-color:transparent;}");
				ui->btnUpload->setText(QObject::tr("开始提交"));
				ui->btnUpload->setProperty("state", enStartUpload);

				CTaskCenterManager::Inst()->stopUploadTask(m_currTaskId);

				// 显示上一步
				ui->btnBackAssets->show();

				new ToastWidget(this, tr("项目 %1 照片上传已取消，请确认后重新提交").arg(jobstate->getProjectName()), QColor("#13cb75"));

				CGlobalHelp::Response(UM_UPDATE_TRANS_SPEED, UM_UPDATE_TRANS_SPEED_UPLOAD, 0);
				return;
			}
		}
		else if (jobstate->getProjectType() == enBlockProject)
		{
			QScopedPointer<SummaryBoxRebuild> box(new SummaryBoxRebuild(ui->lineEditProject->text(),
				m_settingsInfo.rebuildOption.outputTypes, m_settingsInfo.rebuildOption.coordSysFull, totalPics, totalPixel, this));

			if (box->exec() == QDialog::Rejected)
			{
				ui->widget_upload_progress->hide();
				ui->widget_upload_state->setStyleSheet(".QWidget{background-color:transparent;}");
				ui->btnUpload->setText(QObject::tr("开始提交"));
				ui->btnUpload->setProperty("state", enStartUpload);

				CTaskCenterManager::Inst()->stopUploadTask(m_currTaskId);

				// 显示上一步
				ui->btnBackAssets->show();

				new ToastWidget(this, tr("项目 %1 照片上传已取消，请确认后重新提交").arg(jobstate->getProjectName()), QColor("#13cb75"));

				CGlobalHelp::Response(UM_UPDATE_TRANS_SPEED, UM_UPDATE_TRANS_SPEED_UPLOAD, 0);
				return;
			}
		}

        // 弹框提醒
        quint64 totalPixel = 0;
        quint64 totalPics = 0;
        BaseTaskInfo *baseInfo = jobstate->getBaseTaskInfo();

        // 相机一致性检查
        if(baseInfo->groups.count() > 0) {
            QString defaultCamera = baseInfo->groups[0]->cameraProducter + baseInfo->groups[0]->cameraModel;
            QStringList cameraMode;
            QStringList cameraMaker;
            foreach(auto group, baseInfo->groups) {
                QString camera = group->cameraProducter + group->cameraModel;
                if (!cameraMode.contains(group->cameraModel)) {
                    cameraMode.append(group->cameraModel);
                }
                if (!cameraMaker.contains(group->cameraProducter)) {
                    cameraMaker.append(group->cameraProducter);
                }
            }
            if (cameraMode.size() > 1 || cameraMaker.size() > 1) {
                // Util::ShowMessageBoxError(tr("相机不一致，请检查！"));
                int res = Util::ShowMessageBoxQuesion(QObject::tr("相机不一致，请检查！"), QObject::tr("温馨提示"), QObject::tr("继续提交"), QObject::tr("放弃提交"));
                if (res == QDialog::Rejected)
                    return;
            }
        }

        ui->widget_upload_progress->show();
        ui->widget_upload_state->setStyleSheet(".QWidget{background-color:#1f1f27;}");
        ui->btnUpload->setText(QObject::tr("取消上传"));
        ui->btnUpload->setProperty("state", enCancelUpload);

        m_taskIds.clear();
        m_taskIdsAlias.clear();
        m_taskIds.append(jobstate->getTaskId());
        m_taskIdsAlias.append(jobstate->getTaskIdAlias());

        // 隐藏上一步
        ui->btnBackAssets->hide();

        QtConcurrent::run([this]() {
            submitTask(this->m_taskIds, this->m_taskIdsAlias);
        });
    } 

}

void CMainWindow::onCSMenuTriggered(QAction *action)
{
    if(action == m_csPhoneAction) {
        QString csUrl = WSConfig::getInstance()->get_link_address("url_official_website");
        QString csFullUrl = QString("%1/%2").arg(csUrl);// .arg(STRFILEVERSION);
        QDesktopServices::openUrl(QUrl(csFullUrl));
    } else if(action == m_csQQAction) {
        QString csUrl = WSConfig::getInstance()->get_link_address("url_customer_service");
#ifdef FOXRENDERFARM
		QString csFullUrl = csUrl;   //海外客户端skype直接跳到官网 20211227
#else
		QString csFullUrl = QString("%1%2").arg(csUrl).arg(STRFILEVERSION);
#endif        
        QDesktopServices::openUrl(QUrl(csFullUrl));
    } else if(action == m_csHelpAction) {
        QString helpUrl = WSConfig::getInstance()->get_link_address("url_user_guide");
        QDesktopServices::openUrl(QUrl(helpUrl));
    }
}

void CMainWindow::OnShowProcessView(intptr_t wParam, intptr_t lParam)
{
    t_taskid taskid = wParam;
    JobState *jobState = JobStateMgr::Inst()->LookupTaskstate(taskid);
    if(jobState == nullptr) return;

    m_detailViewPage->show();
    m_detailViewPage->raise();
    m_detailViewPage->showView(jobState);

    m_jobListViewPage->itemSelected(taskid);
}

void CMainWindow::cleanClientCache()
{
    if (LocalSetting::getInstance()->getCleanCache()) {
        LOGFMTI("[CMainWindow] Start clean client cache!");
        // 调用 fixt.exe 进行清理
        QProcess *fixt = new QProcess(this);
        QString params = QString("%1 %2 %3").arg("analyzeCache").arg("c:/renderfarm/project").arg(LocalSetting::getInstance()->getCacheCleanLimitDatys());

        QString fixtpath = QString("\"%1/Fixit.exe\" %2").arg(QApplication::applicationDirPath()).arg(params);

        fixt->startDetached(fixtpath);
        if (!fixt->waitForStarted(1000)) {
            LOGFMTE("[CMainWindow] Start fixit app failed, %s!", qPrintable(fixt->readAllStandardError()));
        }
    }
}

void CMainWindow::showLoading(bool show)
{
    ui->widget_page->setEnabled(!show);

    if(show) {
        int sx = (ui->widget_page->width() - m_busyIcon->width()) / 2;
        int sy = (ui->widget_page->height() - m_busyIcon->height()) / 2;
        m_busyIcon->move(sx, sy);

        m_busyIcon->show();
        m_busyIcon->startAnimation();
    } else {
        m_busyIcon->hide();
        m_busyIcon->stopAnimation();
    }
}

void CMainWindow::initSideBar()
{
    m_sideBarGroup = new QButtonGroup(this);
    m_sideBarGroup->setExclusive(true);
    connect(m_sideBarGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
            [=](int id) {

        if (m_atRenderTimer != nullptr)m_atRenderTimer->stop();
        if (m_rebuildRenderTimer != nullptr)m_rebuildRenderTimer->stop();

        ui->stackedWidget_render->setCurrentIndex(id);
        if(id == RENDER_PAGE_HOME) {
            ui->stackedWidget_task->setCurrentIndex(TASK_PAGE_HOME);
            ui->stackedWidget_render->setCurrentIndex(RENDER_PAGE_PROJECT);
            ui->radio_gps_pos->setChecked(true);
			if (m_rebuildParamWidget != nullptr)
			{
				JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
				if (!jobstate)
					return;
				BLOCK_TASK::BlockExchange *blockInfo = jobstate->getBlockExchangeInfo();
				if (blockInfo != nullptr)
				{
					BLOCK_TASK::Photo * currgroup = blockInfo->block.photogroups.begin()->photos.at(0);
					if (currgroup->exifData.gps.Longitude != Null)
					{
						m_rebuildParamWidget->resetView(currgroup->exifData.gps.Longitude.toDouble(), currgroup->exifData.gps.Latitude.toDouble());
					}
					else
					{
						m_rebuildParamWidget->resetView();
					}
				}
				else
				{
					m_rebuildParamWidget->resetView();
				}
			}
			// 刺像控点数据重置
			if (ui->widget_surveysPoints->getKrGcp()) { ui->widget_surveysPoints->deleteKrGcp(); }
//			ui->widget_at_3dview_client->clearData();
			ui->widget_surveysPoints->clearSurveyData();
			ui->tableWidget_at_overview->clearContents();
			ui->tableWidget_cameragroup->clearContents();
			ui->tableWidget_controlpointinfo->clearContents();
			ui->tableWidget_EveryCpInfo->clearContents();
			ui->tableWidget_tiepointsinfo->clearContents();
			ui->tableWidget_PointsPreImageInfo->clearContents();
        } else if (id == RENDER_PAGE_AT) {
			JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
			if (!jobstate)
				return;
			t_taskid at_taskid;
			QHash<qint64, JobState*> m_at_task = jobstate->getChildJobState();
			foreach(JobState* job, jobstate->getChildJobState()) {
				if (job->IsParentItem()) {
					foreach(JobState* subJob, job->getChildJobState()) {
						if (!job->m_taskType.compare("Triangulation"))
						{
							at_taskid = job->m_taskId;
						}
					}
				}
				else {
					if (!job->m_taskType.compare("Triangulation"))
					{
						at_taskid = job->m_taskId;
					}
				}
			}
			HttpCmdManager::getInstance()->qureyTaskFrameRenderingInfo(at_taskid, 1, 50, this);
          //  ui->stackedWidget_task->setCurrentIndex(RENDER_PAGE_AT);    
            renderPageSwitch(false, RENDER_PAGE_AT);
        } else if(id == RENDER_PAGE_AT_RESULT) {
            ui->tabWidget_AT->blockSignals(true);
            ui->stackedWidget_ATResult->setCurrentIndex(ATRESULT_PAGE_HOME);

			m_buttonSurveyPoint->setEnabled(true);
			ui->tabWidget_AT->removeTab(2);
            ui->tabWidget_AT->setCurrentIndex(AT_RESULT_VIEW_CP);

            JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
            if(jobstate == nullptr) return;
            if(jobstate->isATFinished()) {


#if 0  //网页端浏览空三结果           
				QString atUrl = getDetailViewUrl(jobstate, "url_atpreview");
                ui->widget_at_3dview->loadUrl(atUrl);
                ui->widget_at_3dview->hide();
                QString tips = QObject::tr("<style>a{text-decoration: none}</style>空三预览请前往"
                    "<a href='%1'; style='color:#675ffe; font-size:24px;font-weight:bold;'>浏览器</a>查看").arg(atUrl);
                ui->label_open_at_result->setText(tips);
                ui->label_open_at_result->setOpenExternalLinks(true);
                
                // 初始化空三结果
                QString atSurveyUrl = getDetailViewUrl(jobstate, "url_atsurvey");
                ui->widget_survey_view->loadUrl(atSurveyUrl);
#endif
                this->onTabWidgetATChanged(ui->tabWidget_AT->currentIndex());				
                ui->tabWidget_AT->blockSignals(false);

            }
        } else if(id == RENDER_PAGE_RECONSTRUCTION) {
            renderPageSwitch(false, RENDER_PAGE_RECONSTRUCTION);
        } else if(id == RENDER_PAGE_PRODUCTION) {
            JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
            if(jobstate == nullptr) return;
            if(jobstate->isTaskFinished()) {
                QString url = getDetailViewUrl(jobstate, "url_3dpreview");
                ui->widget_3dview->loadUrl(url);
                showShareResultButtons();
            }
		}
		else if (id == RENDER_PAGE_PUBLISH) {
			ui->stackedWidget_render->setCurrentIndex(RENDER_PAGE_PUBLISH);
			
			//InitCloudPublish();
		}
    });
    {
        auto button = new TabbarWidget(QObject::tr("返回首页"), QIcon(QPixmap(":/toolbar/images/toolbar/home.png")));
#ifdef FOXRENDERFARM
		button->setFixedWidth(78);
#endif
        m_sideBarGroup->addButton(button, RENDER_PAGE_HOME);
        ui->widget_sidebar->layout()->addWidget(button);
    }

    {
        auto button = new TabbarWidget(QObject::tr("项目数据"), QIcon(QPixmap(":/toolbar/images/toolbar/data.png")));
#ifdef FOXRENDERFARM
		button->setFixedWidth(78);
#endif
        m_sideBarGroup->addButton(button, RENDER_PAGE_PROJECT);
        ui->widget_sidebar->layout()->addWidget(button);
        button->setChecked(true);
    }

    {
        auto button = new TabbarWidget(QObject::tr("空三处理"), QIcon(QPixmap(":/toolbar/images/toolbar/at.png")));
#ifdef FOXRENDERFARM
		button->setFixedWidth(78);
#endif
        m_sideBarGroup->addButton(button, RENDER_PAGE_AT);
        ui->widget_sidebar->layout()->addWidget(button);
    }

    {
        auto button = new TabbarWidget(QObject::tr("空三成果"), QIcon(QPixmap(":/toolbar/images/toolbar/at-result.png")));
#ifdef FOXRENDERFARM
		button->setFixedWidth(78);
#endif
        m_sideBarGroup->addButton(button, RENDER_PAGE_AT_RESULT);
        ui->widget_sidebar->layout()->addWidget(button);
    }

    {
        auto button = new TabbarWidget(QObject::tr("三维重建"), QIcon(QPixmap(":/toolbar/images/toolbar/reconstruction.png")));
#ifdef FOXRENDERFARM
		button->setFixedWidth(78);
#endif
        m_sideBarGroup->addButton(button, RENDER_PAGE_RECONSTRUCTION);
        ui->widget_sidebar->layout()->addWidget(button);
    }

    {
        auto button = new TabbarWidget(QObject::tr("项目成果"), QIcon(QPixmap(":/toolbar/images/toolbar/production.png")));
#ifdef FOXRENDERFARM
		button->setFixedWidth(78);
#endif
        m_sideBarGroup->addButton(button, RENDER_PAGE_PRODUCTION);
        ui->widget_sidebar->layout()->addWidget(button);
    }

	{
		auto button = new TabbarWidget(QObject::tr("云发布"), QIcon(QPixmap(":/toolbar/images/toolbar/publish.png")));
#ifdef FOXRENDERFARM
		button->setFixedWidth(78);
#endif
		m_sideBarGroup->addButton(button, RENDER_PAGE_PUBLISH);
		ui->widget_sidebar->layout()->addWidget(button);
	}

    resetWidgetConfig();
	ui->lineEditProject->clear();
	ui->lineEdit_range_file_3->clear();
	ui->lineEdit_range_file->clear();
}

void CMainWindow::on_btnSelPic_clicked()
{
    QPoint pos(ui->widget_selpic->mapToGlobal(QPoint(0, 0)).x(),
        ui->widget_selpic->mapToGlobal(QPoint(0, 0)).y() + ui->widget_selpic->height());

    m_selPicMenu->exec(pos);

    QMouseEvent event(QEvent::MouseButtonRelease, QPoint(0, 0), Qt::LeftButton, 0, 0);
    QApplication::sendEvent(ui->widget_selpic, &event);
}

void CMainWindow::on_btnDelPic_clicked()
{
    if(Util::ShowMessageBoxQuesion(tr("确定移除此照片(组)？"), tr("警告")) == QDialog::Rejected)
        return;

    // group
    QTableWidget* picGtroup = ui->tableWidget_pic_group;

    PictureGroup group;

    QWidget *w = qobject_cast<QWidget *>(sender()->parent());
    if(w) {
        int crow = picGtroup->indexAt(w->pos()).row();
        if(crow >= 0) {
            QTableWidgetItem *item = picGtroup->item(crow, 0);
            if (item != nullptr) {
                group = item->data(Qt::UserRole).value<PictureGroup>();           
            }
        }
        resetTableAndListWidget();
    }

    // 删除内存数据
    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if (jobstate != nullptr) {
        BaseTaskInfo* info = jobstate->getBaseTaskInfo();
        if (info == nullptr) return;
        for (int i = 0; i < info->groups.size(); i++) {
            if (info->groups.at(i)->groupId == group.groupId) {
                info->groups.removeAt(i);
            }
        }

        // 重新加载所有表格
        if (!info->groups.isEmpty()) {
            ui->comboBox_pic_group->clear();
            loadPictureGroup(info->groups);
            loadPicturesInfo(info->groups.at(0)->pictures);
        }
    }
}
QPointer<QMenu> CMainWindow::createSelPicMenu()
{
    QMenu *selPicMenu = new QMenu(this);
    selPicMenu->setStyleSheet("QMenu::indicator:exclusive:checked{image: url(:/view/images/view/selected.png);}");

    m_selFileAction = new QAction(selPicMenu);
    m_selDirAction = new QAction(selPicMenu);

    selPicMenu->addAction(m_selFileAction);
    selPicMenu->addAction(m_selDirAction);

    return selPicMenu;
}

void CMainWindow::onSelPicTriggered(QAction *action)
{
    if (action == m_selFileAction) {
        selectFileAction();
    } else if (action == m_selDirAction) {
        selectDirAction();
    }
}

void CMainWindow::selectFileAction()
{
    QStringList filters;
    filters << "JPG files (*.jpg)"
            << "JPEG files (*.jpeg)"
            << "TIFF files (*.tiff)"
            << "PNG files (*.png)"
            << "Any files (*)";
    QStringList files = RBHelper::getSelectedFiles(filters);
    if (files.isEmpty()) return;

	if (checkIllegalpath(files[0])) return;

	//判断是否存在中文路径
	if (files[0].contains(QRegExp("[\\x4e00-\\x9fa5]+")))
	{
		Util::ShowMessageBoxError(QObject::tr("存在中文路径，请修改中文路径后在提交数据！"));
		return;
	}

    LoadPictureInfoThread* loadJob = new LoadPictureInfoThread(files, false, true);
    connect(loadJob, SIGNAL(loadPicInfoFinished(int)), this, SLOT(onLoadPictureInfoFinished(int)));
    loadJob->start();
    // this->showLoading(true);
    ui->widget_page->setEnabled(false);

    TipsToastWidget* toast = new TipsToastWidget(ui->widget_page, QObject::tr("照片读取中，请稍后..."), 16, QColor("#1F1F27"));
    connect(this, SIGNAL(loadPictureInfoFinished()), toast, SLOT(hideToast()));
}

void CMainWindow::selectDirAction()
{
    int ret = Util::ShowMessageBoxQuesion(QObject::tr("是否要包含子目录？"), QObject::tr("选择文件夹"));
    QString path = RBHelper::getSelectedFolder();
    if (path.isEmpty())return;

	if (checkIllegalpath(path)) return;
	//判断是否存在中文路径
	if (path.contains(QRegExp("[\\x4e00-\\x9fa5]+")))
	{
		Util::ShowMessageBoxError(QObject::tr("存在中文路径，请修改中文路径后提交数据！"));
		return;
	}

    QStringList pathList; pathList << path;

    LoadPictureInfoThread* loadJob = new LoadPictureInfoThread(pathList, ret == QDialog::Accepted);
    connect(loadJob, SIGNAL(loadPicInfoFinished(int)), this, SLOT(onLoadPictureInfoFinished(int)));
    loadJob->start();
    // this->showLoading(true);
    ui->widget_page->setEnabled(false);

    TipsToastWidget* toast = new TipsToastWidget(ui->widget_page, QObject::tr("照片读取中，请稍后..."), 16, QColor("#1F1F27"));
    connect(this, SIGNAL(loadPictureInfoFinished()), toast, SLOT(hideToast()));

    LOGFMTI("[CMainWindow] Start parse picture files!");
}

void CMainWindow::onLoadPictureInfoFinished(int state)
{
    LOGFMTI("[CMainWindow] Parse picture files finished!");

    emit loadPictureInfoFinished();
    // this->showLoading(false);
    ui->widget_page->setEnabled(true);
    new ToastWidget(ui->widget_title, QObject::tr("影像数据解析完成!"), QColor("#13cb75"), 5000);

    QList<PictureGroup*> groups;
    if (sender() != NULL) {
        LoadPictureInfoThread* loadJob = qobject_cast<LoadPictureInfoThread*>(sender());
        disconnect(loadJob, SIGNAL(loadPicInfoFinished(int)), this, SLOT(onLoadPictureInfoFinished(int)));
        groups = loadJob->getPictureGroups();
        loadJob->deleteLater();       
    }

    if (groups.isEmpty())
        return;

    // groups
    loadPictureGroup(groups);

    // pictures
    loadPicturesInfo(groups[0]->pictures);

    // 填充基本信息
    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if (jobstate == nullptr) return;

    BaseTaskInfo* info = jobstate->getBaseTaskInfo();
    info->projectName = m_currProjectName;

//  info->jobType = ui->comboProjectType->currentData().toInt();  //"影像数据"放开

    info->jobType = m_projectType;
    if (info->jobType < 0) {
        info->jobType = enBlockProject;
    }
    info->groups.append(groups);
    info->blocks.append(BlockInfo(ui->label_block_path->text()));
    jobstate->updataBaseTaskInfo(info);
}

void CMainWindow::onLoadAtReportInfoFinished(int state)
{
    QObject* obj = sender();
    if (obj != nullptr) {
        LoadAtReportInfoThread *info = qobject_cast<LoadAtReportInfoThread*>(obj);
        disconnect(info, &LoadAtReportInfoThread::loadAtReportInfoFinished, this, &CMainWindow::onLoadAtReportInfoFinished);
        info->deleteLater();
    }

    ui->widget_page->setEnabled(true);

    if(state == CODE_SUCCESS) {
        initATReportPage();
    } else {
        SetErrorMessage(tr("空三报告文件缺失"));
    }
}

void CMainWindow::onLoadAtPointCloudDataInfoFinish(int state)
{
    QObject* obj = sender();
    if (obj != nullptr) {
        LoadAtDataInfoThread *threadAtViewInfo = qobject_cast<LoadAtDataInfoThread*>(obj);
        disconnect(threadAtViewInfo, &LoadAtDataInfoThread::loadAtPointCloudInfoFinished, this, &CMainWindow::onLoadAtPointCloudDataInfoFinish);
        threadAtViewInfo->deleteLater();
    }
    ui->widget_page->setEnabled(true);

	if (state == CODE_SUCCESS) {
        JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
        if (jobstate == nullptr) return;

        QString at_data_file = RBHelper::getTempPath(QString::number(jobstate->getATJob()->getTaskId()));
        at_data_file.append(QDir::separator());
        at_data_file.append("at_data.json");		

        ui->widget_at_3dview_client->loadAtData(at_data_file.toStdString());

		return;

	} else {
		SetErrorMessage(tr("加载空三点云成果数据失败"));
	
	}
}


void CMainWindow::loadPictureGroup(const QList<PictureGroup*>& groups)
{
    QTableWidget* picGroup = ui->tableWidget_pic_group;
    QListWidget* pg = ui->listWidget_pg;    // listWidget_pg
    QTableWidget* camera = ui->tableWidget_camera; // tableWidget_camera
    picGroup->blockSignals(true);
    pg->blockSignals(true);
    camera->blockSignals(true);

    for (int i = 0; i < groups.size(); i++) {
        PictureGroup* group = groups.at(i);
        if(group == NULL) continue;

        /////////////////////////////start pic group//////////////////////////////////
        int row = picGroup->rowCount();
        picGroup->insertRow(row);

        // name
        QString pname = group->groupName;
        QTableWidgetItem *name = new QTableWidgetItem(pname);
        name->setTextAlignment(Qt::AlignCenter);
        name->setData(Qt::UserRole, QVariant::fromValue<PictureGroup>(*group));
        picGroup->setItem(row, 0, name);

        // count
        QTableWidgetItem *count = new QTableWidgetItem(QString::number(group->picsCount));
        count->setTextAlignment(Qt::AlignCenter);
        picGroup->setItem(row, 1, count);

        // resolution
        QTableWidgetItem *resItem = new QTableWidgetItem(group->totalPixelStr);
        resItem->setTextAlignment(Qt::AlignCenter);
        picGroup->setItem(row, 2, resItem);

        // path
        QTableWidgetItem *path = new QTableWidgetItem(group->groupPath);
        path->setTextAlignment(Qt::AlignCenter);
        picGroup->setItem(row, 3, path);

        QWidget* pWidget = new QWidget(picGroup);
        QPushButton* btn_del = new QPushButton();
        connect(btn_del, &QPushButton::clicked, this, &CMainWindow::on_btnDelPic_clicked);
        btn_del->setStyleSheet("QPushButton{border:none;background-color:transparent;font-size:12px;color:#675ffe;}"
                               "QPushButton:hover{color:#7879ff;}");
        btn_del->setText(tr("删除"));
        // btn_del->setStyleSheet("QPushButton{border-image:url(:/images/delete.png);border:none;background-color:transparent;max-height:15px;max-width:15px;padding:0px;}"
        //                        "QPushButton:hover{border-image:url(:/images/delete.png);} ");
        // btn_del->setToolTip(tr("删除"));
        QHBoxLayout* pLayout = new QHBoxLayout(pWidget);
        pLayout->addWidget(btn_del);
        pLayout->setAlignment(Qt::AlignCenter);
        pLayout->setContentsMargins(0, 0, 0, 0);
        pWidget->setLayout(pLayout);
        picGroup->setCellWidget(row, 4, pWidget);
        //////////////////////////////start pg////////////////////////////////////

        QListWidgetItem* pgItem = new QListWidgetItem(pname);
        pgItem->setTextAlignment(Qt::AlignCenter);
        pgItem->setData(Qt::UserRole, QVariant::fromValue<PictureGroup>(*group));
        pg->addItem(pgItem);

        //////////////////////////////start camera table//////////////////////////

        row = camera->rowCount();
        camera->insertRow(row);
        
        // name
        QTableWidgetItem *cname = new QTableWidgetItem(pname);
        cname->setTextAlignment(Qt::AlignCenter);
        cname->setFlags(cname->flags() & ~Qt::ItemIsEditable);
        cname->setData(Qt::UserRole, QVariant::fromValue<PictureGroup>(*group));
        camera->setItem(row, 0, cname);

        // count
        QTableWidgetItem *ccount = new QTableWidgetItem(QString::number(group->picsCount));
        ccount->setTextAlignment(Qt::AlignCenter);
        ccount->setFlags(ccount->flags() & ~Qt::ItemIsEditable);
        camera->setItem(row, 1, ccount);

        // cameraProducter
        QTableWidgetItem *producter = new QTableWidgetItem(group->cameraProducter);
        producter->setTextAlignment(Qt::AlignCenter);
        producter->setFlags(producter->flags() & ~Qt::ItemIsEditable);
        camera->setItem(row, 2, producter);

        // model
        QTableWidgetItem *model = new QTableWidgetItem(group->cameraModel);
        model->setTextAlignment(Qt::AlignCenter);
        model->setFlags(model->flags() & ~Qt::ItemIsEditable);
        camera->setItem(row, 3, model);

        // sensorSize
        QTableWidgetItem *sensorSize = new QTableWidgetItem(group->sensorSize);
        sensorSize->setTextAlignment(Qt::AlignCenter);
        sensorSize->setFlags(sensorSize->flags() ^ Qt::ItemIsEditable);
        camera->setItem(row, 4, sensorSize);

        // focalLength
        QTableWidgetItem *focalLength = new QTableWidgetItem(group->focalLength);
        focalLength->setTextAlignment(Qt::AlignCenter);
        sensorSize->setFlags(sensorSize->flags() ^ Qt::ItemIsEditable);
        camera->setItem(row, 5, focalLength);

        // totalPixelStr
        QTableWidgetItem *totalPixelStr = new QTableWidgetItem(group->totalPixelStr);
        totalPixelStr->setTextAlignment(Qt::AlignCenter);
        totalPixelStr->setFlags(totalPixelStr->flags() & ~Qt::ItemIsEditable);
        camera->setItem(row, 6, totalPixelStr);

        // groupPath
        QTableWidgetItem *groupPath = new QTableWidgetItem(group->groupPath);
        groupPath->setTextAlignment(Qt::AlignCenter);
        groupPath->setFlags(groupPath->flags() & ~Qt::ItemIsEditable);
        camera->setItem(row, 7, groupPath);

        ////////////////////////////////pic group combobox///////////////////////////
        ui->comboBox_pic_group->addItem(pname, group->groupId);
    }

    picGroup->blockSignals(false);
    pg->blockSignals(false);
    camera->blockSignals(false);
}

void CMainWindow::loadPicturesInfo(const QList<PictureInfo>& pictures)
{
    QTableWidget* pictureTab = ui->tableWidget_pictures;
    pictureTab->blockSignals(true);
    pictureTab->clearContents();
    pictureTab->setRowCount(0);

    for (int row = 0; row < pictures.size(); row++) {
        PictureInfo info = pictures.at(row);
        pictureTab->insertRow(row);
    
        // name
        QString pname = info.picName;
        QTableWidgetItem *name = new QTableWidgetItem(pname);
        name->setTextAlignment(Qt::AlignCenter);
        name->setData(Qt::UserRole, QVariant::fromValue<PictureInfo>(info));
        pictureTab->setItem(row, 0, name);

        // size
        QString size = RBHelper::ramUsageToString(info.picSize);
        QTableWidgetItem *sizeItem = new QTableWidgetItem(size);
        sizeItem->setTextAlignment(Qt::AlignCenter);
        pictureTab->setItem(row, 1, sizeItem);

        // picLongitude
        //QTableWidgetItem *picLongitude = new QTableWidgetItem(info.picLongitudeChar + info.picLongitudeRef);
        QTableWidgetItem *picLongitude = new QTableWidgetItem(QString::number(info.picLongitude, 'g', ALTITUDE_DECIMAL_LENGTH));
        picLongitude->setTextAlignment(Qt::AlignCenter);
        pictureTab->setItem(row, 2, picLongitude);

        // picLatitude
        //QTableWidgetItem *picLatitude = new QTableWidgetItem(info.picLatitudeChar + info.picLatitudeRef);
        QTableWidgetItem *picLatitude = new QTableWidgetItem(QString::number(info.picLatitude, 'g', ALTITUDE_DECIMAL_LENGTH));
        picLatitude->setTextAlignment(Qt::AlignCenter);
        pictureTab->setItem(row, 3, picLatitude);

        // picElevation
        QTableWidgetItem *picElevation = new QTableWidgetItem(QString::number(info.picAltitude, 'g', ALTITUDE_DECIMAL_LENGTH));
        picElevation->setTextAlignment(Qt::AlignCenter);
        pictureTab->setItem(row, 4, picElevation);
    }

    pictureTab->blockSignals(false);
}

void CMainWindow::onItemSelectionChanged()
{
#if 0
    QTableWidget* picGtroup = ui->tableWidget_pic_group;
    QTableWidgetItem *item = picGtroup->currentItem();
    if (item == nullptr) return;
    item = picGtroup->item(item->row(), 0);
    if (item == nullptr) return;
    PictureGroup group = item->data(Qt::UserRole).value<PictureGroup>();
#else
    QListWidget *listWidget = ui->listWidget_pg;
    QListWidgetItem *item = listWidget->currentItem();
    if (item == nullptr) return;
    PictureGroup group = item->data(Qt::UserRole).value<PictureGroup>();

    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if (jobstate != nullptr) {
        BaseTaskInfo* info = jobstate->getBaseTaskInfo();
        if (info != nullptr) {
            foreach(PictureGroup* g, info->groups) {
                if (g->groupId == group.groupId) {
                    group = *g;
                    item->setData(Qt::UserRole, QVariant::fromValue<PictureGroup>(group));
                    break;
                }
            }
        }
    }
#endif

    loadPicturesInfo(group.pictures);
}

void CMainWindow::on_btnAddPCtrl_clicked()
{
    CoordEditWindow* coord = new CoordEditWindow;
    if (coord->exec() != QDialog::Accepted) {
        coord->deleteLater();
        return;
    }

    CoordEditInfo info = coord->getCoordEditInfo();
    addCoordInfoToTable(info);
    coord->deleteLater();
}

void CMainWindow::on_btnEditPCtrl_clicked()
{
    QTableWidget* coordInfo = ui->tableWidgetCoord;
    QTableWidgetItem *item = coordInfo->currentItem();
    if (item == nullptr) return;
    item = coordInfo->item(item->row(), 0);
    if (item == nullptr) return;
    CoordEditInfo info = item->data(Qt::UserRole).value<CoordEditInfo>();

    CoordEditWindow* coord = new CoordEditWindow;
    coord->setCoordEditInfo(info);
    if (coord->exec() != QDialog::Accepted) {
        coord->deleteLater();
        return;
    }
    info = coord->getCoordEditInfo();
    coord->deleteLater();

    modifyCoordInfo(item->row(), info);
}

void CMainWindow::on_btnDelPCtrl_clicked()
{
    QTableWidget* coordInfo = ui->tableWidgetCoord;
    coordInfo->blockSignals(true);
    int crow = coordInfo->currentRow();
    coordInfo->removeRow(crow);
    coordInfo->blockSignals(false);
}

void CMainWindow::on_btnEditCoord_clicked()
{
#if 0
    CoordinateSystemEidt system;

    SRSWindow system(ui->label_ctrlpoint->property("sysDefine").toString());

    if(system.exec() != QDialog::Accepted)
        return;
    auto coordSystem = system.getCoordinateSystem();

    ui->label_ctrlpoint->setText(coordSystem.sysFullName);

#endif
}

void CMainWindow::addCoordInfoToTable(const CoordEditInfo& info)
{
    QTableWidget* coordInfo = ui->tableWidgetCoord;
    coordInfo->blockSignals(true);

    int row = coordInfo->rowCount();
    coordInfo->insertRow(row);

    // name
    QString cname = info.name;
    QTableWidgetItem *name = new QTableWidgetItem(cname);
    name->setTextAlignment(Qt::AlignCenter);
    name->setData(Qt::UserRole, QVariant::fromValue<CoordEditInfo>(info));
    coordInfo->setItem(row, 0, name);

    // x
    QTableWidgetItem *xItem = new QTableWidgetItem(QString::number(info.coordValue.coord_x));
    xItem->setTextAlignment(Qt::AlignCenter);
    coordInfo->setItem(row, 1, xItem);

    // y
    QTableWidgetItem *yItem = new QTableWidgetItem(QString::number(info.coordValue.coord_y));
    yItem->setTextAlignment(Qt::AlignCenter);
    coordInfo->setItem(row, 2, yItem);

    // z
    QTableWidgetItem *zItem = new QTableWidgetItem(QString::number(info.coordValue.coord_z));
    zItem->setTextAlignment(Qt::AlignCenter);
    coordInfo->setItem(row, 3, zItem);

    // pic
    QTableWidgetItem *icon = new QTableWidgetItem("");
    icon->setTextAlignment(Qt::AlignCenter);
    icon->setData(Qt::UserRole, QVariant::fromValue<QList<QString>>(info.pictures));
    icon->setFlags(icon->flags() & (~Qt::ItemIsEditable));
    coordInfo->setItem(row, 4, icon);
    coordInfo->setCellWidget(row, 4, getCellButton(QVariant::fromValue<QTableWidgetItem*>(icon), coordInfo));

    coordInfo->blockSignals(false);
}

void CMainWindow::onCellClicked(int row, int column)
{
    
}

void CMainWindow::modifyCoordInfo(int row, const CoordEditInfo& info)
{
    QTableWidget* coordInfo = ui->tableWidgetCoord;
    coordInfo->blockSignals(true);

    // name
    QTableWidgetItem *name = coordInfo->item(row, 0);
    if (name != nullptr) {
        name->setData(Qt::UserRole, QVariant::fromValue<CoordEditInfo>(info));
        name->setText(info.name);
    }

    // x
    QTableWidgetItem *xItem = coordInfo->item(row, 1);
    if (xItem != nullptr) {
        xItem->setText(QString::number(info.coordValue.coord_x));
    }

    // y
    QTableWidgetItem *yItem = coordInfo->item(row, 2);
    if (yItem != nullptr) {
        yItem->setText(QString::number(info.coordValue.coord_y));
    }

    // z
    QTableWidgetItem *zItem = coordInfo->item(row, 3);
    if (zItem != nullptr) {
        zItem->setText(QString::number(info.coordValue.coord_z));
    }

    // pictures
    QTableWidgetItem *icon = coordInfo->item(row, 4);
    if (icon != nullptr) {
        // icon->setIcon(QIcon(QPixmap(info.pictures[0])));
        // icon->setData(Qt::UserRole, QVariant::fromValue<QList<QString>>(info.pictures));
    }

    coordInfo->blockSignals(false);
}

QWidget* CMainWindow::getCellButton(const QVariant& v, QWidget* parent)
{
    QWidget* pWidget = new QWidget();
    QPushButton* checkBtn = new QPushButton(parent);
    checkBtn->setToolTip(QObject::tr("查看图片"));
    checkBtn->setStyleSheet("QPushButton{border-image:url(:/images/fileicon/image.png);border:"
        "none;background-color:transparent;max-height:20px;max-width:20px;padding:0px;}"
        "QPushButton:hover{border-image:url(:/images/fileicon/image.png);} ");
    checkBtn->setProperty("cell_row", v);

    QHBoxLayout* pLayout = new QHBoxLayout(pWidget);
    pLayout->addWidget(checkBtn);
    pLayout->setAlignment(Qt::AlignCenter);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pWidget->setLayout(pLayout);

    // setting
    connect(checkBtn, &QPushButton::clicked, this, [this]() {
        QPushButton *btn = qobject_cast<QPushButton*>(sender());
        if (btn != NULL) {
            QTableWidgetItem* opt = btn->property("cell_row").value<QTableWidgetItem*>();
            opt = ui->tableWidgetCoord->item(opt->row(), 0);
            if (opt == NULL) return;
            CoordEditInfo info = opt->data(Qt::UserRole).value<CoordEditInfo>();
            PicturesViewer* viewer = new PicturesViewer(info.pictures);
            viewer->exec();
            viewer->deleteLater();
        }
    });

    return pWidget;
}

TaskInfoCC* CMainWindow::getTaskInfoCC()
{
    TaskInfoCC* taskInfo = new TaskInfoCC;

    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    // project
    taskInfo->project_name = ui->label_project_name->text();
    taskInfo->coordSystem = ui->widget_coord_edit->getCoordSystemValue();
	getRebuildParams(taskInfo->settings.rebuildOption);
	getAtBlockInfo(taskInfo->blockAtSetting);

    // 检查项目类型是否合法
    if (m_projectType < 0 && jobstate->getBaseTaskInfo() != nullptr) {
        m_projectType = jobstate->getBaseTaskInfo()->jobType;
    }
    // picture group
    QTableWidget* picGtroup = ui->tableWidget_camera;
    for (int i = 0; i < picGtroup->rowCount(); i++) {
        QTableWidgetItem* item = picGtroup->item(i, 0);
        if(item == nullptr) continue;
        PictureGroup group = item->data(Qt::UserRole).value<PictureGroup>();
             
        group.jobType = m_projectType;
        if (jobstate != nullptr) {
            BaseTaskInfo* baseInfo = jobstate->getBaseTaskInfo();
            // 使用baseinfo的group 
            foreach(PictureGroup* ginfo, baseInfo->groups) {
                if (ginfo->groupId == group.groupId) {
                    group = *ginfo;
                    group.jobType = m_projectType;
                    break;
                }
            }

            if (baseInfo->posInfo.applyGroups.contains(group.groupId)) {
                group.posInfo = baseInfo->posInfo;
            }
            // 感应器尺寸
            item = picGtroup->item(i, 4);
            if (item != nullptr) {
                group.sensorSize = item->text();
                for (int i = 0; i < baseInfo->groups.size(); i++) {
                    if (baseInfo->groups.at(i)->groupId == group.groupId) {
                        baseInfo->groups.at(i)->sensorSize = item->text();
                        break;
                    }
                }
            }
            // 获取焦距
            item = picGtroup->item(i, 5);
            if (item != nullptr) {
                group.focalLength = item->text();
                for (int i = 0; i < baseInfo->groups.size(); i++) {
                    if (baseInfo->groups.at(i)->groupId == group.groupId) {
                        baseInfo->groups.at(i)->focalLength = item->text();
                        break;
                    }
                }
            }
        }
        taskInfo->groups.append(group);
    }

    // coord contrl
    if (jobstate->getProjectType() == enImageProject) {
        if (!ui->lineEdit_path_ctrl->text().isEmpty()) {
            ContrlPointInfo ctrlPoint;
            ctrlPoint.coordSys = ui->widget_coord_edit_2->getCoordSystemValue();
            ctrlPoint.fieldOrder = RBHelper::getFieldOrderString(ui->comboBox_field_order_ctrl->currentIndex());
            ctrlPoint.filePath = ui->lineEdit_path_ctrl->text();
            ctrlPoint.ignoreLines = ui->comboBox_ignore_lines_ctrl->currentText();
            ctrlPoint.spliteChar = RBHelper::getSplitChar(ui->comboBox_splite_char_ctrl->currentIndex());
            if (jobstate != nullptr) {
                ctrlPoint.itemDatas = jobstate->getBaseTaskInfo()->ctrlPointInfo.itemDatas;
            }
            taskInfo->ctrlPointInfo = ctrlPoint;
        } else {
            if (jobstate != nullptr) {
                BaseTaskInfo* baseInfo = jobstate->getBaseTaskInfo();
                if (baseInfo != nullptr) {
                    taskInfo->ctrlPointInfo = baseInfo->ctrlPointInfo;
                }
            }
        }
    } else if (jobstate->getProjectType() == enBlockProject) {
        ContrlPointInfo ctrlPoint;
        if (jobstate != nullptr) {
            ctrlPoint.itemDatas = jobstate->getBaseTaskInfo()->ctrlPointInfo.itemDatas;
        }
        taskInfo->ctrlPointInfo = ctrlPoint;
    }


    // pos
    if (jobstate->getProjectType() == enImageProject) {
        if (!ui->lineEdit_path_pos->text().isEmpty()) {
            PositionInfo pos;
            pos.coordSys = ui->widget_coord_edit_3->getCoordSystemValue();
            pos.fieldOrder = RBHelper::getFieldOrderString(ui->comboBox_field_order_pos->currentIndex());
            pos.filePath = ui->lineEdit_path_pos->text();
            pos.ignoreLines = ui->comboBox_ignore_lines_pos->currentText();
            pos.spliteChar = RBHelper::getSplitChar(ui->comboBox_splite_char_pos->currentIndex());
            if (jobstate != nullptr) {
                pos.itemDatas = jobstate->getBaseTaskInfo()->posInfo.itemDatas;
            }
            taskInfo->posInfo = pos;
        } else {
            if (jobstate != nullptr) {
                BaseTaskInfo* baseInfo = jobstate->getBaseTaskInfo();
                if (baseInfo != nullptr) {
                    taskInfo->posInfo = baseInfo->posInfo;
                }
            }
        }
    } else if (jobstate->getProjectType() == enBlockProject) {
        PositionInfo pos;
        if (jobstate != nullptr) {
            pos.itemDatas = jobstate->getBaseTaskInfo()->posInfo.itemDatas;
        }
        taskInfo->posInfo = pos;
    }

    // block
    if (!this->getBlockInfos().isEmpty()) {
        taskInfo->blocks = this->getBlockInfos();
    } else {
        if (jobstate != nullptr) {
            BaseTaskInfo* baseInfo = jobstate->getBaseTaskInfo();
            if (baseInfo != nullptr) {
                taskInfo->blocks = baseInfo->blocks;
            }
        }
    }
    
    // common info 
    taskInfo->frames_per_node = 1;
    taskInfo->gpuCount = MyConfig.paramSet.getGPU(0);
    taskInfo->ram = MyConfig.paramSet.getRAM(0);
    taskInfo->task_timeout = MyConfig.paramSet.overtime.defaultTimeOut;
    taskInfo->task_overtime = MyConfig.paramSet.timeoutstop.maxTimeoutStop;
    taskInfo->action_after_test = 1;
    taskInfo->tiles = 1;
    taskInfo->frame_overtime = MyConfig.paramSet.overtime.maxTimeOut;

    return taskInfo;
}

void CMainWindow::resetTableAndListWidget()
{
    // picture group
    QTableWidget* picGtroup = ui->tableWidget_pic_group;
    picGtroup->blockSignals(true);
    // clean
    for (int i = 0; i < picGtroup->rowCount(); i++) {
        picGtroup->removeRow(0);
    }
    picGtroup->clearContents();
    picGtroup->setRowCount(0);
    picGtroup->blockSignals(false);

    // camera
    picGtroup = ui->tableWidget_camera;
    picGtroup->blockSignals(true);
    // clean
    for (int i = 0; i < picGtroup->rowCount(); i++) {
        picGtroup->removeRow(0);
    }
    picGtroup->clearContents();
    picGtroup->setRowCount(0);
    picGtroup->blockSignals(false);

    // pictures
    QTableWidget* pictures = ui->tableWidget_pictures;
    pictures->blockSignals(true);
    for (int i = 0; i < pictures->rowCount(); i++) {
        pictures->removeRow(0);
    }
    pictures->clearContents();
    pictures->setRowCount(0);
    pictures->blockSignals(false);

    // list_pg
    QListWidget *pgList = ui->listWidget_pg;
    pgList->blockSignals(true);
    pgList->clear();
    pgList->blockSignals(false);

    // coord contrl
    QTableWidget* coordInfo = ui->tableWidgetCoord;
    coordInfo->blockSignals(true);
    // clean
    for (int i = 0; i < coordInfo->rowCount(); i++) {
        coordInfo->removeRow(0);
    }
    coordInfo->clearContents();
    coordInfo->setRowCount(0);
    coordInfo->blockSignals(false);

    coordInfo = ui->tableWidgetCoord_3;
    coordInfo->blockSignals(true);
    // clean
    for (int i = 0; i < coordInfo->rowCount(); i++) {
        coordInfo->removeRow(0);
    }
    coordInfo->clearContents();
    coordInfo->setRowCount(0);
    coordInfo->blockSignals(false);

    // pos info
    QTableWidget* posInfo = ui->tableWidgetCoord_pos_view;
    posInfo->blockSignals(true);
    // clean
    for (int i = 0; i < posInfo->rowCount(); i++) {
        posInfo->removeRow(0);
    }
    posInfo->clearContents();
    posInfo->setRowCount(0);
    posInfo->blockSignals(false);

    posInfo = ui->tableWidgetCoord_4;
    posInfo->blockSignals(true);
    // clean
    for (int i = 0; i < posInfo->rowCount(); i++) {
        posInfo->removeRow(0);
    }
    posInfo->clearContents();
    posInfo->setRowCount(0);
    posInfo->blockSignals(false);

    // 清理照片组combobox
    ui->comboBox_pic_group->clear();
}

void CMainWindow::resetWidgetConfig()
{
	m_atBlock = false; 
	m_atBlockMerage = true;
	m_imageCheck = true;
    updateStackRenderPage(RENDER_PAGE_PROJECT);
    ui->stackedWidget_project->setCurrentIndex(PROJECT_PAGE_PROJECT);
    ui->stackedWidget_AT->setCurrentIndex(AT_PAGE_RENDER);
    ui->stackedWidget_rebuild->setCurrentIndex(REBUILD_PAGE_HOME);
    m_sideBarGroup->buttonClicked(RENDER_PAGE_PROJECT);
    ui->btnAddCtrlPoint->setProperty("nextPage", PROJECT_PAGE_CP_ADD);
    ui->btnAddPos->setProperty("nextPage", PROJECT_PAGE_POS_ADD);
    ui->radio_gps_pos->setChecked(true);
	if (m_rebuildParamWidget != nullptr)
	{
		JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
		if (jobstate != nullptr)
		{
			BLOCK_TASK::BlockExchange *blockInfo = jobstate->getBlockExchangeInfo();
			if (blockInfo != nullptr)
			{
				BLOCK_TASK::Photo * currgroup = blockInfo->block.photogroups.begin()->photos.at(0);
				if (currgroup->exifData.gps.Longitude != Null)
				{
					m_rebuildParamWidget->resetView(currgroup->exifData.gps.Longitude.toDouble(), currgroup->exifData.gps.Latitude.toDouble());
				}
				else
				{
					m_rebuildParamWidget->resetView();
				}
			}
			else
			{
				m_rebuildParamWidget->resetView();
			}
		}
		else
		{
			m_rebuildParamWidget->resetView();
		}

	}

    ui->widget_select_group->hide();
    ui->widget_upload_state->setStyleSheet(".QWidget{background-color:transparent;}");

    // 初始化stackedWidget中的数据
    ui->comboOutputType->clear();
    ui->comboOutputType->addItem("OSGB");
    ui->comboOutputType->addItem("OBJ");
    ui->comboOutputType->addItem("Cesium 3D Tiles");
    ui->comboOutputType->addItem("LAS");
    ui->comboOutputType->setCurrentText(QStringList() << "OSGB");

    // 进度清空
    ui->progressBarAT->setValue(0);
    ui->progressBarRebuild->setValue(0);
    // 定时器清空
    if(m_atRenderTimer != nullptr)
        m_atRenderTimer->stop();
    if(m_rebuildRenderTimer != nullptr)
        m_rebuildRenderTimer->stop();

    // project
//    ui->lineEditProject->clear();
    ui->comboProjectType->setCurrentIndex(0);
    ui->label_block_path->clear();
    // pic group combobox
    ui->comboBox_pic_group->clear();
    ui->radioButton_all_pos->setChecked(true);
    // 自定义坐标不可用
    ui->widget_coord_set->setEnabled(false);

	//参数窗口信息重置
	ui->checkBox_xyz_offset->setChecked(false);
	ui->checkBox_enable_custom_3->setChecked(false);
	ui->checkBox_xyz_offset->setChecked(false);
	ui->checkBox_lockRange_3->setChecked(true);
	ui->lineEdit_x_coord_3->clear();
	ui->lineEdit_y_coord_3->clear();
	ui->lineEdit_z_coord_3->clear();
	ui->lineEdit_x_offset->clear();
	ui->lineEdit_y_offset->clear();
	ui->lineEdit_z_offset->clear();
	ui->comboBox_tile_mode_3->setCurrentIndex(0);

	//初始化重建参数
	ui->comboOutputType_3->clear();
	for (int i = 0; i < OUTPUT_TYPE_TOTAL; i++) {
//        if(i == OUTPUT_TYPE_TIFF) continue; // 不显示TIFF
		ui->comboOutputType_3->addItem(RBHelper::getOutputTypeDisplay((OutputType)i), RBHelper::getOutputTypeString((OutputType)i));
	}
	ui->comboOutputType_3->setCurrentText(QStringList() << "OSGB");


	//空三分块选项重置
	ui->checkBox_blockAT->setChecked(false);
	ui->radioBtn_blockAT->setChecked(true);
	ui->widget_BlockAT_radioBtn->setVisible(0);

    // 清理表格个列表
    resetTableAndListWidget();
	initATBlockListView();
}

void CMainWindow::initTaskInfo(QSharedPointer<RenderParamConfig> param)
{
    if (param == nullptr) return;

    param->renderRAM = QString::number(MyConfig.paramSet.getRAM(0));
    param->gpuCount  = QString::number(MyConfig.paramSet.getGPU(0));
    param->testFrames = "000";
    param->frameTimeoutRemind = QString::number(MyConfig.paramSet.overtime.defaultTimeOut);
    param->frameTimeoutStop = QString::number(MyConfig.paramSet.timeoutstop.defaultTimeoutStop);
    param->muiltFrames = "1";
    param->pluginName = "";
    param->projectName = "";
    param->softVersion = "";
    param->submitLayer = "0";
    param->submitNoded = "0";
}

bool CMainWindow::makeJsonFiles(const QString& argumentFile)
{
    QProcess* maker = new QProcess;
    QString makerPath = RBHelper::getJsonMakerPath();
    makerPath = QDir::toNativeSeparators(makerPath);
    QStringList params;
    params << "-arg" << QDir::toNativeSeparators(argumentFile);

    maker->start(makerPath, params);
    maker->waitForFinished();
    
    int code = maker->exitCode();
    return maker->exitCode() == QProcess::NormalExit;
}

void CMainWindow::on_btnContactService_clicked()
{
    QPoint pos(ui->btnContactService->mapToGlobal(QPoint(0, 0)).x(),
               ui->btnContactService->mapToGlobal(QPoint(0, 0)).y() + ui->btnContactService->height());

    m_csMenu->exec(pos);

    QMouseEvent event(QEvent::MouseButtonRelease, QPoint(0, 0), Qt::LeftButton, 0, 0);
    QApplication::sendEvent(ui->btnContactService, &event);
}

void CMainWindow::on_btnDownloadlist_clicked()
{
    m_downloadViewPage->initView();
    m_downloadViewPage->show();
    m_downloadViewPage->raise();
}

void CMainWindow::submitTask(const QList<qint64>& taskIds, const QList<QString>& taskAlias)
{
    if (taskIds.isEmpty() || taskAlias.isEmpty()) {
        emit makeTaskCfgFinished(-1);
        return;
    }
        
    TaskInfoCC* taskInfo = getTaskInfoCC();
    QList<PictureGroup> groups = taskInfo->groups;
    for (int i = 0; i < taskIds.size(); i++) {
        qint64 taskId = taskIds.at(i);
        QString alias = "";
        for (int j = 0; j < taskAlias.size(); j++) {
            QString talias = taskAlias.at(j);
            if (talias.contains(QString::number(taskId))) {
                alias = talias;
                break;
            }
        }

        QSharedPointer<RenderParamConfig> param(new RenderParamConfig());
        initTaskInfo(param);
        param->jobId = taskId;
        param->jobIdAlias = alias;
        // taskInfo->groups.append(groups.at(i));
		// taskInfo->settings = m_settingsInfo;
        param->task.reset(taskInfo);

        // dump paramers into json
        QString argumentFile;
        if (CTaskCenterManager::Inst()->dumpParam(param, argumentFile)) {
            if (makeJsonFiles(argumentFile)) {
                // start upload task
                if (!StartTask(taskId)) {
                    emit makeTaskCfgFinished(-2);
                    return;
                }
            }
            else {
                emit makeTaskCfgFinished(-1);
                return;
            }
        }

        QThread::sleep(1);
    }

    emit makeTaskCfgFinished(0);
}

void CMainWindow::onUploadTaskFinished(t_taskid taskid)
{
    CTaskCenterManager::Inst()->resubmitTask(taskid);
}

void CMainWindow::onMakeTaskCfgFinished(int code)
{
    if (code == -1) {
        new ToastWidget(this, QObject::tr("创建项目(task.json等)数据失败!"), QColor("#e25c59"), 2000);
        LOGFMTI("[CMainWindow] Make TaskCfg Failed!");
        return;
    } else if (code == -2) {
        new ToastWidget(this, QObject::tr("读取项目(task.json等)数据失败!"), QColor("#e25c59"), 2000);
        LOGFMTI("[CMainWindow] Load TaskCfg Failed!");
        return;
    }

    int nShow = CConfigDb::Inst()->getShowTaskType();
    JobStateMgr::Inst()->SendRefreshTasklist(-1, nShow);
}

void CMainWindow::on_btnImportPos_clicked()
{
}

QString CMainWindow::getDetailViewUrl(JobState* job, const QString& url)
{
    QString host = WSConfig::getInstance()->get_link_address(url);
    QString token = MyConfig.userSet.userKey;
    QString localeName = RBHelper::getLocaleName();
    int platformId = MyConfig.accountSet.platform;
    QString imageUrl = QString("%1?userId=%2&taskId=%3&rayvision_token=%4&languageFlag=%5&platform=%6&from=%7")
        .arg(host)
        .arg(MyConfig.accountSet.userId)
        .arg(job->getTaskId())
        .arg(token)
        .arg(localeName)
        .arg(platformId)
        .arg(WSConfig::getInstance()->get_link_address("path_from"));

    LOGFMTD("url = %s", imageUrl.toStdString().c_str());

    return imageUrl;
}

void CMainWindow::on_btnCancelBlock_clicked()
{
    this->on_btnCancelJob_clicked();
}

void CMainWindow::on_btnBlockNext_clicked()
{
    // todo 检查blocks
    if (getBlockInfos().isEmpty()) {
        new ToastWidget(ui->widget_main, QObject::tr("请添加区块文件"), QColor("#e25c59"), 3000);
        return;
    }
}

void CMainWindow::on_btnAddBlock_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("选择区块文件"), QString(), "XML/XMLZ (*.xml *.xmlz)");
    if(file.isEmpty()) return;

	if (checkIllegalpath(file)) return;


    ui->label_block_path->setText(file);

    // 加载区块文件
    LoadBlockFileThread* loadBlock = new LoadBlockFileThread(file, m_currTaskId);
    connect(loadBlock, &LoadBlockFileThread::loadBlockFileFinished, this, &CMainWindow::onLoadBlockFileFinished);
    loadBlock->start();
    // this->showLoading(true);
    ui->widget_page->setEnabled(false);
    TipsToastWidget* toast = new TipsToastWidget(ui->widget_page, QObject::tr("数据分析中，请稍后..."), 16, QColor("#1F1F27"));
    connect(this, &CMainWindow::loadBlockInfoFinished, toast, &TipsToastWidget::hideToast);
    LOGFMTI("[CMainWindow] Start parse block file!");
}

void CMainWindow::onLoadBlockFileFinished(int code)
{
    LOGFMTI("[CMainWindow] Parse block file Finished!");
    //this->showLoading(false);
    ui->widget_page->setEnabled(true);
    emit loadBlockInfoFinished();

    LoadBlockFileThread* loadJob = qobject_cast<LoadBlockFileThread*>(sender());
    if (loadJob == nullptr)
        return;
    disconnect(loadJob, &LoadBlockFileThread::loadBlockFileFinished, this, &CMainWindow::onLoadBlockFileFinished);
    if (loadJob->getTaskId() != m_currTaskId) {
        loadJob->deleteLater();
        return;
    }
    if (code != CODE_SUCCESS) {     
        showErrorMsg(code);
		if (code == CODE_ERROT_NOT_TIEPOINT_FILE)
		{
			m_blockCheck = false;
		}
        return;
    }
	m_blockCheck = true;
    new ToastWidget(ui->widget_title, QObject::tr("区块文件解析完成!"), QColor("#13cb75"), 5000);

    fillBlockFileInfo(loadJob->getBlockExchangeInfo());

    initCtrlPointPageCtrl(ui->widget_page_ctrl_view);
    initCtrlPointPageCtrl(ui->widget_page_ctrl_submit);

    initCtrlPointPageCtrl(ui->widget_page_pos_view);
    initCtrlPointPageCtrl(ui->widget_page_pos_submit);

    resetCtrlPointViewSet();
    resetPositionViewSet();
}

void CMainWindow::onLoadBlockFileForSurveysFinished(int code)
{
	LOGFMTI("[CMainWindow] Parse block file Finished!");
	//this->showLoading(false);
	ui->widget_page->setEnabled(true);
	emit loadBlockInfoFinished();

	LoadBlockFileThread* loadJob = qobject_cast<LoadBlockFileThread*>(sender());
	if (loadJob == nullptr)
		return;
	disconnect(loadJob, &LoadBlockFileThread::loadBlockFileFinished, this, &CMainWindow::onLoadBlockFileFinished);
	if (loadJob->getTaskId() != m_currTaskId) {
		loadJob->deleteLater();
		return;
	}
	if (code != CODE_SUCCESS) {
		showErrorMsg(code);
		if (code == CODE_ERROT_NOT_TIEPOINT_FILE)
		{
			m_blockCheck = false;
		}
		return;
	}
	m_blockCheck = true;
	new ToastWidget(ui->widget_title, QObject::tr("区块文件解析完成!"), QColor("#13cb75"), 5000);

	ui->widget_surveysPoints->setBlockInfo(loadJob->getBlockExchangeInfo());

}

void CMainWindow::onLoadBlockFileForSurveyWidget(int code)
{
	LOGFMTI("[CMainWindow] Parse block file Finished!,");
	//this->showLoading(false);
	emit loadBlockInfoFinished();

	ui->stackedWidget_render->setCurrentIndex(RENDER_PAGE_SURVEYPOINTS);
	ui->widget_surveysPoints->loadImageData();


}

void CMainWindow::onInitSurveyWidget()
{
	LOGFMTI("[CMainWindow] init surveys widget Finished!,");

//	ui->widget_surveysPoints->initPythonCall();

	ui->widget_surveysPoints->loadImageData();
}

void CMainWindow::fillBlockFileInfo(BLOCK_TASK::BlockExchange* blockInfo)
{
    if (blockInfo == nullptr) return;

    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if (jobstate == nullptr) return;
    jobstate->setBlockExchangeInfo(blockInfo);

    BaseTaskInfo* baseInfo = jobstate->getBaseTaskInfo();
    if (baseInfo == nullptr) return;

    // 控制点数据
    int index = 1;
    qDeleteAll(baseInfo->ctrlPointInfo.itemDatas);
    baseInfo->ctrlPointInfo.itemDatas.clear();
    foreach(BLOCK_TASK::ControlPoint* ctrl, blockInfo->block.controlPoints) {
        ConfigDataItem* item = new ConfigDataItem;
        item->index        = index++;
        item->imageName    = ctrl->name;
        item->picAltitude  = ctrl->position.z;
        item->picLatitude  = ctrl->position.y;
        item->picLongitude = ctrl->position.x;
        baseInfo->ctrlPointInfo.itemDatas.append(item);
    }

    // pos数据
    index = 1;
    qDeleteAll(baseInfo->posInfo.itemDatas);
    baseInfo->posInfo.itemDatas.clear();
    foreach(BLOCK_TASK::Photogroup photoGroup, blockInfo->block.photogroups) {
        foreach(BLOCK_TASK::Photo* photo, photoGroup.photos) {
            ConfigDataItem* item = new ConfigDataItem;
            item->index        = index++;
            item->imageName    = QFileInfo(photo->imagePath).fileName();
            item->picAltitude  = photo->exifData.gps.Altitude;     
            item->picLatitude  = photo->exifData.gps.Latitude;
            item->picLongitude = photo->exifData.gps.Longitude;
			if (photo->exifData.gps.Latitude.isEmpty() && photo->exifData.gps.Longitude.isEmpty())
			{
				item->picLongitude = photo->pose.metaData.center.x;  //exifData ->Metadata.center
				item->picLatitude = photo->pose.metaData.center.y;
				item->picAltitude = photo->pose.metaData.center.z;
			}
            baseInfo->posInfo.itemDatas.append(item);
        }
    }
    jobstate->updataBaseTaskInfo(baseInfo);
}

QList<BlockInfo> CMainWindow::getBlockInfos()
{
    QList<BlockInfo> blocks;

    if (m_projectType == enBlockProject && !ui->label_block_path->text().isEmpty()) {
        BlockInfo block;
        block.filePath = ui->label_block_path->text();
        blocks.append(block);
        return blocks;
    }
    return blocks;
}

void CMainWindow::on_btnOpenPos_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("选择POS文件"), QString(), "TXT (*.txt);;CSV (*.csv)");
    if(file.isEmpty()) return;

    ui->lineEdit_path_pos->setText(file);
}

void CMainWindow::on_btnOpenCtrlPoint_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("选择控制点文件"), QString(), "TXT (*.txt);;CSV (*.csv)");
    if (file.isEmpty()) return;

    ui->lineEdit_path_ctrl->setText(file);
}

void CMainWindow::on_btnSurveyPoint_clicked()
{
	m_buttonSurveyPoint->setEnabled(false);
//	Sleep(10000);
	connect(this, &CMainWindow::initSurveyModulefinish, this, &CMainWindow::SurvayModuel);
	initSurveyModule();
}

void CMainWindow::onSurveyDataLoadFinished(int code)
{
	if (code == CODE_INIT_PYTHON_CALL_FAILED)
	{
		Util::ShowMessageBoxQuesion(QObject::tr("数据计算失败，请检查AT相关坐标系"));
		return;
	}
    ui->stackedWidget_render->setCurrentIndex(RENDER_PAGE_SURVEYPOINTS);
}


void CMainWindow::initSurveyModule()
{
	//初始化python调用
	QString CCLicenseCode = WSConfig::getInstance()->get_key_value("CCGCPLicense");
	if (!KrGcp::initialize(CCLicenseCode))
	{		
		emit initSurveyModulefinish(0);			
	};	
	emit initSurveyModulefinish(1);
	
}
void CMainWindow::InitCloudPublish()
{
#ifdef FOXRENDERFARM
	ui->btn_autobuild->setMinimumWidth(150);
	ui->label_45->setMinimumWidth(150);
	ui->label_54->setMinimumWidth(150);
	ui->label_56->setMinimumWidth(155);
	ui->label_88->setMinimumWidth(150);
	ui->label_57->setMinimumWidth(150);
	ui->label_58->setMinimumWidth(150);
	ui->horizontalSpacer_49->spacerItem()->changeSize(150, 20);
	ui->btn_fmpic->setIcon(QIcon(":/images/view/addAvica.png"));
#else
	ui->btn_fmpic->setIcon(QIcon(":/images/view/add@2x.png"));
#endif
	ui->lineEdit->clear();
	ui->comboBox->clear();
	ui->textEdit_des->clear();
	ui->lineEdit_2->setText("10");
	ui->lineEdit_key->setMaxLength(4);

	
	ui->btn_fmpic->setIconSize(QSize(60, 60));
	//ui->btn_fmpic->setText("添加封面");
	ui->btn_fmpic->setToolButtonStyle(Qt::ToolButtonIconOnly);
	//ui->btn_fmpic->setIcon(QIcon(""));

	ui->stackedWidget->setStyleSheet("color: white;");
	ui->comboBox->clear();
	ui->tabWidget->setStyleSheet("background-color:#1F1F27");
	QList<QString> modeltype;
	modeltype.append(tr("智慧城市"));
	modeltype.append(tr("工地测量"));
	modeltype.append(tr("古建筑/景区"));
	modeltype.append(tr("考古/文物"));
	modeltype.append(tr("公共安防"));
	modeltype.append(tr("建筑工程"));
	modeltype.append(tr("智慧工厂"));
	modeltype.append(tr("其他"));
	ui->comboBox->addItems(modeltype);
	ui->checkBox_pwd->setCheckState(Qt::CheckState::Unchecked);
	ui->btn_autobuild->hide();
	ui->lineEdit_key->hide();
	ui->label_87->hide();
	ui->radioButton_1->setChecked(true);
	ui->spinBox_time->setMaximumHeight(20);
	connect(ui->radioButton_3, SIGNAL(toggled(bool)), this, SLOT(onCheckSlot3(bool)));
	//connect(ui->spinBox_time, SIGNAL(valueChanged(int)), this, SLOT(onSelectTime(int)));
	connect(ui->textEdit_des, SIGNAL(textChanged()), this, SLOT(onCalNum()));
	connect(ui->checkBox_pwd, SIGNAL(stateChanged(int)), this, SLOT(onTipsNum(int)));
	ui->textEdit_des->setStyleSheet("border:1px solid white");
	ui->spinBox_time->setRange(1, 30);
	ui->spinBox_time->hide();
	ui->label_59->hide();
	ui->btn_fmpic->setStyleSheet("border:2px dashed #282832");
	ui->btn_publishcancel->setStyleSheet("QPushButton{border:1px solid #6B6CFF;background-color:#675ffe;}""QPushButton:hover{color:black; background-color:#6B6CFF;}");
	ui->btn_publishok->setStyleSheet("QPushButton{border:1px solid #6B6CFF;background-color:#675ffe;}""QPushButton:hover{color:black; background-color:#6B6CFF;}");

	//ui->btn_fmpic->setIcon(QIcon("E:/img3.jpg"));
	//ui->btn_fmpic->adjustSize();
}
void CMainWindow::InitPubList()
{
#ifdef FOXRENDERFARM
	ui->pushButton_search->setMinimumWidth(140);
	ui->pushButton_4->setMinimumWidth(140);

#endif
	QList<QString> list;	
	list.append(tr("未过期"));
	list.append(tr("已过期"));
	list.append(tr("全部"));
	ui->comboBox_states->clear();
	ui->comboBox_states->addItems(list);
	ui->comboBox_states->setCurrentIndex(0);
	ui->lineEdit_publishname->setPlaceholderText(tr("搜索发布名称"));
	connect(ui->comboBox_states, SIGNAL(currentIndexChanged(int)), this, SLOT(onPublishStatusChanged(int)));
}
void CMainWindow::InitPubTask()
{
	QList<QString> headers;
	headers.append(tr("发布作业ID"));
	headers.append(tr("共享ID"));
	headers.append(tr("状态"));
	headers.append(tr("开始时间"));
	headers.append(tr("结束时间"));
	headers.append(tr("运行时间"));
	headers.append(tr("单价(元/h)"));
	headers.append(tr("余额支付"));
	headers.append(tr("大雁券支付"));
	headers.append(tr("操作"));
	ui->tableWidget_task->setColumnCount(headers.size());
	ui->tableWidget_task->setHorizontalHeaderLabels(headers);
	ui->tableWidget_task->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft); //居左
	QHeaderView *headerView = ui->tableWidget_task->horizontalHeader();
	headerView->setSortIndicator(2, Qt::AscendingOrder);
	headerView->setSortIndicatorShown(true);
	headerView->setSectionsClickable(true);
	//headerView->setStyleSheet("backgroud-color:white;subcontrol-position: center right; padding-right: 8px;");
	//ui->tableWidget_task->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	connect(headerView, SIGNAL(sectionClicked(int)), this, SLOT(on_refreshTable(int)));

}
void CMainWindow::InitPublishSuccess(const CreatePublish & item)
{
#ifdef FOXRENDERFARM
	ui->btnCopy->setMinimumWidth(170);
	ui->label_60->setMinimumWidth(140);
	ui->label_62->setMinimumWidth(140);
	ui->label_64->setMinimumWidth(140);
	ui->label_66->setMinimumWidth(140);
	ui->label_67->setMinimumWidth(140);
	ui->label_69->setMinimumWidth(140);
	ui->label_89->setMinimumWidth(140);
	ui->btnSucDes->setMinimumWidth(80);
	//ui->label_72->setText(tr("发布成功"));
	//ui->label_72->setStyleSheet("QLabel{font:12px;}");
#endif
	if (item.item)
	{
		m_item = *item.item;
	//	ui->label_72->setStyleSheet("font-size;18px");
		//ui->btnCopy->setEnabled(true);
		ui->widget_success->setStyleSheet("border: 10px;background-color:#282832;font-color:white");
		//QFont font;
		//font.setPointSizeF(30);
		//ui->label_72->setFont(font);
		ui->label_72->setText(tr("发布成功"));
		ui->label_72->setStyleSheet("QLabel{font:18px;}");//font-family:SourceHanSansCN-Normal;}");
		ui->label_61->setText(item.item->publishName);
		QString modeltype = ui->comboBox->itemText(item.item->type);
		ui->label_63->setText(modeltype);
		//描述超过71个字符显示详情按钮
		item.item->des.size() > 71 ? ui->btnSucDes->show() : ui->btnSucDes->hide();
		QString str = fontMetrics().elidedText(item.item->des, Qt::ElideRight, ui->label_65->width()*2.53);
		ui->label_65->setText(str);
		//ui->label_65->setText(item.item->des);
		ui->label_65->setWordWrap(true);
		ui->label_65->adjustSize();
		ui->btnSucDes->setStyleSheet("QPushButton{border:none;background-color:transparent;color: #675ffe}""QPushButton:hover{ color: white; text-decoration:underline; background-color: transparent;}");

		ui->lineEdit_3->setText(item.item->url);
		ui->lineEdit_3->setStyleSheet("background: #2F2F3A;border-radius: 4px");
		ui->lineEdit_3->setCursorPosition(0);//设置光标在左侧
		item.item->isNeedKey == 1 ? ui->label_68->setText(item.item->secretKey) : ui->label_68->setText("");
		item.item->isNeedKey == 1 ? ui->label_67->show() : ui->label_67->hide();
		//时间搓转化
		QDateTime time = QDateTime::fromMSecsSinceEpoch(item.item->expiredTime);
		ui->label_70->setText(QString("%1").arg(time.toString("yyyy-MM-dd")));
		ui->label_90->setText(QString("%1").arg(item.item->concurrentCount));
		ui->btnCopy->setStyleSheet("QPushButton{border:1px solid #6B6CFF;background-color:#675ffe;}""QPushButton:hover{color:black; background-color:#6B6CFF;}");
		ui->btn_sucpublish->setStyleSheet("QPushButton{border:1px solid #6B6CFF;background-color:#675ffe;}""QPushButton:hover{color:black; background-color:#6B6CFF;}");
	}
}
void CMainWindow::InitPublishTask(const PublishTaskResponse & task)
{

	int rows = task.tasks.size();
	ui->tableWidget_task->setRowCount(rows);
	for (int i = 0; i < rows; i++)
	{
		//PubTask* taskitem = task.tasks.at(i);
		QSharedPointer<PubTask> taskitem(task.tasks.at(i));
		int status = taskitem->status;
		QString pubStatus = "";
		if (status == 20) {
			pubStatus = tr("进行中");
		}
		else {
			pubStatus = tr("已完成");
		}
		
		//QSharedPointer<PublishWindow> aaa = QSharedPointer<PublishWindow>(new PublishWindow(111));
		ui->tableWidget_task->setItem(i, 0, new QTableWidgetItem(QString("%1").arg(taskitem->publishId)));
		ui->tableWidget_task->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(taskitem->publishJobId)));
		ui->tableWidget_task->setItem(i, 2, new QTableWidgetItem(pubStatus));
		if (taskitem->startTime == 0)
		{
		    ui->tableWidget_task->setItem(i, 3, new QTableWidgetItem("--"));
		}
		else
		{
			QDateTime startTime = QDateTime::fromMSecsSinceEpoch(taskitem->startTime);
			ui->tableWidget_task->setItem(i, 3, new QTableWidgetItem(QString("%1").arg(startTime.toString("yyyy-MM-dd hh:mm:ss"))));
		}
		if (taskitem->completedTime == 0)
		{
			ui->tableWidget_task->setItem(i, 4, new QTableWidgetItem("--"));
		}
		else
		{
			QDateTime completedTime = QDateTime::fromMSecsSinceEpoch(taskitem->completedTime);
			ui->tableWidget_task->setItem(i, 4, new QTableWidgetItem(QString("%1").arg(completedTime.toString("yyyy-MM-dd hh:mm:ss"))));
		}	
		//计算运行时间
		qint64 hour, min, s = 0;
		hour = taskitem->duration / 3600;
		min = (taskitem->duration % 3600) / 60;
		s = taskitem->duration % 3600 % 60;
		QString duration = QString("%1:%2:%3").arg(hour, 2, 10, QLatin1Char('0')).arg(min, 2, 10, QLatin1Char('0')).arg(s, 2, 10, QLatin1Char('0'));
		
		ui->tableWidget_task->setItem(i, 5, new QTableWidgetItem(QString("%1").arg(duration)));
		ui->tableWidget_task->setItem(i, 6, new QTableWidgetItem(QString("%1").arg(taskitem->unitPrice)));
		ui->tableWidget_task->setItem(i, 7, new QTableWidgetItem(QString("%1").arg(taskitem->consume)));
		ui->tableWidget_task->setItem(i, 8, new QTableWidgetItem(QString("%1").arg(taskitem->couponConsume)));
		QPushButton *pBtn = new QPushButton();//按钮显示的名字

		pBtn->setStyleSheet("border:none");
		pBtn->setMaximumWidth(30);
#ifdef FOXRENDERFARM
		pBtn->setMinimumWidth(80);
#endif
		if (pubStatus == tr("进行中"))
		{
			pBtn->setText(tr("中止"));
			pBtn->setStyleSheet("QPushButton{border:none;color:#675ffe;background-color: transparent;}""QPushButton:hover{color: white;text-decoration:underline; background-color: transparent;}");
		}
		else
		{
			pBtn->setText("--");
			pBtn->setStyleSheet("border:none;color:#999999");
			pBtn->setEnabled(false);
		}
		ui->tableWidget_task->setCellWidget(i, 9, pBtn);
		connect(pBtn, &QPushButton::clicked, this, &CMainWindow::onStopTask);
		
	}
    ui->tableWidget_task->horizontalHeader()->setStretchLastSection(true);
	ui->tableWidget_task->horizontalHeader()->setMinimumSectionSize(90);
	
	ui->tableWidget_task->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableWidget_task->horizontalHeader()->setSectionResizeMode(3,QHeaderView::Fixed);
	ui->tableWidget_task->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
	ui->tableWidget_task->setColumnWidth(3, 145);
	ui->tableWidget_task->setColumnWidth(4, 145);
	//ui->tableWidget_task->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	ui->tableWidget_task->horizontalHeader()->setFixedHeight(30);
	
	ui->tableWidget_task->setSelectionBehavior(QAbstractItemView::SelectRows); //设置选中单行
	ui->tableWidget_task->setFocusPolicy(Qt::NoFocus);
	ui->tableWidget_task->setSelectionMode(QTableWidget::SingleSelection);
	ui->tableWidget_task->verticalHeader()->hide();//取消行号
	ui->tableWidget_task->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tableWidget_task->setStyleSheet("border:1px solid #2F2F3A;selection_color: #6B6CFF");
#ifdef FOXRENDERFARM
	ui->tableWidget_task->horizontalHeader()->setMinimumSectionSize(145);
	ui->tableWidget_task->setColumnWidth(0, 80);
	ui->tableWidget_task->setColumnWidth(1, 80);
	ui->tableWidget_task->setColumnWidth(2, 80);
#endif
	
}
void CMainWindow::InitPublishDetail(const PublishItem & item)
{
#ifdef FOXRENDERFARM
	ui->btn_pubcopy->setMinimumWidth(170);
	ui->btn_cancelpub->setMinimumWidth(130);
	//ui->textEdit_lianjie->setMaximumSize(QSize(150, 90));
	//ui->textEdit_lianjie->resize(100, 75);
	ui->label_73->setMinimumWidth(140);
	ui->label_75->setMinimumWidth(140);
	ui->label_77->setMinimumWidth(140);
	ui->label_79->setMinimumWidth(140);
	ui->label_80->setMinimumWidth(140);
	ui->label_82->setMinimumWidth(150);
	ui->label_84->setMinimumWidth(150);
	ui->label_85->setMinimumHeight(200);
	ui->label_86->setMinimumWidth(150);
	ui->widget_right->setMinimumWidth(420);
	ui->btnDes->setMinimumWidth(70);
#endif
	ui->dateEdit->hide();
	ui->label_85->setStyleSheet("background: #2E2E2E");
	ui->dateEdit->setStyleSheet("background-color: transparent;");
	ui->detailwidget->setStyleSheet("color:white");
	ui->label_74->setText(item.publishName);
	ui->label_74->setWordWrap(true);
	ui->label_74->adjustSize();
	QString modeltype = ui->comboBox->itemText(item.type);
	ui->label_76->setText(modeltype);
	//描述超过54个字符显示详情按钮
	item.des.size() > 54 ? ui->btnDes->show() : ui->btnDes->hide();
	QString str = fontMetrics().elidedText(item.des, Qt::ElideRight, ui->label_78->width()*2.53);
	
	ui->label_78->setText(str);
	//QPushButton *btn = new QPushButton(ui->label_78);
	//ui->btnDes->show();

	ui->label_78->setWordWrap(true);
	ui->label_78->adjustSize();
	ui->textEdit_lianjie->setPlainText(item.url);
	ui->textEdit_lianjie->setReadOnly(true); //只读
	item.isNeedKey == 1 ? ui->label_81->setText(item.secretKey) : ui->label_81->setText("");
	item.isNeedKey == 1 ? ui->label_80->show() : ui->label_80->hide();
	ui->label_85->setPixmap(QPixmap(item.localImgUrl));
	ui->label_85->setScaledContents(true);
	//时间搓转化
	QDateTime time = QDateTime::fromMSecsSinceEpoch(item.expiredTime);
	QString dd = QString("%1").arg(time.toString("yyyy-MM-dd hh:mm:ss"));
	ui->label_83->setText(QString("%1").arg(time.toString("yyyy-MM-dd hh:mm:ss")));
	QString cc = time.toString();
	ui->dateEdit->setCalendarPopup(true);
	ui->dateEdit->setDateTime(time);
	ui->dateEdit->resize(250, 200);
	ui->btn_editdata->setStyleSheet("QPushButton{border:none;background-color:transparent;color: #675ffe}""QPushButton:hover{ color: white; text-decoration:underline; background-color: transparent;}");
	ui->btnDes->setStyleSheet("QPushButton{border:none;background-color:transparent;color: #675ffe}""QPushButton:hover{ color: white; text-decoration:underline; background-color: transparent;}");
	//ui->dateEdit = QDateTimeEdit(time,this);
	//ui->dateEdit->set
	//ui->label_83->hide();
	//ui->btn_editdata->hide();
	//ui->dateEdit->setStyleSheet("background-color:#1F1F27;font-color:white");
	connect(ui->dateEdit, SIGNAL(dateTimeChanged(const QDateTime &)), this, SLOT(onUpdateDate(const QDateTime &)));

	connect(this, SIGNAL(updateDate(qint64)), this, SLOT(onUpdateRequest(qint64)));
	//如果已失效，取消发布按钮置灰
	if (item.status == 1)
	{
		ui->label_79->hide();
		ui->textEdit_lianjie->hide();
		//ui->label_80->hide();
		//ui->label_81->hide();
		ui->btn_pubcopy->hide();
		ui->label_82->hide();
		ui->label_83->hide();
		ui->btn_editdata->hide();
		ui->label_84->hide();
		ui->lineEdit_bf->hide();
		ui->btn_cancelpub->hide();
	//	ui->btn_cancelpub->setEnabled(false);
	//	ui->btn_editdata->setEnabled(false);
	//	ui->dateEdit->setEnabled(false);
	}
	else
	{
		ui->label_79->show();
		ui->textEdit_lianjie->show();
		//ui->label_80->show();
	//	ui->label_81->show();
		ui->btn_pubcopy->show();
		ui->label_82->show();
		ui->label_83->show();
		ui->btn_editdata->show();
		ui->label_84->show();
		ui->lineEdit_bf->show();
		ui->btn_cancelpub->show();
	}
	
	ui->label_83->setText(QString("%1").arg(time.toString("yyyy-MM-dd")));
	//ui->lineEdit_bf->
	ui->lineEdit_bf->setText(QString("%1").arg(item.concurrentCount));
	connect(ui->lineEdit_bf, SIGNAL(editingFinished()), this, SLOT(onUpdateBf()), Qt::QueuedConnection);
	//HttpCmdManager::getInstance()->getPublishTask(item.id ,this);

}
void CMainWindow::InitKsTask(const TaskFrameRenderingInfoResponse & item)
{
	QList<QString> headers;
	headers.append(tr("序号"));
	headers.append(tr("空三名称"));
	headers.append(tr("照片数量"));
	headers.append(tr("总像素(GP)"));
	headers.append(tr("开始时间"));
	headers.append(tr("结束时间"));
	headers.append(tr("时长"));
	headers.append(tr("状态"));
	int num = item.detailItems.size();
	if (num <= 1)
	{
		ui->tablewidget_task->hide();
		ui->tableWidget_task2->hide();
		return;
	}
	ui->tablewidget_task->show();
	ui->tableWidget_task2->show();
	ui->tablewidget_task->setRowCount(item.detailItems.size());
	ui->tablewidget_task->setColumnCount(headers.size());
	ui->tablewidget_task->setHorizontalHeaderLabels(headers);
	ui->tablewidget_task->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft); //居左
	for (int i = 0; i < item.detailItems.size(); i++)
	{
		JobState::DetailInfo * info = item.detailItems.at(i);
		qint64 starttime = info->startTime;
		QDateTime startTime = QDateTime::fromMSecsSinceEpoch(starttime);
		QString start = startTime.toString("yyyy-MM-dd hh:mm:ss");

		qint64 endtime = info->endTime;
		QDateTime endTime = QDateTime::fromMSecsSinceEpoch(endtime);
		QString end = startTime.toString("yyyy-MM-dd hh:mm:ss");

		qint64 exetime = info->frameExecuteTime;
		qint64 hour, min, s = 0;
		hour = exetime / 3600;
		min = (exetime % 3600) / 60;
		s = exetime % 3600 % 60;
		QString duration = QString("%1:%2:%3").arg(hour, 2, 10, QLatin1Char('0')).arg(min, 2, 10, QLatin1Char('0')).arg(s, 2, 10, QLatin1Char('0'));

		int status = info->frameStatus;
		QString state = "";
		if (status == 1)
		{
			state = tr("等待中");
		}
		else if (status == 2)
		{
			state = tr("计算中");
		}
		else if (status == 3)
		{
			state = tr("停止");
		}
		else if (status == 4)
		{
			state = tr("已完成");
		}
		else
		{
			state = tr("失败");
		}

		ui->tablewidget_task->setItem(i, 0, new QTableWidgetItem(QString("%1").arg(i + 1)));
		QString kmlName = info->kmlName;
		QString picCount = info->picCount;
		QString pixel = info->pixel;
		if (kmlName == NULL)
		{
			kmlName = "merge";
		}
		if (picCount == NULL)
		{
			picCount = "--";
		}
		if (pixel == NULL)
		{
			pixel = "--";
		}
		ui->tablewidget_task->setItem(i, 1, new QTableWidgetItem(kmlName));
		ui->tablewidget_task->setItem(i, 2, new QTableWidgetItem(picCount));
		ui->tablewidget_task->setItem(i, 3, new QTableWidgetItem(pixel));
		ui->tablewidget_task->setItem(i, 4, new QTableWidgetItem(start));
		ui->tablewidget_task->setItem(i, 5, new QTableWidgetItem(end));
		ui->tablewidget_task->setItem(i, 6, new QTableWidgetItem(duration));
		ui->tablewidget_task->setItem(i, 7, new QTableWidgetItem(state));
		//int index = tasklist->items.count() - i - 1;
		//const MaxTaskItem* info = tasklist->items[index];
		////ui.tableWidget->insertRow(i);
		//QTableWidgetItem * item1 = new QTableWidgetItem();
		//QTableWidgetItem * item2 = new QTableWidgetItem();
		//QTableWidgetItem * item3 = new QTableWidgetItem();
		//item1->setText(info->projectName);
		//item1->setTextAlignment(Qt::AlignLeft);
		////item2->setText(info->taskAlias);
		//item2->setText(QString("%1").arg(info->id));
		//item2->setTextAlignment(Qt::AlignLeft);
		//QString aa = QString("%1").arg((info->startTime));
		//qint64 start = info->startTime;
		//if (aa.size() == 10)
		//	start = info->startTime * 1000;
		//QDateTime startTime = QDateTime::fromMSecsSinceEpoch(start);
		//item3->setText(QString("%1").arg(startTime.toString("yyyy-MM-dd")));
		//item3->setTextAlignment(Qt::AlignLeft);
		//ui.tableWidget->setItem(i, 0, item1);
		//ui.tableWidget->setItem(i, 1, item2);
		//ui.tableWidget->setItem(i, 2, item3);
	}
	ui->tablewidget_task->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tablewidget_task->setSelectionBehavior(QAbstractItemView::SelectRows); //设置选中单行
	ui->tablewidget_task->setFocusPolicy(Qt::NoFocus);
	ui->tablewidget_task->setSelectionMode(QTableWidget::SingleSelection);
	ui->tablewidget_task->verticalHeader()->hide();//取消行号
	ui->tablewidget_task->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tablewidget_task->setStyleSheet("selection-background-color: green");//设置选中行颜色
	ui->tablewidget_task->setStyleSheet("border:1px solid #2F2F3A");
	ui->tablewidget_task->verticalHeader()->setDefaultSectionSize(30);
	ui->tablewidget_task->horizontalHeader()->setFixedHeight(30);//设置表头高度
	ui->tablewidget_task->verticalHeader()->setDefaultSectionSize(30);
	ui->tablewidget_task->horizontalHeader()->setDefaultSectionSize(40);
	
}
void CMainWindow::InitKsTask1(const TaskFrameRenderingInfoResponse & item)
{
	QList<QString> headers;
	headers.append(tr("序号"));
	headers.append(tr("空三名称"));
	headers.append(tr("照片数量"));
	headers.append(tr("总像素(GP)"));
	headers.append(tr("开始时间"));
	headers.append(tr("结束时间"));
	headers.append(tr("时长"));
	headers.append(tr("状态"));
	int num = item.detailItems.size();
	if (num <= 1)
	{
		ui->tableWidget_task2->hide();
		return;
	}
	ui->tableWidget_task2->setRowCount(item.detailItems.size());
	ui->tableWidget_task2->setColumnCount(headers.size());
	ui->tableWidget_task2->setHorizontalHeaderLabels(headers);
	ui->tableWidget_task2->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft); //居左
	for (int i = 0; i < item.detailItems.size(); i++)
	{
		JobState::DetailInfo * info = item.detailItems.at(i);
		qint64 starttime = info->startTime;
		QDateTime startTime = QDateTime::fromMSecsSinceEpoch(starttime);
		QString start = startTime.toString("yyyy-MM-dd hh:mm:ss");

		qint64 endtime = info->endTime;
		QDateTime endTime = QDateTime::fromMSecsSinceEpoch(endtime);
		QString end = startTime.toString("yyyy-MM-dd hh:mm:ss");

		qint64 exetime = info->frameExecuteTime;
		qint64 hour, min, s = 0;
		hour = exetime / 3600;
		min = (exetime % 3600) / 60;
		s = exetime % 3600 % 60;
		QString duration = QString("%1:%2:%3").arg(hour, 2, 10, QLatin1Char('0')).arg(min, 2, 10, QLatin1Char('0')).arg(s, 2, 10, QLatin1Char('0'));

		int status = info->frameStatus;
		QString state = "";
		if (status == 1)
		{
			state = tr("等待中");
		}
		else if (status == 2)
		{
			state = tr("计算中");
		}
		else if (status == 3)
		{
			state = tr("停止");
		}
		else if (status == 4)
		{
			state = tr("已完成");
		}
		else
		{
			state = tr("失败");
		}

		ui->tableWidget_task2 ->setItem(i, 0, new QTableWidgetItem(QString("%1").arg(i + 1)));
		QString kmlName = info->kmlName;
		QString picCount = info->picCount;
		QString pixel = info->pixel;
		if (kmlName == NULL)
		{
			kmlName = "merge";
		}
		if (picCount == NULL)
		{
			picCount = "--";
		}
		if (pixel == NULL)
		{
			pixel = "--";
		}
		ui->tablewidget_task->setItem(i, 1, new QTableWidgetItem(kmlName));
		ui->tablewidget_task->setItem(i, 2, new QTableWidgetItem(picCount));
		ui->tablewidget_task->setItem(i, 3, new QTableWidgetItem(pixel));
		ui->tableWidget_task2 ->setItem(i, 4, new QTableWidgetItem(start));
		ui->tableWidget_task2 ->setItem(i, 5, new QTableWidgetItem(end));
		ui->tableWidget_task2 ->setItem(i, 6, new QTableWidgetItem(duration));
		ui->tableWidget_task2 ->setItem(i, 7, new QTableWidgetItem(state));
	}
	ui->tableWidget_task2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableWidget_task2->setSelectionBehavior(QAbstractItemView::SelectRows); //设置选中单行
	ui->tableWidget_task2->setFocusPolicy(Qt::NoFocus);
	ui->tableWidget_task2->setSelectionMode(QTableWidget::SingleSelection);
	ui->tableWidget_task2->verticalHeader()->hide();//取消行号
	ui->tableWidget_task2->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tableWidget_task2->setStyleSheet("selection-background-color: green");//设置选中行颜色
	ui->tableWidget_task2->setStyleSheet("border:1px solid #2F2F3A");
	ui->tableWidget_task2->verticalHeader()->setDefaultSectionSize(30);
	ui->tableWidget_task2->horizontalHeader()->setFixedHeight(30);//设置表头高度
	ui->tableWidget_task2->verticalHeader()->setDefaultSectionSize(30);
	ui->tableWidget_task2->horizontalHeader()->setDefaultSectionSize(40);

}

void CMainWindow::SurvayModuel(int code)
{
	disconnect(this, &CMainWindow::initSurveyModulefinish, this, &CMainWindow::SurvayModuel);

	if (code == 0) {
		Util::ShowMessageBoxQuesion(QObject::tr("刺点模块初始化失败，请检查数据"));
		m_buttonSurveyPoint->setEnabled(true);
		return;
	}
	else if (code == 1)
	{	
		LOGFMTI("CMainWindow::on_btnSurveyPoint_clicked(), begin");
		JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
		if (jobstate == nullptr) return;
		BaseTaskInfo* baseInfo = jobstate->getBaseTaskInfo();
		int total = baseInfo->ctrlPointInfo.itemDatas.size();
		//控制点信息传入SurveyWidget
		SURVEYS_INFO::surveys_data curr_data;

		ContrlPointInfo curr_ctrlPointInfo = baseInfo->ctrlPointInfo;
		//参考坐标系
		SURVEYS_INFO::SRS curr_scs{ 99,"","" };  //srsID  自定义99，避免与xml中已知srsID 重复
		curr_scs.srs_define = curr_ctrlPointInfo.coordSys;
		curr_data.SpatialReferenceSystems.append(curr_scs);

		//计算参数控制点信息
		QList<at_para_calc::controlPointinfo> e;
		at_para_calc::controlPointinfo ecp;
		int tep_srs = curr_scs.srs_define.mid(curr_scs.srs_define.indexOf(":") + 1).toInt();

		//导入控制点数据
		QList<ConfigDataItem*> curr_itemDatas = baseInfo->ctrlPointInfo.itemDatas;
		for (int i = 0; i<curr_itemDatas.size(); i++)
		{
			ConfigDataItem* node = (ConfigDataItem*)curr_itemDatas.at(i);
			SURVEYS_INFO::surveys_point temp{ -1,tep_srs,"Full","" ,{ "","","" },"0.01","0.01",false ,"" };
			at_para_calc::controlPointinfo ep{ tep_srs ,"",0.0,0.0,0.0 };

			temp.control_id = node->index;
			temp.control_name = node->imageName;
			temp.coordinate.x = node->picLongitude;
			temp.coordinate.y = node->picLatitude;
			temp.coordinate.z = node->picAltitude;
			temp.srs_define = curr_scs.srs_define;
			curr_data.ControlPoints.append(temp);

			ep.SRS = tep_srs;
			ep.name = node->imageName;
			ep.x = node->picLongitude.toDouble();
			ep.y = node->picLatitude.toDouble();
			ep.z = node->picAltitude.toDouble();

			e.append(ep);

		}
		ui->widget_surveysPoints->setControlPint(curr_data);		
		LOGFMTI("CMainWindow::on_btnSurveyPoint_clicked(), 2");
		//设置image_pos.json 路径
		t_taskid at_taskid;
		QHash<qint64, JobState*> m_at_task = jobstate->getChildJobState();
		foreach(JobState* job, jobstate->getChildJobState()) {
			if (job->IsParentItem()) {
				foreach(JobState* subJob, job->getChildJobState()) {
					if (!job->m_taskType.compare("Triangulation"))
					{
						at_taskid = job->m_taskId;
					}
				}
			}
			else {
				if (!job->m_taskType.compare("Triangulation"))
				{
					at_taskid = job->m_taskId;
				}
			}
		}
		QString ImagePosPath = RBHelper::getTempPath(QString::number(at_taskid));

		LOGFMTI("[at_taskid]  %s", ImagePosPath.toStdString().c_str());

		ImagePosPath.append(QDir::separator());
		ImagePosPath.append("image_pos.json");
		ui->widget_surveysPoints->setImagePosPath(ImagePosPath);  //设置xml解析的json数据
		ui->widget_surveysPoints->setAtTaskID(at_taskid);

		QString tempdir = QCoreApplication::applicationDirPath();
		QStringList list = tempdir.split("/");

		QString dir_path = list[0] + "/DaYanTempData/" + QString::number(at_taskid);
		QDir().mkpath(dir_path);

		ui->widget_surveysPoints->setRootDir(dir_path);
		//缩略图存放路径
		ui->widget_surveysPoints->setThumimagePath(dir_path + "/rootThumImageDir");  //rootThumImageDir 指定缩略图存放标识根路径
		ui->widget_surveysPoints->setSenceName(jobstate->getProjectName());

		//测量控制点导出路径

		//选择影像路径
		int ret = Util::ShowMessageBoxQuesion(QObject::tr("选择原始影像路径 , 是否要包含子目录？"), QObject::tr("选择文件夹"));
		QString path = RBHelper::getSelectedFolder();
		if (path.isEmpty())
		{
			m_buttonSurveyPoint->setEnabled(true);
			return;
		}
		QStringList pathList; pathList << path;
		ui->widget_surveysPoints->setIsIncludeSubDir(ret);

		if (path.isEmpty())return;
		if (!AtPathJudgment(QString::number(at_taskid), path))
		{
			QScopedPointer<SurveySelectImage> box(new SurveySelectImage());
			if (box->exec() == QDialog::Accepted) {
				m_buttonSurveyPoint->setEnabled(true);
				return;
			}
			m_buttonSurveyPoint->setEnabled(true);
			return;
		}

		ui->widget_surveysPoints->setImagePath(path);

		TipsToastWidget* toast = new TipsToastWidget(ui->widget_page, QObject::tr("数据分析中，请稍后..."), 16, QColor("#1F1F27"));

		ui->widget_surveysPoints->setLicenseUrl("");
		QString LicenseCode = WSConfig::getInstance()->get_key_value("GCPLicense");
		ui->widget_surveysPoints->setLicenseCode(LicenseCode);


		connect(ui->widget_surveysPoints, &SurveyWidget::createthumbnailFinished, toast, &TipsToastWidget::hideToast);
		connect(ui->widget_surveysPoints, &SurveyWidget::createthumbnailFinished, this, &CMainWindow::onSurveyDataLoadFinished);
		onInitSurveyWidget();


	}
}

void CMainWindow::on_btnDownloadATResult_clicked()
{
    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if(jobstate == nullptr) return;
    jobstate = jobstate->getATJob();
    if (jobstate == nullptr) return;
    if (!jobstate->isCanJobOperable()) {
        new ToastWidget(this, QObject::tr("当前任务不可下载!"), QColor("#e25c59"), 3000);
        return;
    }

    QStringList outputList;
    outputList.append(AT_OUTPUT_PATH);
    m_jobListViewPage->downloadRenderOutputFiles(jobstate->getTaskId(), outputList);
}

void CMainWindow::on_btnDownloadResult_clicked()
{
    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if(jobstate == nullptr) return;
    jobstate = jobstate->getRenderJob();
    if (jobstate == nullptr) return;
    if (!jobstate->isCanJobOperable()) {
        new ToastWidget(this, QObject::tr("当前任务不可下载!"), QColor("#e25c59"), 3000);
        return;
    }

    QStringList outputList = jobstate->getDownloadFileList();
    if (jobstate->getProjectOutputType().isEmpty()) {
        outputList.append(RBHelper::getOutputFileName(OUTPUT_TYPE_OSGB));
    } else {
        outputList.append(jobstate->getProjectOutputType());
    }

    m_jobListViewPage->downloadRenderOutputFiles(jobstate->getTaskId(), outputList);
}

void CMainWindow::on_radioButton_all_pos_toggled(bool check)
{
    if (check) {
        ui->widget_select_group->hide();
    }
}

void CMainWindow::on_radioButton_current_pos_toggled(bool check)
{
    if (check) {
        ui->widget_select_group->show();
    }
}

void CMainWindow::on_comboBox_pic_group_currentTextChanged(const QString &text)
{
    QString name = text;
}

void CMainWindow::on_checkBox_lockRange_stateChanged(int state)
{
    bool bCheck = (state == Qt::Checked ? true : false);
    ui->widget_range_file->setVisible(!bCheck);
}

void CMainWindow::on_checkBox_lockRange_3_stateChanged(int state)
{
	bool bCheck = (state == Qt::Checked ? true : false);
	ui->widget_range_file_3->setVisible(!bCheck);
	ui->widget_range_file_3->setEnabled(!bCheck);
	ui->btnRangeFile_3->setEnabled(!bCheck);

}

void CMainWindow::on_checkBox_blockAT_stateChanged(int state)
{
	bool bCheck = (state == Qt::Checked ? true : false);
	ui->widget_BlockAT_radioBtn->setVisible(bCheck);
}

void CMainWindow::on_btnKmlBack_clicked()
{
	ui->stackedWidget_project->setCurrentIndex(PROJECT_PAGE_PROJECT);
//	updateStackRenderPage(RENDER_PAGE_PROJECT);
	jobStateMachine();
	resetUploadCtrl();
	// 控制点view
	resetCtrlPointViewSet();
	// pos view
	resetPositionViewSet();
	// 侧边栏
	modifySideBarButtonVisible(m_projectType);
}


bool CMainWindow::loadContrlPointData()
{
    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if (jobstate == nullptr) return false;

    // 1. 如果任务还未上传，则从本地目录加载文件数据
    // 2. 任务已经上传完成，数据从project-data中直接填表
    if (jobstate->getProjectType() == enImageProject) {
        if (!jobstate->IsUploadFinished()) {
            if (!QFileInfo::exists(ui->lineEdit_path_ctrl->text())) {
                new ToastWidget(this, QObject::tr("请添加控制点文件!"), QColor("#e25c59"), 2000);
                return false;
            }

            ConfigAnalyzeParams param;
            param.taskId = m_currTaskId;
            param.fileType = enTypeCtrlPointFile;
            param.fieldOrder = RBHelper::getFieldOrderString(ui->comboBox_field_order_ctrl->currentIndex());
            param.filePath = ui->lineEdit_path_ctrl->text();
            param.ignoreLines = ui->comboBox_ignore_lines_ctrl->currentText();
            param.spliteChar = RBHelper::getSplitChar(ui->comboBox_splite_char_ctrl->currentIndex());

            LoadConfigDataThread* loadJob = new LoadConfigDataThread(param);
            int code = loadJob->filePreCheck();
            if (code != CODE_SUCCESS) {
                showErrorMsg(code);
                loadJob->deleteLater();
                return false;
            }
            connect(loadJob, &LoadConfigDataThread::loadDataFinished, this, &CMainWindow::onLoadContrlPointDataFinished);
            loadJob->start();

            this->showLoading(true);
        } else {
            initCtrlPointPageCtrl(ui->widget_page_ctrl_view);
            initCtrlPointPageCtrl(ui->widget_page_ctrl_submit);
        }
    
    } else if (jobstate->getProjectType() == enBlockProject) {
        initCtrlPointPageCtrl(ui->widget_page_ctrl_view);
        initCtrlPointPageCtrl(ui->widget_page_ctrl_submit);      
    }
    
    return true;
}

bool CMainWindow::loadPositionInfoData()
{
    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if (jobstate == nullptr) return false;

    // 1. 如果任务还未上传，则从本地目录加载文件数据
    // 2. 任务已经上传完成，数据从project-data中直接填表
    if (jobstate->getProjectType() == enImageProject) {
        if (!jobstate->IsUploadFinished()) {
            if (!QFileInfo::exists(ui->lineEdit_path_pos->text())) {
                new ToastWidget(this, QObject::tr("请添加Pos文件!"), QColor("#e25c59"), 2000);
                return false;
            }

            ConfigAnalyzeParams param;
            param.taskId = m_currTaskId;
            param.fileType = enTypePosFile;
            param.fieldOrder = RBHelper::getFieldOrderString(ui->comboBox_field_order_pos->currentIndex());
            param.filePath = ui->lineEdit_path_pos->text();
            param.ignoreLines = ui->comboBox_ignore_lines_pos->currentText();
            param.spliteChar = RBHelper::getSplitChar(ui->comboBox_splite_char_pos->currentIndex());

            LoadConfigDataThread* loadJob = new LoadConfigDataThread(param);
            int code = loadJob->filePreCheck();
            if (code != CODE_SUCCESS) {
                showErrorMsg(code);
                loadJob->deleteLater();
                return false;
            }
            connect(loadJob, &LoadConfigDataThread::loadDataFinished, this, &CMainWindow::onLoadPosInfoDataFinished);
            loadJob->start();

            this->showLoading(true);

        } else {
            initPositionPageCtrl(ui->widget_page_pos_view);
            initPositionPageCtrl(ui->widget_page_pos_submit);
        }
    
    } else if (jobstate->getProjectType() == enBlockProject) {
        initPositionPageCtrl(ui->widget_page_pos_view);
        initPositionPageCtrl(ui->widget_page_pos_submit);  
    }

    return true;
}

void CMainWindow::onLoadContrlPointDataFinished(int code)
{
    this->showLoading(false);

    if (code != CODE_SUCCESS) {
        showErrorMsg(code);
        return;
    }
    LoadConfigDataThread* loadJob = qobject_cast<LoadConfigDataThread*>(sender());
    if (loadJob == nullptr)
        return;
    disconnect(loadJob, &LoadConfigDataThread::loadDataFinished, this, &CMainWindow::onLoadContrlPointDataFinished);
    if (loadJob->getTaskId() != m_currTaskId) {
        loadJob->deleteLater();
        return;
    }

    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if (jobstate == nullptr) {
        loadJob->deleteLater();
        return;
    }
        
    // 填充表
    QList<ConfigDataItem*> dataItems = loadJob->getConfigDataItems();
    if (!jobstate->isSubmitCtrlPoint()) { // 是否提交控制点信息， 提交了则直接显示，否则初始化控制点基本信息
        BaseTaskInfo* info = jobstate->getBaseTaskInfo();
        ContrlPointInfo ctrlPoint;
        ctrlPoint.coordSys = ui->widget_coord_edit_2->getCoordSystemValue();
        ctrlPoint.fieldOrder = RBHelper::getFieldOrderString(ui->comboBox_field_order_ctrl->currentIndex());
        ctrlPoint.filePath = ui->lineEdit_path_ctrl->text();
        ctrlPoint.ignoreLines = ui->comboBox_ignore_lines_ctrl->currentText();
        ctrlPoint.spliteChar = RBHelper::getSplitChar(ui->comboBox_splite_char_ctrl->currentIndex());
        ctrlPoint.itemDatas = dataItems;
        info->ctrlPointInfo = ctrlPoint;
        jobstate->updataBaseTaskInfo(info);
    }

    initCtrlPointPageCtrl(ui->widget_page_ctrl_view);
    initCtrlPointPageCtrl(ui->widget_page_ctrl_submit);

    loadJob->deleteLater();
}

void CMainWindow::onLoadPosInfoDataFinished(int code)
{
    this->showLoading(false);

    if (code != CODE_SUCCESS) {
        showErrorMsg(code);
        return;
    }
    LoadConfigDataThread* loadJob = qobject_cast<LoadConfigDataThread*>(sender());
    if (loadJob == nullptr)
        return;
    disconnect(loadJob, &LoadConfigDataThread::loadDataFinished, this, &CMainWindow::onLoadPosInfoDataFinished);
    if (loadJob->getTaskId() != m_currTaskId) {
        loadJob->deleteLater();
        return;
    }

    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if (jobstate == nullptr) {
        loadJob->deleteLater();
        return;
    }

    // 填充表
    QList<ConfigDataItem*> dataItems = loadJob->getConfigDataItems();
    BaseTaskInfo* info = jobstate->getBaseTaskInfo();
    PositionInfo pos;
    pos.coordSys = ui->widget_coord_edit_3->getCoordSystemValue();
    pos.fieldOrder = RBHelper::getFieldOrderString(ui->comboBox_field_order_pos->currentIndex());
    pos.filePath = ui->lineEdit_path_pos->text();
    pos.ignoreLines = ui->comboBox_ignore_lines_pos->currentText();
    pos.spliteChar = RBHelper::getSplitChar(ui->comboBox_splite_char_pos->currentIndex());
    pos.itemDatas = dataItems;
    // 确定哪些group需要设置pos信息
    pos.applyGroups.clear();
    if (ui->radioButton_all_pos->isChecked()) {
        foreach(PictureGroup* group, info->groups) {
            pos.applyGroups.append(group->groupId);
        }     
    } else {
        QStringList groupIds = ui->comboBox_pic_group->selectedItemDatas();
        pos.applyGroups.append(groupIds);
    }

    info->posInfo = pos;
    jobstate->updataBaseTaskInfo(info);
    applyPosInfoForPicture(jobstate);   
    onItemSelectionChanged();

    initPositionPageCtrl(ui->widget_page_pos_view);
    initPositionPageCtrl(ui->widget_page_pos_submit);
    
    loadJob->deleteLater();
}

void CMainWindow::showErrorMsg(int code)
{
    switch (code)
    {
    case CODE_IGNORE_LINE_SET_ERROR:
        new ToastWidget(this, QObject::tr("文件解析失败，忽略行数选择错误!"), QColor("#e25c59"), 5000);
        break;
    case CODE_SPLITE_CHAR_SET_ERROR: 
        new ToastWidget(this, QObject::tr("文件解析失败，分隔符选择错误!"), QColor("#e25c59"), 5000);
        break;
    case CODE_FILE_NOT_EXIST_ERROR: 
        new ToastWidget(this, QObject::tr("文件不存在!"), QColor("#e25c59"), 5000);
        break;
    case CODE_FILE_LOAD_FAILED_ERROR: 
        new ToastWidget(this, QObject::tr("文件加载失败!"), QColor("#e25c59"), 5000);
        break;
    case CODE_JSON_FILE_LOAD_FAILED:
        new ToastWidget(this, QObject::tr("项目文件加载失败，相关文件可能已经丢失或损坏!"), QColor("#e25c59"), 5000);
        break;
    case CODE_UNKOWN_ERROR:
        new ToastWidget(this, QObject::tr("未知错误!"), QColor("#e25c59"), 5000);
        break;
    case CODE_ERROR_NOT_POS_FILE:
        new ToastWidget(this, QObject::tr("文件跟影像数据不匹配，请重新添加!"), QColor("#e25c59"), 5000);
        break;
    case CODE_ERROR_NOT_CTRL_FILE:
        new ToastWidget(this, QObject::tr("没有有效的控制点，请重新添加!"), QColor("#e25c59"), 5000);
        break;
    case CODE_ILLEGAL_FILE:
        new ToastWidget(this, QObject::tr("非法文件!"), QColor("#e25c59"), 5000);
        break;
	case CODE_ERROT_NOT_TIEPOINT_FILE:
		new ToastWidget(this, QObject::tr("空三文件不带连接点，请重新导入!"), QColor("#e25c59"), 5000);
		break;
    case CODE_SUCCESS:         
        break;
    default:
        break;
    }
}

void CMainWindow::loadConfigDataToTable(QTableWidget* table, const QList<ConfigDataItem*>& items)
{
    if (table == nullptr || items.isEmpty())
        return;

    table->clearContents();
    table->setRowCount(0);
    // table->setRowCount(0);
    table->blockSignals(true);
    for (int row = 0; row < items.size(); row++) {

        ConfigDataItem* info = items.at(row);
        table->insertRow(row);

        // num
        QString num = QString::number(info->index);
        QTableWidgetItem *numItem = new QTableWidgetItem(num);
        numItem->setTextAlignment(Qt::AlignCenter);
        table->setItem(row, 0, numItem);

        // image name
        QTableWidgetItem *imageItem = new QTableWidgetItem(info->imageName);
        imageItem->setTextAlignment(Qt::AlignCenter);
        table->setItem(row, 1, imageItem);

        // picLongitude
        QTableWidgetItem *picLongitude = new QTableWidgetItem(info->picLongitude);
        picLongitude->setTextAlignment(Qt::AlignCenter);
        table->setItem(row, 2, picLongitude);

        // picLatitude
        QTableWidgetItem *picLatitude = new QTableWidgetItem(info->picLatitude);
        picLatitude->setTextAlignment(Qt::AlignCenter);
        table->setItem(row, 3, picLatitude);

        // picElevation
        QTableWidgetItem *picElevation = new QTableWidgetItem(info->picAltitude);
        picElevation->setTextAlignment(Qt::AlignCenter);
        table->setItem(row, 4, picElevation);
    }
    table->blockSignals(false);
}

bool CMainWindow::initProjectPage()
{
    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if (jobstate == nullptr) {
        return false;
    }

    resetWidgetConfig();
	ui->lineEditProject->clear();
	ui->lineEdit_range_file_3->clear();
	ui->lineEdit_range_file->clear();
    resetUploadCtrl();
    jobStateMachine();

    ui->label_project_name->setText(jobstate->getProjectName());
    if (jobstate->getProjectName().isEmpty()) {
        ui->label_project_name->setText(jobstate->getBaseTaskInfo()->projectName);
    }

    updateStackRenderPage(RENDER_PAGE_PROJECT);

    // 控制点view
    resetCtrlPointViewSet();
    // pos view
    resetPositionViewSet();
    // 侧边按钮
    modifySideBarButtonVisible(jobstate->getProjectType(), RBHelper::checkProjectExist(m_currTaskId));

    BaseTaskInfo *baseInfo = jobstate->getBaseTaskInfo();
    // 填充列表
    loadPictureGroup(baseInfo->groups);

    if (!baseInfo->groups.isEmpty()) {
        loadPicturesInfo(baseInfo->groups[0]->pictures);
		RecommendedCoordinateSystem(baseInfo->groups[0]->pictures[0].picLongitude, baseInfo->groups[0]->pictures[0].picLatitude);
    }
    return true;
}

bool CMainWindow::initATReportPage()
{
	JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
	if (jobstate == nullptr) return false;					
	QString report_file = RBHelper::getTempPath(QString::number(jobstate->getATJob()->getTaskId()));
	report_file.append(QDir::separator());
	report_file.append("report.json");
	AT_REPORT_INFO::At_Report reportInfo;
	getReportInfo(report_file, reportInfo);

#if 1
	/*空三成果概述**********************************************************/
    ui->tableWidget_at_overview->setRowCount(1);
    ui->tableWidget_at_overview->setColumnCount(6);
    // col 1
    {
//         QTableWidgetItem *item_name_row1 = new QTableWidgetItem(QObject::tr("作业名称"));
//         ui->tableWidget_at_overview->setItem(0, 0, item_name_row1);

        QTableWidgetItem *item_name_row2 = new QTableWidgetItem(reportInfo.project.name);
        ui->tableWidget_at_overview->setItem(0, 0, item_name_row2);
    }
    // col 2
    {
//         QTableWidgetItem *item_cam_row1 = new QTableWidgetItem(QObject::tr("相机组数量"));
//         ui->tableWidget_at_overview->setItem(0, 1, item_cam_row1);

        QTableWidgetItem *item_cam_row2 = new QTableWidgetItem(QString::number(reportInfo.project.photo_group_num));
        ui->tableWidget_at_overview->setItem(0, 1, item_cam_row2);
    }
    // col 3
    {
//         QTableWidgetItem *item_pic_row1 = new QTableWidgetItem(QObject::tr("相片数"));
//         ui->tableWidget_at_overview->setItem(0, 2, item_pic_row1);

        QTableWidgetItem *item_pic_row2 = new QTableWidgetItem(QString::number(reportInfo.project.photo_num));
        ui->tableWidget_at_overview->setItem(0, 2, item_pic_row2);
    }
	//col 4 
	{
// 		QTableWidgetItem *item_cal_pic_row1 = new QTableWidgetItem(QObject::tr("校准照片数"));
// 		ui->tableWidget_at_overview->setItem(0, 3, item_cal_pic_row1);

		QTableWidgetItem *item_cal_pic_row2 = new QTableWidgetItem(QString::number(reportInfo.project.calibration_photo_num));
		ui->tableWidget_at_overview->setItem(0, 3, item_cal_pic_row2);
	}
	//col 5 
	{
// 		QTableWidgetItem *item_err_pic_row1 = new QTableWidgetItem(QObject::tr("错误照片数"));
// 		ui->tableWidget_at_overview->setItem(0, 4, item_err_pic_row1);

		QTableWidgetItem *item_err_pic_row2 = new QTableWidgetItem(QString::number(reportInfo.project.error_photo_num));
		ui->tableWidget_at_overview->setItem(0, 4, item_err_pic_row2);
	}
	//col 6
	{
// 		QTableWidgetItem *item_rms_row1 = new QTableWidgetItem(QObject::tr("重投影误差RMS"));
// 		ui->tableWidget_at_overview->setItem(0, 5, item_rms_row1);

		QTableWidgetItem *item_rms_row2 = new QTableWidgetItem(reportInfo.project.RMS);
		ui->tableWidget_at_overview->setItem(0, 5, item_rms_row2);
	}

#endif

	/*相机组信息**********************************************************/
	ui->tableWidget_cameragroup->setRowCount(reportInfo.photogroups.size());
	ui->tableWidget_cameragroup->setColumnCount(13);
// 	// row 1 表头
// 	QTableWidgetItem *item_photogroups_col1 = new QTableWidgetItem(QObject::tr("照片组名称"));
// 	ui->tableWidget_cameragroup->setItem(0, 0, item_photogroups_col1);
// 
// 	QTableWidgetItem *item_photogroups_col2 = new QTableWidgetItem(QObject::tr("相片数量"));
// 	ui->tableWidget_cameragroup->setItem(0, 1, item_photogroups_col2);
// 
// 	QTableWidgetItem *item_photogroups_col3 = new QTableWidgetItem(QObject::tr("影像宽"));
// 	ui->tableWidget_cameragroup->setItem(0, 2, item_photogroups_col3);
// 
// 	QTableWidgetItem *item_photogroups_col4 = new QTableWidgetItem(QObject::tr("影像高"));
// 	ui->tableWidget_cameragroup->setItem(0, 3, item_photogroups_col4);
// 
// 	QTableWidgetItem *item_photogroups_col5 = new QTableWidgetItem(QObject::tr("传感器尺寸[mm]"));
// 	ui->tableWidget_cameragroup->setItem(0, 4, item_photogroups_col5);
// 
// 	QTableWidgetItem *item_photogroups_col6 = new QTableWidgetItem(QObject::tr("f"));
// 	ui->tableWidget_cameragroup->setItem(0, 5, item_photogroups_col6);
// 
// 	QTableWidgetItem *item_photogroups_col7 = new QTableWidgetItem(QObject::tr("x"));
// 	ui->tableWidget_cameragroup->setItem(0, 6, item_photogroups_col7);
// 
// 	QTableWidgetItem *item_photogroups_col8 = new QTableWidgetItem(QObject::tr("y"));
// 	ui->tableWidget_cameragroup->setItem(0, 7, item_photogroups_col8);
// 
// 	QTableWidgetItem *item_photogroups_col9 = new QTableWidgetItem(QObject::tr("K1"));
// 	ui->tableWidget_cameragroup->setItem(0, 8, item_photogroups_col9);
// 
// 	QTableWidgetItem *item_photogroups_col10 = new QTableWidgetItem(QObject::tr("K2"));
// 	ui->tableWidget_cameragroup->setItem(0, 9, item_photogroups_col10);
// 
// 	QTableWidgetItem *item_photogroups_col11 = new QTableWidgetItem(QObject::tr("K3"));
// 	ui->tableWidget_cameragroup->setItem(0, 10, item_photogroups_col11);
// 
// 	QTableWidgetItem *item_photogroups_col12 = new QTableWidgetItem(QObject::tr("P1"));
// 	ui->tableWidget_cameragroup->setItem(0, 11, item_photogroups_col12);
// 
// 	QTableWidgetItem *item_photogroups_col13 = new QTableWidgetItem(QObject::tr("P2"));
// 	ui->tableWidget_cameragroup->setItem(0, 12, item_photogroups_col13);

	for (size_t i = 0;i<reportInfo.photogroups.size();i++)
	{
		QTableWidgetItem *item_photogroups_col1 = new QTableWidgetItem(reportInfo.photogroups[i].name);
		ui->tableWidget_cameragroup->setItem(i, 0, item_photogroups_col1);

		QTableWidgetItem *item_photogroups_col2 = new QTableWidgetItem(QString::number(reportInfo.photogroups[i].photos_num));
		ui->tableWidget_cameragroup->setItem(i, 1, item_photogroups_col2);

		QTableWidgetItem *item_photogroups_col3 = new QTableWidgetItem(reportInfo.photogroups[i].width);
		ui->tableWidget_cameragroup->setItem(i, 2, item_photogroups_col3);

		QTableWidgetItem *item_photogroups_col4 = new QTableWidgetItem(reportInfo.photogroups[i].height);
		ui->tableWidget_cameragroup->setItem(i, 3, item_photogroups_col4);

		QTableWidgetItem *item_photogroups_col5 = new QTableWidgetItem(reportInfo.photogroups[i].sensor_size);
		ui->tableWidget_cameragroup->setItem(i, 4, item_photogroups_col5);

		QTableWidgetItem *item_photogroups_col6 = new QTableWidgetItem(reportInfo.photogroups[i].camera.focal_length);
		ui->tableWidget_cameragroup->setItem(i, 5, item_photogroups_col6);

		QTableWidgetItem *item_photogroups_col7 = new QTableWidgetItem(reportInfo.photogroups[i].camera.x);
		ui->tableWidget_cameragroup->setItem(i, 6, item_photogroups_col7);

		QTableWidgetItem *item_photogroups_col8 = new QTableWidgetItem(reportInfo.photogroups[i].camera.y);
		ui->tableWidget_cameragroup->setItem(i, 7, item_photogroups_col8);

		QTableWidgetItem *item_photogroups_col9 = new QTableWidgetItem(reportInfo.photogroups[i].camera.K1);
		ui->tableWidget_cameragroup->setItem(i, 8, item_photogroups_col9);

		QTableWidgetItem *item_photogroups_col10 = new QTableWidgetItem(reportInfo.photogroups[i].camera.K2);
		ui->tableWidget_cameragroup->setItem(i, 9, item_photogroups_col10);

		QTableWidgetItem *item_photogroups_col11 = new QTableWidgetItem(reportInfo.photogroups[i].camera.K3);
		ui->tableWidget_cameragroup->setItem(i, 10, item_photogroups_col11);

		QTableWidgetItem *item_photogroups_col12 = new QTableWidgetItem(reportInfo.photogroups[i].camera.P1);
		ui->tableWidget_cameragroup->setItem(i, 11, item_photogroups_col12);

		QTableWidgetItem *item_photogroups_col13 = new QTableWidgetItem(reportInfo.photogroups[i].camera.P2);
		ui->tableWidget_cameragroup->setItem(i, 12, item_photogroups_col13);
	}

	/*连接点信息**********************************************************/
	ui->tableWidget_tiepointsinfo->setRowCount(1);
	ui->tableWidget_tiepointsinfo->setColumnCount(6);
	// col 1 
	{
// 		QTableWidgetItem *item_tiepointsinfo_row00 = new QTableWidgetItem(QObject::tr("点数"));
// 		ui->tableWidget_tiepointsinfo->setItem(0, 0, item_tiepointsinfo_row00);

		QTableWidgetItem *item_tiepointsinfo_row10 = new QTableWidgetItem(reportInfo.tie_point.NumberOfPoints);
		ui->tableWidget_tiepointsinfo->setItem(0, 0, item_tiepointsinfo_row10);
	}
	//col 2
	{
// 		QTableWidgetItem *item_tiepointsinfo_row01 = new QTableWidgetItem(QObject::tr("每个点的中位影像数"));
// 		ui->tableWidget_tiepointsinfo->setItem(0, 1, item_tiepointsinfo_row01);

		QTableWidgetItem *item_tiepointsinfo_row11 = new QTableWidgetItem(reportInfo.tie_point.MedianNumberOfPhotosPerPoint);
		ui->tableWidget_tiepointsinfo->setItem(0, 1, item_tiepointsinfo_row11);
	}
	//col 3
	{
// 		QTableWidgetItem *item_tiepointsinfo_row02 = new QTableWidgetItem(QObject::tr("每个影像的中位点数"));
// 		ui->tableWidget_tiepointsinfo->setItem(0, 2, item_tiepointsinfo_row02);

		QTableWidgetItem *item_tiepointsinfo_row12 = new QTableWidgetItem(reportInfo.tie_point.MedianNumberOfPointsPerPhoto);
		ui->tableWidget_tiepointsinfo->setItem(0, 2, item_tiepointsinfo_row12);
	}
	//col 4
	{
// 		QTableWidgetItem *item_tiepointsinfo_row03 = new QTableWidgetItem(QObject::tr("中位重投影误差[像素]"));
// 		ui->tableWidget_tiepointsinfo->setItem(0, 3, item_tiepointsinfo_row03);

		QTableWidgetItem *item_tiepointsinfo_row13 = new QTableWidgetItem(reportInfo.tie_point.MedianReprojectionError);
		ui->tableWidget_tiepointsinfo->setItem(0, 3, item_tiepointsinfo_row13);
	}
	//col 5
	{
// 		QTableWidgetItem *item_tiepointsinfo_row04 = new QTableWidgetItem(QObject::tr("重投影误差[像素]"));
// 		ui->tableWidget_tiepointsinfo->setItem(0, 4, item_tiepointsinfo_row04);

		QTableWidgetItem *item_tiepointsinfo_row14 = new QTableWidgetItem(reportInfo.tie_point.RMSOfReprojectionErrors);
		ui->tableWidget_tiepointsinfo->setItem(0, 4, item_tiepointsinfo_row14);
	}
	//col 6
	{
// 		QTableWidgetItem *item_tiepointsinfo_row05 = new QTableWidgetItem(QObject::tr("与光线的距离的RMS[米]"));
// 		ui->tableWidget_tiepointsinfo->setItem(0, 5, item_tiepointsinfo_row05);

		QTableWidgetItem *item_tiepointsinfo_row15 = new QTableWidgetItem(reportInfo.tie_point.RMSOfDistancesToRays);
		ui->tableWidget_tiepointsinfo->setItem(0, 5, item_tiepointsinfo_row15);
	}
	

	/*每个影像的连接点信息**********************************************************/
	ui->tableWidget_PointsPreImageInfo->setRowCount(reportInfo.photo.size());
	ui->tableWidget_PointsPreImageInfo->setColumnCount(4);
// 	// row 1 表头
// 	QTableWidgetItem *PointsPreImage_col1 = new QTableWidgetItem(QObject::tr("影像名称"));
// 	ui->tableWidget_PointsPreImageInfo->setItem(0, 0, PointsPreImage_col1);
// 
// 	QTableWidgetItem *PointsPreImage_col2 = new QTableWidgetItem(QObject::tr("连接点数"));
// 	ui->tableWidget_PointsPreImageInfo->setItem(0, 1, PointsPreImage_col2);
// 
// 	QTableWidgetItem *PointsPreImage_col3 = new QTableWidgetItem(QObject::tr("重投影误差[像素]"));
// 	ui->tableWidget_PointsPreImageInfo->setItem(0, 2, PointsPreImage_col3);
// 
// 	QTableWidgetItem *PointsPreImage_col4 = new QTableWidgetItem(QObject::tr("与光线的距离的RMS[米]"));
// 	ui->tableWidget_PointsPreImageInfo->setItem(0, 3, PointsPreImage_col4);

	for (size_t i = 0; i < reportInfo.photo.size(); i++)
	{
		QTableWidgetItem *PointsPreImage_col1 = new QTableWidgetItem(reportInfo.photo[i].file_name);
		ui->tableWidget_PointsPreImageInfo->setItem(i, 0, PointsPreImage_col1);

		QTableWidgetItem *PointsPreImage_col2 = new QTableWidgetItem(reportInfo.photo[i].NumberOfPoints);
		ui->tableWidget_PointsPreImageInfo->setItem(i, 1, PointsPreImage_col2);

		QTableWidgetItem *PointsPreImage_col3 = new QTableWidgetItem(reportInfo.photo[i].RMSOfReprojectionErrors);
		ui->tableWidget_PointsPreImageInfo->setItem(i, 2, PointsPreImage_col3);

		QTableWidgetItem *PointsPreImage_col4 = new QTableWidgetItem(reportInfo.photo[i].RMSOfDistancesToRays);
		ui->tableWidget_PointsPreImageInfo->setItem(i, 3, PointsPreImage_col4);
	}

	/*控制点整体误差信息**********************************************************/
	ui->tableWidget_controlpointinfo->setRowCount(1);
	QTableWidgetItem *cps_col1 = new QTableWidgetItem(reportInfo.cps_info.NumberOfPoints);
	ui->tableWidget_controlpointinfo->setItem(0, 0, cps_col1);

	QTableWidgetItem *cps_col2 = new QTableWidgetItem(reportInfo.cps_info.RMSOfVerticalErrors);
	ui->tableWidget_controlpointinfo->setItem(0, 1, cps_col2);

	QTableWidgetItem *cps_col3 = new QTableWidgetItem(reportInfo.cps_info.RMSOfHorizontalErrors);
	ui->tableWidget_controlpointinfo->setItem(0, 2, cps_col3);

	QTableWidgetItem *cps_col4 = new QTableWidgetItem(reportInfo.cps_info.RMSOf3DErrors);
	ui->tableWidget_controlpointinfo->setItem(0, 3, cps_col4);

	QTableWidgetItem *cps_col5 = new QTableWidgetItem(reportInfo.cps_info.RMSOfReprojectionErrors);
	ui->tableWidget_controlpointinfo->setItem(0, 4, cps_col5);

	QTableWidgetItem *cps_col6 = new QTableWidgetItem(reportInfo.cps_info.MedianReprojectionError);
	ui->tableWidget_controlpointinfo->setItem(0, 5, cps_col6);

	QTableWidgetItem *cps_col7 = new QTableWidgetItem(reportInfo.cps_info.RMSOfDistancesToRays);
	ui->tableWidget_controlpointinfo->setItem(0, 6, cps_col7);


	/*各个控制点误差信息**********************************************************/
	ui->tableWidget_EveryCpInfo->setRowCount(reportInfo.cp_info.size());
	for (size_t i = 0; i < reportInfo.cp_info.size(); i++)
	{
		QTableWidgetItem *cp_col1 = new QTableWidgetItem(reportInfo.cp_info[i].Name);
		ui->tableWidget_EveryCpInfo->setItem(i, 0, cp_col1);

		QTableWidgetItem *cp_col2 = new QTableWidgetItem(reportInfo.cp_info[i].HorizontalXError3D);
		ui->tableWidget_EveryCpInfo->setItem(i, 1, cp_col2);

		QTableWidgetItem *cp_col3 = new QTableWidgetItem(reportInfo.cp_info[i].HorizontalYError3D);
		ui->tableWidget_EveryCpInfo->setItem(i, 2, cp_col3);

		QTableWidgetItem *cp_col4 = new QTableWidgetItem(reportInfo.cp_info[i].VerticalError3D);
		ui->tableWidget_EveryCpInfo->setItem(i, 3, cp_col4);

		QTableWidgetItem *cp_col5 = new QTableWidgetItem(reportInfo.cp_info[i].Error3D);
		ui->tableWidget_EveryCpInfo->setItem(i, 4, cp_col5);

		QTableWidgetItem *cp_col6 = new QTableWidgetItem(reportInfo.cp_info[i].RMSOfReprojectionErrors);
		ui->tableWidget_EveryCpInfo->setItem(i, 5, cp_col6);

		QTableWidgetItem *cp_col7 = new QTableWidgetItem(reportInfo.cp_info[i].RMSOfDistancesToRays);
		ui->tableWidget_EveryCpInfo->setItem(i, 6, cp_col7);

	}


    return true;
}

void CMainWindow::resetUploadCtrl()
{
    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if (jobstate == nullptr) return;

    // 如果任务已经是非上传任务，则将上传控件隐藏
    if (!jobstate->IsUploadFinished()) {
        ui->widget_upload_state->show();
    } else {
        ui->widget_upload_state->hide();
    }

    ui->label_upload_state->setText(jobstate->getStatusText());
    QString picCount = QString("%1/%2").arg(jobstate->getCompleteFileCount()).arg(jobstate->getTotalFileCount());
    ui->label_picture_count->setText(picCount);
    ui->label_upload_remain->setText("0");
    ui->label_upload_speed->setText("0Mbps");
    ui->progressBar_upload->setValue(0);

    if (jobstate->getTransStatus() != enNothing && jobstate->IsUploadCanStop()) {
        ui->widget_upload_progress->show();
        ui->widget_upload_state->setStyleSheet(".QWidget{background-color:#1f1f27;}");
        ui->btnUpload->setText(QObject::tr("取消上传"));
        ui->btnUpload->setProperty("state", enCancelUpload);
    } else {
        ui->widget_upload_progress->hide();
        ui->widget_upload_state->setStyleSheet(".QWidget{background-color:transparent;}");
        ui->btnUpload->setText(QObject::tr("开始提交"));
        ui->btnUpload->setProperty("state", enStartUpload);
    }
}

void CMainWindow::updateUploadState()
{
    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if (jobstate == nullptr) return;

    // state
    ui->label_upload_state->setText(jobstate->getStatusText());
    // count
    qreal percentage = jobstate->getUploadProgressByFileSize();
    if (percentage < 0.0) percentage = 0.0;
    if (percentage > 1.0) percentage = 1.0;
    if (percentage == 1.0 && jobstate->getTransStatus() != enUploadCompleted) {
        percentage = 0.0;
    }
    percentage = QString::number(percentage * 100.0f, 'f', 2).toFloat();
    QString picCount = QString("%1/%2(%3%)").arg(jobstate->getCompleteFileCount()).arg(jobstate->getTotalFileCount()).arg(percentage);
    ui->label_picture_count->setText(picCount);
    // speed
    QString speed = RBHelper::convertTransSpeedToStr(jobstate->getTransSpeed(), DISPLAY_UNIT_BIT);
    ui->label_upload_speed->setText(speed);
    // progress
    ui->progressBar_upload->setValue(percentage);
    // remain 
    qint32 remain = jobstate->getUploadRemainTime();
    QString time = RBHelper::timeCostToString(remain);
    if (remain <= 0) {
        time = "-";
    }
    ui->label_upload_remain->setText(time);
}

void CMainWindow::onTaskItemDoubleClicked(int taskId)
{
	// 隐藏下载页面
	this->onHideDownloadView();
	// 隐藏详情页
	if (m_detailViewPage != nullptr) {
		m_detailViewPage->hide();
	}
	JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(taskId);
	if (jobstate == nullptr) return;
	if (jobstate->IsChildItem()) return;

	if (jobstate->getProjectType() == enImageProject)
	{
		if (!RBHelper::checkProjectExist(taskId))
		{
			CTaskCenterManager::Inst()->downloadTaskJson(taskId, taskId, MyConfig.accountSet.userId);
			new ToastWidget(this, QObject::tr("配置文件下载中，请稍等"), QColor("#675ffe"), 2000);
			return;
		}
	}

	if (jobstate->IsParentTaskSubmitting()
		|| jobstate->IsParentTaskStateUpdating()
		|| jobstate->IsParentTaskSubmitFailed()) // 提交失败,提交中，状态更新中 的任务不能操作
		return;

	m_currTaskId = taskId;
	jobStateMachine();

	if (!RBHelper::checkProjectExist(taskId)) {
		if (jobstate->IsUploadFinished()) {
			LOGFMTE("[CMainWindow] Project not exist! taskid = %d", taskId);
			renderPageSwitch();
			//如果本地没有项目数据，则只展示空三和重建等相关的按钮
			modifySideBarButtonVisible(jobstate->getProjectType(), false);
		}
	}
	else {
		// 如果本地没有项目数据，则只展示空三和重建等相关的按钮
		updateStackRenderPage(RENDER_PAGE_PROJECT);

		// 加载本地项目数据
		LoadProjectDataThread* project = new LoadProjectDataThread(taskId, jobstate->getBaseTaskInfo());
		connect(project, &LoadProjectDataThread::loadProjectDataFinished, this, &CMainWindow::onLoadProjectDataFinished);
		project->start();

		this->showLoading(true);
	}

	// 是否展示上一步
	if (jobstate->getJobType() == UPLOAD_JOB) {
		if (jobstate->IsUploadCanStop()) {
			ui->btnBackAssets->hide();
		}
		else {
			ui->btnBackAssets->show();
		}
	}

	// 如果得到的结果是enPageDisplayNone，则直接返回
	if (jobstate->pageDisplayByState() == enPageDisplayNone) {
		return;
	}
	// 切换到项目数据页
	ui->stackedWidget_task->setCurrentIndex(TASK_PAGE_RENDER);
}

void CMainWindow::renderPageSwitch(bool switchPage, int pageId)
{
    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if (jobstate == nullptr) return;

    // 没有上传完成的任务不做切换
    if (!jobstate->IsUploadFinished())
        return;

    // 如果提交失败，则不切换
    if (!jobstate->isSubmitSuccess())
        return;

    // 如果等到的结果是enPageDisplayNone，则直接返回
    if (jobstate->pageDisplayByState() == enPageDisplayNone) {
        return;
    }

    if (pageId == RENDER_PAGE_AT) {
        enTaskStateToPageNum displayPage = (enTaskStateToPageNum)jobstate->getATJobDisplayState();
        switch (displayPage) {
        case enPageATHome: {
            /*空三未提交*/
            ui->stackedWidget_AT->setCurrentIndex(AT_PAGE_RENDER);
        }
            break;
        case enPageATProgress: {
            /*空三中*/
            ui->stackedWidget_AT->setCurrentIndex(AT_PAGE_RENDER);
            m_atRenderTimer->start(1000 * 5);
            if (jobstate->getATJob() != nullptr) {
                ui->progressBarAT->setValue(jobstate->getATJob()->getProgressPercent());
            }
        }
            break;
        case enPageATStop: {
            /*空三渲染停止*/
            ui->stackedWidget_AT->setCurrentIndex(AT_PAGE_HOME);
            m_atRenderTimer->stop();
        }
            break;
        case enPageATFailed: {
            /*空三渲染失败*/
            ui->btnATFailed->setToolTip(jobstate->getStatusText());
            ui->stackedWidget_AT->setCurrentIndex(AT_PAGE_FAILED);
            m_atRenderTimer->stop();
        }
            break;
        case enPageATFinished: {
            /*空三渲染成功*/
            ui->progressBarAT->setValue(100);
            ui->stackedWidget_AT->setCurrentIndex(AT_PAGE_SUCCESS);
            m_atRenderTimer->stop();
        }
            break;
        }
    
    } else if (pageId == RENDER_PAGE_RECONSTRUCTION) {
        enTaskStateToPageNum displayPage = (enTaskStateToPageNum)jobstate->getRebuildJobDisplayState();
        // 重建完成，则使用生产任务的状态
        if (displayPage == enPageRebuildFinished) {
            displayPage = (enTaskStateToPageNum)jobstate->getRenderJobDisplayState();
        }

        switch (displayPage) {
        case enPageRebuildHome: {
            /*等待重建*/
            /*重建未提交*/
            ui->stackedWidget_rebuild->setCurrentIndex(REBUILD_PAGE_HOME);
        }
            break;
        case enPageRebuildWaiting: {
            /*重建等待*/
            ui->stackedWidget_rebuild->setCurrentIndex(REBUILD_PAGE_WAITING);
            m_rebuildRenderTimer->start(1000 * 5);
            if(jobstate->getRebuildJob() != nullptr) {
                ui->progressBarRebuild->setValue(jobstate->getRebuildJob()->getRenderProgress() * 100.0);
            }
            // 重建等待
            ui->label_at_render_5->setText(QObject::tr("等待重建"));
        }
        break;
        case enPageRebuildProgress: {
            /*重建中*/
            ui->stackedWidget_rebuild->setCurrentIndex(REBUILD_PAGE_RENDER);
            m_rebuildRenderTimer->start(1000 * 5);
            if (jobstate->getRebuildJob() != nullptr) {
                ui->progressBarRebuild->setValue(jobstate->getRebuildJob()->getRenderProgress()* 100.0);
            }
            // 重建中
            ui->label_at_render_2->setText(QObject::tr("重建中"));
        }
            break;
        case enPageRebuildStop: {
            /*重建渲染停止*/
            ui->stackedWidget_rebuild->setCurrentIndex(REBUILD_PAGE_HOME);
            m_rebuildRenderTimer->stop();
        }
            break;
        case enPageRebuildFailed: {
            /*重建渲染失败*/
            ui->btnRebuildFailed->setToolTip(jobstate->getStatusText());
            ui->stackedWidget_rebuild->setCurrentIndex(REBUILD_PAGE_FAILED);
            m_rebuildRenderTimer->stop();
            // 重建失败
            ui->label_at_render_4->setText(QObject::tr("重建失败"));
        }
            break;
        case enPageRebuildFinished: {
            /*重建渲染成功*/
            ui->progressBarRebuild->setValue(100);
            ui->stackedWidget_rebuild->setCurrentIndex(REBUILD_PAGE_SUCCESS);
            m_rebuildRenderTimer->stop();
            // 重建完成
            ui->label_at_render_3->setText(QObject::tr("重建完成"));
        }
            break;
        case enPageRenderHome: {
            /*等待重建*/
            /*重建未提交*/
            ui->stackedWidget_rebuild->setCurrentIndex(REBUILD_PAGE_HOME);
        }
            break;
        case enPageRenderWaiting: {
            /*重建等待*/
            ui->stackedWidget_rebuild->setCurrentIndex(REBUILD_PAGE_WAITING);
            m_rebuildRenderTimer->start(1000 * 5);
            if (jobstate->getRenderJob() != nullptr) {
                ui->progressBarRebuild->setValue(jobstate->getRenderJob()->getRenderProgress() * 100.0);
            }
            // 生产等待
            ui->label_at_render_5->setText(QObject::tr("等待生产"));
        }
            break;
        case enPageRenderProgress: {
            /*重建中*/
            ui->stackedWidget_rebuild->setCurrentIndex(REBUILD_PAGE_RENDER);
            m_rebuildRenderTimer->start(1000 * 5);
            if (jobstate->getRenderJob() != nullptr) {
                ui->progressBarRebuild->setValue(jobstate->getRenderJob()->getRenderProgress()* 100.0);
            }
            // 生产中
            ui->label_at_render_2->setText(QObject::tr("生产中"));
        }
            break;
        case enPageRenderStop: {
            /*生产停止 用失败的状态 */
            ui->btnRebuildFailed->setToolTip(jobstate->getStatusText());
            ui->stackedWidget_rebuild->setCurrentIndex(REBUILD_PAGE_FAILED);
            m_rebuildRenderTimer->stop();
            // 生产失败
            ui->label_at_render_4->setText(QObject::tr("生产失败"));
        }
            break;
        case enPageRenderFailed: {
            /*重建渲染失败*/
            ui->btnRebuildFailed->setToolTip(jobstate->getStatusText());
            ui->stackedWidget_rebuild->setCurrentIndex(REBUILD_PAGE_FAILED);
            m_rebuildRenderTimer->stop();
            // 生产失败
            ui->label_at_render_4->setText(QObject::tr("生产失败"));
        }
            break;
        case enPageRenderFinished: {
            /*重建渲染成功*/
            ui->progressBarRebuild->setValue(100);
            ui->stackedWidget_rebuild->setCurrentIndex(REBUILD_PAGE_SUCCESS);
            m_rebuildRenderTimer->stop();
            // 生产完成
            ui->label_at_render_3->setText(QObject::tr("生产完成"));
        }
            break;
        }

    } else {
        enTaskStateToPageNum displayPage = (enTaskStateToPageNum)jobstate->pageDisplayByState();

        if (displayPage > enPageDisplayNone && displayPage < enPageRebuildHome) {
            if (switchPage) updateStackRenderPage(RENDER_PAGE_AT, false);
        } else if (displayPage >= enPageRebuildHome) {
            if (switchPage) updateStackRenderPage(RENDER_PAGE_RECONSTRUCTION, false);
        }

        switch (displayPage) {
        case enPageATHome: {
            /*空三未提交*/
            ui->stackedWidget_AT->setCurrentIndex(AT_PAGE_RENDER);
        }
            break;
        case enPageATProgress: {
            /*空三中*/
            ui->stackedWidget_AT->setCurrentIndex(AT_PAGE_RENDER);
            m_atRenderTimer->start(1000 * 5);
            if (jobstate->getATJob() != nullptr) {
                ui->progressBarAT->setValue(jobstate->getATJob()->getProgressPercent());
            }
        }
            break;
        case enPageATStop: {
            /*空三渲染停止*/
            ui->stackedWidget_AT->setCurrentIndex(AT_PAGE_HOME);
            m_atRenderTimer->stop();
        }
            break;
        case enPageATFailed: {
            /*空三渲染失败*/
            ui->btnATFailed->setToolTip(jobstate->getStatusText());
            ui->stackedWidget_AT->setCurrentIndex(AT_PAGE_FAILED);
            m_atRenderTimer->stop();
        }
            break;
        case enPageATFinished: {
            /*空三渲染成功*/
            ui->progressBarAT->setValue(100);
            ui->stackedWidget_AT->setCurrentIndex(AT_PAGE_SUCCESS);
            m_atRenderTimer->stop();
        }
            break;
        case enPageRebuildHome: {
            /*等待重建*/
            /*重建未提交*/
            ui->stackedWidget_rebuild->setCurrentIndex(REBUILD_PAGE_HOME);
            m_rebuildRenderTimer->stop();
        }
            break;
        case enPageRebuildWaiting: {
            /*重建等待*/
            ui->stackedWidget_rebuild->setCurrentIndex(REBUILD_PAGE_WAITING);
            m_rebuildRenderTimer->start(1000 * 5);
            if(jobstate->getRebuildJob() != nullptr) {
                ui->progressBarRebuild->setValue(jobstate->getRebuildJob()->getRenderProgress() * 100.0);
            }
            // 重建等待
            ui->label_at_render_5->setText(QObject::tr("等待重建"));
        }
            break;
        case enPageRebuildProgress: {
            /*重建中*/
            ui->stackedWidget_rebuild->setCurrentIndex(REBUILD_PAGE_RENDER);
            m_rebuildRenderTimer->start(1000 * 5);
            if (jobstate->getRebuildJob() != nullptr) {
                ui->progressBarRebuild->setValue(jobstate->getRebuildJob()->getRenderProgress() * 100.0);
            }
            // 重建中
            ui->label_at_render_2->setText(QObject::tr("重建中"));
        }
            break;
        case enPageRebuildStop: {
            /*重建渲染停止*/
            ui->stackedWidget_rebuild->setCurrentIndex(REBUILD_PAGE_HOME);
            m_rebuildRenderTimer->stop();
        }
        break;
        case enPageRebuildFailed: {
            /*重建渲染失败*/
            ui->btnRebuildFailed->setToolTip(jobstate->getStatusText());
            ui->stackedWidget_rebuild->setCurrentIndex(REBUILD_PAGE_FAILED);
            m_rebuildRenderTimer->stop();
            // 重建失败
            ui->label_at_render_4->setText(QObject::tr("重建失败"));
        }
            break;
        case enPageRebuildFinished: {
            /*重建渲染成功*/
            ui->progressBarRebuild->setValue(100);
            ui->stackedWidget_rebuild->setCurrentIndex(REBUILD_PAGE_SUCCESS);
            updateStackRenderPage(RENDER_PAGE_RECONSTRUCTION, false);
            m_rebuildRenderTimer->stop();
            // 重建完成
            ui->label_at_render_3->setText(QObject::tr("重建完成"));
        }
            break;
        case enPageRenderHome: {
            /*等待重建*/
            /*重建未提交*/
            ui->stackedWidget_rebuild->setCurrentIndex(REBUILD_PAGE_HOME);
            m_rebuildRenderTimer->stop();
        }
            break;
        case enPageRenderWaiting: {
            ui->stackedWidget_rebuild->setCurrentIndex(REBUILD_PAGE_WAITING);
            m_rebuildRenderTimer->start(1000 * 5);
            if (jobstate->getRenderJob() != nullptr) {
                ui->progressBarRebuild->setValue(jobstate->getRenderJob()->getRenderProgress() * 100.0);
            }
            // 生产等待
            ui->label_at_render_5->setText(QObject::tr("等待生产"));
        }
            break;
        case enPageRenderProgress: {
            /*生产中*/
            ui->stackedWidget_rebuild->setCurrentIndex(REBUILD_PAGE_RENDER);
            m_rebuildRenderTimer->start(1000 * 5);
            if (jobstate->getRenderJob() != nullptr) {
                ui->progressBarRebuild->setValue(jobstate->getRenderJob()->getRenderProgress() * 100.0);
            }
            // 生产中
            ui->label_at_render_2->setText(QObject::tr("生产中"));
        }
            break;
        case enPageRenderStop: {
            /*生产停止， 使用失败状态*/
            ui->btnRebuildFailed->setToolTip(jobstate->getStatusText());
            ui->stackedWidget_rebuild->setCurrentIndex(REBUILD_PAGE_FAILED);
            m_rebuildRenderTimer->stop();
            // 生产失败
            ui->label_at_render_4->setText(QObject::tr("生产失败"));
        }
            break;
        case enPageRenderFailed: {
            /*生产失败*/
            ui->btnRebuildFailed->setToolTip(jobstate->getStatusText());
            ui->stackedWidget_rebuild->setCurrentIndex(REBUILD_PAGE_FAILED);
            m_rebuildRenderTimer->stop();
            // 生产失败
            ui->label_at_render_4->setText(QObject::tr("生产失败"));
        }
            break;
        case enPageRenderFinished: {
            /*生产成功*/
            ui->progressBarRebuild->setValue(100);
            ui->stackedWidget_rebuild->setCurrentIndex(REBUILD_PAGE_SUCCESS);
			updateStackRenderPage(RENDER_PAGE_RECONSTRUCTION, false);
            m_rebuildRenderTimer->stop();
            // 生产完成
            ui->label_at_render_3->setText(QObject::tr("生产完成"));
        }
            break;
		case enPagePublish: {
			/*发布界面*/
		//	ui->progressBarRebuild->setValue(100);
			//ui->stackedWidget_rebuild->setCurrentIndex(REBUILD_PAGE_SUCCESS);
			ui->stackedWidget_render->setCurrentIndex(RENDER_PAGE_PUBLISH);
			ui->stackedWidget->setCurrentIndex(PUBLISH_PAGE_SUM);

			//updateStackRenderPage(RENDER_PAGE_PUBLISH, true);
			updateStackRenderPage(RENDER_PAGE_PUBLISH, true);
			m_rebuildRenderTimer->stop();
			// 生产完成
			//ui->label_at_render_3->setText(QObject::tr("生产完成"));
		}
		   break;
        }  
    }   
}

void CMainWindow::onLoadProjectDataFinished(int code)
{
    this->showLoading(false);

    if (code != CODE_SUCCESS) {
        showErrorMsg(code);
        return;
    }

    LoadProjectDataThread* loadJob = qobject_cast<LoadProjectDataThread*>(sender());
    if (loadJob == nullptr)
        return;
    disconnect(loadJob, &LoadProjectDataThread::loadProjectDataFinished, this, &CMainWindow::onLoadProjectDataFinished);
    if (loadJob->getTaskId() != m_currTaskId) {
        loadJob->deleteLater();
        return;
    }

    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if (jobstate == nullptr) return;

    BaseTaskInfo* baseInfo = loadJob->getBaseTaskInfo();
    jobstate->updataBaseTaskInfo(baseInfo);
    jobstate->setProjectType(jobstate->getBaseTaskInfo()->jobType);
  
    initProjectPage();
    renderPageSwitch();
    if (jobstate->IsUploading()) {
        updateUploadState();
    }
    
    // 加载区块文件内容
    if (jobstate->getProjectType() == enBlockProject) {      
        if (!baseInfo->blocks.isEmpty()) {
            if (baseInfo->posInfo.itemDatas.isEmpty() && baseInfo->ctrlPointInfo.itemDatas.isEmpty()) {
                LoadBlockFileThread* loadBlock = new LoadBlockFileThread(baseInfo->blocks[0].filePath, jobstate->getTaskId());
                connect(loadBlock, &LoadBlockFileThread::loadBlockFileFinished, this, &CMainWindow::onLoadBlockFileFinished);
                loadBlock->start();
                this->showLoading(true);
                TipsToastWidget* toast = new TipsToastWidget(ui->widget_page, QObject::tr("区块数据加载中，请稍后..."), 16, QColor("#1F1F27"));
                connect(loadBlock, &LoadBlockFileThread::loadBlockFileFinished, toast, &TipsToastWidget::hideToast);
            } else {
                resetCtrlPointViewSet();
                resetPositionViewSet();
            }
        }
    }
}

void CMainWindow::fillCtrlPointView(bool fill)
{
    if (fill) {
        JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
        if (jobstate == nullptr) return;

        BaseTaskInfo* baseInfo = jobstate->getBaseTaskInfo();
        ui->widget_coord_edit_2->setCoordSystemValue(baseInfo->ctrlPointInfo.coordSys);
        ui->comboBox_field_order_ctrl->setCurrentIndex(RBHelper::getFieldOrderIndex(baseInfo->ctrlPointInfo.fieldOrder));
        ui->lineEdit_path_ctrl->setText(baseInfo->ctrlPointInfo.filePath);
        ui->comboBox_ignore_lines_ctrl->setCurrentText(baseInfo->ctrlPointInfo.ignoreLines);
        ui->comboBox_splite_char_ctrl->setCurrentIndex(RBHelper::getSplitCharIndex(baseInfo->ctrlPointInfo.spliteChar));

    } else {

        ui->widget_coord_edit_2->setCoordSystemValue("");
        ui->comboBox_field_order_ctrl->setCurrentIndex(0);
        ui->lineEdit_path_ctrl->setText("");
        ui->comboBox_ignore_lines_ctrl->setCurrentText("0");
        ui->comboBox_splite_char_ctrl->setCurrentIndex(0);
    }
}

void CMainWindow::fillPositionInfoView(bool fill)
{
    if (fill) {
        JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
        if (jobstate == nullptr) return;

        BaseTaskInfo* baseInfo = jobstate->getBaseTaskInfo();
        ui->widget_coord_edit_3->setCoordSystemValue(baseInfo->posInfo.coordSys);
        ui->comboBox_field_order_pos->setCurrentIndex(RBHelper::getFieldOrderIndex(baseInfo->posInfo.fieldOrder));
        ui->lineEdit_path_pos->setText(baseInfo->posInfo.filePath);
        ui->comboBox_ignore_lines_pos->setCurrentText(baseInfo->posInfo.ignoreLines);
        ui->comboBox_splite_char_pos->setCurrentIndex(RBHelper::getSplitCharIndex(baseInfo->posInfo.spliteChar));

    } else {

        ui->widget_coord_edit_3->setCoordSystemValue("");
        ui->comboBox_field_order_pos->setCurrentIndex(0);
        ui->lineEdit_path_pos->setText("");
        ui->comboBox_ignore_lines_pos->setCurrentText("0");
        ui->comboBox_splite_char_pos->setCurrentIndex(0);
    }
}

void CMainWindow::resetCtrlPointViewSet()
{
    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if (jobstate == nullptr) return;

    if (jobstate->getProjectType() == enImageProject) {
        ui->btnAddCtrlPoint->setEnabled(true);
        if (jobstate->IsUploadFinished()) {
            if (jobstate->isSubmitCtrlPoint()) {
                ui->btnAddCtrlPoint->setText(QObject::tr("查看控制点数据"));
                ui->btnAddCtrlPoint->setProperty("nextPage", PROJECT_PAGE_CP_SUBMIT);
                fillCtrlPointView(true);

            } else {
                // todo 没有提交控制点数据，显示啥
                ui->btnAddCtrlPoint->setText(QObject::tr("查看控制点数据"));
                ui->btnAddCtrlPoint->setEnabled(false);
            }

        } else {
            ui->btnAddCtrlPoint->setText(QObject::tr("添加控制点数据"));
            ui->btnAddCtrlPoint->setProperty("nextPage", PROJECT_PAGE_CP_ADD);
            if (jobstate->isSubmitCtrlPoint()) {
                fillCtrlPointView(true);
            } else {
                fillCtrlPointView(false);
            }
        }

    } else if (jobstate->getProjectType() == enBlockProject) {  
        BaseTaskInfo* baseInfo = jobstate->getBaseTaskInfo();
        ui->btnAddCtrlPoint->setEnabled(true);
        ui->btnAddCtrlPoint->setText(QObject::tr("查看控制点数据"));
        ui->btnAddCtrlPoint->setProperty("nextPage", PROJECT_PAGE_CP_SUBMIT);
        fillCtrlPointView(false);
        if (baseInfo != nullptr) {
            if (baseInfo->ctrlPointInfo.itemDatas.isEmpty()) {
                ui->btnAddCtrlPoint->setEnabled(false);
            }
        } else {
            ui->btnAddCtrlPoint->setEnabled(false);
        }
    } 
}

void CMainWindow::resetPositionViewSet()
{
    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if (jobstate == nullptr) return;

    if (jobstate->getProjectType() == enImageProject) {
        ui->btnAddPos->setEnabled(true);
        if (jobstate->IsUploadFinished()) {
            if (jobstate->isSubmitPosInfo()) {
                ui->btnAddPos->setText(QObject::tr("查看POS数据"));
                ui->btnAddPos->setProperty("nextPage", PROJECT_PAGE_POS_SUBMIT);
                fillPositionInfoView(true);
            } else {
                // todo 没有提交控制点数据，显示啥
                ui->btnAddPos->setText(QObject::tr("查看POS数据"));
                ui->btnAddPos->setEnabled(false);
            }
        } else {
            ui->btnAddPos->setText(QObject::tr("添加POS数据"));
            ui->btnAddPos->setProperty("nextPage", PROJECT_PAGE_POS_ADD);
            if (jobstate->isSubmitPosInfo()) {
                fillPositionInfoView(true);
            } else {
                fillPositionInfoView(false);
            }
        }

    } else if (jobstate->getProjectType() == enBlockProject) {
        BaseTaskInfo* baseInfo = jobstate->getBaseTaskInfo();
        ui->btnAddPos->setEnabled(true);
        ui->btnAddPos->setText(QObject::tr("查看POS数据"));
        ui->btnAddPos->setProperty("nextPage", PROJECT_PAGE_POS_SUBMIT);
        fillPositionInfoView(false);
        if (baseInfo != nullptr) {
            if (baseInfo->posInfo.itemDatas.isEmpty()) {
                ui->btnAddPos->setEnabled(false);
            }
        } else {
            ui->btnAddPos->setEnabled(false);
        }
    }
}

void CMainWindow::updateStackRenderPage(int index, bool click)
{ 
    ui->stackedWidget_render->setCurrentIndex(index);
    QAbstractButton *btn = m_sideBarGroup->button(index);
    if (btn != nullptr) {
        btn->setChecked(true);
        if (click)
            btn->click();
    }
}
void CMainWindow::updateStackRenderPageNewProcess(int index, bool click)
{
// 	ui->stackedWidget_render->setCurrentIndex(index);
// 	QAbstractButton *btn = m_sideBarGroup->button(RENDER_PAGE_PROJECT);
// 
// 	renderPageSwitch(false, RENDER_PAGE_RECONSTRUCTION);
// 	if (btn != nullptr) {
// 		btn->setChecked(true);
// 		if (click)
// 			btn->click();
// 	}
	ui->stackedWidget_rebuild->setCurrentIndex(REBUILD_PAGE_WAITING);
// 	m_rebuildRenderTimer->start(1000 * 5);
// 	if (jobstate->getRenderJob() != nullptr) {
// 		ui->progressBarRebuild->setValue(jobstate->getRenderJob()->getRenderProgress() * 100.0);
// 	}

}


void CMainWindow::modifySideBarButtonVisible(int projectType, bool showProjBtn)
{
    QList<QAbstractButton *> btns = m_sideBarGroup->buttons();
    foreach(QAbstractButton *btn, btns) {
        if (btn == nullptr) continue;
        btn->show();
        if (projectType == enBlockProject) {
            if(btn == m_sideBarGroup->button(RENDER_PAGE_AT)
                || btn == m_sideBarGroup->button(RENDER_PAGE_AT_RESULT)) {
                btn->hide();
            }
        }

        if (btn == m_sideBarGroup->button(RENDER_PAGE_PROJECT)) {
            if(!showProjBtn) btn->hide();
        }
    }

#ifndef _DEBUG
    // m_sideBarGroup->button(RENDER_PAGE_AT_RESULT)->hide();
#endif
}

void CMainWindow::OnLocateProjectPage(intptr_t wParam, intptr_t lParam)
{
    int page = (int)wParam;
    t_taskid taskId = (t_taskid)lParam;
    m_currTaskId = taskId;

    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(taskId);
    if (jobstate == nullptr) return;
    if (jobstate->IsChildItem()) return;

    // 隐藏传输日志窗口
    if(m_floatLogPanel != nullptr) m_floatLogPanel->hide();
#if 0  //网页端空三预览
    if (page == RENDER_PAGE_AT_RESULT) {
        QString atUrl = getDetailViewUrl(jobstate, "url_atpreview");
        QDesktopServices::openUrl(QUrl(atUrl));
        return;
    }
#endif

    if (jobstate->IsParentTaskSubmitting()
        || jobstate->IsParentTaskStateUpdating()
        || jobstate->IsParentTaskSubmitFailed()) // 提交失败,提交中，状态更新中 的任务不能操作
        return;
   
    // 加载项目数据
    if (RBHelper::checkProjectExist(taskId)) {       
        // 加载本地项目数据
        LoadProjectDataThread* project = new LoadProjectDataThread(taskId, jobstate->getBaseTaskInfo());
        project->setLocatePage(page);
        connect(project, &LoadProjectDataThread::loadProjectDataFinished, this, &CMainWindow::onLoadHistoryProjectFinished);
        project->start();

        this->showLoading(true);
    } else {

        modifySideBarButtonVisible(jobstate->getProjectType(), false);
        jobStateMachine();
        ui->stackedWidget_task->setCurrentIndex(TASK_PAGE_RENDER);
        updateStackRenderPage(page);       
    }
}

void CMainWindow::OnInfolistUi(intptr_t wParam, intptr_t lParam)
{
    t_taskid taskId = (t_taskid)wParam;
    int type = (int)lParam;

    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(taskId);
    if(jobstate == nullptr) return;

    m_detailViewPage->updateFrameDetail(jobstate, type);
}

void CMainWindow::onLoadHistoryProjectFinished(int code)
{
    this->showLoading(false);

    LoadProjectDataThread* loadJob = qobject_cast<LoadProjectDataThread*>(sender());
    if (loadJob == nullptr)
        return;
    disconnect(loadJob, &LoadProjectDataThread::loadProjectDataFinished, this, &CMainWindow::onLoadHistoryProjectFinished);
    if (loadJob->getTaskId() != m_currTaskId) {
        loadJob->deleteLater();
        return;
    }

    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if (jobstate == nullptr) return;

    if (code != CODE_SUCCESS) {
        modifySideBarButtonVisible(jobstate->getProjectType(), false);
        showErrorMsg(code);
        return;
    }

    BaseTaskInfo* baseInfo = loadJob->getBaseTaskInfo();
    jobstate->updataBaseTaskInfo(loadJob->getBaseTaskInfo());
    jobstate->setProjectType(jobstate->getBaseTaskInfo()->jobType);

    initProjectPage();

    jobStateMachine();
    ui->stackedWidget_task->setCurrentIndex(TASK_PAGE_RENDER);
    updateStackRenderPage(loadJob->getLocatePage());    

    // 加载区块文件内容
    if (jobstate->getProjectType() == enBlockProject) {
        if (!baseInfo->blocks.isEmpty()) {
            LoadBlockFileThread* loadBlock = new LoadBlockFileThread(baseInfo->blocks[0].filePath, jobstate->getTaskId());
            connect(loadBlock, &LoadBlockFileThread::loadBlockFileFinished, this, &CMainWindow::onLoadBlockFileFinished);
            loadBlock->start();
            this->showLoading(true);
            TipsToastWidget* toast = new TipsToastWidget(ui->widget_page, QObject::tr("区块数据加载中，请稍后..."), 16, QColor("#1F1F27"));
            connect(loadBlock, &LoadBlockFileThread::loadBlockFileFinished, toast, &TipsToastWidget::hideToast);
        }
    }
}

void CMainWindow::initTurnPageCtrl()
{
    // ui->widget_page_ctrl_view->hide();
    ui->label_ctrl_total->hide();
    ui->widget_page_ctrl_view->setProperty("page_code_ctrl_name", enNameCtrlPointView);
    connect(ui->widget_page_ctrl_view, &PaginationCtrl::pageChanged, this, &CMainWindow::onTablePageChanged);

    ui->widget_page_ctrl_submit->setProperty("page_code_ctrl_name", enNameCtrlPointSubmit);
    connect(ui->widget_page_ctrl_submit, &PaginationCtrl::pageChanged, this, &CMainWindow::onTablePageChanged);

    ui->label_pos_total->hide();
    // ui->widget_page_pos_view->hide();
    ui->widget_page_pos_view->setProperty("page_code_ctrl_name", enNamePositionView);
    connect(ui->widget_page_pos_view, &PaginationCtrl::pageChanged, this, &CMainWindow::onTablePageChanged);

    ui->widget_page_pos_submit->setProperty("page_code_ctrl_name", enNamePositionSubmit);
    connect(ui->widget_page_pos_submit, &PaginationCtrl::pageChanged, this, &CMainWindow::onTablePageChanged);
}

void CMainWindow::onTablePageChanged(int page, int rowCount)
{
    PaginationCtrl* pageCtrl = qobject_cast<PaginationCtrl*>(sender());
    if (pageCtrl == nullptr) return;

    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if (jobstate == nullptr) return;

    BaseTaskInfo* baseInfo = jobstate->getBaseTaskInfo();

    int pageNum = page - 1;
    pageNum = pageNum < 0 ? 0 : pageNum;

    int ctrl_name = pageCtrl->property("page_code_ctrl_name").toInt();
    switch (ctrl_name)
    {
    case enNameCtrlPointView: {
        QList<ConfigDataItem*> curItems;
        curItems = baseInfo->ctrlPointInfo.itemDatas.mid(pageNum * rowCount, rowCount);
        loadConfigDataToTable(ui->tableWidgetCoord, curItems);      
    }
        break;
    case enNameCtrlPointSubmit: {
        QList<ConfigDataItem*> curItems;
        curItems = baseInfo->ctrlPointInfo.itemDatas.mid(pageNum * rowCount, rowCount);
        loadConfigDataToTable(ui->tableWidgetCoord_3, curItems);
    }
        break;
    case enNamePositionView: {
        QList<ConfigDataItem*> curItems;
        curItems = baseInfo->posInfo.itemDatas.mid(pageNum * rowCount, rowCount);
        loadConfigDataToTable(ui->tableWidgetCoord_pos_view, curItems);
    }
        break;
    case enNamePositionSubmit: {
        QList<ConfigDataItem*> curItems;
        curItems = baseInfo->posInfo.itemDatas.mid(pageNum * rowCount, rowCount);
        loadConfigDataToTable(ui->tableWidgetCoord_4, curItems);
    }
        break;
    default:
        break;
    }
}

void CMainWindow::initCtrlPointPageCtrl(PaginationCtrl* ctrl)
{
    if (ctrl == nullptr)
        return;

    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if (jobstate == nullptr) return;

    BaseTaskInfo* baseInfo = jobstate->getBaseTaskInfo();
    int total = baseInfo->ctrlPointInfo.itemDatas.size();
    int rowCount = 30;

    ctrl->setPageInfo(total, rowCount);

    int pageNum = ctrl->getCurrentPage() - 1;
    pageNum = pageNum < 0 ? 0 : pageNum;
    QList<ConfigDataItem*> curItems = baseInfo->ctrlPointInfo.itemDatas.mid(pageNum * rowCount, rowCount);
    loadConfigDataToTable(ui->tableWidgetCoord, curItems);
    loadConfigDataToTable(ui->tableWidgetCoord_3, curItems);
}

void CMainWindow::initPositionPageCtrl(PaginationCtrl* ctrl)
{
    if (ctrl == nullptr)
        return;

    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if (jobstate == nullptr) return;

    BaseTaskInfo* baseInfo = jobstate->getBaseTaskInfo();
    int total = baseInfo->posInfo.itemDatas.size();
    int rowCount = 30;

    ctrl->setPageInfo(total, rowCount);

    int pageNum = ctrl->getCurrentPage() - 1;
    pageNum = pageNum < 0 ? 0 : pageNum;
    QList<ConfigDataItem*> curItems;
    curItems = baseInfo->posInfo.itemDatas.mid(pageNum * rowCount, rowCount);
    loadConfigDataToTable(ui->tableWidgetCoord_pos_view, curItems);
    loadConfigDataToTable(ui->tableWidgetCoord_4, curItems);
}

void CMainWindow::jobStateMachine()
{
    // 1. 上传任务，只有项目数据按钮可用
    // 2. 上传完成并提交，空三处理按钮可用
    // 3. 空三完成， 空三成功按钮可用，重建按钮可用
    // 4. 重建完成，项目成果按钮可用
	ui->checkBox_blockAT->hide();//隐藏空三分块复选框

    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if (jobstate == nullptr) return;

    // 空三结果
    m_buttonATResult->hide();
    if(jobstate->getProjectType() == enImageProject) {
        m_buttonATResult->show();
		ui->checkBox_blockAT->show(); //如果是影像任务，显示空三分块复选框
    }
    // 重建成果,如果输出为空，则不可以点击
    m_buttonResult->setEnabled(true);
    if (jobstate->getRenderJob() != nullptr && jobstate->getRenderJob()->getProjectOutputType().isEmpty()) {
        m_buttonResult->setEnabled(false);
    }

    QList<int> enableBtnIds;
    enableBtnIds.append(RENDER_PAGE_HOME);
    enableBtnIds.append(RENDER_PAGE_PROJECT);

    if (jobstate->getJobType() != UPLOAD_JOB) {
        if (jobstate->isTaskFinished()) {
            enableBtnIds.append(RENDER_PAGE_AT);
            enableBtnIds.append(RENDER_PAGE_AT_RESULT);
            enableBtnIds.append(RENDER_PAGE_RECONSTRUCTION);
            enableBtnIds.append(RENDER_PAGE_PRODUCTION);
        } else if (jobstate->isATFinished()) {
            enableBtnIds.append(RENDER_PAGE_AT);
            enableBtnIds.append(RENDER_PAGE_AT_RESULT);
            enableBtnIds.append(RENDER_PAGE_RECONSTRUCTION);
        } else {
            enableBtnIds.append(RENDER_PAGE_AT);
        }
    }

    // 更新按钮状态
    QList<QAbstractButton *> btns = m_sideBarGroup->buttons();
    foreach(QAbstractButton *btn, btns) {
        if (btn == nullptr) continue;
        btn->setEnabled(false);
        foreach (int btnId , enableBtnIds){
            if (btn == m_sideBarGroup->button(btnId)) {
                btn->setEnabled(true);
            }
        }
    }
}

void CMainWindow::applyPosInfoForPicture(JobState* jobstate)
{
    if (jobstate == nullptr)
        return;

    BaseTaskInfo* info = jobstate->getBaseTaskInfo();
    foreach(QString groupId, info->posInfo.applyGroups) {
        for (int i = 0; i < info->groups.size(); i++) {
            if (groupId != info->groups[i]->groupId) {
                continue;
            }

            auto pics = info->groups[i]->pictures;
            auto posInfo = info->posInfo.itemDatas;

            for (int j = 0; j < pics.size(); j++) {
                foreach(ConfigDataItem* item, posInfo) {
                    if (item->imageName == pics.at(j).picName) {
                        pics[j].picLatitude = item->picLatitude.toDouble();
                        pics[j].picAltitude = item->picAltitude.toDouble();
                        pics[j].picLongitude = item->picLongitude.toDouble();
                        break;
                    }
                }
                info->groups[i]->pictures = pics;
            }
        }
    }
    jobstate->updataBaseTaskInfo(info);

    // 选中第一个
    ui->listWidget_pg->setCurrentRow(0);
}

bool CMainWindow::isBlockImageDataMatch(JobState* jobstate, QMap<QString, QStringList>& notMatchs)
{
    if (jobstate == nullptr)
        return false;

    BLOCK_TASK::BlockExchange *blockInfo = jobstate->getBlockExchangeInfo();
    BaseTaskInfo* baseInfo = jobstate->getBaseTaskInfo();

    // 不存在区块文件，直接返回真，表示不是区块任务
    if (blockInfo == nullptr)
        return true;

    // 如果基础数据为空，则返回false
    if (baseInfo == nullptr)
        return false;

    // 区块中所有文件数据
    QStringList blockImageFiles;
    foreach(BLOCK_TASK::Photogroup photegroup, blockInfo->block.photogroups) {
        foreach(BLOCK_TASK::Photo* photo, photegroup.photos) {
            // blockImageFiles.append(QDir::toNativeSeparators(photo->imagePath));
            blockImageFiles.append(QFileInfo(photo->imagePath).fileName());
        }
    }

    // 比对数据
    foreach(PictureGroup* group, baseInfo->groups) {
        foreach(PictureInfo picInfo, group->pictures) {
            QString path = QDir::toNativeSeparators(picInfo.picPath);
            QString name = QFileInfo(path).fileName();
            if (!blockImageFiles.contains(name)) { // 如果不存在，则记录
                notMatchs[group->groupName].append(path);
            }
        }    
    }

    return notMatchs.isEmpty();
}

void CMainWindow::onHideDownloadView()
{
    if (m_downloadViewPage != nullptr) {
        m_downloadViewPage->hide();
    }
}


bool CMainWindow::getRebuildParams(RebuildOptionParams &param)
{
	QStringList outputTypes = ui->comboOutputType_3->selectedItemDatas();
	QString coordSys = ui->widget_coord_edit_5->getCoordSystemValue();
	QString coordSysFull = ui->widget_coord_edit_5->getCoordSystem();
	bool lockRange = ui->checkBox_lockRange_3->isChecked();
	QString rangeFile = ui->lineEdit_range_file_3->text();

	CoordValue customOriginCoord; // 自定义原点

	CoordValue CoordOffset;  //成果数据偏移
	if (ui->checkBox_enable_custom_3->isChecked()) {
		if (ui->lineEdit_x_coord_3->text().isEmpty()) {
			SetErrorMessage(QObject::tr("请输入自定义原点X坐标"));
			return false;
		}
		if (ui->lineEdit_y_coord_3->text().isEmpty()) {
			SetErrorMessage(QObject::tr("请输入自定义原点Y坐标"));
			return false;
		}
		if (ui->lineEdit_z_coord_3->text().isEmpty()) {
			SetErrorMessage(QObject::tr("请输入自定义原点Z坐标"));
			return false;
		}
		customOriginCoord.coord_x = ui->lineEdit_x_coord_3->text().toDouble();
		customOriginCoord.coord_y = ui->lineEdit_y_coord_3->text().toDouble();
		customOriginCoord.coord_z = ui->lineEdit_z_coord_3->text().toDouble();
		customOriginCoord.enable = true;
	}

	if (ui->checkBox_xyz_offset->isChecked()) {
		if (ui->lineEdit_x_offset->text().isEmpty()) {
			SetErrorMessage(QObject::tr("请输入X坐标偏移值"));
			return false;
		}
		if (ui->lineEdit_y_offset->text().isEmpty()) {
			SetErrorMessage(QObject::tr("请输入Y坐标偏移值"));
			return false;
		}
		if (ui->lineEdit_z_offset->text().isEmpty()) {
			SetErrorMessage(QObject::tr("请输入Z坐标偏移值"));
			return false;
		}
		CoordOffset.coord_x = ui->lineEdit_x_offset->text().toDouble();
		CoordOffset.coord_y = ui->lineEdit_y_offset->text().toDouble();
		CoordOffset.coord_z = ui->lineEdit_z_offset->text().toDouble();
		CoordOffset.enable = true;
	}

	if (outputTypes.isEmpty()) {
		SetErrorMessage(QObject::tr("请选择输出格式"));
		return false;
	}

	if (coordSys.isEmpty()) {
		SetErrorMessage(QObject::tr("请选择坐标系"));
		return false;
	}

	if (!lockRange && rangeFile.isEmpty()) {
		SetErrorMessage(QObject::tr("请选择范围文件"));
		return false;
	}

	// 用户自定义坐标内容
	QString userCoordContent = CProfile::Inst()->getUserCoordSystemContent(coordSys);

	// 瓦片
	int tileMode = ui->comboBox_tile_mode_3->currentIndex();
	QString tileSize = ui->lineEdit_tile_size_3->text();
	if (tileMode == 0) {
		// if (tileSize.isEmpty()) {
		//     SetErrorMessage(QObject::tr("请输入瓦片大小"));
		//     return false;
		// }
		// 
		// if (tileSize.toInt() < 50 || tileSize.toInt() > 150) {
		//     SetErrorMessage(QObject::tr("瓦片大小不合法，请输入50-150范围内的值"));
		//     return false;
		// }
	}

	param.coordSys = coordSys;
	param.coordSysFull = coordSysFull;
	param.lockRange = lockRange;
	param.originCoord = customOriginCoord;
	param.offsetCoord = CoordOffset;
	param.outputTypes = outputTypes;
	param.rangeFile = rangeFile;
	param.usrCoordContent = userCoordContent;
	param.tileMode = tileMode;
	param.tileSize = tileSize;

	return true;
}



bool CMainWindow::isPosInfoMatchedImageData(JobState* jobstate, QMap<QString, QStringList>& notMatchs)
{
    if (jobstate == nullptr)
        return false;

    BaseTaskInfo* baseInfo = jobstate->getBaseTaskInfo();

    // 区块中所有文件数据
    QStringList posInfoNames;
    foreach(ConfigDataItem* item, baseInfo->posInfo.itemDatas) {
        posInfoNames.append(item->imageName);
    }

    foreach(PictureGroup* group, baseInfo->groups) {
        foreach(PictureInfo picInfo, group->pictures) {
            QString path = QDir::toNativeSeparators(picInfo.picPath);
            QString name = QFileInfo(path).fileName();
            if (!posInfoNames.contains(name)) { // 如果不存在，则记录
                notMatchs[group->groupName].append(path);
            }
        }
    }

    return notMatchs.isEmpty();
}

// void CMainWindow::on_btnBackAssets_clicked()
// {
//     ui->stackedWidget_task->setCurrentIndex(TASK_PAGE_IMAGE);
// 
//     // 填充一些数据
//     JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
//     if (jobstate == nullptr) return;
//     BaseTaskInfo* baseInfo = jobstate->getBaseTaskInfo();
// 
//     if (!baseInfo->blocks.isEmpty()) {
//         ui->label_block_path->setText(baseInfo->blocks.at(0).filePath);
//     }
// }

void CMainWindow::on_btnBackAssets_clicked()
{
    if (m_projectType == enBlockProject) {
        ui->stackedWidget_task->setCurrentIndex(TASK_PAGE_PARA);
    } else if (m_projectType == enImageProject) {
        ui->stackedWidget_task->setCurrentIndex(TASK_PAGE_IMAGE);
    } else {
        ui->stackedWidget_task->setCurrentIndex(TASK_PAGE_IMAGE);
    }	
}


void CMainWindow::applyBlockPosForPicture(JobState* jobstate)
{
    if (jobstate == nullptr)
        return;

    BaseTaskInfo* info = jobstate->getBaseTaskInfo();
    if (info == nullptr)
        return;

    BLOCK_TASK::BlockExchange* blockInfo = jobstate->getBlockExchangeInfo();
    if (blockInfo == nullptr)
        return;

    foreach(BLOCK_TASK::Photogroup group, blockInfo->block.photogroups) {
        for (int i = 0; i < info->groups.size(); i++) {

            auto pics = info->groups[i]->pictures;
            auto photos = group.photos;

            for (int j = 0; j < pics.size(); j++) {
                foreach(BLOCK_TASK::Photo* photo, photos) {
                    if (QFileInfo(photo->imagePath).fileName().toLower() == pics.at(j).picName.toLower()) {
                        pics[j].picLatitude  = photo->exifData.gps.Latitude.toDouble();
                        pics[j].picAltitude  = photo->exifData.gps.Altitude.toDouble();
                        pics[j].picLongitude = photo->exifData.gps.Longitude.toDouble();

						if (photo->exifData.gps.Latitude.isEmpty() && photo->exifData.gps.Longitude.isEmpty())
						{
							pics[j].picLongitude = photo->pose.metaData.center.x.toDouble();  //exifData ->Metadata.center
							pics[j].picLatitude = photo->pose.metaData.center.y.toDouble();
							pics[j].picAltitude = photo->pose.metaData.center.z.toDouble();
						}

                        break;
                    }
                }
                info->groups[i]->pictures = pics;
            }
        } 
    }

    jobstate->updataBaseTaskInfo(info);

    // 选中第一个
    ui->listWidget_pg->setCurrentRow(0);
}

void CMainWindow::on_checkBox_enable_custom_stateChanged(int state)
{
    ui->widget_coord_set->setEnabled(state == Qt::Checked ? true : false);
}

void CMainWindow::on_checkBox_enable_custom_3_stateChanged(int state)
{
	ui->widget_coord_set_3->setEnabled(state == Qt::Checked ? true : false);
}
void CMainWindow::on_checkBox_xyz_offset_stateChanged(int state)
{
	ui->widget_xyz_offset->setEnabled(state == Qt::Checked ? true : false);
}

void CMainWindow::on_btnRangeFile_3_clicked()
{
	QString file = QFileDialog::getOpenFileName(this, tr("选择范围文件"), QString(), "KML (*.kml)");

	if (file.isEmpty()) return;
	if (checkIllegalpath(file)) return;
	ui->lineEdit_range_file_3->setText(file);
}
void CMainWindow::on_buttonShareResult_clicked()
{
	ResultsShareWindow* share = new ResultsShareWindow(m_currTaskId);
	share->exec();
}

void CMainWindow::on_buttonResetView_clicked()
{
    static qint64 timeElapsed;
    if (GetTickCount() - timeElapsed < 5000)
        return;
    timeElapsed = GetTickCount();

    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if (jobstate == nullptr) return;
    ui->widget_3dview->reloadUrl();

    // if (jobstate->isTaskFinished()) {
    //     QString url = getDetailViewUrl(jobstate, "url_3dpreview");
    //     ui->widget_3dview->loadUrl(url);
    // }
}

void CMainWindow::showShareResultButtons()
{
    if (m_brCornerWidget != nullptr) {
        int rx = ui->widget_3dview->pos().x() + ui->widget_3dview->width() - 50 - m_brCornerWidget->width();
        int ry = ui->widget_3dview->pos().y() + ui->widget_3dview->height() - 50 - m_brCornerWidget->height();
        m_brCornerWidget->move(rx, ry);
    }
}

void CMainWindow::checkAtKml(const QString & kml)
{
	int row = 0;
	QString line;
	while (row < (ui->listWidget_blockATKML->count()))
	{
		line = ui->listWidget_blockATKML->item(row)->text();
		if (kml == line)
		{
			int res = Util::ShowMessageBoxQuesion(QObject::tr("KML文件重复，请重新修改！"), QObject::tr("温馨提示"), QObject::tr("取消"), QObject::tr("确定"));
			if (res == QDialog::Rejected|| res == QDialog::Accepted)
				break;
		}
		row++;
	}
	return;
}

void CMainWindow::getAllAtKml(QStringList & listKml)
{
	for (size_t i = 0; i < ui->listWidget_blockATKML->count(); i++)
	{
//		QString line = ui->listWidget_blockATKML->item(i)->data(Qt::EditRole).toString();

		QListWidgetItem *item = ui->listWidget_blockATKML->item(i);
		QWidget* pwig = ui->listWidget_blockATKML->itemWidget(item);

		QLineEdit * lEdit = pwig->findChild<QLineEdit *>("lineEdit_atKml");

		QString line = lEdit->text();
		if (line == "")
			return;
		listKml.push_back(line);
	}
}

void CMainWindow::on_btnGuide_clicked()
{
    BlockGuideWindow guideWindow;
    guideWindow.exec();
}

void CMainWindow::onUserAuthRequested()
{
    // QUrl authUrl = RBHelper::getUserCenterLinkUrl("url_user_auth");
    /*QUrl authUrl = RBHelper::getLinkUrl("path_user_auth");

    QString url = QObject::tr("<a href = '%1'><span style='color:#83bfed;text-decoration:none;'>实名认证</span></a>").arg(authUrl.toString());
    QString msg = QObject::tr("<p>为了不影响您的账户使用和保护您的账户安全，请完成 %1</p>").arg(url);
    if (MyConfig.accountSet.realnameStatus == REALNAME_STATUS_DELAY) {
        msg = QObject::tr("<p>为了不影响您的账户使用和保护您的账户安全，请11月1号前完成 %1</p>").arg(url);
    }

    QPointer<CMessageBox> msgBox(new CMessageBox(this));
    msgBox->setWindowFlags(msgBox->windowFlags() ^ Qt::WindowStaysOnTopHint);
    msgBox->setMessage(msg, CMessageBox::MB_ERROR);
    connect(msgBox.data(), &CMessageBox::linkClicked, this, [this](const QString& v) {
        QDesktopServices::openUrl(QUrl(v));
    });
    connect(msgBox.data(), SIGNAL(linkClicked(const QString&)), msgBox, SLOT(close()));
    msgBox->show();*/

    if (MyConfig.accountSet.realnameStatus == REALNAME_STATUS_DONE)
        return;

    QString msg;
    if (MyConfig.userSet.accountType == MAIN_ACCOUNT) {
        // QUrl authUrl = RBHelper::getUserCenterLinkUrl("url_user_auth");
        QUrl authUrl = RBHelper::getLinkUrl("path_user_auth");

        QString url = QObject::tr("<a href = '%1'><span style='color:#83bfed;text-decoration:none;'>实名认证</span></a>").arg(authUrl.toString());
        msg = QObject::tr("<p>为了保护您的账户安全和功能使用，请提交作业前完成 %1</p>").arg(url);
        if (MyConfig.accountSet.realnameStatus == REALNAME_STATUS_DELAY &&
            MyConfig.accountSet.delayAuthTime != 0) {
            QDateTime delayAuthTime = QDateTime::fromMSecsSinceEpoch(MyConfig.accountSet.delayAuthTime);
            QString datetime = delayAuthTime.toString("yyyy-MM-dd");
            msg = QObject::tr("<p>为了保护您的账户安全和功能使用，请在 %1 号前完成 %2</p>").arg(datetime).arg(url);
        }
    }
    else {
        msg = QObject::tr("<p>主账号未实名，为了不影响作业提交，请通知主账号实名认证</p>");
    }

    QPointer<CMessageBox> msgBox(new CMessageBox(this));
    msgBox->setWindowFlags(msgBox->windowFlags() ^ Qt::WindowStaysOnTopHint);
    msgBox->setMessage(msg, CMessageBox::MB_ERROR);
    connect(msgBox.data(), &CMessageBox::linkClicked, this, [this](const QString& v) {
        QDesktopServices::openUrl(QUrl(v));
    });
    connect(msgBox.data(), SIGNAL(linkClicked(const QString&)), msgBox, SLOT(close()));
    msgBox->show();

    /*ui->btnNoticeClose->hide();
    ui->label_notice->setText(msg);
    ui->widget_notice->show();

    connect(ui->label_notice, &QLabel::linkActivated, this, [this](const QString& v) {
        QDesktopServices::openUrl(QUrl(v));
    });*/
}

void CMainWindow::showATKmlBlock()
{
	QStringList templist;
	getAllAtKml(templist);

	TaskAtKmlBlock *cItemWidget = new TaskAtKmlBlock;
	cItemWidget->setKmlList(templist);

	QListWidgetItem *pItem = new QListWidgetItem;
	pItem->setTextAlignment(Qt::AlignCenter);//设置居中显示
	pItem->setSizeHint(QSize(700, 60));//设一项的大小
	

	ui->listWidget_blockATKML->addItem(pItem);
	ui->listWidget_blockATKML->setItemWidget(pItem, cItemWidget);

	connect(cItemWidget, &TaskAtKmlBlock::atXmlNumAdd, this, &CMainWindow::showATKmlBlock);
	connect(cItemWidget, &TaskAtKmlBlock::atXmlDelete, this, &CMainWindow::setCurrentKmlItem);
	connect(cItemWidget, &TaskAtKmlBlock::atXmlDelete, this, &CMainWindow::deleteATKmlBlock);
	return;
}

void CMainWindow::deleteATKmlBlock()
{
	if (ui->listWidget_blockATKML->count() == 1)
	{
		Util::ShowMessageBoxError(QObject::tr("当前项不能删除！"));
		return;
	}
	int res = Util::ShowMessageBoxQuesion(QObject::tr("确定要删除该范围文件吗？"), QObject::tr("删除"), QObject::tr("确定"), QObject::tr("取消"));
	if (res == QDialog::Rejected)
	{
		return;
	}
	else {
		QListWidgetItem* item = ui->listWidget_blockATKML->currentItem();
	    ui->listWidget_blockATKML->removeItemWidget(item);
		delete item;
		return;
	}

}

void CMainWindow::setCurrentKmlItem(TaskAtKmlBlock * curr_item)
{
	for (size_t i = 0; i < ui->listWidget_blockATKML->count(); i++)
	{
		QListWidgetItem *item = ui->listWidget_blockATKML->item(i);
		QWidget* pwig = ui->listWidget_blockATKML->itemWidget(item);
		QLineEdit * lEdit = pwig->findChild<QLineEdit *>("lineEdit_atKml");
		QString line = lEdit->text();
		if (line == curr_item->getEditTxt())
		{
			ui->listWidget_blockATKML->setCurrentItem(item);
			return;
		}
	}
}

bool CMainWindow::assetsCheckEnable()
{
    QTableWidget* picGroup = ui->tableWidget_pic_group;
    int rowCount = ui->tableWidget_pic_group->rowCount();

    if (rowCount < 1) {
        new ToastWidget(this, QObject::tr("请添加照片组"), QColor("#e25c59"), 2000);
        return false;
    }


    if (m_projectType == enBlockProject && ui->label_block_path->text().isEmpty()) {
        new ToastWidget(this, QObject::tr("请上传区块数据"), QColor("#e25c59"), 2000);
        return false;
    }

    // 数量大于10张
    int totalPicCount = 0;
    for (int i = 0; i < picGroup->rowCount(); i++) {
        PictureGroup group = picGroup->item(i, 0)->data(Qt::UserRole).value<PictureGroup>();
        totalPicCount += group.picsCount;
    }
    if (totalPicCount < 10) {
        new ToastWidget(this, QObject::tr("照片数量不能低于10张"), QColor("#e25c59"), 2000);
        return false;
    }

    // 照片序号需要连续
    for (int i = 0; i < picGroup->rowCount(); i++) {
        PictureGroup group = picGroup->item(i, 0)->data(Qt::UserRole).value<PictureGroup>();

        QList<QString> picNames;
        foreach(auto picture, group.pictures) {
            picNames.append(picture.picName);
        }
        qSort(picNames.begin(), picNames.end(), [](const QString &v1, const QString &v2) -> bool {
            return v1 < v2;
        });

        // 取第一张图片编号
        if (picNames.count() > 1) {
            QString numStart;
            const QString name = QFileInfo(picNames[0]).baseName();
            for (int i = name.size() - 1; i >= 0; i--) {
                if (name[i].isDigit())
                    numStart.prepend(name[i]);
                else
                    break;
            }

            QString picName = picNames[0];
            const QString picPattern = picName.replace(numStart, "*");
            int patternLen = numStart.length();
            if (patternLen == 0) {
                int res = Util::ShowMessageBoxQuesion(QObject::tr("照片序号不连续，请检查。以下文件不存在:<br/>照片组:%1<br/>文件:%2")
                    .arg(group.groupName).arg(picName), QObject::tr("温馨提示"), QObject::tr("继续提交"), QObject::tr("放弃提交"));
                if (res == QDialog::Rejected)
                    return false;
                else
                    break;
            }

            int startValue = numStart.toInt();
            int endValue = startValue + picNames.count();
            for (int i = startValue; i < endValue; i++) {
                QString numberPadding = QStringLiteral("%1").arg(i, patternLen, 10, QLatin1Char('0'));
                QString picName = picPattern;
                picName.replace("*", numberPadding);
                if (!picNames.contains(picName)) {
                    int res = Util::ShowMessageBoxQuesion(QObject::tr("照片序号不连续，请检查。以下文件不存在:<br/>照片组:%1<br/>文件:%2")
                        .arg(group.groupName).arg(picName), QObject::tr("温馨提示"), QObject::tr("继续提交"), QObject::tr("放弃提交"));
                    if (res == QDialog::Rejected)
                        return false;
                    else
                        break;
                }
            }
        }
    }

    // 照片组内相机需要一致
    for (int i = 0; i < picGroup->rowCount(); i++) {
        PictureGroup group = picGroup->item(i, 0)->data(Qt::UserRole).value<PictureGroup>();

        QSet<QString> cameraSet;
        foreach(auto picture, group.pictures) {
            cameraSet.insert(picture.cameraProducter + " " + picture.cameraModel);
        }

        if (cameraSet.count() > 1) {
            QString camera;
            QSetIterator<QString> i(cameraSet);
            while (i.hasNext()) {
                qDebug() << i.next();
            }

            // Util::ShowMessageBoxError(QObject::tr("照片组内相机型号需要保持一致，请检查"), QObject::tr("温馨提示"));
            int res = Util::ShowMessageBoxQuesion(QObject::tr("照片组内相机型号需要保持一致，请检查！"), QObject::tr("温馨提示"), QObject::tr("继续提交"), QObject::tr("放弃提交"));
            if (res == QDialog::Rejected)
                return false;
        }
    }

    // 检查照片组是否重复
    QList<QString> picsUniqueCode;
    QStringList repeatPath;
    for (int i = 0; i < picGroup->rowCount(); i++) {
        PictureGroup group = picGroup->item(i, 0)->data(Qt::UserRole).value<PictureGroup>();
        foreach(auto picture, group.pictures) {
            if (!picture.fileUniqueCode.isEmpty() && picsUniqueCode.contains(picture.fileUniqueCode)) {
                repeatPath.append(picture.picPath);
                continue;
            }
            picsUniqueCode.append(picture.fileUniqueCode);
        }
    }

    if (!repeatPath.isEmpty()) {
        QString repeat;
        for (int i = 0; i < qMin(10, repeatPath.size()); i++) {
            repeat.append(repeatPath.at(i));
            repeat.append("\n");
        }
        QString msg = QObject::tr("照片组存在重复照片，请检查。以下文件重复:\r\n%1").arg(repeat);
        // Util::ShowMessageBoxError(QObject::tr("照片组存在重复照片，请检查。以下文件重复:\r\n%1").arg(repeat), QObject::tr("温馨提示"));
        int res = Util::ShowMessageBoxQuesion(msg, QObject::tr("温馨提示"), QObject::tr("继续提交"), QObject::tr("放弃提交"));
        if (res == QDialog::Rejected)
            return false;
    }

    // 检查所有照片组的相机是否一致
    QStringList cameraMode;
    QStringList cameraMaker;
    for (int i = 0; i < picGroup->rowCount(); i++) {
        PictureGroup group = picGroup->item(i, 0)->data(Qt::UserRole).value<PictureGroup>();
        if (!cameraMode.contains(group.cameraModel)) {
            cameraMode.append(group.cameraModel);
        }
        if (!cameraMaker.contains(group.cameraProducter)) {
            cameraMaker.append(group.cameraProducter);
        }
    }
    if (cameraMode.size() > 1 || cameraMaker.size() > 1) {
        // Util::ShowMessageBoxError(QObject::tr("照片组间相机不一致，请检查。"), QObject::tr("温馨提示"));
        int res = Util::ShowMessageBoxQuesion(QObject::tr("照片组间相机不一致，请检查！"), QObject::tr("温馨提示"), QObject::tr("继续提交"), QObject::tr("放弃提交"));
        if (res == QDialog::Rejected)
            return false;
    }

    // jobstate
    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if (jobstate == nullptr) return false;

    // 检查区块数据与影像数据的匹配性
    QMap<QString, QStringList> notMatchs;
    if (!this->isBlockImageDataMatch(jobstate, notMatchs)) {
        int res = Util::ShowMessageBoxQuesion(QObject::tr("区块文件和影像信息不匹配，请检查！"), QObject::tr("温馨提示"), QObject::tr("继续提交"), QObject::tr("放弃提交"));
        if (res == QDialog::Rejected)
            return false;
    }

    if (m_projectType == -1) {
        Util::ShowMessageBoxError(QObject::tr("项目类型有误，请重新创建任务！"), QObject::tr("温馨提示"));
        return false;
    }

    jobstate->setJobType(UPLOAD_JOB);
    jobstate->setProjectType(m_projectType);

    return true;
}

void CMainWindow::onTabWidgetATChanged(int index)
{
    ui->widget_page->setEnabled(false);

    JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
    if (jobstate == nullptr) return;
    if (!jobstate->isATFinished())
        return;

    if (index == AT_RESULT_VIEW_CP) {
        LoadAtDataInfoThread* threadAtViewInfo = new LoadAtDataInfoThread(jobstate->getATJob()->getTaskId());
        connect(threadAtViewInfo, &LoadAtDataInfoThread::loadAtPointCloudInfoFinished, this, &CMainWindow::onLoadAtPointCloudDataInfoFinish);
        // connect(threadAtViewInfo, &LoadAtDataInfoThread::loadAtPointCloudInfoFinished, threadAtViewInfo, &LoadAtDataInfoThread::deleteLater);
        
        // 显示进度
        TipsToastWidget* toast = new TipsToastWidget(ui->widget_page, QObject::tr("数据加载中，请稍后...(0%)"), 16, QColor("#1F1F27"));
        connect(threadAtViewInfo, &LoadAtDataInfoThread::loadAtPointCloudInfoFinished, toast, &TipsToastWidget::hideToast);
        connect(threadAtViewInfo, &LoadAtDataInfoThread::loadAtPointCloudInfoFinished, toast, &TipsToastWidget::deleteLater);

        // 更新进度
        connect(threadAtViewInfo, static_cast<void(LoadAtDataInfoThread::*)(qint64, qint64)>(&LoadAtDataInfoThread::downloadProgress), this, [this, toast](qint64 bytesReceived, qint64 bytesTotal) {
            if (bytesTotal <= 0)
                return;
            double progress = bytesReceived * 100.0 / bytesTotal;
            QString msg = QObject::tr("数据加载中，请稍后...(%1%)").arg(QString::number(progress, 'f', 2));
            if(toast != nullptr) toast->updateMessage(msg);
        });

        // 加载
        threadAtViewInfo->loadAtViewDataUrl();
    
    } else if (index == AT_RESULT_VIEW_REPORT) {
        LoadAtReportInfoThread *threadATReport = new LoadAtReportInfoThread(jobstate->getATJob()->getTaskId());
        connect(threadATReport, &LoadAtReportInfoThread::loadAtReportInfoFinished, this, &CMainWindow::onLoadAtReportInfoFinished);
        // connect(threadATReport, &LoadAtReportInfoThread::loadAtReportInfoFinished, threadATReport, &LoadAtReportInfoThread::deleteLater);
        
        TipsToastWidget* toast = new TipsToastWidget(ui->widget_page, QObject::tr("数据加载中，请稍后...(0%)"), 16, QColor("#1F1F27"));
        connect(threadATReport, &LoadAtReportInfoThread::loadAtReportInfoFinished, toast, &TipsToastWidget::hideToast);
        connect(threadATReport, &LoadAtReportInfoThread::loadAtReportInfoFinished, toast, &TipsToastWidget::deleteLater);

        // 更新进度
        connect(threadATReport, static_cast<void(LoadAtReportInfoThread::*)(qint64, qint64)>(&LoadAtReportInfoThread::downloadProgress), this, [this, toast](qint64 bytesReceived, qint64 bytesTotal) {
            if (bytesTotal <= 0)
                return;
            double progress = bytesReceived * 100.0 / bytesTotal;
            QString msg = QObject::tr("数据加载中，请稍后...(%1%)").arg(QString::number(progress, 'f', 2));
            if (toast != nullptr) toast->updateMessage(msg);
        });

        threadATReport->loadAtReportDataUrl();

    } else if (AT_RESULT_VIEW_3D) {
    
    }
   
}

void CMainWindow::onCalNum()
{
	QString text = ui->textEdit_des->toPlainText();
	int num = text.size();
	if (num < 15)
	{
		QString tips = tr("至少填写15字");
		ui->label_59->show();
		ui->label_59->setText(tips);
		ui->label_59->setStyleSheet("color:red;");
	}
	else if (num > 500)
	{
        int position = ui->textEdit_des->textCursor().position();
		QTextCursor textCursor = ui->textEdit_des->textCursor();
		text.remove(position - (num - 500), num - 500);
		ui->textEdit_des->setText(text);
		textCursor.setPosition(position - (num - 500));
		ui->textEdit_des->setTextCursor(textCursor);
		QString tips = tr("至多填写500字");
		ui->label_59->show();
		ui->label_59->setText(tips);
		ui->label_59->setStyleSheet("color:red;");
	}
	else
	{
		ui->label_59->hide();
	}
}

void CMainWindow::onTipsNum(int state)
{
	if (state == 2)
	{
		ui->lineEdit_key->clear();
		ui->btn_autobuild->show();
		ui->lineEdit_key->show();
	}
	else
	{
		ui->btn_autobuild->hide();
		ui->lineEdit_key->hide();
	}
}

bool CMainWindow::getAtBlockInfo(blockATSettingInfo & atinfo)
{
	atinfo.toBlock = m_atBlock;
	atinfo.toMerge = m_atBlockMerage;
	getAllAtKml(atinfo.many_at_kml);
	return true;
}void CMainWindow::OnResubmitTaskRet(intptr_t wParam, intptr_t lParam)
{
    // 刷新页面
    JobStateMgr::Inst()->TaskOperatorRet();
    // 禁用页面
    ui->widget_page->setEnabled(false);

    TipsToastWidget* toast = new TipsToastWidget(ui->widget_page, QObject::tr("提交成功，请等待结果返回"), 16, QColor("#1F1F27"));
    QTimer::singleShot(1000 * 10, this, [this, wParam, toast]() { // 10秒后跳转
        // 启用
        ui->widget_page->setEnabled(true);

        // 刷新页面
        JobStateMgr::Inst()->TaskOperatorRet();

        if (toast != nullptr) {
            toast->hideToast();
            toast->deleteLater();
        }

        // 先回主界面，再切换
        if (m_atRenderTimer != nullptr)m_atRenderTimer->stop();
        if (m_rebuildRenderTimer != nullptr)m_rebuildRenderTimer->stop();

        ui->stackedWidget_render->setCurrentIndex(RENDER_PAGE_HOME);
        ui->stackedWidget_task->setCurrentIndex(TASK_PAGE_HOME);
        // ui->stackedWidget_render->setCurrentIndex(RENDER_PAGE_PROJECT);
        ui->radio_gps_pos->setChecked(true);
		if (m_rebuildParamWidget != nullptr)
		{
			JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_currTaskId);
			if (jobstate != nullptr)
			{
				BLOCK_TASK::BlockExchange *blockInfo = jobstate->getBlockExchangeInfo();
				if (blockInfo != nullptr)
				{
					BLOCK_TASK::Photo * currgroup = blockInfo->block.photogroups.begin()->photos.at(0);
					if (currgroup->exifData.gps.Longitude != Null)
					{
						m_rebuildParamWidget->resetView(currgroup->exifData.gps.Longitude.toDouble(), currgroup->exifData.gps.Latitude.toDouble());
					}
					else
					{
						m_rebuildParamWidget->resetView();
					}
				}
				else
				{
					m_rebuildParamWidget->resetView();
				}
			}
			else
			{
				m_rebuildParamWidget->resetView();
			}

		}
        // 刺像控点数据重置
        if (ui->widget_surveysPoints->getKrGcp()) { ui->widget_surveysPoints->deleteKrGcp(); }

        // 切换页面
        t_taskid taskId = (t_taskid)wParam;
        JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(taskId);
        if (jobstate == nullptr) return;
        if (jobstate->IsChildItem()) {
            // 清理空三任务的进度
            jobstate->setProgressPercent(0);
            jobstate = jobstate->getParentItem();
            if (jobstate == nullptr)
                return;
        }

        this->onTaskItemDoubleClicked(jobstate->getTaskId());

    });  
}

const QString CMainWindow::getHash(QString _filePath)
{
	QString sMd5;
	QFile file(_filePath);

	if (file.open(QIODevice::ReadOnly))
	{
		QByteArray bArray = QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5);
		sMd5 = QString(bArray.toHex()).toUpper();
	}

	file.close();
	return sMd5;
}

void CMainWindow::getReportInfo(const QString &_filePath, AT_REPORT_INFO::At_Report &_reportInfo)
{
    //读入所有内容
    QFile file(_filePath);
    file.open(QIODevice::ReadOnly);
    auto data = file.readAll();
    file.close();

    //解析
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError)
    {
        qDebug() << "配置错误，读取image_pos.json文件失败";
        return;
    }

    QJsonObject obj = doc.object();
    auto keys = obj.keys();
    for (auto &k : keys)
    {
        QString object = k;
        if (object == "tie_point")
        {
            QJsonObject info = obj[k].toObject();
            AT_REPORT_INFO::TiePoint tp;
            tp.NumberOfPoints = info["NumberOfPoints"].toString();
            tp.MedianNumberOfPointsPerPhoto = info["MedianNumberOfPointsPerPhoto"].toString();
            tp.MedianNumberOfPhotosPerPoint = info["MedianNumberOfPhotosPerPoint"].toString();
            tp.MedianReprojectionError = info["MedianReprojectionError"].toString();
            tp.RMSOfReprojectionErrors = info["RMSOfReprojectionErrors"].toString();
            tp.RMSOfDistancesToRays = info["RMSOfDistancesToRays"].toString();

            _reportInfo.tie_point = tp;
        }

        if (object == "project")
        {
            QJsonObject info = obj[k].toObject();
            AT_REPORT_INFO::Project pj;
            pj.name = info["name"].toString();
            pj.data_set = info["data_set"].toString();
            pj.photo_group_num = info["photo_group_num"].toInt();
            pj.photo_num = info["photo_num"].toInt();
            pj.calibration_photo_num = info["calibration_photoo_num"].toInt();
            pj.error_photo_num = info["error_photo_num"].toInt();
            pj.downsampling_rate = info["downsampling_rate"].toDouble();
            pj.RMS = info["RMS"].toString();

            _reportInfo.project = pj;
        }

        if (object == "photo")
        {
            QJsonArray info = obj[k].toArray();
            for (auto &arr : info)
            {
                QJsonObject oec = arr.toObject();
                AT_REPORT_INFO::PhotoInfo temp;
                temp.file_name = oec["file_name"].toString();
                temp.NumberOfPoints = oec["NumberOfPoints"].toString();
                temp.RMSOfReprojectionErrors = oec["RMSOfReprojectionErrors"].toString();
                temp.RMSOfDistancesToRays = oec["RMSOfDistancesToRays"].toString();

                _reportInfo.photo.push_back(temp);

            }
        }

        if (object == "photogroups")
        {
            QJsonArray infoArray = obj[k].toArray();
            for (auto &arr : infoArray)
            {
                QJsonObject info = arr.toObject();
                AT_REPORT_INFO::Photogroup temp;
                temp.name = info["name"].toString();
                temp.photos_num = info["photos_num"].toInt();
                temp.width = info["width"].toString();
                temp.height = info["height"].toString();
                temp.sensor_size = info["sensor_size"].toString();
                temp.focal_length = info["focal_length"].toString();
                QJsonObject camera = info["camera"].toObject();
                temp.camera.x = camera["x"].toString();
                temp.camera.y = camera["y"].toString();
                temp.camera.K1 = camera["K1"].toString();
                temp.camera.K2 = camera["K2"].toString();
                temp.camera.K3 = camera["K3"].toString();
                temp.camera.P1 = camera["P1"].toString();
                temp.camera.P2 = camera["P2"].toString();
                temp.camera.focal_length = camera["focal_length"].toString();

                _reportInfo.photogroups.push_back(temp);
            }
        }

        if (object == "control_Point")
        {
			QJsonObject control_Point = obj[k].toObject();
			QJsonArray Points = control_Point["Points"].toArray();

			QJsonObject AfterATCP = control_Point["AfterAT"].toObject();
			for (auto &arr : Points)
			{
				QJsonObject cp = arr.toObject();
				AT_REPORT_INFO::CpAccuracy temp;
				temp.Category = cp["Category"].toString();
				temp.CheckPoint = cp["CheckPoint"].toString();
				temp.VerticalAccuracy = cp["VerticalAccuracy"].toString();
				temp.HorizontalAccuracy = cp["HorizontalAccuracy"].toString();
				temp.Name = cp["Name"].toString();

				QJsonObject AfterAt = cp["AfterAT"].toObject();
				temp.HorizontalXError3D = AfterAt["HorizontalXError3D"].toString();
				temp.VerticalError3D = AfterAt["VerticalError3D"].toString();
				temp.HorizontalYError3D = AfterAt["HorizontalYError3D"].toString();
				temp.HorizontalError3D = AfterAt["HorizontalError3D"].toString();
				temp.NumberOfPhotos = AfterAt["NumberOfPhotos"].toString();
				temp.Error3D = AfterAt["Error3D"].toString();
				temp.RMSOfReprojectionErrors = AfterAt["RMSOfReprojectionErrors"].toString();
				temp.RMSOfDistancesToRays = AfterAt["RMSOfDistancesToRays"].toString();

				if (temp.HorizontalXError3D == nullptr || temp.HorizontalYError3D == nullptr || temp.VerticalError3D == nullptr)
				{
					continue;
				}
				_reportInfo.cp_info.push_back(temp);
			}

			_reportInfo.cps_info.NumberOfPoints = QString::number(_reportInfo.cp_info.size());
			_reportInfo.cps_info.MedianNumberOfPhotosPerPoint = AfterATCP["MedianNumberOfPhotosPerPoint"].toString();
			_reportInfo.cps_info.MedianNumberOfPointsPerPhoto = AfterATCP["MedianNumberOfPointsPerPhoto"].toString();
			_reportInfo.cps_info.RMSOfVerticalErrors = AfterATCP["RMSOfVerticalErrors"].toString();
			_reportInfo.cps_info.RMSOfHorizontalErrors = AfterATCP["RMSOfHorizontalErrors"].toString();
			_reportInfo.cps_info.MedianReprojectionError = AfterATCP["MedianReprojectionError"].toString();
			_reportInfo.cps_info.RMSOf3DErrors = AfterATCP["RMSOf3DErrors"].toString();
			_reportInfo.cps_info.RMSOfReprojectionErrors = AfterATCP["RMSOfReprojectionErrors"].toString();
			_reportInfo.cps_info.RMSOfDistancesToRays = AfterATCP["RMSOfDistancesToRays"].toString();
        }

	}
}


QString CMainWindow::getRemoteATImagePath(const QString &_filePath)
{
	//读入所有内容
	QFile file(_filePath);
	file.open(QIODevice::ReadOnly);
	auto data = file.readAll();
	file.close();
	//解析
	QJsonParseError parseError;
	QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
	if (parseError.error != QJsonParseError::NoError)
	{
		qDebug() << "配置错误，读取image_pos.json文件失败";
		LOGFMTI("CMainWindow::getRemoteATImagePath(),load image_pos.json file image path fail");
		return NULL;
	}
	QJsonObject obj = doc.object();
	auto keys = obj.keys();
	for (auto &k : keys)
	{
		QJsonObject group = obj[k].toObject();
		auto photos = group["photos"].toObject();
		QStringList img_paths = photos.keys();
		for (auto &image : img_paths)
		{
			return image;
			break;
		}
		return NULL;
	}
	return NULL;
}

bool CMainWindow::AtPathJudgment(const QString& folder, const QString &selectFolder)
{
	//获取服务器空三影像存储位置的绝对路径
	QString imagePos_path = RBHelper::getTempPath(folder);
	imagePos_path.append(QDir::separator());
	imagePos_path.append("image_pos.json");
	QString AtImagePath = getRemoteATImagePath(imagePos_path);
	if (NULL == AtImagePath)
	{
		return false;  //检查下image_pos.json是否下载完成
	}
	//判断路径层级信息，第一个根路径可以不同，其他必须相同
	QStringList remortAtImg = AtImagePath.split("/");
	QStringList localSelectImg = selectFolder.split("/");

	if (remortAtImg.size() <= localSelectImg.size())
	{
		return false;
	}

	QString splicePath = localSelectImg[0];
	//根据本地选择路径查找组合影像路径后查找本地影像（规则，除了盘符可以不同，其他路径层级必须完全一致）
	//对比除盘符外的路径层级,
	for (size_t i = 1; i < localSelectImg.size(); i++)
	{
		if (remortAtImg[i]!= localSelectImg[i])
		{
			return false;
		}
	}

	//拼接路径，查找对应影像是否存在
	//拼接本地路径
	for (size_t i = 1;i<localSelectImg.size();i++)
	{
		splicePath = splicePath + "/" + localSelectImg[i];
	}
	//拼接影像剩余绝对路径
	for (size_t i = localSelectImg.size(); i<remortAtImg.size();i++)
	{
		splicePath = splicePath + "/" + remortAtImg[i];
	}
	//查找这个影像，如果能找到则路径OK

	QFileInfo fileInfo(splicePath);
	if (!fileInfo.isFile())
	{
		return  false;
	}
	return true;

}

bool CMainWindow::checkIllegalpath(const QString& path)
{
	//非法路径
	QStringList illPath;
	illPath << "C:/" 
		<< "D:/work" 
		<< "D:/7-Zip"
		<< "D:/enfwork" 
		<< "D:/log" 
		<< "D:/luts" 
		<< "D:/plugins" 
		<< "D:/renderwork" 
		<< "D:/apache-flume-1.8.0" 
		<< "D:/renderman" 
		<< "D:/filebeat"
		; 

	for (size_t i = 0; i < illPath.size(); i++)
	{
		if (path.indexOf(illPath[i]) == 0)
		{
			Util::ShowMessageBoxError(QObject::tr("非法路径，该路径平台脚本不支持，不支持的路径:<br> C:/ <br> D:/work <br>D:/7-Zip<br>D:/enfwork<br>D:/log<br>D:/luts<br>D:/plugins<br>D:/renderwork<br>D:/apache-flume-1.8.0<br>D:/renderman<br>D:/filebeat<br><br> 当前选择路径:%1<br>请更改数据本机存储路径并重新选择").arg(path));
			return true;
		}
	}
	return false;	
}

bool CMainWindow::checkRebulidCoordSys()
{
	QString coordSysBlock = ui->widget_coord_edit_5->getCoordSystemValue(); //区块重建窗口
	
	if (coordSysBlock == "EPSG:4326" || coordSysBlock == "EPSG:4490")
	{
		return false;
	}
	return true;
}

bool CMainWindow::checkTaskCloneCoordSys()
{
	RebuildOptionParams rebuildParam;
	if (!m_rebuildParamWidget->getRebuildParams(rebuildParam)) {
		return false;
	}
	QString coordSys = rebuildParam.coordSys;  //克隆窗口

	if (coordSys == "EPSG:4326" || coordSys == "EPSG:4490")
	{
		return false;
	}
	return true;
}