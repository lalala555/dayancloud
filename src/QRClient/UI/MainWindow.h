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
class Widget;   //����ص�
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

	//�����б�
	void on_btnPublish_clicked();
	//�ڷ����ɹ��󷵻ط����б�
	void on_btn_sucpublish_clicked();
	//�ڷ����б�����ҳ
	void on_btnReturn_clicked();
	//������ҳ���ط����б�
	void on_btn_returnlist_clicked();
	//�Ʒ���
	void on_pushButton_4_clicked();
	//�Զ�����
	void on_btn_autobuild_clicked();

	//�ϴ�����
	void on_btn_addpic_clicked();

	//�Ʒ���ȷ��
	void on_btn_publishok_clicked();
	//�Ʒ���ȡ��
	void on_btn_publishcancel_clicked();
	//�ϴ�����ͼƬ
	void on_btn_fmpic_clicked();
	//�鿴��������
	void onShowDetail(PublishItem *item);
	//��������ҳ
	void onPageDetail();
	//�������Ӻ�����
	void on_btn_pubcopy_clicked();
	//�������Ӻ�����
	void on_btnCopy_clicked();
	//�༭����
	void on_btn_editdata_clicked();
	//ȡ������
	void on_btn_cancelpub_clicked();
	//�ڷ����б���ݷ�����������
	void on_pushButton_search_clicked();
	//����״̬ɸѡ�����б�
	void onPublishStatusChanged(int index);
	//�Զ����������
	void onCheckSlot3(bool state);
	//ѡ������
	void onSelectTime(int day);
	//����ҳ��ֹ��ť
	void onStopTask();
	//�޸�ʱ��
	void onUpdateDate(const QDateTime &time);
	//�������
	void on_refreshTable(int col);
	//�༭ʱ�䰴ť����
	//void on_btn_editdata_clicked();
	//����޸ĵ�ʱ��
	void onUpdateTime();
	//�����޸�ʱ��
	void onUpdateRequest(qint64 time);
	//�����޸Ĳ���������
	void onUpdateBf();
	//�鿴��������������
	void on_btnDes_clicked();
	//�����ɹ�ҳ�����������
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
    // ������Ϣ���
    void onRecvNotifyMsg(const QList<NoticeContent>);
    void onRecvNoticeWndRet(int);
    void showRollingMsg(const QList<NoticeContent> msgs);
    // ��¼�쳣
    void onLoginException(intptr_t wParam, intptr_t lParam);
    void onRecvResponse(QSharedPointer<ResponseHead> response);

	//�ϴ��ļ�
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
    //����һ��������
    bool StartTask(t_taskid taskid, char subFrom = CMD_SUBMIT_CLIENT, void *pData = 0);
    void runCheckUpdate(int flag);
    //�ύ���񷵻���Ϣ
    void OnUserBalanceNotify(QSharedPointer<BalanceResponse> balance);
    void DownloadAutoUpdateApp();
    // ������Ϣ
    void SetErrorMessage(const QString &message = "");
    void SetToastMessage(const QString &message = "");
    // ��ʼ����߰�ť
    void initSideBar();
    // ������
    void cleanClientCache();
    void showLoading(bool show);
	void initSurveyModule();

	//��ʼ���Ʒ�������
	void InitCloudPublish();
	//��ʼ�������б�
	void InitPubList();
	//��ʼ������task
	void InitPubTask();
	//���ݴ��������ݳ�ʼ�������ɹ�ҳ��
	void InitPublishSuccess(const CreatePublish &item);
	//��ʼ����������tableview
	void InitPublishTask(const PublishTaskResponse &task);
	//��ʼ����������ҳ��
	void InitPublishDetail(const PublishItem &item);
	//��ʼ��
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
	//�������ִ�С
	void onCalNum();
	//�����Զ�������Կ
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

    QPointer<QPushButton>      m_buttonShareResult; // ����ɹ�
    QPointer<QPushButton>      m_buttonResetView;   // ���¼��سɹ�
    /* timers */
    int m_upgradeCheckTimer;
    int m_refreshTimer;
    int m_runAllUploadTimer;
    int m_scheduleTimer;
    int m_downloadTimer;
    QPointer<QTimer> m_atRenderTimer;
    QPointer<QTimer> m_rebuildRenderTimer;

    QStringList                m_scripts;      // �ű��б�
    bool                       m_isLogout;         // �Ƿ���ע��

    // �ͷ��˵�
    QPointer<QMenu>            m_csMenu;
    QPointer<QAction>          m_csPhoneAction;          // �ͷ��绰
    QPointer<QAction>          m_csQQAction;             // �ͷ�QQ
    QPointer<QAction>          m_csHelpAction;           // �����ĵ�

    // ѡ����Ƭ�˵�
    QPointer<QMenu>            m_selPicMenu;
    QPointer<QAction>          m_selFileAction;          // ѡ���ļ�
    QPointer<QAction>          m_selDirAction;           // ѡ���ļ���

	//����ʱ����url
	QString m_publishUrl;
	int m_keId;//�����Ŀ�id
	int m_validDate; //����ʱ����Ч��
	QString m_serverPic;//��������ַ
	int m_currentId;//��ǰ����ҳid
	bool m_sortUpDown = false;//����taskid����
	PublishItem m_item;//��ǰ��������
	QCalendarWidget *m_widget = NULL;//ʱ�䴰��
	QPointer<QTimer> m_detailTimer;//����ҳ��ʱ��,ÿ�ν�������ҳ��ȴ�task�ӿڷ��غ����

    enum {
        ATReport_Project,
        ATReport_PhotoGroup,
        ATReport_PhotoAverlapingDegree, // ��Ƭ�ص���
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

    int                        m_projectType; // ��Ŀ���� Ӱ������
    QList<qint64> m_taskIds;
    QList<QString> m_taskIdsAlias;
    // ��ǰչʾ��ҵ����
    t_taskid m_currTaskId;
    QString m_currProjectName;
    QString m_currTaskAlias;

    QWidget* m_brCornerWidget;
    TaskCloneWindow* m_rebuildParamWidget;
	Widget *m_surveysPoints;  //����ص�

//	RebuildOptionParams m_rebuildParam;
	bool m_blockCheck;
	bool m_imageCheck;

	bool m_atBlock; //�����ֿ�
	bool m_atBlockMerage;  //�����ϲ�
};

#endif // CMAINWINDOW_H
