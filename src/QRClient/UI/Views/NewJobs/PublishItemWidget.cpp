#include "stdafx.h"
#include "PublishItemWidget.h"
#include "ThreadJob\QDownloadImage.h"
#include "Common\FileUtil.h"
#include "HttpCommand\HttpCmdManager.h"

//#include "QImageViewer.h"

PublistItemWidget::PublistItemWidget(PublishItem* item, QWidget *parent)
    : QWidget(parent)
    , m_item(item)
{
    ui.setupUi(this);
	ui.verticalWidget->setStyleSheet("background: #2E2E2E");
	ui.btnDetail->setStyleSheet("QPushButton:hover{color: #ffffff; background-color:#7879ff;}");
	//ui.widget_publish_info->setMaximumSize(300, 200);
	//ui.widget_center->setStyleSheet(QString::fromUtf8("border:1px solid blue"));
  //  connect(this, &QWidget::customContextMenuRequested, this, &PublistItemWidget::onCustomContextMenuRequested);
  //  connect(this, &PublistItemWidget::updateViews, this, &PublistItemWidget::onUpdateViews);
   // connect(ui.widget_image, &QImageViewer::onItemClicked, this, &PublistItemWidget::onImageWidgetClicked);
   // m_progressViewer = (QImageViewer*)ui.widget_image;

    initView(m_item);
	m_threadpool.setMaxThreadCount(15);
}

PublistItemWidget::~PublistItemWidget()
{
}

void PublistItemWidget::initView(PublishItem* item)
{
	
	// 发布名称
	QString name = QString("%1").arg(item->publishName);
	//发布时间转时间cuo
	QDateTime time = QDateTime::fromMSecsSinceEpoch(item->publishTime);
	//塞值
	QString date = QString("%1").arg(time.toString("yyyy-MM-dd"));

	//QString elided = getElidedText(ui.label_scene, name, 200);
	//if (name == "111111111")
		//item->status = 1;
	int status = item->status;
	if (status == 1)
	{
		ui.label_reason->show();
		ui.label_reason->setToolTip("已过期");
		ui.label_reason->setStyleSheet("QToolTip{background-color: #ffffff; color:black; font-size:12px;}");
	}
	else
	{
		ui.label_reason->hide();
	}
	//添加省略号
	QFont font;
	font.setPointSize(8);
	ui.label_scene->setFont(font);
	QString str = fontMetrics().elidedText(name, Qt::ElideRight, ui.label_scene->width());
	ui.label_scene->setText(str);
	ui.label_scene->adjustSize();
	ui.label_publishtime->setText(date);
	if (!item->coverImgUrl.isEmpty())
	{
	//下载图片
	QString strFileName = File::getFileBaseName(item->coverImgUrl).toLocal8Bit();

	QString tempPath = RBHelper::makePathFromTempPath("Small").toLocal8Bit();
	QDir temp(tempPath);// (file1);
	if (!temp.exists(tempPath))
	{
		temp.mkdir(tempPath);
	}
	QString file1 = QString("%1/%2")
		.arg(tempPath)
		.arg(item->id);
	QDir file(file1);// (file1);
	if (!file.exists(file1))
	{
		file.mkdir(file1);
	}
	
	/*if (!file.open(QIODevice::Append))
	{
		return ;
	}*/

	QString localPath = QString("%1/%2/%3")
		.arg(tempPath)
		.arg(item->id)
		.arg(strFileName);
	QFileInfo fileinfo(localPath);
	
		if (fileinfo.exists())
		{//如果文件路径存在，直接读取
			//QPalette aa = ui.label_image->palette();
		//	QImage aaa(fileinfo.filePath());
			//QImage bbb = aaa.scaled(ui.label_image->width(), ui.label_image->height(), Qt::IgnoreAspectRatio);
			//aa.setBrush(QPalette::Window, QBrush(bbb));
			//ui.label_image->setPalette(aa);
			ui.label_image->setPixmap(QPixmap(fileinfo.filePath()));
			item->localImgUrl = fileinfo.filePath();
			ui.label_image->setScaledContents(true);
		}
		else
		{ //如果文件路劲不存在，下载
			QDownloadImage *pack = new QDownloadImage(item->id, 0, item->coverImgUrl, localPath);
			m_threadpool.start(pack);
			connect(pack, SIGNAL(onFinished(qint64, int, const QString&)), this, SLOT(onFinished(qint64, int, const QString&)));
		}
	}
	//ui.label_image->setPixmap(QPixmap(item->coverImgUrl));
	//ui.label_image->adjustSize();
   // ui.label_image->setStyleSheet(QString("background-image: url(%1)").arg("E:\imag3.jpg"));

}


