#pragma once

#include <QWidget>
#include "ui_PublishItemWidget.h"
#include <HttpCommand/HttpCommon.h>
#include <QThreadPool>

//class QImageViewer;

class PublistItemWidget : public QWidget
{
    Q_OBJECT
public:
 
public:
    PublistItemWidget(PublishItem* item, QWidget *parent = Q_NULLPTR);
    ~PublistItemWidget();
 
    void setSelected(bool selected);
//    void setModelIndex(const QModelIndex& index) { m_index = index; }

private:
	void initView(PublishItem* item);
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
    //返回下载图片
    void onFinished(qint64 taskid, int index, const QString& localUrl);
    void onCustomContextMenuRequested(const QPoint &pos);
    void onUpdateViews();
    void onOpitionClicked();
    void onImageWidgetClicked();

	void on_btnDetail_clicked();
	//收到消息
	void onRecvResponse(QSharedPointer<ResponseHead> response);

signals:
    void menuRequested(const QPoint &pos);
    void updateViews();
    void opitionPagesignal(int);
    void itemDetail(PublishItem *item);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual bool eventFilter(QObject *watched, QEvent *event);

private:
	Ui::PublishItemWidget ui;
	PublishItem* m_item;
    QPushButton* m_download;
    QPushButton* m_openFileDlg;
    QLabel *m_loading;
    QMovie* m_movie;
	QThreadPool m_threadpool;
   //ImageViewer* m_progressViewer;
    bool m_isSelected;
  //  QModelIndex m_index;
};
//Q_DECLARE_METATYPE(PublistItemWidget*)
