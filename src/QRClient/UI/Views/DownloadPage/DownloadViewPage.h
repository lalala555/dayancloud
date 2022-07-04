#pragma once

#include <QWidget>
#include "ui_DownloadViewPage.h"
#include "DownloadState.h"
#include "DownloadListView.h"

class DownloadViewPage : public LayerWidget
{
    Q_OBJECT

public:
    DownloadViewPage(QWidget *parent = Q_NULLPTR);
    ~DownloadViewPage();
    void retranslateUi();
    void initView();
    void addTaskState(CDownloadState *downloadstate);
    bool removeDownloadState(t_taskid dwTaskid);
    void updateTasklistUi(bool bClean);
    void refreshView();
    void updateWidgetGeometry(QObject* obj);
    std::map<t_taskid, CDownloadState*> getDownloadStateList(int pageNum, int rowCount);
    void updateRow(int row);
protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void showEvent(QShowEvent* event);
    virtual void paintEvent(QPaintEvent *p);

private slots:
    void onPageChanged(int pageNum, int rowCount);
    void onDisplayItemCountChanged(int count);
    void onDeleteItem(t_taskid taskId);
    void onTaskOpition(t_taskid taskId, const QString& type);

private:
    Ui::DownloadViewPage ui;
    DownloadListView *m_listView;
    int m_width;
    int m_height;
    qint32 m_currentPage;
};
