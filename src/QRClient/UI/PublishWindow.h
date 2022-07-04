#pragma once

#include <QWidget>
#include "Dialog.h"
#include "ui_PublishWindow.h"
#include "HttpCommand/HttpCommon.h"
#include <QThread>

class SearchLineEdit;

class PublishWindow : public Dialog
{
    Q_OBJECT

public:
    PublishWindow(QWidget *parent = Q_NULLPTR);
    ~PublishWindow();
    void initUi();

	//获得选中行的壳id
	int getselectedId();
	//多线程请求数据
	//virtual void run();

private slots:
    //收到消息回复
    void onRecvResponse(QSharedPointer<ResponseHead> data);
	//取消
	void on_btn_cancel_clicked();
	//下一步
	void on_btn_next_clicked();
	
	//搜索框值改变
	void onSearchContentChanged(QString value);
	//搜索框搜素
	void onSearchTriggered();
	//回车键
	void onReturnPressed();
	//选中单行
	void onSelectionRow(int row, int column);

private:
	void onSearchContent(QString key, QString content);
  
private:
	Ui::PublishWindow ui;
    qint64 m_taskId;
	QProgressIndicator* m_busyIcon;
	SearchLineEdit *m_Box;
};
