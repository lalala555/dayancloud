#pragma once

#include <QObject>
#include <QListWidget>

class JobListView;
class JobListWidget;
class TaskItemFactory;
class JobListModel;
class FloatLogPanel;

class JobListViewPage : public QObject
{
    Q_OBJECT

public:
    JobListViewPage(JobListView *listView, FloatLogPanel* floatPanel, QObject *parent = 0);
    ~JobListViewPage();
    void retranslateUi();
    void initView();
    void addTaskState(JobState *taskState);
    void removeTaskState(int taskId);
    void removeRow(int row);
    void showLoading(bool bShow);
    void deleteTaskUi(int taskId);
    void updateTasklistUi(bool bClean);
    void SetShowType(int nType);    // ����ѡ����ʾ״̬������
    int  GetShowType();             // ��ȡѡ��״̬����
    void showTypeTask(int nType);
    void refresh();
    void itemSelected(qint64 taskId);

    void appendLog(const QString &appendLog, int page);

    // ɸѡ������
    void setProxySortKey(int key, int sortType);
    void setProxyFilterKey(int key);
    // �ϴ��ٶ�
    QString getUploadSpeedText();
    float getUploadSpeed();
    bool isLowUploadSpeed();
    // �����ٶ�
    QString getDownloadSpeedText();
    float getDownloadSpeed();
    // ��������
    void cancelAllDownloads();
    // ˢ����
    void updateRow(qint32 row);
    // ��ǰҳ
    void setCurrentPage(int page);
    // ˢ������ҳ��
    void refreshDownloadPage();
    // �������
    void downloadRenderOutputFiles(qint64 taskId, const QStringList& files);
    // ���ز˵���ʾ����
    void downloadMenuDisplay(JobState* jobState);
public:
    // �ϴ��в˵�����
    void MenuStartUpload();
    void MenuStopUpload();
    void MenuDeleteUpload();
    void MenuTaskReupload();

    // ��Ⱦ�˵�����
    void MenuTaskStart();
    void MenuTaskFullspeed();
    void MenuTaskStop();
    void MenuTaskResub();
    void MenuTaskAbort();
    void MenuTaskDelete();
    void MenuTaskRefresh();
    void MenuTaskDownload();
    void MenuShowLocalSavePath(JobState* jobState = NULL);
    void MenuShowPreview();
    void MenuCloneTask();
    void MenuTaskLevel(int nType);
    void MenuFailFrameTask(bool bChecked);
    void MenuSaveasDownload(const QStringList& outputList);
    bool UseVoucher();
    bool isCanUseVoucher();
    void MenuUseVoucher();

    // ���ز˵�
    void MenuStopDownload();
    void MenuStartDownload();

    // ������ʷ����
    void MenuLoadExistTask();
    // ��������ID
    void MenuCopyTaskId();
    // ��ʾ������־
    void MenuShowTransLog();
private slots:
    void onCustomContextMenuRequested(QPoint pos);
    void onJobPopMenuSlot(QAction *action);
    void onUploadPopMenuSlot(QAction *action);
    void onEnteredSlot(const QModelIndex & index);
    void onClickedSlot(const QModelIndex & index);
    void onDoubleClickedSlot(const QModelIndex & index);
    void onReachedBottom();
    void onHeaderChanged();
    void onDownloadClicked(JobState* job, const QStringList& outputList);
    void onOpenFileDlgClicked(JobState* job);
    void onDownloadMenuTriggered(QAction *action);

private:
    void RunTaskState(JobState *pState, enBtnCheckType nType);
    void SelectTaskState(enBtnCheckType nType);
    int GetSelectCount();
    JobState* GetSelectState();
    QList<JobState*> GetSelectStates();
    // ��ť����
    void BtnTaskStart(JobState *pState);
    void BtnTaskFullspeed(JobState *pState);
    void BtnTaskStop(JobState *pState);
    void BtnTaskResub(JobState *pState);
    void BtnTaskDelete(JobState *pState);
    void BtnTaskAbort(JobState *pState);

signals:
    void taskItemDoubleClicked(qint64 taskId);
    void hideDownloadView();

private:
    JobListView *m_listView;
    QPointer<QMenu> m_popJobMenu;
    QPointer<QMenu> m_popUploadMenu;
    FloatLogPanel* m_floatLogPanel;

    int m_lastRefreshTime;

    // �ϴ��˵�
    QAction* m_startUploadAction;
    QAction* m_pauseUploadAction;
    QAction* m_deleteUploadAction;
    QAction* m_restartUploadAction;
    QAction* m_forceSubmitAction; // ǿ���ύ
    // ��Ⱦ�˵�
    QAction* m_refreshAction;
    QAction* m_startAction;
    QAction* m_pauseAction;
    QAction* m_resubFailedAction;
    QAction* m_locateAction;
    QAction* m_deleteAction;
    // ���ز˵�
    QAction* m_stopDownloadAction;
    QAction* m_startDownloadAction;
    // �����ļ�
    QPointer<QMenu>     m_downloadMenu;
    QPointer<QAction>   m_selOSGBAction;  // ѡ��osgb�ļ�
    QPointer<QAction>   m_selObjAction;   // ѡ��obj����
    QPointer<QAction>   m_sel3DTilesAction;  // ѡ��3d����
    QPointer<QAction>   m_selLASAction;   // ѡ��las����
    QPointer<QAction>   m_selTIFFAction;
    QPointer<QAction>   m_selEditableObjAction;
    QPointer<QAction>   m_sel3MXAction; 
    QPointer<QAction>   m_selFBXAction;
    QPointer<QAction>   m_selS3CAction;
    QPointer<QAction>   m_selAllAction;   // ѡ��ȫ��
    // ������������
    QPointer<QAction>   m_loadExistTask; 
    // ���������
    QPointer<QAction>   m_copyTaskId;
    // ��ʾ������־
    QPointer<QAction>   m_showTransLog;
    // ��¡����
    QPointer<QAction>   m_cloneTask;

    QProcess *m_extProcess;

    int m_currentPage;

    QMap<int, QAction*> m_outputActions;
};
