#include "stdafx.h"
#include "DetailViewPage.h"
#include "NewJobs/JobStateMgr.h"
#include "Common/LangTranslator.h"
#include "UI/Base/Controls/HeaderView.h"
#include "HttpCommand/HttpCmdManager.h"
#include "UI/Base/Controls/MultiFilterView.h"

DetailViewPage::DetailViewPage(QWidget *parent)
    : LayerWidget(parent, Qt::FramelessWindowHint)
    , m_jobState(NULL)
    , m_timerAutoRefresh(new QTimer(this))
    , m_curJobState(NULL)
{
    ui.setupUi(this);

    m_frameFilterView = new MultiFilterView();
    m_frameFilterView->hide();
    m_frameFilterView->hideSearchBox();
    m_frameFilterView->setCheckBoxEnableFilter(true);
    m_frameFilterView->setViewSize(160, 210);

    connect(m_timerAutoRefresh, SIGNAL(timeout()), this, SLOT(onRefreshTimer()));

    // 滚动加载
    m_vscrollBar = ui.tableDetail->verticalScrollBar();
    connect(m_vscrollBar, SIGNAL(valueChanged(int)), this, SLOT(onSliderChanged(int)));

    // 隐藏费用
    ui.tableDetail->hideColumn(COLUMN_DETAIL_FEE);
}

DetailViewPage::~DetailViewPage()
{
//    delete m_timerAutoRefresh;
}

void DetailViewPage::initView()
{
    DetailHeaderView* header = new DetailHeaderView(Qt::Horizontal, this);
    ui.tableDetail->setHorizontalHeader(header);
	
#ifdef FOXRENDERFARM
	ui.tableDetail->setColumnWidth(COLUMN_DETAIL_FEE, 150);
#endif

    connect(header, SIGNAL(showFilter(int)), this, SLOT(onHeaderSectionClicked(int)));
    connect(ui.tableDetail, &QTableWidget::itemSelectionChanged, this, &DetailViewPage::onTableItemChanged);

    m_tabViewModel = new QStandardItemModel(ui.tableView);
    ui.tableView->setModel(m_tabViewModel);

    // 筛选
    this->initFrameStatusFilter();
    connect(m_frameFilterView, SIGNAL(okBtnClickedSignal()), this, SLOT(onFrameStateFilterSelected()));
	
#ifdef FOXRENDERFARM
	ui.tableView->setColumnWidth(COLUMN_PARAM_KEY, 50);
	ui.tableView->setColumnWidth(COLUMN_PARAM_VALUE, 350);
#else
	ui.tableView->setColumnWidth(COLUMN_PARAM_KEY, 100);
	ui.tableView->setColumnWidth(COLUMN_PARAM_VALUE, 300);
	
#endif
//	ui.tableView->resizeColumnToContents(COLUMN_PARAM_KEY);
//	ui.tableView->resizeColumnToContents(COLUMN_PARAM_VALUE);
    if (ui.tableView->horizontalHeader() != nullptr) {
        // ui.tableView->horizontalHeader()->setStretchLastSection(true);
    }
}

QString DetailViewPage::getDetailViewUrl(JobState* job, qint64 frameId, const QString& url)
{
    // 拿到render任务号，用于显示结果
    JobState* renderJob = m_curJobState->getRenderJob();
    t_taskid renderJobId = job->getTaskId(); 
    if (renderJob != nullptr) {
        renderJobId = renderJob->getTaskId();
    }

    QString host = WSConfig::getInstance()->get_link_address(url);
    QString token = MyConfig.userSet.userKey;
    QString localeName = RBHelper::getLocaleName();
    int platformId = MyConfig.accountSet.platform;
    QString imageUrl = QString("%1?userId=%2&taskId=%3&rayvision_token=%4&languageFlag=%5&platform=%6&from=%7&frameId=%9")
        .arg(host)
        .arg(MyConfig.accountSet.userId)
        .arg(renderJobId)
        .arg(token)
        .arg(localeName)
        .arg(platformId)
        .arg(WSConfig::getInstance()->get_link_address("path_from"))
        .arg(frameId);

    LOGFMTD("url = %s", imageUrl.toStdString().c_str());

    return imageUrl;
}