void PublistItemWidget::onUpdateViews()
{ 
    //// 是否需要实时刷新控件，不需要则等待50ms后再刷新
    //if (this->needUpdateView() == false) {
    //    static qint64 timeElapsed;
    //    if (GetTickCount() - timeElapsed < 50) {
    //        return;
    //    }
    //    timeElapsed = GetTickCount();
    //}
    // 
    //// job
    //int jobType = m_jobstate->getJobType();
    //switch (jobType)
    //{
    //case UPLOAD_JOB:
    //    this->uploadStateView();
    //    break;
    //case RENDER_JOB:
    //    this->renderingStateView();
    //    break;
    //case DOWNLOAD_JOB:
    //    this->downloadStateView();
    //    break;
    //default:
    //    break;
    //} 
    //m_progressViewer->updateJobstate(m_jobstate); 
}


void PublistItemWidget::removeOpitionWidget()
{
  /*  QLayout* layout = ui.widget_option->layout();
    for (int i = 0; i < layout->count(); i++) {
        QLayoutItem* item = layout->itemAt(i);
        if (item !=NULL && item->widget() != NULL) {
            item->widget()->hide();
            item->widget()->deleteLater();
            layout->removeItem(item);
        }
    }*/
}

void PublistItemWidget::changeCtrl()
{
   // int jobType = m_jobstate->getJobType();
}

void PublistItemWidget::showDynamicInfo()
{
    //// 是否显示耗时和完成时间
    //if (m_jobstate && m_jobstate->IsCanShowTaskCostTime()) {
    //    ui.widget_state_label->show();

    //    ui.label_state_left->setText(m_jobstate->getCompelteDateString("yyyy-MM-dd hh:mm"));
    //    QString time_cost = QObject::tr("累计耗时%1").arg(m_jobstate->getRenderTimeCostString());
    //    ui.label_state_right->setText(time_cost);

    //    // time tooltips
    //    time_cost = QObject::tr("多机累计耗时：%1").arg(m_jobstate->getRenderTimeCostString());
    //    // QString tips = QString("使用机器数量：%1 \r\n%2").arg(m_jobstate->getRenderTiles()).arg(time_cost);
    //    //ui.label_state_right->setToolTip(time_cost);

    //} else if(m_jobstate && m_jobstate->IsUploading()){
    //    ui.widget_state_label->show();

    //} else if (m_jobstate && m_jobstate->IsDownloading()) {
    //    ui.widget_state_label->show();
    //}
}

void PublistItemWidget::commonInfoView()
{
#ifdef FOXRENDERFARMSIZE
	ui.stackedWidget_status->setFixedWidth(480);
	ui.widget_state->setFixedWidth(480);
	ui.widget_progress->setFixedWidth(480);
	ui.widget_state_label->setFixedWidth(480);
		
#endif

 //   // 发布名称
 //   QString name = QString("%1").arg(m_item->publishName);
	////发布时间
	//QString date = QString("%1").arg(m_item->expiredTime);
 //
 //   QString elided = getElidedText(ui.label_scene, name, 200);
 //   ui.label_scene->setText(name);
 //   ui.label_reason->setToolTip("已失效");
	//ui.label_publishtime->setText(date);
}

void PublistItemWidget::uploadStateView()
{
    //// time
    //ui.label_time->setText(m_jobstate->getStartDateString("yyyy-MM-dd hh:mm"));
    //ui.label_status->setText("");
    //ui.label_state_left->setText("");
    //ui.label_state_right->setText("");
    //ui.btnOpition->hide(); 
}

