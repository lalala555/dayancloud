#include "stdafx.h"
#include "DownloadViewItem.h"

DownloadViewItem::DownloadViewItem(CDownloadState* downloadstate, QWidget *parent)
    : QWidget(parent)
    , m_downloadState(downloadstate)
{
    ui.setupUi(this);
    this->setAttribute(Qt::WA_PaintOnScreen);

    connect(ui.btnOption, &QPushButton::clicked, this, &DownloadViewItem::onOptionClicked);
    connect(ui.btnDelete, &QPushButton::clicked, this, &DownloadViewItem::onDeleteClicked);
    connect(ui.btnOpenPath, &QPushButton::clicked, this, &DownloadViewItem::onOpenPathClicked);
    connect(this, &DownloadViewItem::updateViews, this, &DownloadViewItem::onUpdateViews);

    initView(downloadstate);
}

DownloadViewItem::~DownloadViewItem()
{
}

void DownloadViewItem::initView(CDownloadState * downloadstate)
{
    if (downloadstate == nullptr)
        return;
    m_downloadState = downloadstate;

    ui.btnOption->hide();

    ui.labelSence->setText(m_downloadState->getProjectName());
    ui.labelTaskId->setText(m_downloadState->getJobIdAlias());

    ui.labelState->setText("");
    ui.labelSize->setText("");


    setViewStyle();
    updateBtnStyle();

    update();
}

void DownloadViewItem::updateView(CDownloadState* jobstate)
{
    if (m_downloadState->getJobId() == jobstate->getJobId()) {
        m_downloadState = jobstate;
        emit updateViews();
    }
}

static qint64 timeElapsed = 0;
void DownloadViewItem::onUpdateViews()
{ 
    if (GetTickCount() - timeElapsed < 50) {
        return;
    }
    timeElapsed = GetTickCount();

    updateBtnStyle();
    // updateStateCtrl();
    this->update();
}

t_taskid DownloadViewItem::getTaskId()
{
    if (m_downloadState == nullptr)
        return 0;
    return m_downloadState->getJobId();
}

void DownloadViewItem::setViewStyle()
{
    //ui.progressBar->setStyleSheet("QProgressBar{border: none; text-align: center; background: #0c0c10; border-radius:2px;}"
    //    "QProgressBar::chunk {background: #6b6cff; border-radius:2px;}");

    ui.btnDelete->setStyleSheet("QPushButton{background-color: transparent;border-image:url(:/images/download/delete_normal.png);max-width:10px; max-height:10px;border:none;}"
                                "QPushButton:hover{border-image:url(:/images/download/delete_hover.png);}"
                                "QPushButton:pressed{border-image:url(:/images/download/delete_normal.png);}");

    ui.btnOpenPath->setStyleSheet("QPushButton{background-color: transparent;border-image:url(:/images/download/file_normal.png);max-width:14px; max-height:12px;border:none;}"
                                "QPushButton:hover{border-image:url(:/images/download/file_hover.png);}"
                                "QPushButton:pressed{border-image:url(:/images/download/file_normal.png);}");

    ui.labelSize->setStyleSheet("QLabel{font-size: 12px;color: #999999;}");
    ui.labelState->setStyleSheet("QLabel{font-size: 12px;color: #999999;}");
    ui.labelTaskId->setStyleSheet("QLabel{font-size: 12px;color: #999999;}");
    ui.labelSence->setStyleSheet("QLabel{font-size: 14px;color: #ffffff;}");
}

void DownloadViewItem::onDeleteClicked()
{
    emit deleteItem(getTaskId());
}

void DownloadViewItem::onOpenPathClicked()
{
    if (m_downloadState == nullptr)
        return;

    QString projectSavePath = m_downloadState->GetLocalSavePath();
    QString outputPath = projectSavePath;
    if (!projectSavePath.contains(m_downloadState->getOutputLabel()))
        outputPath = m_downloadState->getOutputPath(projectSavePath);

    try {
        outputPath = QDir::toNativeSeparators(outputPath);
        QDesktopServices::openUrl(QUrl::fromLocalFile(outputPath));
    } catch (std::exception &e) {
    }
}

void DownloadViewItem::onUpdateState()
{
}

