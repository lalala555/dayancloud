/***********************************************************************
* Module:  MainWindow.h
* Author:  hqr
* Modified: 2016/11/14 15:00:46
* Purpose: Declaration of the class
***********************************************************************/
#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QPointer>
#include <QSystemTrayIcon>
#include <QJsonObject>
#include "Window.h"
#include "GUIUpdater.h"
#include "ui_MainWindow.h"
#include "kernel/request_interface.h"
#include "kernel/render_struct.h"
#include "Update/UpdateManager.h"
#include "HttpCommand/HttpCommon.h"
#include "Kernel/TaskElement.h"
#include "kernel/rendercmd_define.h"
#include "HttpCommand/HttpRestApi.h"

namespace Ui
{
class CMainWindow;
}

class QDragEnterEvent;
class QDropEvent;
class QSettingWindow;
class BalanceWindow;
class SearchBox;
class AboutWindow;
class UpdateForm;
class UpdateNotify;
class WeixinQRCode;
class NoticePublishMgr;
class JobViewPage;
class JobListViewPage;
class DownloadViewPage;
class DetailViewPage;
class FloatLogPanel;
class LoadAtReportInfoThread;
class TaskCloneWindow;
class LoadAtDataInfoThread;
class Widget;   //刺相控点
class TaskAtKmlBlock;

Q_DECLARE_METATYPE(QTableWidgetItem*)

class CMainWindow : public Window, public IResponseSink
{
    Q_OBJECT
public:
    friend class JobViewPage;

public:
    explicit CMainWindow(QWidget *parent = 0);
    ~CMainWindow();