void DetailViewPage::showLoading(bool show)
{
    ui.tableDetail->showLoading(show);
}

void DetailViewPage::updateWidgetGeometry(QObject* obj)
{
    if(!obj) return;

    QWidget *parentWidget = static_cast<QWidget*>(obj);

    int width = parentWidget->width() * 0.7;
    int x = parentWidget->x() + parentWidget->width() * 0.3;
    int y = parentWidget->y() - 45;

    this->setFixedSize(width, parentWidget->height());
    this->move(x, y);

    this->updateGeometry();
}

void DetailViewPage::retranslateUi()
{
}

void DetailViewPage::changeEvent(QEvent *event)
{
    switch(event->type()) {
    case QEvent::LanguageChange:
        this->retranslateUi();
        break;
    default:
        LayerWidget::changeEvent(event);
        break;
    }
}

void DetailViewPage::resizeEvent(QResizeEvent *event)
{
    LayerWidget::resizeEvent(event);
    updateWidgetGeometry(parentWidget());
}

void DetailViewPage::showEvent(QShowEvent* event)
{
    LayerWidget::showEvent(event);
    updateWidgetGeometry(parentWidget());
}

void DetailViewPage::paintEvent(QPaintEvent *p)
{
    QStylePainter painter(this);
    //用style画背景 (会使用setstylesheet中的内容)
    QStyleOption opt;
    opt.initFrom(this);
    opt.rect = rect();
    painter.drawPrimitive(QStyle::PE_Widget, opt);

    LayerWidget::paintEvent(p);
}

void DetailViewPage::addTaskParams(const QString& item)
{
    QTableView* tableView = ui.tableView;
    int row = tableView->model()->rowCount();


	ui.tableView->resizeColumnToContents(COLUMN_PARAM_KEY);
	ui.tableView->resizeColumnToContents(COLUMN_PARAM_VALUE);

   

    QStringList pList = item.split("|");
    if(pList.count() == 2) {
        QString key = pList.at(0);
        QString valueStr = pList.at(1);

        if("render_table_sort_rendersubmit_time" == key) {
            valueStr = QDateTime::fromMSecsSinceEpoch(valueStr.toLongLong()).toString("yyyy-MM-dd hh:mm:ss");
        }
        // show gpu cards
        if("graphics_cards_num" == key) {
            // 不是GPU平台，则不显示GPU卡数
            return;
        }
        // 不显示一机多帧
        if ("analysis_multi_selection_multi_label" == key ||
            "senior_setting_multi_frame_label" == key) {
            return;
        }
        // 不显示分块信息
        if ("render_blocker_num" == key) {
            return;
        }

        // 瓦片数量
        if ("user_consume_table_sort_frame" == key) {
            if (m_jobState->m_frameDetails.total > 0) {
                valueStr = QObject::tr("%1块").arg(m_jobState->m_frameDetails.total);  //流程优化后，计数方式更新
            }
        }

        // 只显示壳作业
        if ("render_table_sort_id" == key) {
            if (m_jobState != nullptr && m_jobState->getParentItem() != nullptr) {
                JobState* pJob = m_jobState->getParentItem();
                valueStr = pJob->getTaskIdAlias();
            }          
        }
        // 宽
        if ("render_pic_width" == key) {
            m_picWidth = valueStr;
            return;
        }
        // 高
        if ("render_pic_height" == key) {
            m_picHeight = valueStr;
            return;
        }

        // 总像素
        // if ("render_total_pixel" == key) {
        //     qint64 pixel = valueStr.toULongLong();
        //     valueStr = RBHelper::pixelsToString(pixel);
        // }

        if(valueStr.isEmpty())
            valueStr = "-";

        key = LangTranslator::RequstErrorI18n(key);
        if(key.isEmpty()) return;

        QStandardItem* model_key = new QStandardItem(key);
        model_key->setFlags(Qt::NoItemFlags);
        m_tabViewModel->setItem(row, COLUMN_PARAM_KEY, model_key);

        QStandardItem* model_val = new QStandardItem(valueStr);
        model_val->setToolTip(valueStr);
        model_val->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        m_tabViewModel->setItem(row, COLUMN_PARAM_VALUE, model_val);
    }
}

