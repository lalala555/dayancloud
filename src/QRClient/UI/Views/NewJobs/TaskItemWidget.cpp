#include "stdafx.h"
#include "TaskItemWidget.h"
#include "QImageViewer.h"

TaskItemWidget::TaskItemWidget(JobState* jobstate, QWidget *parent)
    : QWidget(parent)
    , m_jobstate(jobstate)
    , m_isSelected(false)
{
    ui.setupUi(this);

    connect(this, &QWidget::customContextMenuRequested, this, &TaskItemWidget::onCustomContextMenuRequested);
    connect(this, &TaskItemWidget::updateViews, this, &TaskItemWidget::onUpdateViews);
    connect(ui.btnOpition, &QPushButton::clicked, this, &TaskItemWidget::onOpitionClicked);
    connect(ui.widget_progress_image, &QImageViewer::onItemClicked, this, &TaskItemWidget::onImageWidgetClicked);

    ui.btnOpition->installEventFilter(this);

    m_progressViewer = (QImageViewer*)ui.widget_progress_image;
    ui.stackedWidget_status->setCurrentIndex(enStaticInfo);

    this->initView();
}

TaskItemWidget::~TaskItemWidget()
{
}

void TaskItemWidget::initView(JobState* jobstate)
{
    if (jobstate != NULL) {
        m_jobstate = jobstate;
    }
    if (m_jobstate == NULL)
        return;

    this->commonInfoView();

    // job
    int jobType = m_jobstate->getJobType();
    switch (jobType)
    {
    case UPLOAD_JOB:
        this->uploadStateView();
        break;
    case DOWNLOAD_JOB:
    case RENDER_JOB:
        this->renderingStateView();
        break;
    default:
        break;
    }
    // process image
    m_progressViewer->startLoadProcessImage(m_jobstate);
}

void TaskItemWidget::updateView(JobState * jobstate)
{
    if (m_jobstate->getTaskId() == jobstate->getTaskId()) {
        m_jobstate = jobstate;
        emit updateViews();
    }
}

void TaskItemWidget::onUpdateViews()
{ 
    // 是否需要实时刷新控件，不需要则等待50ms后再刷新
    if (this->needUpdateView() == false) {
        static qint64 timeElapsed;
        if (GetTickCount() - timeElapsed < 50) {
            return;
        }
        timeElapsed = GetTickCount();
    }
     
    // job
    int jobType = m_jobstate->getJobType();
    switch (jobType)
    {
    case UPLOAD_JOB:
        this->uploadStateView();
        break;
    case RENDER_JOB:
        this->renderingStateView();
        break;
    case DOWNLOAD_JOB:
        this->downloadStateView();
        break;
    default:
        break;
    } 
    m_progressViewer->updateJobstate(m_jobstate); 
}

t_taskid TaskItemWidget::getTaskId()
{
    if (m_jobstate != NULL)
        return m_jobstate->getTaskId();

    return t_taskid();
}

void TaskItemWidget::removeOpitionWidget()
{
    QLayout* layout = ui.widget_option->layout();
    for (int i = 0; i < layout->count(); i++) {
        QLayoutItem* item = layout->itemAt(i);
        if (item !=NULL && item->widget() != NULL) {
            item->widget()->hide();
            item->widget()->deleteLater();
            layout->removeItem(item);
        }
    }
}

void TaskItemWidget::changeCtrl()
{
    int jobType = m_jobstate->getJobType();
}

void TaskItemWidget::showDynamicInfo()
{
    // 是否显示耗时和完成时间
    if (m_jobstate && m_jobstate->IsCanShowTaskCostTime()) {
        ui.widget_state_label->show();

        ui.label_state_left->setText(m_jobstate->getCompelteDateString("yyyy-MM-dd hh:mm"));
        QString time_cost = QObject::tr("累计耗时%1").arg(m_jobstate->getRenderTimeCostString());
        ui.label_state_right->setText(time_cost);

        // time tooltips
        time_cost = QObject::tr("多机累计耗时：%1").arg(m_jobstate->getRenderTimeCostString());
        // QString tips = QString("使用机器数量：%1 \r\n%2").arg(m_jobstate->getRenderTiles()).arg(time_cost);
        //ui.label_state_right->setToolTip(time_cost);

    } else if(m_jobstate && m_jobstate->IsUploading()){
        ui.widget_state_label->show();

    } else if (m_jobstate && m_jobstate->IsDownloading()) {
        ui.widget_state_label->show();
    }
}