    void showTrayMessage(QString msg);

private:
    virtual bool Response(int type, intptr_t wParam, intptr_t lParam = 0);

protected:
    virtual void closeEvent(QCloseEvent *event);
    virtual void timerEvent(QTimerEvent *event);
    virtual bool eventFilter(QObject *obj, QEvent *event);
    virtual void changeEvent(QEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void resizeEvent(QResizeEvent *);

private slots:
    void onMoreMenuTriggered(QAction *action);
    void onPopupUserMenu();
	void on_btnMenu_clicked()
	{
		QPoint pos(ui->btnMenu->mapToGlobal(QPoint(0, 0)).x(),
			ui->btnMenu->mapToGlobal(QPoint(0, 0)).y() + ui->btnMenu->height());

		m_moreMenu->exec(pos);

		QMouseEvent event(QEvent::MouseButtonRelease, QPoint(0, 0), Qt::LeftButton, 0, 0);
		QApplication::sendEvent(ui->btnMenu, &event);
	}

    void on_btnTest_clicked();
    void on_btnCreateJob_clicked();
    void on_btnCancelJob_clicked();
    void on_btnJobNext_clicked();

	//发布列表
	void on_btnPublish_clicked();
	//在发布成功后返回发布列表
	void on_btn_sucpublish_clicked();
	//在发布列表返回首页
	void on_btnReturn_clicked();
	//在详情页返回发布列表
	void on_btn_returnlist_clicked();
	//云发布
	void on_pushButton_4_clicked();
	//自动生成
	void on_btn_autobuild_clicked();

	//上传封面
	void on_btn_addpic_clicked();

	//云发布确定
	void on_btn_publishok_clicked();
	//云发布取消
	void on_btn_publishcancel_clicked();
	//上传封面图片
	void on_btn_fmpic_clicked();
	//查看发布详情
	void onShowDetail(PublishItem *item);
	//跳到详情页
	void onPageDetail();
	//复制链接和文字
	void on_btn_pubcopy_clicked();
	//复制链接和文字
	void on_btnCopy_clicked();
	//编辑日期
	void on_btn_editdata_clicked();
	//取消发布
	void on_btn_cancelpub_clicked();
	//在发布列表根据发布名称搜索
	void on_pushButton_search_clicked();
	//根据状态筛选发布列表
	void onPublishStatusChanged(int index);
	//自定义过期天数
	void onCheckSlot3(bool state);
	//选择天数
	void onSelectTime(int day);
	//详情页中止按钮
	void onStopTask();
	//修改时间
	void onUpdateDate(const QDateTime &time);
	//表格排序
	void on_refreshTable(int col);
	//编辑时间按钮触发
	//void on_btn_editdata_clicked();
	//获得修改的时间
	void onUpdateTime();
	//请求修改时间
	void onUpdateRequest(qint64 time);
	//请求修改并发限制数
	void onUpdateBf();
	//查看发布描述的详情
	void on_btnDes_clicked();
	//发布成功页面点描述详情
	void on_btnSucDes_clicked();
	void on_btnParaPre_clicked();
	void on_btnParaNext_clicked();

	void on_btnSubmitATBlockAT_clicked();

    void on_btnSubmitCancel_clicked();
    void on_btnSubmitPreStep_clicked();
    void on_btnSubmitNextStep_clicked();

    void on_btnATSubmit_clicked();
    void on_btnATCancel_clicked();
    void on_btnATShow_clicked();
    void on_btnATFailed_clicked();

    void on_btnRangeFile_clicked();
    void on_btnRebuildSubmit_clicked();
    void on_btnRebuildCancel_clicked();
    void on_btnRebuildCancel2_clicked();
    void on_btnRebuildShow_clicked();
    void on_btnRebuildFailed_clicked();

    void on_btnBacktoProject1_clicked();
    void on_btnBacktoProject2_clicked();
    void on_btnBacktoProject3_clicked();
    void on_btnBacktoProject4_clicked();

    void on_btnAddCtrlPoint_clicked();
    void on_btnAddCtrlPoint2_clicked();
    void on_btnCtrlPointView_clicked();
    void on_btnCtrlPointPreStep_clicked();
    void on_btnCtrlPointSubmit_clicked();

    void on_btnAddPos_clicked();
    void on_btnPosView_clicked();
    void on_btnPosPreStep_clicked();
    void on_btnPosSubmit_clicked();

    void on_btnUpload_clicked();

    void onGuideWndClosed();
    void onCSMenuTriggered(QAction *action);
    void onSelPicTriggered(QAction *action);

    void on_btnSelPic_clicked();
    void on_btnDelPic_clicked();
    void on_btnImportPos_clicked();
    void on_btnAddPCtrl_clicked();
    void on_btnEditPCtrl_clicked();
    void on_btnDelPCtrl_clicked();
    void on_btnEditCoord_clicked();
    void on_btnContactService_clicked();
    void on_btnDownloadlist_clicked();

    void on_btnCancelBlock_clicked();
    void on_btnBlockNext_clicked();
    void on_btnAddBlock_clicked();

    void on_btnOpenPos_clicked();
    void on_btnOpenCtrlPoint_clicked();

    void on_btnSurveyPoint_clicked();
    void on_btnDownloadATResult_clicked();
    void on_btnDownloadResult_clicked();

    void on_radioButton_all_pos_toggled(bool check);
    void on_radioButton_current_pos_toggled(bool check);

    void on_comboBox_pic_group_currentTextChanged(const QString &text);
    void on_checkBox_lockRange_stateChanged(int state);

    void on_btnBackAssets_clicked();

    void on_checkBox_enable_custom_stateChanged(int state);

    void on_buttonShareResult_clicked();
    void on_buttonResetView_clicked();

    void on_btnGuide_clicked();

	void on_checkBox_lockRange_3_stateChanged(int state);
	void on_checkBox_enable_custom_3_stateChanged(int state);
	void on_checkBox_xyz_offset_stateChanged(int state);
	void on_btnRangeFile_3_clicked();
	void on_checkBox_blockAT_stateChanged(int state);

	void on_btnKmlBack_clicked();
private:
    void initStyle();
    void initUI();
    void initViews();
    void initWindow();
    void initTrayIcon();
    void addEventListeners();
    void removeEventListeners();
    void retranslateUi();
    void updateComponents();
    void flushUserInfo();
    void backToHome();
    void backToProjectHome();
	void initATBlockListView();
	void RecommendedCoordinateSystem(const double& lon ,const double& lat);


    QPointer<QMenu> createMoreMenu();
    QPointer<QMenu> createTrayMenu();
    QPointer<QMenu> createCSMenu();
    QPointer<QMenu> createSelPicMenu();

    void AutoUpdateAfterQuit();
	void LogoutAccountLogin();
public slots :
    void init();
private slots:
    void updateComponentsFinished(int errorCode, const QString& result);
    void onSystemTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onShowAbout();
    void onTrayMenuActionSlot();
    void onUpdateCheckReturn(int updateCode);
    void onSearchContent(QString key, QString content);
    // 推送消息相关
    void onRecvNotifyMsg(const QList<NoticeContent>);
    void onRecvNoticeWndRet(int);
    void showRollingMsg(const QList<NoticeContent> msgs);
    // 登录异常
    void onLoginException(intptr_t wParam, intptr_t lParam);
    void onRecvResponse(QSharedPointer<ResponseHead> response);

	//上传文件
	void onUploadResponse(QSharedPointer<ResponseHead> response);

    void getComponentsConfigFinished(int errorCode, const QString& result);
    void onUserAuthRequested();
	void showATKmlBlock();
	void deleteATKmlBlock();
	void setCurrentKmlItem(TaskAtKmlBlock * curr_item);


private slots:
    void OnCommandType(intptr_t wParam, intptr_t lParam);
    void OnUpdateTasklistUi(intptr_t wParam, intptr_t lParam);
    void OnShowMsgBox(intptr_t wParam, intptr_t lParam);
    void OnShowMsgError(intptr_t wParam, intptr_t lParam);
    void OnSearchtoTaskList(intptr_t wParam, intptr_t lParam);
    void OnLoginRet(intptr_t wParam, intptr_t lParam);
    void OnDeleteTaskUi(intptr_t wParam, intptr_t lParam);
    void OnChangeTransMode(intptr_t wParam, intptr_t lParam);
    void OnChangeTransServer(intptr_t wParam, intptr_t lParam);
    void OnAppendLogInfo(intptr_t wParam, intptr_t lParam);
    void onLogout(intptr_t wParam, intptr_t lParam);
    void OnViewStateChanged(intptr_t wParam, intptr_t lParam);
    void OnStartUploadTask(intptr_t wParam, intptr_t lParam);
    void OnRelaunch(intptr_t wParam, intptr_t lParam);
    void OnShowLoading(intptr_t wParam, intptr_t lParam);
    void OnCheckUpdateRet(intptr_t wParam, intptr_t lParam);
    void OnQuit4Update(intptr_t wParam, intptr_t lParam);
    void OnDownloadUpdateRet(intptr_t wParam, intptr_t lParam);
    void OnSubmitTaskRet(intptr_t wParam, intptr_t lParam);
    void OnUpLoadAndDownLoadError(intptr_t wParam, intptr_t lParam);
    void OnSpeedLimitChanged(intptr_t wParam, intptr_t lParam);
    void OnLocalProjectPathChanged(intptr_t wParam, intptr_t lParam);
    void OnLoadFileFinished(intptr_t wParam, intptr_t lParam);
    void OnMessageCompleted(intptr_t wParam, intptr_t lParam);
    void OnUpdateClient(intptr_t wParam, intptr_t lParam);
    void OnNewsContentResponse(intptr_t wParam, intptr_t lParam);
    void OnNeedUserLogin(intptr_t wParam, intptr_t lParam);
    void OnTaskOperatorSuccess(intptr_t wParam, intptr_t lParam);
    void OnShowProcessView(intptr_t wParam, intptr_t lParam);
    void OnLocateProjectPage(intptr_t wParam, intptr_t lParam);
    void OnInfolistUi(intptr_t wParam, intptr_t lParam);
    void OnResubmitTaskRet(intptr_t wParam, intptr_t lParam);
private:
    void clickTitlebar();
    //开启一个新任务
    bool StartTask(t_taskid taskid, char subFrom = CMD_SUBMIT_CLIENT, void *pData = 0);
    void runCheckUpdate(int flag);
    //提交任务返回消息
    void OnUserBalanceNotify(QSharedPointer<BalanceResponse> balance);
    void DownloadAutoUpdateApp();
    // 各种消息
    void SetErrorMessage(const QString &message = "");
    void SetToastMessage(const QString &message = "");
    // 初始化侧边按钮
    void initSideBar();
    // 清理缓存
    void cleanClientCache();
    void showLoading(bool show);
	void initSurveyModule();

	//初始化云发布界面
	void InitCloudPublish();
	//初始化发布列表
	void InitPubList();
	//初始化详情task
	void InitPubTask();
	//根据传来的数据初始化发布成功页面
	void InitPublishSuccess(const CreatePublish &item);
	//初始化发布任务tableview
	void InitPublishTask(const PublishTaskResponse &task);
	//初始化发布详情页面
	void InitPublishDetail(const PublishItem &item);
	//初始化
	void InitKsTask(const TaskFrameRenderingInfoResponse & item);
	//
	void InitKsTask1(const TaskFrameRenderingInfoResponse & item);
	

private:
    void selectFileAction();
    void selectDirAction();
private slots:
    void loadPictureGroup(const QList<PictureGroup*>& group);
    void loadPicturesInfo(const QList<PictureInfo>& pictures);
    void onItemSelectionChanged();
    void addCoordInfoToTable(const CoordEditInfo& info);
    void onCellClicked(int row, int column);
    void modifyCoordInfo(int row, const CoordEditInfo& info);
    QWidget* getCellButton(const QVariant& v, QWidget* parent = NULL);
    void onUploadTaskFinished(t_taskid taskid);
    void onLoadPictureInfoFinished(int state);
    void onLoadAtReportInfoFinished(int state);
	void onLoadAtPointCloudDataInfoFinish(int state);
    void onLoadContrlPointDataFinished(int code);
    void onLoadPosInfoDataFinished(int code);
    void onTaskItemDoubleClicked(int taskId);
    void onLoadProjectDataFinished(int code);
    void onMakeTaskCfgFinished(int code);
    void onTablePageChanged(int page, int rowCount);
    void onLoadHistoryProjectFinished(int code);
    void onLoadBlockFileFinished(int code);
	void onLoadBlockFileForSurveysFinished(int code);
	void onLoadBlockFileForSurveyWidget(int code);
	void onInitSurveyWidget();
    void onHideDownloadView();
	bool getRebuildParams(RebuildOptionParams &param);
    void onSurveyDataLoadFinished(int code);
    void onTabWidgetATChanged(int index);
	void SurvayModuel(int code);
    bool getAtBlockInfo(blockATSettingInfo &atinfo);
	//计算文字大小
	void onCalNum();
	//隐藏自动生成密钥
	void onTipsNum(int state);
	
	

signals:
    void makeTaskCfgFinished(int code);
    void loadPictureInfoFinished();
    void loadBlockInfoFinished();
	void initSurveyModulefinish(int code);
	void updateDate(qint64 date);

private:
    TaskInfoCC* getTaskInfoCC();
    void resetWidgetConfig();
    void initTaskInfo(QSharedPointer<RenderParamConfig> param);
    bool makeJsonFiles(const QString& argumentFile);
    void submitTask(const QList<qint64>& taskId, const QList<QString>& taskAlias);
    QString getDetailViewUrl(JobState* job, const QString& url);
    QList<BlockInfo> getBlockInfos();
    bool loadContrlPointData();
    bool loadPositionInfoData();
    void showErrorMsg(int code);
    void loadConfigDataToTable(QTableWidget* table, const QList<ConfigDataItem*>& items);
    bool initProjectPage();
    bool initATReportPage();
    void resetUploadCtrl();
    void updateUploadState();
    void fillCtrlPointView(bool fill);
    void fillPositionInfoView(bool fill);
    void resetCtrlPointViewSet();
    void resetPositionViewSet();
    void updateStackRenderPage(int index, bool click = true);
	void updateStackRenderPageNewProcess(int index, bool click = true);
    void modifySideBarButtonVisible(int projectType, bool showProjBtn = true);
    void initTurnPageCtrl();
    void initCtrlPointPageCtrl(PaginationCtrl* ctrl);
    void initPositionPageCtrl(PaginationCtrl* ctrl);
    void jobStateMachine();
    void renderPageSwitch(bool switchPage = true, int pageId = 999);
    void resetTableAndListWidget();
    void applyPosInfoForPicture(JobState* jobstate);
    void fillBlockFileInfo(BLOCK_TASK::BlockExchange* blockInfo);
    bool isBlockImageDataMatch(JobState* jobstate, QMap<QString, QStringList>& notMatchs);
    bool isPosInfoMatchedImageData(JobState* jobstate, QMap<QString, QStringList>& notMatchs);
    void applyBlockPosForPicture(JobState* jobstate);
    void showShareResultButtons();
	void checkAtKml(const QString& kml);
	void getAllAtKml(QStringList &listKml);

	bool assetsCheckEnable();
	const QString getHash(QString _filePath);
	void getReportInfo(const QString &_filePath, AT_REPORT_INFO::At_Report &_reportInfo);
	QString getRemoteATImagePath(const QString &_filePath);
	bool AtPathJudgment(const QString& folder,const QString &selectFolder);
	bool checkIllegalpath(const QString& path);
	bool checkRebulidCoordSys();
	bool checkTaskCloneCoordSys();


    Ui::CMainWindow*           ui;
    GUIUpdater                 m_guiUpdater;
    QPointer<QButtonGroup>     m_sideBarGroup;
    QPointer<SearchBox>        m_taskSearchBox;
    QPointer<JobListViewPage>  m_jobListViewPage;
    QPointer<QSystemTrayIcon>  m_trayIcon;
    QPointer<NoticePublishMgr> m_noticeMgr;
    QPointer<QSettingWindow>   m_settingWindow;
    QPointer<DownloadViewPage> m_downloadViewPage;
    QPointer<DetailViewPage>   m_detailViewPage;
    QPointer<FloatLogPanel>    m_floatLogPanel;
    /* menu & action*/
    QPointer<QMenu>            m_trayiconMenu;
    QPointer<QAction>          m_aboutAction;
    QPointer<QAction>          m_logoutAction;
    QPointer<QAction>          m_quitAction;

    QPointer<QMenu>            m_diagMenu;
    QPointer<QAction>          m_translogShowAction;
                               
    QPointer<QMenu>            m_moreMenu;
    QPointer<QAction>          m_setupAction;
    QPointer<QAction>          m_updateAction;
    QPointer<QAction>          m_helpAction;
    QPointer<QAction>          m_webAction;

    QPointer<UpdateForm>       m_updateWnd;
    QPointer<UpdateNotify>     m_updateNotify;
    UpdateFlag                 m_updateTrigger;

    UpdateManager              m_updateManager;

    QPointer<QPushButton>      m_buttonATResult;
    QPointer<QPushButton>      m_buttonResult;
	QPointer<QPushButton>      m_buttonSurveyPoint;

    QPointer<QPushButton>      m_buttonShareResult; // 分享成果
    QPointer<QPushButton>      m_buttonResetView;   // 重新加载成果
    /* timers */
    int m_upgradeCheckTimer;
    int m_refreshTimer;
    int m_runAllUploadTimer;
    int m_scheduleTimer;
    int m_downloadTimer;
    QPointer<QTimer> m_atRenderTimer;
    QPointer<QTimer> m_rebuildRenderTimer;

    QStringList                m_scripts;      // 脚本列表
    bool                       m_isLogout;         // 是否是注销

    // 客服菜单
    QPointer<QMenu>            m_csMenu;
    QPointer<QAction>          m_csPhoneAction;          // 客服电话
    QPointer<QAction>          m_csQQAction;             // 客服QQ
    QPointer<QAction>          m_csHelpAction;           // 帮助文档

    // 选择照片菜单
    QPointer<QMenu>            m_selPicMenu;
    QPointer<QAction>          m_selFileAction;          // 选择文件
    QPointer<QAction>          m_selDirAction;           // 选择文件夹

	//发布时封面url
	QString m_publishUrl;
	int m_keId;//发布的壳id
	int m_validDate; //发布时的有效期
	QString m_serverPic;//服务器地址
	int m_currentId;//当前详情页id
	bool m_sortUpDown = false;//发布taskid排序
	PublishItem m_item;//当前发布详情
	QCalendarWidget *m_widget = NULL;//时间窗口
	QPointer<QTimer> m_detailTimer;//详情页定时器,每次进入详情页会等待task接口返回后继续

    enum {
        ATReport_Project,
        ATReport_PhotoGroup,
        ATReport_PhotoAverlapingDegree, // 照片重叠度
        ATReport_Photo,
        ATReport_TiePoint,
        ATReport_CtrlPoint,
        ATReport_Total,
    };
//  QPointer<LoadAtReportInfoThread> m_threadATReport;
// 	QPointer<LoadAtDataInfoThread> m_threadAtViewInfo;
    QMap<int, QWidget*> m_anchorATReport;

    QProgressIndicator*        m_busyIcon;

    ParamSettinsInfo           m_settingsInfo;

    int                        m_projectType; // 项目类型 影像、区块
    QList<qint64> m_taskIds;
    QList<QString> m_taskIdsAlias;
    // 当前展示作业数据
    t_taskid m_currTaskId;
    QString m_currProjectName;
    QString m_currTaskAlias;

    QWidget* m_brCornerWidget;
    TaskCloneWindow* m_rebuildParamWidget;
	Widget *m_surveysPoints;  //刺相控点

//	RebuildOptionParams m_rebuildParam;
	bool m_blockCheck;
	bool m_imageCheck;

	bool m_atBlock; //空三分块
	bool m_atBlockMerage;  //空三合并
};

#endif // CMAINWINDOW_H