void PublistItemWidget::renderingStateView()
{
    //ui.btnOpition->hide();
    //ui.widget_progress_image->setEnabled(true);
    //changeCtrl();

    //// time
    //ui.label_time->setText(m_jobstate->getStartDateString("yyyy-MM-dd hh:mm"));
    //showDynamicInfo();
    //// state
    ////ui.label_status->setText(m_jobstate->getStatusText());

    //// 正在渲染
    //if (m_jobstate->getTaskStatus() == JobState::enRenderTaskRendering) {
    //    // 超时提醒
    //    if (m_jobstate->isOverTime()) {
    //        QString status = QString("<a style=color:#ff8a00;>%1</a>").arg(m_jobstate->getStatusText());
    //        //ui.label_status->setText(status);
    //    }
    //} else if (m_jobstate->getTaskStatus() == JobState::enRenderTaskFailed
    //    || m_jobstate->getTaskStatus() == JobState::enRenderTaskFinishHasFailed) { // 渲染失败
    //    QString status = QString("<a style=color:#e25c59;>%1</a>").arg(m_jobstate->getStatusText());
    //    //ui.label_status->setText(status);
    //    ui.label_reason->setPixmap(QPixmap(":/view/images/view/tips_failed.png"));
    //    ui.label_reason->setToolTip(tr("重建失败，请联系客服查看原因"));
    //    //ui.widget_reason->show();

    //} else if (m_jobstate->IsTimeoutStop() // 超时停止
    //           || m_jobstate->getTaskStatus() == JobState::enRenderTaskStop // 停止
    //           || m_jobstate->getTaskStatus() == JobState::enRenderUserStop // 用户停止
    //           || m_jobstate->getTaskStatus() == JobState::enRenderTaskArrearageStop) { // 欠费停止

    //    QString status = m_jobstate->getStatusText();    
    //    ui.label_reason->setPixmap(QPixmap(":/view/images/view/tips_stop.png"));

    //    // 不同的停止状态的提示
    //    if (m_jobstate->IsTimeoutStop()) {
    //        status = QString("<a style=color:#ff8a00;>%1</a>").arg(m_jobstate->getStatusText());
    //        ui.label_reason->setToolTip(tr("1、当前作业渲染时间超过用户设置最大渲染时长，请联系在线客服处理。\r\n2、超时停止时长在提交作业前可通过插件提交面板设置。"));

    //    } else if (m_jobstate->getTaskStatus() == JobState::enRenderTaskArrearageStop) {
    //        status = QString("<a style=color:#ff8a00;>%1</a>").arg(m_jobstate->getStatusText());
    //        ui.label_reason->setToolTip(tr("作业已欠费停止，请及时充值。"));

    //    } else {      
    //        ui.label_reason->setToolTip(m_jobstate->getStopReasonText());
    //    }
    //    //ui.label_status->setText(status);
    //    //ui.widget_reason->show();

    //} else if (m_jobstate->getTaskStatus() == JobState::enRenderTaskSubmitFailed) {
    //    QString status = QString("<a style=color:#e25c59;>%1</a>").arg(m_jobstate->getStatusText());
    //    //ui.label_status->setText(status);
    //    ui.label_reason->setPixmap(QPixmap(":/view/images/view/tips_failed.png"));
    //    ui.label_reason->setToolTip(tr("提交失败，请联系客服查看原因"));
    //    //ui.widget_reason->show();

    //}

    //if (m_jobstate->getProjectType() == enImageProject) {
    //    if (m_jobstate->isTaskFinished()) {
    //        ui.btnOpition->show();
    //        ui.btnOpition->setText(QObject::tr("查看成果"));
    //        ui.btnOpition->setProperty("nextPage", RENDER_PAGE_PRODUCTION);
    //    } else if (m_jobstate->isATFinished() && (!m_jobstate->isRebuildFinished() 
    //        || !m_jobstate->is3DTilesFinished() || !m_jobstate->isMajorJobFinished())) {
    //        ui.btnOpition->show();
    //        ui.btnOpition->setText(QObject::tr("查看空三成果"));
    //        ui.btnOpition->setProperty("nextPage", RENDER_PAGE_AT_RESULT);
    //    }

    //} else if (m_jobstate->getProjectType() == enBlockProject) {
    //    if (m_jobstate->isTaskFinished()) {
    //        ui.btnOpition->show();
    //        ui.btnOpition->setText(QObject::tr("查看成果"));
    //        ui.btnOpition->setProperty("nextPage", RENDER_PAGE_PRODUCTION);
    //    }
    //}
}
void PublistItemWidget::onFinished(qint64 taskid, int index, const QString& localUrl)
{
	ui.label_image->setPixmap(QPixmap(localUrl));
	m_item->localImgUrl = localUrl;
	ui.label_image->setScaledContents(true);
}
void PublistItemWidget::downloadStateView()
{
    //changeCtrl();
    //ui.widget_progress_image->setEnabled(true);
    //// time
    //ui.label_time->setText(m_jobstate->getStartDateString("yyyy-MM-dd hh:mm"));

    //if (m_jobstate->IsDownloadFinished()) { // 下载完成
    //    // time cost
    //    showDynamicInfo();
    //    // state
    //    //ui.label_status->setText(m_jobstate->getStatusText());

    //} else { // 下载中
    //    ui.widget_state_label->show();

    //    ui.label_status->setText("");
    //    ui.label_state_left->setText("");
    //    ui.label_state_right->setText("");
    //    return; 

    //    QString p = QString("%1%").arg(m_jobstate->getDownloadProgress());
    //    ui.label_state_left->setText(p);
    //    // speed
    //    ui.label_state_right->setText(RBHelper::convertTransSpeedToStr(m_jobstate->getTransSpeed(), DISPLAY_UNIT_BIT));
    //}
}

