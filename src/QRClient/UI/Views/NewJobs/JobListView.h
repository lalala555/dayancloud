#pragma once

#include <QObject>
#include "CustListView.h"

class JobListModel;
class JobListItemDelegate;
class JobListProxyModel;
class TaskItemWidget;

class JobListView : public QListView
{
    Q_OBJECT

public:
    JobListView(QWidget *parent);
    ~JobListView();

    void addState(JobState *pState);
    void addStates(const QList<JobState*>& jobs);
    void removeState(JobState *pState);
    void setRootItem(JobState *pState);
    QList<JobState*> getSelectTaskInfos();
    JobState* getSelectTaskInfo();
    void updateView();
    void clearAllItems();
    void setTaskItem(const QModelIndex &index, TaskItemWidget* widget);
    TaskItemWidget* getTaskItem(const QModelIndex &index) const;
    void updateRow(int row);

    // 筛选和排序
    void setProxySortKey(int key, int sortType);
    void setProxyFilterKey(int key);
    // 更新主界面按钮状态
    void updateMainUiRenderBtnState();

    void showLoading(bool bShow);
    void setEmptyHint(const QString& hint);

    JobState* getCurrentTaskInfo(const QModelIndex &index);

    // 设置选中
    void setItemSelected(JobState *pState);
protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void showEvent(QShowEvent *e);
    virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    virtual void resizeEvent(QResizeEvent *e);
    virtual void paintEvent(QPaintEvent *e);

private slots:
    void onSliderChanged(int p);
    void onItemClicked(const QModelIndex&);

signals:
    void reachedBottom(); // 滚到底部
    void viewResize(const QSize& size);

private:
    JobListModel* m_jobListModel;
    JobListItemDelegate* m_listDelegate;
    JobListProxyModel* m_proxyModel;
    QScrollBar* m_vscrollBar;
    QString m_emptyHint;
    bool m_isShowLoading;
    QProgressIndicator* m_busyIcon;
};
