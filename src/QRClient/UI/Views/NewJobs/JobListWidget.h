#pragma once

#include <QObject>
#include <QListWidget>

class JobListModel;
class JobListItemDelegate;
class JobListProxyModel;
class TaskItemWidget;

class JobListWidget : public QListWidget
{
    Q_OBJECT

public:
    JobListWidget(QWidget *parent);
    ~JobListWidget();

    void addJobState(JobState* job, TaskItemWidget* widget);
    void removeState(JobState *pState);
    void setRootItem(JobState *pState);
    QList<JobState*> getSelectTaskInfos();
    JobState* getSelectTaskInfo();
    void updateView();
    void clearAllItems();
    void setTaskItem(const QModelIndex &index, TaskItemWidget* widget);
    TaskItemWidget* getTaskItem(const QModelIndex &index) const;

    // 筛选和排序
    void setProxySortKey(int key, int sortType);
    void setProxyFilterKey(int key);
    // 更新主界面按钮状态
    void updateMainUiRenderBtnState();

private slots:
    void onSliderChanged(int p);
    void onItemClicked(const QModelIndex&);

signals:
    void reachedBottom(); // 滚到底部
    void downloadSignal(JobState*);
    void openFileDlgSignal(JobState*);
    void viewResize(const QSize& size);

protected:
    virtual void paintEvent(QPaintEvent *e);

private:
    JobListModel* m_jobListModel;
    JobListItemDelegate* m_listDelegate;
    JobListProxyModel* m_proxyModel;
    QScrollBar* m_vscrollBar;
};