void PublistItemWidget::onCustomContextMenuRequested(const QPoint &pos)
{
    emit menuRequested(pos);
}

void PublistItemWidget::setSelected(bool selected)
{
    m_isSelected = selected;

    if (selected) {
        this->setStyleSheet("#widget_center{border: 1px solid #675ffe; border-radius: 10px;}"
            "#widget_center:hover{ border: 1px solid #675ffe; border-radius: 10px;}");
    } else {
        this->setStyleSheet("#widget_center{border: 1px solid #1f1f27; border-radius: 10px;}"
            "#widget_center:hover{ border: 1px solid #675ffe; border-radius: 10px;}");
    }
}

QString PublistItemWidget::getElidedText(QLabel* label, const QString& text, int maxWidth)
{
    if (label == NULL)
        return text;

    QFontMetrics fontMet = label->fontMetrics();
    return fontMet.elidedText(text, Qt::ElideMiddle, maxWidth);
}


void PublistItemWidget::paintEvent(QPaintEvent *event)
{
//    Q_UNUSED(event);
//    QWidget::paintEvent(event);
//
//    QPainter painter(this);
//    QPen   pen;
//    QColor bkgColor(31, 31, 39);
//    QRect  rect = ui.widget_center->rect();
//    qreal _width = rect.width();
//    qreal _height = rect.height();
//    qreal radius = 10;
//    QRectF rectF(rect.x(), rect.y(), _width, _height);
//
//    painter.setRenderHint(QPainter::Antialiasing, true);//	边缘平滑..
//
//    if (m_jobstate->getJobType() == RENDER_JOB) {
//     
//        // 绘制背景
//        pen.setColor(bkgColor);
//        pen.setWidth(0);
//        painter.setPen(pen);
//        painter.setBrush(bkgColor);
//        painter.drawRoundedRect(rectF, radius, radius);
//
//        // 绘制前景
//        qreal _percentage = getRenderingProgress();
//        if (_percentage > 1.0) _percentage = 1.0;
//        qreal rx = rect.x() + ui.widget_progress_image->width();
//        _width = _width - ui.widget_progress_image->width();
//
//        QRectF rectF(rx, rect.y(), _width, _height);
//        QLinearGradient lineGradient(rx, 0, _width, _height);
//        lineGradient.setInterpolationMode(QGradient::ColorInterpolation);
//        QColor foregroundColor(33, 34, 73, 240);
//        if (m_jobstate->TaskIsFinished()) {
//            lineGradient.setColorAt(0, foregroundColor);
//            lineGradient.setColorAt(1, foregroundColor);
//        } else {
//            lineGradient.setColorAt(0, foregroundColor);
//            lineGradient.setColorAt(_percentage, QColor(31, 31, 39, 150));
//            lineGradient.setColorAt(1, QColor(31, 31, 39, 50));
//        }
//
//        QBrush brush(lineGradient);
//        painter.setBrush(lineGradient);
//        QPainterPath ppath;
//        QRect rect2(rx, rect.y(), _width, _height);
//        ppath.addRegion(RBHelper::roundRect(rect2, radius, false, true, false, true));
//        // painter.fillPath(ppath, brush);
//
//        // 绘制进度条背景
//        QRect prect = ui.stackedWidget_status->geometry();
//        rx = prect.x();
//        qreal ry = rect.height() / 2 + 3;
//#ifdef FOXRENDERFARMSIZE
//		_width = 480;
//#else
//		_width = 260;
//#endif
//       
//        _height = 5;
//        QRectF rectP(rx, ry, _width, _height);
//        QColor progressBkgColor("#0c0c10");
//        pen.setColor(progressBkgColor);
//        pen.setWidth(1);
//        painter.setPen(pen);
//        painter.setBrush(progressBkgColor);
//        painter.drawRoundedRect(rectP, 3, 3);
//        // 绘制进度条前景
//        QRectF rectC = QRectF(rx, ry, _width * _percentage, _height);
//        painter.setPen(Qt::NoPen);
//        painter.setBrush(QColor("#675ffe"));
//        painter.drawRoundedRect(rectC, 3, 3);
//
//        // 状态
//        painter.setPen(m_jobstate->getStateColor());
//        QFont font = ui.label_status->font();
//        font.setPixelSize(14);
//        painter.setFont(font);
//        QString status = m_jobstate->getStatusText();
//        painter.drawText(QPoint(rx, ry - 20), status);
//
//    } else if (showTransProgress()) {
//        this->updateWidgets();
//
//        // 绘制背景
//        pen.setColor(bkgColor);
//        pen.setWidth(0);
//        painter.setPen(pen);
//        painter.setBrush(bkgColor);
//        painter.drawRoundedRect(rectF, radius, radius);
//
//        painter.save();
//        qreal _percentage = 0;
//        QRect prect = ui.stackedWidget_status->geometry();
//        qreal rx = prect.x();
//        qreal ry = rect.height() / 2 + 3;
//        _width = 260; //ui.stackedWidget_status->rect().width();
//        _height = 5;
//
//        QRectF rectP(rx, ry, _width, _height);
//        QLinearGradient lineGradient(rx, ry, rx + _width, ry + _height);
//        lineGradient.setInterpolationMode(QGradient::ColorInterpolation);
//        QRectF rectC(rx, ry, 0, _height);
//
//        float prog = m_jobstate->getUploadProgressByFileSize();
//        _percentage = QString::number(m_jobstate->getUploadProgressByFileSize(), 'g', 4).toDouble();
//        if (_percentage < 0.0) _percentage = 0.0;
//        if (_percentage > 1.0) _percentage = 1.0;
//        if (_percentage == 1.0 && m_jobstate->getTransStatus() != enUploadCompleted) {
//            _percentage = 0.0;
//        }
//        rectC = QRectF(rx, ry, _width * _percentage, _height);
//
//        QColor startColor(30, 214, 187);
//        QColor stopColor(36, 167, 248);
//        lineGradient.setColorAt(0, startColor);
//        lineGradient.setColorAt(_percentage, stopColor);
//        lineGradient.setColorAt(1, stopColor);
//
//        // 绘制进度条背景
//        QColor progressBkgColor("#0c0c10");
//        pen.setColor(progressBkgColor);
//        pen.setWidth(1);
//        painter.setPen(pen);
//        painter.setBrush(progressBkgColor);
//        painter.drawRoundedRect(rectP, 3, 3);
//
//        // 绘制前景
//        painter.setPen(Qt::NoPen);
//        painter.setBrush(QColor("#675ffe"));
//        painter.drawRoundedRect(rectC, 3, 3);
//
//        // 状态
//        painter.setPen(Qt::white);
//        QFont font = ui.label_status->font();
//        font.setPixelSize(14);
//        painter.setFont(font);
//        QString status = m_jobstate->getStatusText();
//        painter.drawText(QPoint(rx, ry - 20), status);
//
//        // 进度
//        font = ui.label_state_left->font();
//        font.setPixelSize(12);
//        painter.setFont(font);
//        painter.setPen(QColor("#999999"));
//        double progress = _percentage * 100.0f;
//        QString p = QString::number(progress, 'f', 2);
//        p = QString("%1%").arg(p);
//        painter.drawText(QPoint(rx, ry + 30), p);
//        // 速度
//        QString speed = RBHelper::convertTransSpeedToStr(m_jobstate->getTransSpeed(), DISPLAY_UNIT_BIT);
//        painter.drawText(QPoint(rx + _width - 50, ry + 30), speed);
//        painter.restore();
//
//    } else {
//        // 绘制背景
//        pen.setColor(bkgColor);
//        pen.setWidth(0);
//        painter.setPen(pen);
//        painter.setBrush(bkgColor);
//        painter.drawRoundedRect(rectF, radius, radius);
//    }
}

void PublistItemWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

bool PublistItemWidget::needUpdateView()
{
  /*  return false;
    int jobType = m_jobstate->getJobType();
    if (jobType == RENDER_JOB)
        return false;
    if (jobType == UPLOAD_JOB && m_jobstate->IsUploading()) {
        return true;
    } else if (jobType == DOWNLOAD_JOB && m_jobstate->IsDownloading()) {
        return true;
    }*/
    return false;
}

void PublistItemWidget::onViewResize(const QSize& size)
{
    this->resize(size);
}

bool PublistItemWidget::showTransProgress()
{
   /* int jobType = m_jobstate->getJobType();
    if (jobType == RENDER_JOB)
        return false;
    if (jobType == UPLOAD_JOB)
        return true;

    int transState = m_jobstate->getTransStatus();
    if (jobType == DOWNLOAD_JOB && transState != enDownloadCompleted)
        return true;
		*/
    return false;
}

void PublistItemWidget::updateWidgets()
{
    //if (m_jobstate->IsUploading() || m_jobstate->IsDownloading()) {
    //    // ui.stackedWidget_status->update();
    //}
}

void PublistItemWidget::onOpitionClicked()
{
   /* QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (btn == nullptr) return;

    int page = btn->property("nextPage").toInt();
    CGlobalHelp::Response(UM_LOCATE_PROJECT_PAGE, (intptr_t)page, (intptr_t)m_jobstate->getTaskId());*/
}

