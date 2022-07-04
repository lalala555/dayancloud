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
    void SetShowType(int nType);    // 设置选中显示状态的类型
    int  GetShowType();             // 获取选中状态类型
    void showTypeTask(int nType);
    void refresh();
    void itemSelected(qint64 taskId);

    void appendLog(const QString &appendLog, int page);

    // 筛选和排序
    void setProxySortKey(int key, int sortType);
    void setProxyFilterKey(int key);
    // 上传速度
    QString getUploadSpeedText();
    float getUploadSpeed();
    bool isLowUploadSpeed();
    // 下载速度
    QString getDownloadSpeedText();
    float getDownloadSpeed();
    // 禁用下载
    void cancelAllDownloads();
    // 刷新行
    void updateRow(qint32 row);
    // 当前页
    void setCurrentPage(int page);
    // 刷新下载页面
    void refreshDownloadPage();
    // 下载输出
    void downloadRenderOutputFiles(qint64 taskId, const QStringList& files);
    // 下载菜单显示隐藏
    void downloadMenuDisplay(JobState* jobState);
public:
    // 上传中菜单操作
    void MenuStartUpload();
    void MenuStopUpload();
    void MenuDeleteUpload();
    void MenuTaskReupload();

    // 渲染菜单操作
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

    // 下载菜单
    void MenuStopDownload();
    void MenuStartDownload();

    // 加载历史任务
    void MenuLoadExistTask();
    // 复制任务ID
    void MenuCopyTaskId();
    // 显示传输日志
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
    // 按钮操作
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

    // 上传菜单
    QAction* m_startUploadAction;
    QAction* m_pauseUploadAction;
    QAction* m_deleteUploadAction;
    QAction* m_restartUploadAction;
    QAction* m_forceSubmitAction; // 强制提交
    // 渲染菜单
    QAction* m_refreshAction;
    QAction* m_startAction;
    QAction* m_pauseAction;
    QAction* m_resubFailedAction;
    QAction* m_locateAction;
    QAction* m_deleteAction;
    // 下载菜单
    QAction* m_stopDownloadAction;
    QAction* m_startDownloadAction;
    // 下载文件
    QPointer<QMenu>     m_downloadMenu;
    QPointer<QAction>   m_selOSGBAction;  // 选择osgb文件
    QPointer<QAction>   m_selObjAction;   // 选择obj件夹
    QPointer<QAction>   m_sel3DTilesAction;  // 选择3d件夹
    QPointer<QAction>   m_selLASAction;   // 选择las件夹
    QPointer<QAction>   m_selTIFFAction;
    QPointer<QAction>   m_selEditableObjAction;
    QPointer<QAction>   m_sel3MXAction; 
    QPointer<QAction>   m_selFBXAction;
    QPointer<QAction>   m_selS3CAction;
    QPointer<QAction>   m_selAllAction;   // 选择全部
    // 加载已有任务
    QPointer<QAction>   m_loadExistTask; 
    // 复制任务号
    QPointer<QAction>   m_copyTaskId;
    // 显示传输日志
    QPointer<QAction>   m_showTransLog;
    // 克隆任务
    QPointer<QAction>   m_cloneTask;

    QProcess *m_extProcess;

    int m_currentPage;

    QMap<int, QAction*> m_outputActions;
};
