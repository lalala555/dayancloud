#pragma once

#include <QWidget>
#include "ui_DownloadViewItem.h"
#include "DownloadState.h"

class DownloadViewItem : public QWidget
{
    Q_OBJECT

public:
    DownloadViewItem(CDownloadState* downloadstate, QWidget *parent = Q_NULLPTR);
    ~DownloadViewItem();
    void initView(CDownloadState* downloadstate = NULL);
    t_taskid getTaskId();
    void updateView(CDownloadState* jobstate);

private:
    void setViewStyle();
    void updateBtnStyle();
    void updateStateCtrl();

private slots:
    void onOptionClicked();
    void onDeleteClicked();
    void onOpenPathClicked();
    void onUpdateState();
    void onUpdateViews();

signals:
    void updateViews();
    void deleteItem(t_taskid taskId);
    void taskOpition(t_taskid taskId, const QString& type);

protected:
    virtual void paintEvent(QPaintEvent* event);

private:
    Ui::DownloadViewItem ui;
    CDownloadState* m_downloadState;
};