QString DetailViewPage::getFrameNameByType(int type, const QString& name)
{
    QString compName = name;
    switch (type) {
    case JobState::DetailInfo::pre_frame:
        break;
    case JobState::DetailInfo::photon_frame:
        //compName = QString("%1(%2)").arg(name).arg(QObject::tr("光子"));
        break;
    case JobState::DetailInfo::photon_compose_frame:
        compName = QString("%1(%2)").arg(name).arg(QObject::tr("合成"));
        break;
    case JobState::DetailInfo::priority_frame:
        compName = QString("%1(%2)").arg(name).arg(QObject::tr("优先"));
        break;
    case JobState::DetailInfo::major_frame: 
        compName = QString("%1(%2)").arg(name).arg(QObject::tr("主图"));
        break;
    case JobState::DetailInfo::priority_compose_frame:
        compName = QString("%1(%2)").arg(name).arg(QObject::tr("合成"));
        break;
    case JobState::DetailInfo::major_compose_frame:
        compName = QString("%1(%2)").arg(name).arg(QObject::tr("合成"));
        break;
    case JobState::DetailInfo::houdini_balance_frame:
        break;
    case JobState::DetailInfo::max_channel_frame:
        break;
    case JobState::DetailInfo::houdini_distributed_normal:
        break;
    case JobState::DetailInfo::houdini_distributed_master:
        break;
    }
    return compName;
}

void DetailViewPage::addDetail(JobState::DetailInfo *detailInfo, bool isOvertime)
{
    QTableWidget *tableDetail = ui.tableDetail;
    tableDetail->blockSignals(true);

    int row = tableDetail->rowCount();
    tableDetail->insertRow(row);

    QString name = detailInfo->gopName.isEmpty() ? detailInfo->frameIndex : detailInfo->gopName;
    QTableWidgetItem *frameNameItem = new QTableWidgetItem(getFrameNameByType(detailInfo->frameType, name));
    frameNameItem->setTextAlignment(Qt::AlignCenter);
    frameNameItem->setToolTip(detailInfo->gopName);
    tableDetail->setItem(row, COLUMN_DETAIL_FRAMES, frameNameItem);

    QTableWidgetItem *statusItem = new QTableWidgetItem(this->frameStatusToString(detailInfo->frameStatus));
    statusItem->setTextAlignment(Qt::AlignCenter);
    statusItem->setToolTip(this->frameStatusToString(detailInfo->frameStatus));
    tableDetail->setItem(row, COLUMN_DETAIL_STATUS, statusItem);

    QString progress = detailInfo->frameProgress == "-" ? detailInfo->frameProgress : QString("%1%").arg(detailInfo->frameProgress);   
    QTableWidgetItem *progressItem = new QTableWidgetItem(progress);
    progressItem->setTextAlignment(Qt::AlignCenter);
    progressItem->setToolTip(progress);
    tableDetail->setItem(row, COLUMN_DETAIL_PROGRESS, progressItem);

    QString execTime = detailInfo->frameExecuteTime > 0 ? this->getRenderTotalTimeStr(detailInfo->frameExecuteTime) : "";
    QTableWidgetItem *execTimeItem = new QTableWidgetItem(execTime);
    execTimeItem->setTextAlignment(Qt::AlignCenter);
    execTimeItem->setToolTip(execTime);
    tableDetail->setItem(row, COLUMN_DETAIL_EXECTIME, execTimeItem);

    QTableWidgetItem *feeItem = new QTableWidgetItem(this->costToString(detailInfo));
    feeItem->setTextAlignment(Qt::AlignCenter);
    feeItem->setToolTip(this->costToString(detailInfo));
    tableDetail->setItem(row, COLUMN_DETAIL_FEE, feeItem);

    QTableWidgetItem *timeSatrtItem = new QTableWidgetItem(this->timeToString(detailInfo->startTime));
    timeSatrtItem->setTextAlignment(Qt::AlignCenter);
    timeSatrtItem->setToolTip(this->timeToString(detailInfo->startTime));
    tableDetail->setItem(row, COLUMN_DETAIL_START_TIME, timeSatrtItem);

    QTableWidgetItem *timeEndItem = new QTableWidgetItem(this->timeToString(detailInfo->endTime));
    timeEndItem->setTextAlignment(Qt::AlignCenter);
    timeEndItem->setToolTip(this->timeToString(detailInfo->endTime));
    tableDetail->setItem(row, COLUMN_DETAIL_END_TIME, timeEndItem);

    QTableWidgetItem *cpuItem = new QTableWidgetItem(RBHelper::cpuUsageToString(detailInfo->averageCpu));
    cpuItem->setTextAlignment(Qt::AlignCenter);
    cpuItem->setToolTip(RBHelper::cpuUsageToString(detailInfo->averageCpu));
    tableDetail->setItem(row, COLUMN_DETAIL_CPU_USAGE, cpuItem);

    QTableWidgetItem *ramUseageItem = new QTableWidgetItem(RBHelper::ramUsageToString(detailInfo->averageMemory));
    ramUseageItem->setTextAlignment(Qt::AlignCenter);
    ramUseageItem->setToolTip(RBHelper::ramUsageToString(detailInfo->averageMemory));
    tableDetail->setItem(row, COLUMN_DETAIL_RAM_USAGE, ramUseageItem);

    frameNameItem->setData(Qt::UserRole, QVariant::fromValue<JobState::DetailInfo*>(detailInfo));

    tableDetail->blockSignals(false);
}