void TaskItemWidget::commonInfoView()
{
#ifdef FOXRENDERFARMSIZE
	ui.stackedWidget_status->setFixedWidth(480);
	ui.widget_state->setFixedWidth(480);
	ui.widget_progress->setFixedWidth(480);
	ui.widget_state_label->setFixedWidth(480);
		
#endif

    ui.widget_state_label->hide();
    ui.widget_reason->hide();
    ui.widget_progress_image->setEnabled(false);
    ui.label_status->hide();
    ui.widget_option->hide();

    // jobinfo
    QString name = QString("%1").arg(m_jobstate->getProjectName());
    if (!m_jobstate->isCanJobOperable() && m_jobstate->getJobType() != UPLOAD_JOB) {
        name = QString("[%1]%2").arg(tr("过期")).arg(name);
    }
    QString elided = getElidedText(ui.label_scene, name, 200);
    ui.label_scene->setText(elided);
    ui.label_scene->setToolTip(m_jobstate->getProjectName());
    QString info = QObject::tr("照片量：%1张  控制点：%2").arg(m_jobstate->getPicCount()).arg(m_jobstate->getCoordCount());
    ui.label_camera->setText(info);
    ui.label_job_id->setText(m_jobstate->getTaskIdAlias());

    // state
    // ui.label_status->setText(m_jobstate->getStatusText());

    // fee
    QUrl url;
    // 子账号 跳转至消费清单
    if (MyConfig.userSet.isChildAccount()) {
        url = RBHelper::getLinkUrl("path_account_child_recharge");
    } else {
        url = RBHelper::getLinkUrl("path_account_parent_recharge");
    }

    if (m_jobstate->getJobType() != UPLOAD_JOB) {
        QString act_fee = m_jobstate->getConsumpFee();
#ifdef FOXRENDERFARM
		QString act_txt = QString("<style>a{text-decoration: none}</style><a style='color:#675ffe; font-size:16px;font-weight:bold;'>$</a>"
			"<a href='%1'; style='color:#675ffe; font-size:24px;font-weight:bold;'>%2</a>").arg(url.toString()).arg(act_fee);
#else
		QString act_txt = QString("<style>a{text-decoration: none}</style><a style='color:#675ffe; font-size:16px;font-weight:bold;'>￥</a>"
			"<a href='%1'; style='color:#675ffe; font-size:24px;font-weight:bold;'>%2</a>").arg(url.toString()).arg(act_fee);
#endif
        ui.label_actual_fee->setText(act_txt);
        ui.label_actual_fee->setOpenExternalLinks(true);
        if (act_fee.isEmpty() || act_fee == "-") {
            ui.label_actual_fee->setText("");
            ui.label_actual_fee->setOpenExternalLinks(false);
        }
        QString feeTips = m_jobstate->getJobFeeTips().toString();
        ui.label_actual_fee->setToolTip(feeTips);
    }

    changeCtrl();
}

void TaskItemWidget::uploadStateView()
{
    // time
    ui.label_time->setText(m_jobstate->getStartDateString("yyyy-MM-dd hh:mm"));
    ui.label_status->setText("");
    ui.label_state_left->setText("");
    ui.label_state_right->setText("");
    ui.btnOpition->hide(); 
}

