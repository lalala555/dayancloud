#pragma once

#include <QObject>
#include <QListView>
#include "DownloadState.h"

class DownloadListModel;
class DownloadListItemDelegate;
class DownloadListProxyModel;

class DownloadListView : public QListView
{
    Q_OBJECT

public:
    DownloadListView(QWidget *parent = 0);
    ~DownloadListView();
    void addState(CDownloadState *pState);
    void addStates(const QList<CDownloadState*>& jobs);
    void removeState(CDownloadState *pState);
    void setRootItem(CDownloadState *pState);
    void showLoading(bool bShow);
    void refreshModel();
    void updateRow(int row);
protected:
    virtual void showEvent(QShowEvent *e);
    virtual void resizeEvent(QResizeEvent *e);

signals:
    void deleteItem(t_taskid taskId);
    void taskOpition(t_taskid taskId, const QString& type);

private:
    DownloadListModel* m_downloadListModel;
    DownloadListItemDelegate* m_listDelegate;
    QProgressIndicator* m_busyIcon;
    DownloadListProxyModel* m_proxyModel;
};
