#include "stdafx.h"
#include "QImageViewer.h"
#include "HttpCommand/HttpCmdManager.h"
#include "UI/Base/Widgets/ImageWidget.h"
#include "ThreadJob/QDownloadImage.h"

QImageViewer::QImageViewer(QWidget *parent)
    : QWidget(parent)
    , m_finishedImages(0)
{
    ui.setupUi(this);

    m_host = WSConfig::getInstance()->get_link_address("url_task_host");

    connect(&m_freshTimer, SIGNAL(timeout()), this, SLOT(onLoadProcessImage()));
}

QImageViewer::~QImageViewer()
{
    m_freshTimer.stop();
}

void QImageViewer::startLoadProcessImage(JobState* job)
{
    if (job == NULL) return;
    m_job = job;

    m_freshTimer.start(100);
}

void QImageViewer::updateView()
{
    // this->update();
    // ui.widget_image->update();
}

void QImageViewer::updateJobstate(JobState* job)
{
    if (m_job != job)
        return;

    // m_job = job;
}

void QImageViewer::loadProgressImage(QSharedPointer<TaskProcessImgResponse> processImg)
{
#if 0
    QGridLayout* layout = (QGridLayout*)ui.widget_image->layout();
    // 隐藏weiget
    for (int i = 0; i < layout->count(); i++) {
        QLayoutItem *item = layout->itemAt(i);
        if (item != NULL) {
            QWidget *widget = item->widget();
            if (widget != NULL) {
                widget->hide();
            }
        }
    }

    // 填充
    int matrix = sqrt((double)processImg->block);
    QList<TaskGrabInfo*> grabInfo = processImg->processImgList;

    if (grabInfo.isEmpty()) return;

    for (int i = 0; i < matrix; i++) {
        for (int j = 0; j < matrix; j++) {
            TaskGrabInfo* grab = grabInfo.at(i * matrix + j);
            ImageWidget* image = NULL;
            grab->col = j;
            grab->row = i;

            if (layout->itemAt(i * matrix + j)) {
                image = (ImageWidget*)layout->itemAt(i * matrix + j)->widget();
            } else {
                image = new ImageWidget(m_host, i, j, matrix, ui.widget_image);
                // 添加到布局           
                layout->addWidget(image, i, j);
                connect(this, SIGNAL(startLoadImage()), image, SLOT(downloadImage()));
                connect(image, SIGNAL(mouseClicked()), this, SLOT(onMouseClicked()));
            }

            if (grab != NULL) {
                image->setGrabInfo(grab);
                image->hideInfoWidget();
                image->show();
            }
        }
    }
    ui.widget_image->setLayout(layout);

    emit startLoadImage();
#else
    m_matrix = sqrt((double)processImg->block);
    m_job->m_grabInfo = processImg->processImgList;

    if (m_job->m_grabInfo.isEmpty()) {
        TaskGrabInfo* grab = new TaskGrabInfo;
        m_job->m_grabInfo.append(grab);
        this->showProcessImage();
        return;
    } 

    for (int i = 0; i < m_matrix; i++) {
        for (int j = 0; j < m_matrix; j++) {
            int index = i * m_matrix + j;
            TaskGrabInfo* grab = m_job->m_grabInfo.at(index);
            grab->col = j;
            grab->row = i;
            grab->index  = index;
            grab->matrix = m_matrix;
            m_job->m_grabInfo[index] = grab;
           
            QString imgUrl = QString("%1%2").arg(m_host).arg(grab->grabUrl);
            this->addImageDownload(m_job->getTaskId(), index, imgUrl, grab);
        }
    }
#endif
}

void QImageViewer::addImageDownload(qint64 taskId, int index, const QString& strUrl, TaskGrabInfo* grab)
{
    QDownloadImage *pack = new QDownloadImage(taskId, index, strUrl, "", true);
    QThreadPool::globalInstance()->start(pack);
    connect(pack, SIGNAL(onFinished(qint64, int, const QByteArray&)), this, SLOT(onFinished(qint64, int, const QByteArray&)));
}

void QImageViewer::onFinished(qint64 taskId, int index, const QByteArray& imageData)
{
    if (taskId != m_job->getTaskId()) return;
    m_finishedImages++;

    for (int i = 0; i < m_job->m_grabInfo.size(); i++) {
        TaskGrabInfo* grab = m_job->m_grabInfo.at(i);
        if (grab->index == index && !imageData.isEmpty()) {
            grab->imageData = imageData;
            m_job->m_grabInfo[i] = grab;
            break;
        }
    }

    if (m_job->m_grabInfo.size() == m_finishedImages) {
        this->showProcessImage();
    }
}