void TaskItemWidget::renderingStateView()
{
    ui.btnOpition->hide();
    ui.widget_progress_image->setEnabled(true);
    changeCtrl();

    // time
    ui.label_time->setText(m_jobstate->getStartDateString("yyyy-MM-dd hh:mm"));
    showDynamicInfo();
    // state
    //ui.label_status->setText(m_jobstate->getStatusText());

    // 正在渲染
    if (m_jobstate->getTaskStatus() == JobState::enRenderTaskRendering) {
        // 超时提醒
        if (m_jobstate->isOverTime()) {
            QString status = QString("<a style=color:#ff8a00;>%1</a>").arg(m_jobstate->getStatusText());
            //ui.label_status->setText(status);
        }
    } else if (m_jobstate->getTaskStatus() == JobState::enRenderTaskFailed
        || m_jobstate->getTaskStatus() == JobState::enRenderTaskFinishHasFailed) { // 渲染失败
        QString status = QString("<a style=color:#e25c59;>%1</a>").arg(m_jobstate->getStatusText());
        //ui.label_status->setText(status);
        ui.label_reason->setPixmap(QPixmap(":/view/images/view/tips_failed.png"));
        ui.label_reason->setToolTip(tr("重建失败，请联系客服查看原因"));
        //ui.widget_reason->show();

    } else if (m_jobstate->IsTimeoutStop() // 超时停止
               || m_jobstate->getTaskStatus() == JobState::enRenderTaskStop // 停止
               || m_jobstate->getTaskStatus() == JobState::enRenderUserStop // 用户停止
               || m_jobstate->getTaskStatus() == JobState::enRenderTaskArrearageStop) { // 欠费停止

        QString status = m_jobstate->getStatusText();    
        ui.label_reason->setPixmap(QPixmap(":/view/images/view/tips_stop.png"));

        // 不同的停止状态的提示
        if (m_jobstate->IsTimeoutStop()) {
            status = QString("<a style=color:#ff8a00;>%1</a>").arg(m_jobstate->getStatusText());
            ui.label_reason->setToolTip(tr("1、当前作业渲染时间超过用户设置最大渲染时长，请联系在线客服处理。\r\n2、超时停止时长在提交作业前可通过插件提交面板设置。"));

        } else if (m_jobstate->getTaskStatus() == JobState::enRenderTaskArrearageStop) {
            status = QString("<a style=color:#ff8a00;>%1</a>").arg(m_jobstate->getStatusText());
            ui.label_reason->setToolTip(tr("作业已欠费停止，请及时充值。"));

        } else {      
            ui.label_reason->setToolTip(m_jobstate->getStopReasonText());
        }
        //ui.label_status->setText(status);
        //ui.widget_reason->show();

    } else if (m_jobstate->getTaskStatus() == JobState::enRenderTaskSubmitFailed) {
        QString status = QString("<a style=color:#e25c59;>%1</a>").arg(m_jobstate->getStatusText());
        //ui.label_status->setText(status);
        ui.label_reason->setPixmap(QPixmap(":/view/images/view/tips_failed.png"));
        ui.label_reason->setToolTip(tr("提交失败，请联系客服查看原因"));
        //ui.widget_reason->show();

    }

    if (m_jobstate->getProjectType() == enImageProject) {
        if (m_jobstate->isTaskFinished()) {
            ui.btnOpition->show();
            ui.btnOpition->setText(QObject::tr("查看成果"));
            ui.btnOpition->setProperty("nextPage", RENDER_PAGE_PRODUCTION);
        } else if (m_jobstate->isATFinished() && (!m_jobstate->isRebuildFinished() 
            || !m_jobstate->is3DTilesFinished() || !m_jobstate->isMajorJobFinished())) {
            ui.btnOpition->show();
            ui.btnOpition->setText(QObject::tr("查看空三成果"));
            ui.btnOpition->setProperty("nextPage", RENDER_PAGE_AT_RESULT);
        }

    } else if (m_jobstate->getProjectType() == enBlockProject) {
        if (m_jobstate->isTaskFinished()) {
            ui.btnOpition->show();
            ui.btnOpition->setText(QObject::tr("查看成果"));
            ui.btnOpition->setProperty("nextPage", RENDER_PAGE_PRODUCTION);
        }
    }
}

void TaskItemWidget::downloadStateView()
{
    changeCtrl();
    ui.widget_progress_image->setEnabled(true);
    // time
    ui.label_time->setText(m_jobstate->getStartDateString("yyyy-MM-dd hh:mm"));

    if (m_jobstate->IsDownloadFinished()) { // 下载完成
        // time cost
        showDynamicInfo();
        // state
        //ui.label_status->setText(m_jobstate->getStatusText());

    } else { // 下载中
        ui.widget_state_label->show();

        ui.label_status->setText("");
        ui.label_state_left->setText("");
        ui.label_state_right->setText("");
        return; 

        QString p = QString("%1%").arg(m_jobstate->getDownloadProgress());
        ui.label_state_left->setText(p);
        // speed
        ui.label_state_right->setText(RBHelper::convertTransSpeedToStr(m_jobstate->getTransSpeed(), DISPLAY_UNIT_BIT));
    }
}

void TaskItemWidget::onCustomContextMenuRequested(const QPoint &pos)
{
    emit menuRequested(pos);
}