QList<JobState::DetailInfo *> DetailViewPage::getSelectedDetails()
{
    QList<JobState::DetailInfo *> list;

    QModelIndexList indexList = ui.tableDetail->selectionModel()->selectedIndexes();
    int row;
    foreach(QModelIndex index, indexList) {
        if(index.column() == COLUMN_DETAIL_FRAMES) {
            row = index.row();
            JobState::DetailInfo *info = ui.tableDetail->item(row, COLUMN_DETAIL_FRAMES)->data(Qt::UserRole).value<JobState::DetailInfo *>();
            info->checked = true;
            list.push_back(info);
        }
    }

    return list;
}

QString DetailViewPage::getRenderTotalTimeStr(qint32 time)
{
    // 显示渲染总时间
    qint32 totalTime = time;
    // 转换成时分秒
    QString times = QString("%1:%2:%3")
        .arg(totalTime / 60 / 60, 2, 10, QLatin1Char('0'))
        .arg((totalTime / 60) % 60, 2, 10, QLatin1Char('0'))
        .arg(totalTime % 60, 2, 10, QLatin1Char('0'));

    return times;
}

QString DetailViewPage::frameStatusToString(int status)
{
    QString state = RBHelper::frameStatusToString(status);

    return state;
}

QString DetailViewPage::timeToString(qint64 time)
{
    return RBHelper::UtcToDateTimeString(time);
}

QString DetailViewPage::costToString(JobState::DetailInfo *detailInfo) // 花费
{
    return RBHelper::feeCostToString(detailInfo->couponFee, detailInfo->arrearsFee, detailInfo->feeAmount);
}

void DetailViewPage::refresh()
{
    // m_tabViewModel->clear();
    showLoading(true);

    QTimer::singleShot(5 * 1000, this, SLOT(onRefreshTimer()));
}

void DetailViewPage::on_btnFrameStart_clicked()
{
    LOGFMTI("[Detail]Button::start");

    QList<JobState::DetailInfo *>DetailInfos = getSelectedDetails();
    if(DetailInfos.isEmpty()) return;

    QList<qint64> frameIds;
    foreach(JobState::DetailInfo *pinfo, DetailInfos)
    {
        if(pinfo->checked) {
            frameIds.append(pinfo->id);
        }
    }
    HttpCmdManager::getInstance()->frameOperatorStart(m_jobState->getTaskId(), frameIds, false);
}

