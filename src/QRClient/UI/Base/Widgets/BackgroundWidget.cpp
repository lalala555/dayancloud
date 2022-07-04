#include "stdafx.h"
#include "BackgroundWidget.h"

BackgroundWidget::BackgroundWidget(QWidget *parent)
    : QWidget(parent)
    , m_progress(0.0)
    , m_opacity(0.2)
{
    m_backColor.setRgba(qRgba(31, 31, 39, 30));
    this->setWindowOpacity(1.0);

    this->setAutoFillBackground(true);
}

BackgroundWidget::~BackgroundWidget()
{

}

void BackgroundWidget::setBackColor(const QColor& color, double opacity)
{
    m_backColor = color;
    m_opacity = opacity;
    update();
}

void BackgroundWidget::setProgress(float progress)
{
    m_progress = progress;
}

void BackgroundWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    int prog = m_progress * width();
    if(prog > width()) prog = width();
    QRectF outRect(0, 0, prog, height());
    p.setOpacity(m_opacity);
    p.setPen(Qt::NoPen);
    p.setBrush(QBrush(m_backColor));
    p.drawRect(outRect);

    QPalette palette = this->palette();  
    palette.setColor(QPalette::Background, QColor(51,51,51,50)); // 最后一项为透明度  
    //palette.setBrush(QPalette::Background, QBrush(QPixmap(":/background.png")));  
    this->setPalette(palette);  
}