void TaskItemWidget::setSelected(bool selected)
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

QString TaskItemWidget::getElidedText(QLabel* label, const QString& text, int maxWidth)
{
    if (label == NULL)
        return text;

    QFontMetrics fontMet = label->fontMetrics();
    return fontMet.elidedText(text, Qt::ElideMiddle, maxWidth);
}

qreal TaskItemWidget::getRenderingProgress()
{
    if (m_jobstate == NULL)
        return 0;

    return m_jobstate->getRenderProgress();
}

void TaskItemWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QWidget::paintEvent(event);

    QPainter painter(this);
    QPen   pen;
    QColor bkgColor(31, 31, 39);
    QRect  rect = ui.widget_center->rect();
    qreal _width = rect.width();
    qreal _height = rect.height();
    qreal radius = 10;
    QRectF rectF(rect.x(), rect.y(), _width, _height);

    painter.setRenderHint(QPainter::Antialiasing, true);//	边缘平滑..

    if (m_jobstate->getJobType() == RENDER_JOB) {
     
        // 绘制背景
        pen.setColor(bkgColor);
        pen.setWidth(0);
        painter.setPen(pen);
        painter.setBrush(bkgColor);
        painter.drawRoundedRect(rectF, radius, radius);

        // 绘制前景
        qreal _percentage = getRenderingProgress();
        if (_percentage > 1.0) _percentage = 1.0;
        qreal rx = rect.x() + ui.widget_progress_image->width();
        _width = _width - ui.widget_progress_image->width();

        QRectF rectF(rx, rect.y(), _width, _height);
        QLinearGradient lineGradient(rx, 0, _width, _height);
        lineGradient.setInterpolationMode(QGradient::ColorInterpolation);
        QColor foregroundColor(33, 34, 73, 240);
        if (m_jobstate->TaskIsFinished()) {
            lineGradient.setColorAt(0, foregroundColor);
            lineGradient.setColorAt(1, foregroundColor);
        } else {
            lineGradient.setColorAt(0, foregroundColor);
            lineGradient.setColorAt(_percentage, QColor(31, 31, 39, 150));
            lineGradient.setColorAt(1, QColor(31, 31, 39, 50));
        }

        QBrush brush(lineGradient);
        painter.setBrush(lineGradient);
        QPainterPath ppath;
        QRect rect2(rx, rect.y(), _width, _height);
        ppath.addRegion(RBHelper::roundRect(rect2, radius, false, true, false, true));
        // painter.fillPath(ppath, brush);

        // 绘制进度条背景
        QRect prect = ui.stackedWidget_status->geometry();
        rx = prect.x();
        qreal ry = rect.height() / 2 + 3;
#ifdef FOXRENDERFARMSIZE
		_width = 480;
#else
		_width = 260;
#endif
       
        _height = 5;
        QRectF rectP(rx, ry, _width, _height);
        QColor progressBkgColor("#0c0c10");
        pen.setColor(progressBkgColor);
        pen.setWidth(1);
        painter.setPen(pen);
        painter.setBrush(progressBkgColor);
        painter.drawRoundedRect(rectP, 3, 3);
        // 绘制进度条前景
        QRectF rectC = QRectF(rx, ry, _width * _percentage, _height);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor("#675ffe"));
        painter.drawRoundedRect(rectC, 3, 3);

        // 状态
        painter.setPen(m_jobstate->getStateColor());
        QFont font = ui.label_status->font();
        font.setPixelSize(14);
        painter.setFont(font);
        QString status = m_jobstate->getStatusText();
        painter.drawText(QPoint(rx, ry - 20), status);

    } else if (showTransProgress()) {
        this->updateWidgets();

        // 绘制背景
        pen.setColor(bkgColor);
        pen.setWidth(0);
        painter.setPen(pen);
        painter.setBrush(bkgColor);
        painter.drawRoundedRect(rectF, radius, radius);

        painter.save();
        qreal _percentage = 0;
        QRect prect = ui.stackedWidget_status->geometry();
        qreal rx = prect.x();
        qreal ry = rect.height() / 2 + 3;
        _width = 260; //ui.stackedWidget_status->rect().width();
        _height = 5;

        QRectF rectP(rx, ry, _width, _height);
        QLinearGradient lineGradient(rx, ry, rx + _width, ry + _height);
        lineGradient.setInterpolationMode(QGradient::ColorInterpolation);
        QRectF rectC(rx, ry, 0, _height);

        float prog = m_jobstate->getUploadProgressByFileSize();
        _percentage = QString::number(m_jobstate->getUploadProgressByFileSize(), 'g', 4).toDouble();
        if (_percentage < 0.0) _percentage = 0.0;
        if (_percentage > 1.0) _percentage = 1.0;
        if (_percentage == 1.0 && m_jobstate->getTransStatus() != enUploadCompleted) {
            _percentage = 0.0;
        }
        rectC = QRectF(rx, ry, _width * _percentage, _height);

        QColor startColor(30, 214, 187);
        QColor stopColor(36, 167, 248);
        lineGradient.setColorAt(0, startColor);
        lineGradient.setColorAt(_percentage, stopColor);
        lineGradient.setColorAt(1, stopColor);

        // 绘制进度条背景
        QColor progressBkgColor("#0c0c10");
        pen.setColor(progressBkgColor);
        pen.setWidth(1);
        painter.setPen(pen);
        painter.setBrush(progressBkgColor);
        painter.drawRoundedRect(rectP, 3, 3);

        // 绘制前景
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor("#675ffe"));
        painter.drawRoundedRect(rectC, 3, 3);

        // 状态
        painter.setPen(Qt::white);
        QFont font = ui.label_status->font();
        font.setPixelSize(14);
        painter.setFont(font);
        QString status = m_jobstate->getStatusText();
        painter.drawText(QPoint(rx, ry - 20), status);

        // 进度
        font = ui.label_state_left->font();
        font.setPixelSize(12);
        painter.setFont(font);
        painter.setPen(QColor("#999999"));
        double progress = _percentage * 100.0f;
        QString p = QString::number(progress, 'f', 2);
        p = QString("%1%").arg(p);
        painter.drawText(QPoint(rx, ry + 30), p);
        // 速度
        QString speed = RBHelper::convertTransSpeedToStr(m_jobstate->getTransSpeed(), DISPLAY_UNIT_BIT);
        painter.drawText(QPoint(rx + _width - 50, ry + 30), speed);
        painter.restore();

    } else {
        // 绘制背景
        pen.setColor(bkgColor);
        pen.setWidth(0);
        painter.setPen(pen);
        painter.setBrush(bkgColor);
        painter.drawRoundedRect(rectF, radius, radius);
    }
}

void TaskItemWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

bool TaskItemWidget::needUpdateView()
{
    return false;
    int jobType = m_jobstate->getJobType();
    if (jobType == RENDER_JOB)
        return false;
    if (jobType == UPLOAD_JOB && m_jobstate->IsUploading()) {
        return true;
    } else if (jobType == DOWNLOAD_JOB && m_jobstate->IsDownloading()) {
        return true;
    }
    return false;
}

void TaskItemWidget::onViewResize(const QSize& size)
{
    this->resize(size);
}

bool TaskItemWidget::showTransProgress()
{
    int jobType = m_jobstate->getJobType();
    if (jobType == RENDER_JOB)
        return false;
    if (jobType == UPLOAD_JOB)
        return true;

    int transState = m_jobstate->getTransStatus();
    if (jobType == DOWNLOAD_JOB && transState != enDownloadCompleted)
        return true;

    return false;
}

void TaskItemWidget::updateWidgets()
{
    if (m_jobstate->IsUploading() || m_jobstate->IsDownloading()) {
        // ui.stackedWidget_status->update();
    }
}

void TaskItemWidget::onOpitionClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (btn == nullptr) return;

    int page = btn->property("nextPage").toInt();
    CGlobalHelp::Response(UM_LOCATE_PROJECT_PAGE, (intptr_t)page, (intptr_t)m_jobstate->getTaskId());
}

void TaskItemWidget::enterEvent(QEvent *event)
{
    this->activateWindow();
    ui.btnOpition->activateWindow();
    QWidget::enterEvent(event);
}

bool TaskItemWidget::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui.btnOpition && event->type() == QEvent::MouseButtonPress) {
        emit ui.btnOpition->clicked();
        if(!event->isAccepted()) {
            event->ignore();
            return true;
        }
    }
    return false;
}

void TaskItemWidget::onImageWidgetClicked()
{
    // this->setSelected(true);
    emit itemClicked(m_index);
}