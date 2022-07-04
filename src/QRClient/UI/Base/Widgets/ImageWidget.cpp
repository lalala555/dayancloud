#include "stdafx.h"
#include "ImageWidget.h"

ImageWidget::ImageWidget(QString host, int row, int col, int matrix, QWidget *parent)
    : QWidget(parent)
    , m_matrix(matrix)
    , m_col(col)
    , m_row(row)
    , m_movie(NULL)
    , m_host(host)
{
    ui.setupUi(this);
    this->setAutoFillBackground(true);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    QPixmap pix(":/guide/images/guide/background.png");
    this->onUpdateImage(pix);
    this->setStyleSheet("QLabel{color:#999999;}");
    connect(this, SIGNAL(updateImage(const QPixmap&)), this, SLOT(onUpdateImage(const QPixmap&)));
    connect(&m_download, SIGNAL(finished(QNetworkReply*)), this, SLOT(onDownloadImageFinished(QNetworkReply*)));

    ui.label_limit->hide();
}

ImageWidget::~ImageWidget()
{

}

void ImageWidget::setGrabInfo(TaskGrabInfo* grab)
{
    m_grab.col = grab->col;
    m_grab.couponFee = grab->couponFee;
    m_grab.endTime = grab->endTime;
    m_grab.feeAmount = grab->feeAmount;
    m_grab.frameBlock = grab->frameBlock;
    m_grab.frameEst = grab->frameEst;
    m_grab.frameIndex = grab->frameIndex;
    m_grab.framePercent = grab->framePercent;
    m_grab.frameStatus = grab->frameStatus;
    m_grab.frameUsed = grab->frameUsed;
    m_grab.grabUrl = grab->grabUrl;
    m_grab.index = grab->index;
    m_grab.isMaxPrice = grab->isMaxPrice;
    m_grab.renderInfo = grab->renderInfo;
    m_grab.renderType = grab->renderType;
    m_grab.row = grab->row;
    m_grab.startTime = grab->startTime;

    this->initGrabInfo();
}

void ImageWidget::downloadImage()
{
    QNetworkRequest request;

    QSslConfiguration config;
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(config);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
    QString url = QString("%1%2").arg(m_host).arg(m_grab.grabUrl);
    url = RBHelper::encodingUrl(url);
    request.setUrl(QUrl(url));

    m_download.clearAccessCache();
    m_download.get(request);
}

void ImageWidget::onDownloadImageFinished(QNetworkReply* reply)
{
    reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    QByteArray data = "empty response";

    QNetworkReply::NetworkError errorCode = reply->error();
    if(errorCode == QNetworkReply::NoError) {
        data = reply->readAll();
        QString content = QString(data);
        if(content == "null" || content.isEmpty()) {
            data = "empty response";
        }
    } else {
        QString errorMsg = reply->errorString();
        qDebug() << errorMsg;
        data = reply->readAll();
    }

    //QtConcurrent::run(this, &ImageWidget::cropImage, data);
    this->cropImage(data);
    reply->abort();
    reply->close();
    reply->deleteLater();
}

void ImageWidget::cropImage(const QByteArray& data)
{
    QPixmap *pix = new QPixmap(":/guide/images/guide/background.png");
    bool loadok = pix->loadFromData(data, "jpg");
    if(data.isEmpty() || !loadok){
        emit updateImage(QPixmap(":/images/preview.png"));
        //qDebug() << data;
        return;
    }

    int imgWidth = (pix->width() + 1) / m_matrix;
    int imgHeight = (pix->height() + 1) / m_matrix;

    QPixmap pixmap(*pix);
    pixmap = pix->copy(m_col * imgWidth, m_row * imgHeight, imgWidth, imgHeight);

    emit updateImage(pixmap);
    delete pix;
    pix = NULL;
}

void ImageWidget::onUpdateImage(const QPixmap& pix)
{
    this->m_pixmap = pix.scaled(this->size(), Qt::KeepAspectRatio);
    update();
}

void ImageWidget::paintEvent(QPaintEvent *p)
{
    Q_UNUSED(p);
    QPainter painter(this);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.setRenderHints(QPainter::SmoothPixmapTransform);
    painter.setRenderHint(QPainter::Antialiasing, true);//	边缘平滑..
    painter.drawPixmap(this->rect(), m_pixmap);

    QStylePainter style(this);
    //用style画背景 (会使用setstylesheet中的内容)
    QStyleOption opt;
    opt.initFrom(this);
    opt.rect=rect();
    style.drawPrimitive(QStyle::PE_Widget, opt);

    QWidget::paintEvent(p);
}

void ImageWidget::enterEvent(QEvent *)
{
    this->setStyleSheet("QLabel{color:#eeeeee;}");
    ui.widget_bg->setBackColor(qRgba(200, 200, 200, 80), 0.5);
    update();
}

void ImageWidget::leaveEvent(QEvent *)
{
    this->setStyleSheet("QLabel{color:#333333;}");
    ui.widget_bg->setBackColor(qRgba(31, 31, 39, 30));
    update();
}

void ImageWidget::initGrabInfo()
{
    if(m_grab.isMaxPrice.toInt() == 1){
        ui.label_limit->show();
    }
    if(m_grab.frameStatus.contains("等待")){
        if(m_movie == NULL){
            m_movie = new QMovie(":/images/waiting.gif");
            ui.label_loading->setMovie(m_movie);
            m_movie->start();
        }      
    }else{
        if(m_movie!=NULL)
            m_movie->stop();
        ui.label_loading->hide();
    }
    ui.label_state->setText(m_grab.frameStatus);
    ui.label_info->setText(m_grab.renderInfo);
    
    QString progress;
    if(m_grab.frameStatus.toInt() == 4){
        progress = QString("￥%1 C%2").arg(m_grab.feeAmount).arg(m_grab.couponFee);
    }else{
        progress = QString("%1/%2").arg(m_grab.startTime).arg(m_grab.endTime);
    }
    ui.label_progress->setText(progress);
    ui.widget_bg->setProgress(m_grab.framePercent.toFloat());
}

void ImageWidget::hideInfoWidget()
{
    ui.widget_bg->hide();
    ui.widget_limit->hide();
    ui.widget_loading->hide();
}

void ImageWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit mouseClicked();
    }
    // QWidget::mousePressEvent(event);
}