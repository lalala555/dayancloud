#include "stdafx.h"
#include "TreeWidget.h"

TreeWidget::TreeWidget(QWidget *parent)
    : QTreeWidget(parent)
{
    m_busyIcon = new QProgressIndicator(this);
    m_busyIcon->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_busyIcon->show();
}

TreeWidget::~TreeWidget()
{
}

void TreeWidget::showLoading(bool bShow)
{
    if (bShow) {
        m_busyIcon->show();
        m_busyIcon->startAnimation();
    } else {
        m_busyIcon->stopAnimation();
        m_busyIcon->hide();
    }
}

void TreeWidget::setEmptyHint(const QString& hint)
{
    m_emptyHint = hint;
}

void TreeWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    if (this->model()->rowCount() == 0) {
        QTreeWidget::paintEvent(e);
        QPainter painter(viewport());
        QRect rect = painter.viewport();

        int gx = 0;
        int gy = 0;

        if (!m_emptyHint.isEmpty()) {
            //如果是文件
            QFileInfo info(m_emptyHint);
            if (info.isFile()) {
                QPixmap pix(m_emptyHint);

                gx = (rect.width() - pix.width()) / 2;
                gy = (rect.height() - pix.height() - 50) / 2;
                painter.drawPixmap(gx, gy, pix.width(), pix.height(), pix);
                return;
            } else {
                QFont font;
                font.setFamily("Microsoft YaHei");
                font.setBold(true);
                font.setPointSize(10);
                QFontMetrics fm(font);
                QRect fontRect = fm.boundingRect(m_emptyHint);
                painter.setFont(font);

                gx = (rect.width() - fontRect.width()) / 2;
                gy = (rect.height() - fontRect.height() - 50) / 2;

                // 如果文字过长则要将文字切断
                if (fontRect.width() > rect.width() - 20) {
                    QTextOption option(Qt::AlignHCenter | Qt::AlignVCenter);
                    option.setWrapMode(QTextOption::WordWrap);

                    painter.drawText(rect, m_emptyHint, option);
                } else {
                    painter.drawText(gx, gy, fontRect.width(), fontRect.height(), Qt::AlignCenter, m_emptyHint);
                }
                

                return;
            }
        }
    }

    QTreeWidget::paintEvent(e);
}

void TreeWidget::mousePressEvent(QMouseEvent *e)
{
    QTreeWidget::mousePressEvent(e);
}

void TreeWidget::mouseReleaseEvent(QMouseEvent *e)
{
    QTreeWidget::mouseReleaseEvent(e);
}

void TreeWidget::showEvent(QShowEvent *e)
{
    QTreeWidget::showEvent(e);
}

void TreeWidget::resizeEvent(QResizeEvent *e)
{
    QTreeWidget::resizeEvent(e);

    int sx = (this->width() - m_busyIcon->width()) / 2;
    int sy = (this->height() - m_busyIcon->height()) / 2;
    m_busyIcon->move(sx, sy);
}

void TreeWidget::keyPressEvent(QKeyEvent* e)
{
    QTreeWidget::keyPressEvent(e);
}

void TreeWidget::enterEvent(QEvent *e)
{
    this->verticalScrollBar()->show();
    this->horizontalScrollBar()->show();
}

void TreeWidget::leaveEvent(QEvent *e)
{
    this->verticalScrollBar()->hide();
    this->horizontalScrollBar()->hide();
}