void QImageViewer::showProcessImage()
{
    if (m_job->m_grabInfo.isEmpty()) return;
    // return;
    QGridLayout* layout = (QGridLayout*)ui.widget_image->layout();
    // 隐藏weiget
    for (int i = 0; i < layout->count(); i++) {
        QLayoutItem *item = layout->itemAt(i);
        if (item != NULL) {
            QWidget *widget = item->widget();
            if (widget != NULL) {
                widget->hide();
            }
        }
    }

    for (int i = 0; i < m_job->m_grabInfo.size(); i++) {
        TaskGrabInfo *grab = m_job->m_grabInfo.at(i);
        int index = grab->row * grab->matrix + grab->col;
        ImageWidget* image = NULL;

        if (layout->itemAt(index)) {
            image = (ImageWidget*)layout->itemAt(index)->widget();
        } else {
            image = new ImageWidget(m_host, grab->row, grab->col, grab->matrix, ui.widget_image);
            // 添加到布局           
            layout->addWidget(image, grab->row, grab->col);
            connect(image, SIGNAL(mouseClicked()), this, SLOT(onMouseClicked()));
        }

        image->hideInfoWidget();
        image->cropImage(grab->imageData);
        image->show();
    }
    
    ui.widget_image->setLayout(layout);
}

void QImageViewer::loadPhotonImage()
{
    qDeleteAll(m_job->m_grabInfo);
    m_job->m_grabInfo.clear();
    m_finishedImages = 0;
    m_freshTimer.stop();
    HttpCmdManager::getInstance()->queryPhotonProgress(m_job->getTaskId(), this);
    //m_freshTimer.start(LocalConfig::getInstance()->detailInterval);
}

void QImageViewer::loadMajorImage()
{
    qDeleteAll(m_job->m_grabInfo);
    m_job->m_grabInfo.clear();
    m_finishedImages = 0;
    m_freshTimer.stop();
    HttpCmdManager::getInstance()->queryMajorProgress(m_job->getTaskId(), this);
    //m_freshTimer.start(LocalConfig::getInstance()->detailInterval);
}

void QImageViewer::onLoadProcessImage()
{
    m_freshTimer.stop();

    if (m_job->isPhotonJobProccessEnable()) { // 有光子任务，且光子任务正在渲染，则加载光子进度图
        this->loadPhotonImage();

    } else if (m_job->isMajorJobProccessEnable()) { // 主图正在渲染，加载主图的进度图
        this->loadMajorImage();

    } else if (m_job->getJobType() == UPLOAD_JOB) { // 上传任务
        // 创建一个假数据用于显示
        TaskGrabInfo *grab = new TaskGrabInfo;
        m_job->m_grabInfo.append(grab);
        this->showProcessImage();

    } else { // 其他状态 ，失败，停止，等待, 完成
        if (needRequestImageData()) {
            this->loadMajorImage();
        } else {
            this->showProcessImage();
        }
    }
}

void QImageViewer::onRecvResponse(QSharedPointer<ResponseHead> response)
{
    if (response == Q_NULLPTR)
        return;
    int code = response->code;
    if (code == HTTP_ERROR_SUCCESS) {
        if (response->cmdType == RESTAPI_JOB_TASK_PROCESS) {
            QSharedPointer<TaskProcessImgResponse> processImg = qSharedPointerCast<TaskProcessImgResponse>(response);
            this->loadProgressImage(processImg);
        }
    } else {
        // 创建一个假数据用于显示
        TaskGrabInfo *grab = new TaskGrabInfo;
        m_job->m_grabInfo.append(grab);
        this->showProcessImage();
    }
}

void QImageViewer::onMouseClicked()
{
    if (m_job->isCanShowDetailPage()) {
        CGlobalHelp::Response(UM_SHOW_PROCESS_VIEW, m_job->getTaskId(), 0);
        emit onItemClicked(m_job->getTaskId());
    }  
}

bool QImageViewer::needRequestImageData()
{
    if (m_job == NULL) return true;
    if (m_job->m_grabInfo.isEmpty()) return true;
    bool needRequest = false;

    // 统计图片为空的情况
    int hasEmpty = 0;
    foreach(TaskGrabInfo* grab, m_job->m_grabInfo) {
        if (grab->imageData.isEmpty()) {
            hasEmpty++;
        }
    }

    // 1. 渲染完成，渲染完成时间低于10分钟的，需要请求数据, 如果存在空数据，则需要请求
    // 2. 渲染停止的，结束时间低于5分钟的，需要重新请求
    switch (m_job->getStatus()) {
    case JobState::enRenderTaskFinished:
    case JobState::enRenderTaskFinishHasFailed:
    case JobState::enRenderTaskFinishTest: {
        qint64 finishedTime = m_job->getCompelteDate();
        qint64 currentTime = MyConfig.currentDateTime.toMSecsSinceEpoch();
        needRequest = (currentTime - finishedTime) < 10 * 60 * 1000;
        needRequest = needRequest || (hasEmpty > 0);
    }
        break;
    case JobState::enRenderTaskStop:
    case JobState::enRenderUserStop:
    case JobState::enRenderTaskArrearageStop:
    case JobState::enRenderOverTimeStop: {
        qint64 finishedTime = m_job->getCompelteDate();
        qint64 currentTime = MyConfig.currentDateTime.toMSecsSinceEpoch();
        needRequest = (currentTime - finishedTime) < 5 * 60 * 1000;
    }
        break;
    default:
        break;
    }

    return needRequest;
}

void QImageViewer::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QWidget::paintEvent(event);
}

void QImageViewer::resizeEvent(QResizeEvent *event)
{
    this->setMask(RBHelper::roundRect(this->rect(), 10, true, false, true, false));
    QWidget::resizeEvent(event);
}