void DetailViewPage::on_btnFrameStop_clicked()
{
    LOGFMTI("[Detail]Menu::abort");

    QList<JobState::DetailInfo *>DetailInfos = getSelectedDetails();
    if(DetailInfos.isEmpty()) return;

    QList<qint64> frameIds;
    foreach(JobState::DetailInfo *pinfo, DetailInfos) {
        if(pinfo->checked) {
            frameIds.append(pinfo->id);
        }
    }
    HttpCmdManager::getInstance()->frameOperatorStop(m_jobState->getTaskId(), frameIds, false);
}

void DetailViewPage::on_btnFrameResub_clicked()
{
    LOGFMTI("[Button]Menu::resub");

    QList<JobState::DetailInfo *>DetailInfos = getSelectedDetails();
    if(DetailInfos.isEmpty()) return;

    QList<qint64> frameIds;
    foreach(JobState::DetailInfo *pinfo, DetailInfos)
    {
        if(pinfo->checked) {
            frameIds.append(pinfo->id);
        }
    }

    HttpCmdManager::getInstance()->frameOperatorRecommit(m_jobState->getTaskId(), frameIds, false);
}

void DetailViewPage::showView(JobState *jobState)
{
    if(jobState == nullptr || m_curJobState == jobState)
        return;
    m_curJobState = jobState;
    
    if (jobState->getProjectType() == enImageProject) {
        // 空三完成，重建未完成
        if (jobState->isATFinished() && !jobState->isRebuildFinished()) {
            m_jobState = jobState->getRebuildJob();
        } 

        if (jobState->isContains3DTilesJob()) {
            // 空三完成，重建完成, 3dtiles未完成
            if (jobState->isATFinished() && jobState->isRebuildFinished() && !jobState->is3DTilesFinished()) {
                m_jobState = jobState->get3DTilesJob();
            }
            // 空三完成，重建完成, 3dtiles完成
            if (jobState->isATFinished() && jobState->isRebuildFinished() && jobState->is3DTilesFinished()) {
                m_jobState = jobState->get3DTilesJob();
            }
        } else {
            // 空三完成，重建完成
            if (jobState->isATFinished() && jobState->isRebuildFinished() && jobState->isMajorJobFinished()) {
                m_jobState = jobState->getRenderJob();
            }
        }

    } else if(jobState->getProjectType() == enBlockProject){
        // 重建未完成显示重建详情
        if (!jobState->isRebuildFinished()) {
            m_jobState = jobState->getRebuildJob();
        }

        if (jobState->isContains3DTilesJob()) {
            // 3dtiles 未完成 显示生产详情
            if (jobState->isRebuildFinished() && !jobState->is3DTilesFinished()) {
                m_jobState = jobState->get3DTilesJob();
            }
            // 重建完成 && 3dtiles 完成
            if (jobState->isRebuildFinished() && jobState->is3DTilesFinished()) {
                m_jobState = jobState->get3DTilesJob();
            }
        } else {
            if (jobState->isRebuildFinished() && jobState->isMajorJobFinished()) {
                m_jobState = jobState->getRenderJob();
            }
        }
    }

    if (m_jobState == nullptr)
        return;


    ui.label->setText(m_jobState->getProjectName());
    clearViewContent();
    JobStateMgr::Inst()->RequestDetailInfoTask(m_jobState);
    HttpCmdManager::getInstance()->loadingTaskParamters(m_jobState->getTaskId());

    loadDetailPreview(0);

    // 开启定时器,30s刷新
    m_timerAutoRefresh->start(1000 * 30);
}