void PublistItemWidget::enterEvent(QEvent *event)
{
   /* this->activateWindow();
    ui.btnOpition->activateWindow();
    QWidget::enterEvent(event);*/
}

bool PublistItemWidget::eventFilter(QObject *watched, QEvent *event)
{
   /* if(watched == ui.btnOpition && event->type() == QEvent::MouseButtonPress) {
        emit ui.btnOpition->clicked();
        if(!event->isAccepted()) {
            event->ignore();
            return true;
        }
    }*/
    return false;
}

void PublistItemWidget::onImageWidgetClicked()
{
    // this->setSelected(true);
   // emit itemClicked(m_index);
}

void PublistItemWidget::on_btnDetail_clicked()
{
	int id = m_item->id;
	HttpCmdManager::getInstance()->getPublishById(id, this);
	//emit itemDetail(*m_item);
}

void PublistItemWidget::onRecvResponse(QSharedPointer<ResponseHead> response)
{
	if (response->code == HTTP_ERROR_SUCCESS)
	{
		QSharedPointer<CreatePublish> getPublish = qSharedPointerCast<CreatePublish>(response);
		PublishItem *item = new PublishItem();
		item = getPublish->item;
		item->localImgUrl = m_item->localImgUrl;
		item->status = m_item->status;
		item->publishTime = m_item->publishTime;
		emit itemDetail(item);
	}
}
