/***********************************************************************
* Module:  DetailViewPage.h
* Author:  hqr
* Created: 2021/06/15 18:26:00
* Modifier: hqr
* Modified: 2021/06/15 18:26:00
* Purpose: Declaration of the class
***********************************************************************/
#pragma once

#include <QWidget>
#include "ui_DetailViewPage.h"

class MultiFilterView;

class DetailViewPage : public LayerWidget
{
    Q_OBJECT

    enum {
        COLUMN_DETAIL_FRAMES,
        COLUMN_DETAIL_STATUS,
        COLUMN_DETAIL_PROGRESS,
        COLUMN_DETAIL_FEE,
        COLUMN_DETAIL_EXECTIME,
        COLUMN_DETAIL_START_TIME,
        COLUMN_DETAIL_END_TIME,
        COLUMN_DETAIL_CPU_USAGE,
        COLUMN_DETAIL_RAM_USAGE,
    };
    enum {
        COLUMN_PARAM_KEY,
        COLUMN_PARAM_VALUE,
    };
    enum {
        DETAIL_BTN_PAGE,
        DETAIL_INFO_PAGE,
    };
public:
    DetailViewPage(QWidget *parent = Q_NULLPTR);
    ~DetailViewPage();
    void retranslateUi();
    void initView();

    void addTaskParams(const QString& item);
    void addDetail(JobState::DetailInfo *detailInfo, bool isOvertime);
    QList<JobState::DetailInfo *> getSelectedDetails();

    QString getRenderTotalTimeStr(qint32 time);
    QString frameStatusToString(int status);
    QString timeToString(qint64 time);
    QString costToString(JobState::DetailInfo *detailInfo);

    void refresh();
    void loadFrameDetail(int type);
    void updateFrameDetail(JobState *jobState, int type);
    void showView(JobState *pTaskState);
    void showLoading(bool show);
    void updateWidgetGeometry(QObject* obj);

    void clearViewContent();
    void addDetailInfo(QString key, QString value);

    QString getFrameNameByType(int type, const QString& name);
protected:
    virtual void changeEvent(QEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void showEvent(QShowEvent* event);
    virtual void paintEvent(QPaintEvent *p);

private slots:
    void on_btnHide_clicked();
    void on_btnFrameStart_clicked();
    void on_btnFrameStop_clicked();
    void on_btnFrameResub_clicked();
    // 初始化筛选
    void initFrameStatusFilter();
    void onHeaderSectionClicked(int logicalIndex);
    void onFrameStateFilterSelected();
    // item被点击
    void onTableItemChanged();
    // 定时刷新
    void onRefreshTimer();
    // 滚动加载
    void onReachedBottom();
    void onReachedTop();
    void onSliderChanged(int p);
    QString getDetailViewUrl(JobState* job, qint64 frameId, const QString& url);
    void loadDetailPreview(qint64 frameId);
    // 按钮状态
    void changeButtonState(const QList<JobState::DetailInfo *>& infos);
private:
    Ui::DetailViewPage ui;
    QStandardItemModel* m_tabViewModel;
    JobState *m_jobState;
    JobState *m_curJobState;
    MultiFilterView* m_frameFilterView;
    QTimer* m_timerAutoRefresh;
    QScrollBar* m_vscrollBar;
    QString m_picWidth;
    QString m_picHeight;
    QString m_resolvingPower;
};
