#include "stdafx.h"
#include "ResultsShareWindow.h" 
// #include "qzxing/QZXing.h"
#include "HttpCommand/HttpCmdManager.h"
#include "Common/LangTranslator.h"
#include "qrencode/qrencode.h"
#include <QClipboard>
#include <QPixmap>

ResultsShareWindow::ResultsShareWindow(qint64 taskId, QWidget *parent)
    : Dialog(parent)
    , m_taskId(taskId)
{
    ui.setupUi(this);  

    m_busyIcon = new QProgressIndicator(ui.widgetMain);
    m_busyIcon->setAttribute(Qt::WA_TransparentForMouseEvents);

    ui.widget_2->setStyleSheet("background-color:#ffffff");

    initUi();
}

ResultsShareWindow::~ResultsShareWindow()
{
}

void ResultsShareWindow::initUi()
{
    // 隐藏部分控件
    ui.label_copyLink_tips->hide();
#ifdef FOXRENDERFARM
	ui.widget_right->hide();
	ui.btnCopyCode->setFixedWidth(180);
#endif // 

	ui.label_logo->setPixmap(QPixmap(LOGO_ICON));
    // 请求链接
    HttpCmdManager::getInstance()->queryShareResultLink(m_taskId, 7, this);
    showLoading(true);
}

void ResultsShareWindow::showLoading(bool show)
{
    ui.widgetMain->setEnabled(!show);

    if (show) {
        int sx = (ui.widgetMain->width() - m_busyIcon->width()) / 2;
        int sy = (ui.widgetMain->height() - m_busyIcon->height()) / 2;
        m_busyIcon->move(sx, sy);

        m_busyIcon->show();
        m_busyIcon->startAnimation();

    } else {
        m_busyIcon->hide();
        m_busyIcon->stopAnimation();
    }
}

void ResultsShareWindow::on_btnCopyCode_clicked()
{
    QString link = ui.lineEdit_link->text();
    QString code = ui.lineEdit_code->text();
#ifdef FOXRENDERFARM
	QString copyStr = QObject::tr("Link：%1 \r\nPassword：%2\r\n--Share from AvicaCloud users").arg(link).arg(code);
#else
	QString copyStr = QObject::tr("链接：%1 \r\n提取码：%2\r\n--来自大雁云用户的分享").arg(link).arg(code);
#endif    

    QClipboard *copy = QGuiApplication::clipboard();
    if (copy != nullptr) {
        copy->setText(copyStr);
    }

    new ToastWidget(ui.widgetTitle, QObject::tr("复制链接成功"), QColor("#6B6CFF"));

    // ui.label_copyLink_tips->show();
    // QTimer::singleShot(3000, ui.label_copyLink_tips, SLOT(hide()));
}

void ResultsShareWindow::on_btnCopyQRCode_clicked()
{
    const QPixmap* pix = ui.label_QR_image->pixmap();

    QClipboard *copy = QGuiApplication::clipboard();
    if (copy != nullptr && pix != nullptr) {
        copy->setImage(pix->toImage());
        new ToastWidget(ui.widgetTitle, QObject::tr("复制二维码成功"), QColor("#6B6CFF"));
    }
}

void ResultsShareWindow::onRecvResponse(QSharedPointer<ResponseHead> data)
{
    if (data->code == HTTP_ERROR_SUCCESS) {
        QSharedPointer<ShareResultLinkResponse> shreLinks = qSharedPointerCast<ShareResultLinkResponse>(data);
        if (m_taskId == shreLinks->taskId) {
            fillShareLinkContent(shreLinks->shareUrl, shreLinks->shareCode);
        }        
    }
    else if (data->code == HTTP_ERROR_DO_NOT_HAVE_ANY_MORE_RECORD) {
        new ToastWidget(ui.widgetTitle, QObject::tr("没有更多记录"), QColor("#e25c59"));
    } else if (data->code == HTTP_ERROR_TASK_NOT_EXIST) {
        new ToastWidget(ui.widgetTitle, QObject::tr("任务不存在"), QColor("#e25c59"));
    } else {
        QString message = "";
        QString errorText = LangTranslator::RequstErrorI18n(data->msg);
        if (!errorText.isEmpty())
            message = errorText;
        else
            message = QObject::tr("获取成果分享链接失败 (错误码 %1)").arg(data->code);

        new ToastWidget(ui.widgetTitle, message, QColor("#e25c59"));
    }
    showLoading(false);
}

void ResultsShareWindow::fillShareLinkContent(QString shareUrl, QString code)
{
    QString link = QString("%1%2").arg(WSConfig::getInstance()->get_host_url()).arg(shareUrl);
    ui.lineEdit_link->setText(link);
    ui.lineEdit_code->setText(code);

    QPixmap shareMap = makeShareQRCode(shareUrl, code);
    ui.label_QR_image->setPixmap(shareMap);
}

QPixmap ResultsShareWindow::makeShareQRCode(QString shareUrl, QString code)
{
    QString data = QString("%1%2&code=%3").arg(WSConfig::getInstance()->get_host_url()).arg(shareUrl).arg(code);
    QImage barcode(QSize(ui.label_QR_image->width(), ui.label_QR_image->height()), QImage::Format_RGBA8888);
    barcode.fill(QColor(255, 255, 255, 255));

    QRcode* qrcode = QRcode_encodeString(qPrintable(data), 1, QR_ECLEVEL_M, QR_MODE_8, 1);
    if (qrcode == nullptr) {
        return QPixmap();
    }
    
    int width = ui.label_QR_image->width();
    int height = ui.label_QR_image->height();
    QPainter painter(&barcode);
    QColor foreground(Qt::black);
    painter.setBrush(foreground);
    const int qr_width = qrcode->width > 0 ? qrcode->width : 1;
    double scale_x = (double)width / (double)qr_width;
    double scale_y = (double)height / (double)qr_width;
    for (int y = 0; y < qr_width; y++)
    {
        for (int x = 0; x < qr_width; x++)
        {
            unsigned char b = qrcode->data[y * qr_width + x];
            if (b & 0x01)
            {
                QRectF r(int(x * scale_x), int(y * scale_y), int(scale_x), int(scale_y));
                painter.drawRect(r);
            }
        }
    }
    
    

#ifdef FOXRENDERFARM
	QPixmap picture(":/images/qr_avica_main.png");
#else
	QPixmap picture(":/images/qr_main.png");
#endif

    int logoWidth = width * 0.25;
    int logoHeight = height * 0.25;
    painter.drawPixmap(width / 2 - logoWidth / 2, height / 2 - logoHeight / 2, logoWidth, logoHeight, picture);

    return QPixmap::fromImage(barcode);
}