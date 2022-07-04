#pragma once

#include <QWidget>
#include "ui_TaskItemWidget.h"

class QImageViewer;

class TaskItemWidget : public QWidget
{
    Q_OBJECT
public:
    enum StackedType {
        enStaticInfo,   // 静态信息，渲染信息，下载完成的信息等 
        enDynamicInfo,  // 动态信息，主要是传输信息
    };
public:
    TaskItemWidget(JobState* jobstate, QWidget *parent = Q_NULLPTR);
    ~TaskItemWidget();
    void initView(JobState* jobstate = NULL);
    void updateView(JobState* jobstate);
    t_taskid getTaskId();
    void setSelected(bool selected);
    void setModelIndex(const QModelIndex& index) { m_index = index; }

private:
    void commonInfoView();
    void uploadStateView();
    void renderingStateView();
    void downloadStateView();
    void removeOpitionWidget();
    void showDynamicInfo();
    void changeCtrl();
    QString getElidedText(QLabel* label, const QString& text, int maxWidth);
    qreal getRenderingProgress();
    bool needUpdateView();
    bool showTransProgress();
    void updateWidgets();

public:
    void onViewResize(const QSize& size);

private slots:
    void onCustomContextMenuRequested(const QPoint &pos);
    void onUpdateViews();
    void onOpitionClicked();
    void onImageWidgetClicked();

signals:
    void menuRequested(const QPoint &pos);
    void updateViews();
    void opitionPagesignal(int);
    void itemClicked(const QModelIndex& index);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::TaskItemWidget ui;
    JobState* m_jobstate;
    QPushButton* m_download;
    QPushButton* m_openFileDlg;
    QLabel *m_loading;
    QMovie* m_movie;
    QImageViewer* m_progressViewer;
    bool m_isSelected;
    QModelIndex m_index;
};
Q_DECLARE_METATYPE(TaskItemWidget*)
