#include "stdafx.h"
#include "PublishWindow.h" 
// #include "qzxing/QZXing.h"
#include "HttpCommand/HttpCmdManager.h"
#include "Common/LangTranslator.h"
#include "qrencode/qrencode.h"
#include "HttpTaskListCmd.h"
#include <QClipboard>
#include <QPixmap>

PublishWindow::PublishWindow(QWidget *parent)
    //: Dialog(parent)
{
    ui.setupUi(this); 
	setFocusPolicy(Qt::StrongFocus);
	setWindowTitle(tr("发布"));
	m_Box = ui.lineEditSearch;
	QAction *searchAction = ui.lineEditSearch->addAction(QIcon(":/view/images/view/search_normal.png"), QLineEdit::TrailingPosition);
	connect(searchAction, SIGNAL(triggered()), this, SLOT(onSearchTriggered()));
	connect(ui.tableWidget, SIGNAL(cellClicked(int,int)), this, SLOT(onSelectionRow(int,int)));
	connect(ui.lineEditSearch, SIGNAL(textChanged(const QString &)), this, SLOT(onSearchContentChanged(const QString &)));
    connect(ui.lineEditSearch, SIGNAL(returnPressed()), this, SLOT(onReturnPressed()));
	ui.lineEditSearch->setPlaceholderText(tr("搜索"));
	ui.btn_next->setStyleSheet("QPushButton{border:1px solid #675ffe;color:white;background-color:#transparent}""QPushButton:hover{ background-color:#675ffe; color:white;}");
	ui.btn_cancel->setStyleSheet("QPushButton{border:1px solid #675ffe;color:#675ffe;background-color:transparent;}""QPushButton:hover{ background-color:#675ffe; color:white;}");
	ui.btn_next->setEnabled(false);

    initUi();
}

PublishWindow::~PublishWindow()
{
}

void PublishWindow::initUi()
{
	new ToastWidget(this, tr("请稍等..."), QColor("#6B6CFF"), 5000);
    // 请求任务列表
	FilterContent filterContent = HttpCmdManager::getInstance()->filterContent;
	filterContent.filterType = NORMAL_FILTER;
	// 搜索时全部清空先前的筛选记录
	filterContent.stateList.clear();
	filterContent.producerList.clear();
	filterContent.projList.clear();
	filterContent.userList.clear();
	filterContent.searchWord = "";
	filterContent.content = "";
	HttpCmdManager::getInstance()->renderListNew(filterContent, this);
}

int PublishWindow::getselectedId()
{
	int row = ui.tableWidget->currentRow();
	int value = ui.tableWidget->item(row, 1)->text().toInt();
	return value;
}



void PublishWindow::onRecvResponse(QSharedPointer<ResponseHead> data)
{
	if (data->code == HTTP_ERROR_SUCCESS)
	{
		FilterContent filterContent = HttpCmdManager::getInstance()->filterContent;
		//新增刷新任务会清空关键词
		filterContent.searchWord = "";
		filterContent.content = "";
		QSharedPointer<MaxTaskItemsResponse> tasklist = qSharedPointerCast<MaxTaskItemsResponse>(data);
		
		QList<QString> headers;
		headers.append(tr("项目名称"));
		headers.append(tr("项目ID"));
		headers.append(tr("创建时间"));
		ui.tableWidget->setRowCount(tasklist->items.count());
		ui.tableWidget->setColumnCount(headers.size());
		ui.tableWidget->setHorizontalHeaderLabels(headers);
		ui.tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft); //居左
		if (tasklist->items.count() == 0)
		{
			new ToastWidget(this, QObject::tr("输入有误，请重新输入"), QColor("#6B6CFF"), 2000);
			return;
		}
		for (int i = 0; i < tasklist->items.count(); i++)
		{
			int index = tasklist->items.count() - i -1;
			const MaxTaskItem* info = tasklist->items[index];
			//ui.tableWidget->insertRow(i);
			QTableWidgetItem * item1 = new QTableWidgetItem();
			QTableWidgetItem * item2 = new QTableWidgetItem();
			QTableWidgetItem * item3 = new QTableWidgetItem();
			item1->setText(info->projectName);
			item1->setTextAlignment(Qt::AlignLeft);
			//item2->setText(info->taskAlias);
			item2->setText(QString("%1").arg(info->id));
			item2->setTextAlignment(Qt::AlignLeft);
			QString aa = QString("%1").arg((info->startTime));
			qint64 start = info->startTime;
			if(aa.size() == 10)
			 start = info->startTime * 1000;
			QDateTime startTime = QDateTime::fromMSecsSinceEpoch(start);
			item3->setText(QString("%1").arg(startTime.toString("yyyy-MM-dd")));
			item3->setTextAlignment(Qt::AlignLeft);
			ui.tableWidget->setItem(i, 0, item1);
			ui.tableWidget->setItem(i, 1, item2);
			ui.tableWidget->setItem(i, 2, item3);
		}
		ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
		ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); //设置选中单行
		ui.tableWidget->setFocusPolicy(Qt::NoFocus);
		ui.tableWidget->setSelectionMode(QTableWidget::SingleSelection);
		ui.tableWidget->verticalHeader()->hide();//取消行号
		ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
		ui.tableWidget->setStyleSheet("selection-background-color: green");//设置选中行颜色
		ui.tableWidget->setStyleSheet("border:1px solid #2F2F3A");
		ui.tableWidget->verticalHeader()->setDefaultSectionSize(30);
		ui.tableWidget->horizontalHeader()->setFixedHeight(30);//设置表头高度
		ui.tableWidget->verticalHeader()->setDefaultSectionSize(30);
		ui.tableWidget->horizontalHeader()->setDefaultSectionSize(40);
		//ui.widgetBody->setStyleSheet("border:1px solid #2F2F3A");
		//ui.btn_next->setStyleSheet("background-color:#6B6CFF;border-radius: 4px;color:white");
		//ui.btn_cancel->setStyleSheet("background-color:rgba(0, 0, 0, 0);border-radius: 4px;");
		ui.lineEditSearch->setStyleSheet("width:572px;height: 34px;background: #2F2F3A;border-radius: 6px");
		//ui.horizontalWidget->setStyleSheet("border:none");
		//ui.widgetBtns->setStyleSheet("border:none");
	}
	else {
		new ToastWidget(this, QObject::tr("输入有误，请重新输入"), QColor("#6B6CFF"), 2000);
	}
}