void DetailViewPage::loadFrameDetail(int type)
{   
    if(type == enDetailsUi) {
        int cnt = m_jobState->m_frameDetails.detailItemList.count();
        JobState::DetailInfo *detailInfo = nullptr;
        QList<JobState::DetailInfo*> details = m_jobState->m_frameDetails.detailItemList;
        for(int i = 0; i < cnt; i++) {
            detailInfo = details[i];
            if(detailInfo) {
                bool isOvertime = detailInfo->isFrameOverTime();
                addDetail(detailInfo, isOvertime);
            }
        }
        showLoading(false);
    } else if(type == enTaskParamters) {
        m_tabViewModel->clear();
        m_picWidth.clear();
        m_picHeight.clear();
        m_resolvingPower.clear();

        for(int i = 0; i < m_jobState->m_taskParam.count(); i++) {
            addTaskParams(m_jobState->m_taskParam[i]);
        }

        // 分辨率
        if (!m_picWidth.isEmpty() && !m_picHeight.isEmpty()) {
            m_resolvingPower = QString("%1|%2*%3").arg("render_table_sort_resolving_power").arg(m_picWidth).arg(m_picHeight);
            addTaskParams(m_resolvingPower);
        }
    }
}

void DetailViewPage::updateFrameDetail(JobState *jobState, int type)
{
    if (jobState == nullptr || m_jobState == nullptr)
        return;
    if (jobState->getTaskId() != m_jobState->getTaskId())
        return;
    m_jobState = jobState;

    if(type == enTaskParamters) {
        this->loadFrameDetail(type);

        // 加载详细信息
        this->addDetailInfo("Done", QString::number(m_jobState->GetDoneCount()));
        this->addDetailInfo("Failed", QString::number(m_jobState->GetFailedCount()));
        this->addDetailInfo("Executing", QString::number(m_jobState->GetExcuteCount()));
        this->addDetailInfo("Waiting", QString::number(m_jobState->GetWaitCount()));
        this->addDetailInfo("Aborted", QString::number(m_jobState->GetAbortCount()));

    } else if(type == enDetailOperate) {
        this->refresh();
    } else if (type == enTaskConsume) {
        this->addDetailInfo("ConsumeInfo", QString());
    } else if (type == enDetailsUi) {
        this->loadFrameDetail(type);
        this->loadFrameDetail(enTaskParamters);
    }
}

void DetailViewPage::on_btnHide_clicked()
{
    // 清理数据
    clearViewContent();

    m_timerAutoRefresh->stop();

    m_curJobState = nullptr;
}

void DetailViewPage::clearViewContent()
{
    m_tabViewModel->clear();

    ui.stackedWidget->setCurrentIndex(DETAIL_INFO_PAGE);
    ui.tableDetail->clearContents();
    ui.tableDetail->setRowCount(0);
}

void DetailViewPage::initFrameStatusFilter()
{
    m_frameFilterView->addFilterItem(RBHelper::frameStatusToString(JobState::DetailInfo::task_frame_status_wait), JobState::DetailInfo::task_frame_status_wait);
    m_frameFilterView->addFilterItem(RBHelper::frameStatusToString(JobState::DetailInfo::task_frame_status_rendering), JobState::DetailInfo::task_frame_status_rendering);
    m_frameFilterView->addFilterItem(RBHelper::frameStatusToString(JobState::DetailInfo::task_frame_status_stop), JobState::DetailInfo::task_frame_status_stop);
    m_frameFilterView->addFilterItem(RBHelper::frameStatusToString(JobState::DetailInfo::task_frame_status_completed), JobState::DetailInfo::task_frame_status_completed);
    m_frameFilterView->addFilterItem(RBHelper::frameStatusToString(JobState::DetailInfo::task_frame_status_failed), JobState::DetailInfo::task_frame_status_failed);

    // 避免切换语言后勾选状态被重置
    if (m_jobState != nullptr) {
        QList<int> stateList = m_jobState->m_frameDetails.stateList;
        if (!stateList.isEmpty()) {
            // 先全部反选
            m_frameFilterView->updateAllCheckBoxState(Qt::Unchecked);

            foreach(int state, stateList) {
                m_frameFilterView->updateItemState(state);
            }
        } else {
            m_frameFilterView->updateAllCheckBoxState(Qt::Checked);
        }
    }
}

