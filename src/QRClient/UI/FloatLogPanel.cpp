#include "stdafx.h"
#include "FloatLogPanel.h"
#include "ui_FloatLogPanel.h"

FloatLogPanel::FloatLogPanel(QWidget *parent)
    : LayerWidget(parent)
    , ui(new Ui::FloatLogPanel())
{
    ui->setupUi(this);

    this->setVisible(false);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAutoFillBackground(true);
    QPalette palette;
    QColor color(56, 56, 56, 230);
    palette.setBrush(backgroundRole(), color);
    this->setPalette(palette);

    m_width = this->width();
    m_height = this->height();

    this->updateWidgetGeometry(parent);
    QObject::connect(ui->btnClose, SIGNAL(clicked()), this, SIGNAL(emitCloseBtnClicked()));
}

FloatLogPanel::~FloatLogPanel()
{
    delete ui;
}

void FloatLogPanel::appendLog(const QString& text, int page)
{
    if (page == PAGE_UPLOAD) {
        int transLog_lines = ui->logEdit_upload->document()->lineCount();
        if (transLog_lines > 500)
            ui->logEdit_upload->clear();

        QString s = QString("<html>[%1]%2</html>").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")).arg(text);

        ui->logEdit_upload->append(s);
        ui->logEdit_upload->ensureCursorVisible();

    } else if (page == PAGE_DOWNLOAD) {
        int transLog_lines = ui->logEdit_download->document()->lineCount();
        if (transLog_lines > 500)
            ui->logEdit_download->clear();

        QString s = QString("<html>[%1]%2</html>").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")).arg(text);

        ui->logEdit_download->append(s);
        ui->logEdit_download->ensureCursorVisible();
    }
}

bool FloatLogPanel::eventFilter(QObject* obj, QEvent* event)
{
    if(event->type() == QEvent::Resize) {
        updateWidgetGeometry(obj);
        return false;
    } else {
        return LayerWidget::eventFilter(obj, event);
    }
}

void FloatLogPanel::updateWidgetGeometry(QObject* obj)
{
    if(!obj)
        return;

    QWidget *parentWidget = static_cast<QWidget*>(obj);
    int x = parentWidget->x();
    int y = parentWidget->y() + parentWidget->height() - m_height - 50;

    int w_bar = 0;
    int h_bar = 0;

    this->setFixedSize(parentWidget->width(), m_height);
    this->move(x, y);

    this->updateGeometry();
}

void FloatLogPanel::changeEvent(QEvent *event)
{
    switch(event->type()) {
    case QEvent::LanguageChange: {
        ui->retranslateUi(this);
    }
    break;
    default:
        QWidget::changeEvent(event);
        break;
    }
}

void FloatLogPanel::resizeEvent(QResizeEvent *event)
{
    LayerWidget::resizeEvent(event);
    updateWidgetGeometry(parentWidget());
}

void FloatLogPanel::showEvent(QShowEvent* event)
{
    LayerWidget::showEvent(event);
    updateWidgetGeometry(parentWidget());
}

void FloatLogPanel::paintEvent(QPaintEvent *p)
{
    QStylePainter painter(this);
    //用style画背景 (会使用setstylesheet中的内容)
    QStyleOption opt;
    opt.initFrom(this);
    opt.rect = rect();
    painter.drawPrimitive(QStyle::PE_Widget, opt);

    LayerWidget::paintEvent(p);
}