void DownloadViewItem::onOptionClicked()
{
    QObject* recv = (QObject*)sender();
    if (recv == nullptr) return;

    QPushButton *btn = qobject_cast<QPushButton*>(recv);
    QString option = btn->property("option").toString();

    if (option == "start") {
        emit taskOpition(getTaskId(), "start");
    } else if (option == "stop") {
        emit taskOpition(getTaskId(), "stop");
    } else if (option == "retry") {
        emit taskOpition(getTaskId(), "retry");
    }
}

void DownloadViewItem::updateBtnStyle()
{
    if (m_downloadState == nullptr)
        return;
    int status = m_downloadState->GetTransState();

    if (status == enTransDownloading
        || status == enDownloadWaiting
        || status == enDownloadQueued
        || status == enDownloadUnfinish
        || status == enDownloadLasttime) {
        ui.btnOption->setStyleSheet("QPushButton{background-color: transparent;border-image:url(:/images/download/stop_normal.png);max-width:8px; max-height:10px;border:none;}"
                                      "QPushButton:hover{border-image:url(:/images/download/stop_hover.png);}"
                                      "QPushButton:pressed{border-image:url(:/images/download/stop_normal.png);}");
        ui.btnOption->setProperty("option", "stop");
        ui.btnOption->show();

    } else if (status == enDownloadStop) {
        ui.btnOption->setStyleSheet("QPushButton{background-color: transparent;border-image:url(:/images/download/start_normal.png);max-width:8px; max-height:10px;border:none;}"
                                    "QPushButton:hover{border-image:url(:/images/download/start_hover.png);}"
                                    "QPushButton:pressed{border-image:url(:/images/download/start_normal.png);}");

        ui.btnOption->setProperty("option", "start");
        ui.btnOption->show();
    
    } else if (status == enDownloadCompleted ) {

        ui.btnOption->hide();
    
    } else if (status == enDownloadCompletedWithError
        || status == enDownloadError) {
    
        ui.btnOption->setStyleSheet("QPushButton{background-color: transparent;border-image:url(:/images/download/retry_normal.png);max-width:10px; max-height:11px;border:none;}"
                                    "QPushButton:hover{border-image:url(:/images/download/retry_hover.png);}"
                                    "QPushButton:pressed{border-image:url(:/images/download/retry_normal.png);}");

        ui.btnOption->setProperty("option", "retry");
        ui.btnOption->show();

    } else {
        ui.btnOption->hide();
    }
}

void DownloadViewItem::updateStateCtrl()
{
    if (m_downloadState == nullptr)
        return;

    ui.labelState->setText(m_downloadState->GetStatusString());
    if (m_downloadState->IsDownloading()) {
        ui.labelSize->setText(m_downloadState->GetSpeed());
    } else {
        ui.labelSize->setText("");
    }    
}

void DownloadViewItem::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QWidget::paintEvent(event);

    QPainter painter(this);
    QPen   pen;
    QColor bkgColor(31, 31, 39);
    QRect  rect = ui.widget_progress->rect();
    qreal _width = rect.width();
    qreal _height = rect.height();

    painter.setRenderHint(QPainter::Antialiasing, true);//	边缘平滑..

    // 绘制前景
    qreal _percentage = m_downloadState->GetProgress() / 100.0;
    if (_percentage > 1.0) _percentage = 1.0;
    qreal rx = 0;
    
    // 绘制进度条背景
    QRect prect = ui.widgetProgress->geometry();
    rx = prect.x() + ui.widget_base_info->width() + 8;
    qreal ry = prect.y();
    _width = prect.width();
    _height = 4;
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
    QRect state_rect = ui.widget_4->rect();
    painter.setPen(QColor("#999999"));
    QFont font = ui.labelState->font();
    font.setPixelSize(12);
    painter.setFont(font);
    QString status = m_downloadState->GetStatusString();
    painter.drawText(QPoint(rx, ry - 10), status);

    // 速度
    QString speed = m_downloadState->GetSpeed();
    painter.setFont(font);
    // painter.drawText(ui.labelSize->pos(), speed);
    QFontMetrics titleMet(font);
    int textWidth = titleMet.width(speed);
    painter.drawText(QPoint(rx + state_rect.width() - textWidth, ry - 10), speed);
    painter.restore();
}