void PublishWindow::on_btn_cancel_clicked()
{
	QDialog::reject();
}

void PublishWindow::on_btn_next_clicked()
{
	//跳转到云发布页面
	QDialog::accept();
}

void PublishWindow::onSearchContent(QString key, QString content)
{
	FilterContent &filterContent = HttpCmdManager::getInstance()->filterContent;
	// QString content = m_taskSearchBox->getSearchContent();
	filterContent.searchWord = content;

	if (content.isEmpty()) {
		filterContent.filterType = NORMAL_FILTER;
	}
	else {
		filterContent.filterType = CONTENT_FILTER;
	}
	// 搜索时全部清空先前的筛选记录
	filterContent.stateList.clear();
	filterContent.producerList.clear();
	filterContent.projList.clear();
	filterContent.userList.clear();

	filterContent.content = content;
	filterContent.showType = TYPE_SHOW_ALLD;
	filterContent.pageNum = 1;
	filterContent.stateList.append(TYPE_SHOW_ALLD);
	HttpCmdManager::getInstance()->renderListNew(filterContent,this);
	filterContent.searchWord = "";
}

void PublishWindow::onSearchContentChanged(QString value)
{
	//QString content = ui.lineEditSearch->text();
	//QString strKey;
	//onSearchContent(strKey, content);
}

void PublishWindow::onSearchTriggered()
{
	QString content = ui.lineEditSearch->text();
	QString strKey;
	onSearchContent(strKey, content);
}

void PublishWindow::onReturnPressed()
{
	this->show();
	QString content = ui.lineEditSearch->text();

	FilterContent &filterContent = HttpCmdManager::getInstance()->filterContent;
	// QString content = m_taskSearchBox->getSearchContent();
	filterContent.searchWord = content;

	if (content.isEmpty()) {
		filterContent.filterType = NORMAL_FILTER;
	}
	else {
		filterContent.filterType = CONTENT_FILTER;
	}
	// 搜索时全部清空先前的筛选记录
	filterContent.stateList.clear();
	filterContent.producerList.clear();
	filterContent.projList.clear();
	filterContent.userList.clear();

	filterContent.content = content;
	filterContent.showType = TYPE_SHOW_ALLD;
	filterContent.pageNum = 1;
	filterContent.stateList.append(TYPE_SHOW_ALLD);
	HttpCmdManager::getInstance()->renderListNew(filterContent, this);
	filterContent.searchWord = "";
}

void PublishWindow::onSelectionRow(int row, int column)
{
	//ui.btn_next->setStyleSheet("background-color:#6B6CFF;border-radius: 4px;color:#675ffe");
	ui.btn_next->setStyleSheet("QPushButton{border:1px solid #675ffe;color:white;background-color:#675ffe}""QPushButton:hover{ background-color:#7879ff; color:white;}");
	ui.btn_next->setEnabled(true);
}