void DetailViewPage::onHeaderSectionClicked(int logicalIndex)
{
    // 显示之前先隐藏
    m_frameFilterView->hide();
    m_frameFilterView->setFixedHeight(210);

    if (logicalIndex == COLUMN_DETAIL_STATUS) {
        this->initFrameStatusFilter();

        QHeaderView* header = ui.tableDetail->horizontalHeader();
        QRect rct = header->frameRect();
        QRect colRect;
        int scrollValue = 0;

        QScrollBar* scroll = ui.tableDetail->horizontalScrollBar();
        if (scroll != nullptr) {
            scrollValue = scroll->value();
        }

        colRect.setX(rct.x());
        colRect.setY(rct.y());
        colRect.setWidth(header->sectionSize(logicalIndex));
        colRect.setHeight(rct.height());

        QPoint pos = colRect.bottomLeft();
        pos.setX(colRect.width() - scrollValue);

        m_frameFilterView->setHeaderSectionRect(colRect);
        m_frameFilterView->move(ui.tableDetail->mapToGlobal(pos));
        if (m_frameFilterView->height() >= ui.tableDetail->height()) {
            m_frameFilterView->setFixedHeight(ui.tableDetail->height() - 30);
        }
        m_frameFilterView->showView();
    }
}

void DetailViewPage::onFrameStateFilterSelected()
{
    if (m_jobState == NULL) return;

    QList<QVariant> filters = m_frameFilterView->getSelectItemProperty();
    m_jobState->m_frameDetails.stateList.clear();

    foreach(QVariant data, filters) {
        if (data.toInt() != TYPE_SHOW_ALLD) {
            m_jobState->m_frameDetails.stateList.append(data.toInt());
        }
    }

    ui.tableDetail->clearContents();
    ui.tableDetail->setRowCount(0);
    ui.stackedWidget->setCurrentIndex(DETAIL_INFO_PAGE);

    JobStateMgr::Inst()->RequestDetailInfoTask(m_jobState);
    this->showLoading(true);

    // m_frameFilterView->hide();
}

void DetailViewPage::addDetailInfo(QString key, QString value)
{   

#ifdef FOXRENDERFARM
	QString info = QString("%1 %2").arg(value).arg(QObject::tr(""));
#else
	QString info = QString("%1 %2").arg(value).arg(QObject::tr("块"));
#endif
    if (key == "Done") {
        ui.label_finished->setText(info);
    } else if (key == "Failed") {
        ui.label_failed->setText(info);
    } else if (key == "Executing") {
        ui.label_rendering->setText(info);
    } else if (key == "Waiting") {
        ui.label_wait->setText(info);
    } else if (key == "Aborted") {
        ui.label_stop->setText(info);
    } else if (key == "ConsumeInfo") {
        if (m_jobState != nullptr) {
            JobState::TaskFrameConsume& consume = m_jobState->m_frameDetails.jobConsume;
            ui.label_use_time->setText(RBHelper::timeCostToString(consume.taskTimeConsuming));

            QString totalConsume;
            if (!qFuzzyCompare(consume.userAccountConsume, 0.00)) {
#ifdef FOXRENDERFARM
                totalConsume += "$" + QString().setNum(consume.userAccountConsume, 'f', 2);
#else
                totalConsume += "￥" + QString().setNum(consume.userAccountConsume, 'f', 2);
#endif
            }
            totalConsume += " ";
            if (!qFuzzyCompare(consume.couponConsume, 0.00)) {
                totalConsume += "C" + QString().setNum(consume.couponConsume, 'f', 2);
            }
            totalConsume = totalConsume.trimmed();
            if (totalConsume.isEmpty())
                totalConsume = "0";

            if (MyConfig.userSet.isChildAccount() && MyConfig.accountSet.hideJobCharge) {
                ui.label_consume->setText("-");
            } else {
                ui.label_consume->setText(totalConsume);
            }

        }
    }
}

