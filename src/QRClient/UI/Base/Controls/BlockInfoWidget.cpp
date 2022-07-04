#include "stdafx.h"
#include "BlockInfoWidget.h"

BlockInfoWidget::BlockInfoWidget(QWidget *parent)
    : QWidget(parent)
    , m_widgeItem(NULL)
{
    ui.setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    connect(ui.widget, SIGNAL(clicked()), this, SLOT(onWidgetClicked()));
    connect(ui.widget_2, SIGNAL(clicked()), this, SLOT(onWidgetClicked()));

    m_delete = new QPushButton(this);
    m_delete->setStyleSheet("QPushButton{border-image:url(:/images/delete.png);border:"
        "none;background-color:transparent;max-height:15px;max-width:15px;padding:0px;}"
        "QPushButton:hover{border-image:url(:/images/delete.png);} ");
    m_delete->hide();

    connect(m_delete, SIGNAL(clicked()), this, SLOT(onDeleteBtnClicked()));
}

BlockInfoWidget::~BlockInfoWidget()
{
}

void BlockInfoWidget::setBlockIcon(const QString& icon)
{
    ui.label_icon->setPixmap(QPixmap(icon));
}

void BlockInfoWidget::setBlockName(const QString& name)
{
    ui.label_filename->setText(name);
}

void BlockInfoWidget::setBlockFilePath(const QString& path)
{
    m_filePath = path;
    this->setToolTip(path);
}

void BlockInfoWidget::onWidgetClicked()
{
    QFileInfo info(m_filePath);
    QDesktopServices::openUrl(QUrl::fromLocalFile(info.absoluteDir().absolutePath()));
}

void BlockInfoWidget::onDeleteBtnClicked()
{
    emit deleteItem(m_widgeItem);
}

void BlockInfoWidget::enterEvent(QEvent *e)
{
    QPoint pos = m_delete->pos();
    pos.setX(this->width() - m_delete->width());
    m_delete->move(pos);
    m_delete->show();
    QWidget::enterEvent(e);
}

void BlockInfoWidget::leaveEvent(QEvent *e)
{
    m_delete->hide();
    QWidget::leaveEvent(e);
}

void BlockInfoWidget::paintEvent(QPaintEvent * ev)
{
    QWidget::paintEvent(ev);
}