void DetailViewPage::onTableItemChanged()
{
    QModelIndexList indexList = ui.tableDetail->selectionModel()->selectedIndexes();
    if (indexList.isEmpty()) {
        ui.stackedWidget->setCurrentIndex(DETAIL_INFO_PAGE);
    }else{
        ui.stackedWidget->setCurrentIndex(DETAIL_BTN_PAGE);
    }  

    // 显示帧
    QList<JobState::DetailInfo *>DetailInfos = getSelectedDetails();
    if (DetailInfos.isEmpty()) return;
#ifdef _DEBUG
    loadDetailPreview(DetailInfos.at(0)->id);
#endif // DEBUG   

    // 按钮状态
    changeButtonState(DetailInfos);
}

void DetailViewPage::onRefreshTimer() 
{
    if (m_jobState == nullptr)
        return;

    clearViewContent();
    JobStateMgr::Inst()->RequestDetailInfoTask(m_jobState);
    HttpCmdManager::getInstance()->loadingTaskParamters(m_jobState->getTaskId());
}

void DetailViewPage::onReachedBottom()
{
    int pageNum = m_jobState->m_frameDetails.pageNum + 1;
    int pageSize = m_jobState->m_frameDetails.size;
    QString search = m_jobState->m_frameDetails.searchKey;
    QList<int>& stateList = m_jobState->m_frameDetails.stateList;
    bool repeatPageNum = m_jobState->m_frameDetails.reqestPageNums.contains(pageNum); // 记录请求的页码,用于防止重复请求
    if (pageNum <= m_jobState->m_frameDetails.pageCount && repeatPageNum == false) {
        if (search.isEmpty() && stateList.isEmpty()) {
            HttpCmdManager::getInstance()->qureyTaskFrameRenderingInfo(m_jobState->getTaskId(), pageNum, pageSize);
        } else {
            HttpCmdManager::getInstance()->searchTaskFrameRenderingInfo(m_jobState->getTaskId(), search, stateList, pageNum, pageSize);
        }
        m_jobState->m_frameDetails.reqestPageNums.append(pageNum);
    }
}

void DetailViewPage::onReachedTop()
{
    int pageNum = m_jobState->m_frameDetails.pageNum - 1;
    int pageSize = m_jobState->m_frameDetails.size;
    if (pageNum > 0) {
        HttpCmdManager::getInstance()->qureyTaskFrameRenderingInfo(m_jobState->getTaskId(), pageNum, pageSize);
    }
}

void DetailViewPage::onSliderChanged(int p)
{
    if (p > 0 && p == m_vscrollBar->maximum()) {
        onReachedBottom();
    } else if (p == m_vscrollBar->minimum()) {
        // onReachedTop();
    }
}

void DetailViewPage::loadDetailPreview(qint64 frameId)
{
    if (m_jobState == nullptr)
        return;

    QString url = getDetailViewUrl(m_jobState, frameId, "url_3dpreview");
    ui.widget_3dview->loadUrl(url);
}

void DetailViewPage::changeButtonState(const QList<JobState::DetailInfo *>& infos)
{
    if (infos.isEmpty()) return;

    int isFrameCanStart = 0;
    int isFrameCanStop = 0;
    int isFrameCanResub = 0;

    foreach(JobState::DetailInfo *detailInfo, infos) {

        if (detailInfo->isFrameCanStart()) {
            isFrameCanStart++;
        }
        
        if (detailInfo->isFrameCanStop()) {
            isFrameCanStop++;
        }

        if (detailInfo->isFrameCanResub()) {
            isFrameCanResub++;
        }
    }

    // 开始按钮
    if (isFrameCanStart == infos.size() && m_jobState->isCanJobOperable()) {
        ui.btnFrameStart->setEnabled(true);
    } else {
        ui.btnFrameStart->setEnabled(false);
    }

    // 停止按钮
    if (isFrameCanStop == infos.size() && m_jobState->isCanJobOperable()) {
        ui.btnFrameStop->setEnabled(true);
    } else {
        ui.btnFrameStop->setEnabled(false);
    }

    // 重提按钮
    if (isFrameCanResub == infos.size() && m_jobState->isCanJobOperable()) {
        ui.btnFrameResub->setEnabled(true);
    } else {
        ui.btnFrameResub->setEnabled(false);
    }
}