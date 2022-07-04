#include "stdafx.h"
#include "CustListView.h"

CustListView::CustListView(QWidget *parent)
    : QListView(parent)
{
    m_busyIcon = new QProgressIndicator(this);
    m_busyIcon->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_busyIcon->show();
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
}

CustListView::~CustListView()
{
}

void CustListView::showLoading(bool bShow)
{
    if (bShow) {
        m_busyIcon->show();
        m_busyIcon->startAnimation();
    } else {
        m_busyIcon->stopAnimation();
        m_busyIcon->hide();
    }
}

void CustListView::setEmptyHint(const QString& hint)
{
    m_emptyHint = hint;
}

/*void CustListView::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    if (this->model()->rowCount() == 0) {
        QListView::paintEvent(e);
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
                font.setPointSize(15);
                QFontMetrics fm(font);
                QRect fontRect = fm.boundingRect(m_emptyHint);
                painter.setFont(font);

                gx = (rect.width() - fontRect.width()) / 2;
                gy = (rect.height() - fontRect.height() - 50) / 2;
                painter.drawText(gx, gy, fontRect.width(), fontRect.height(), Qt::AlignCenter, m_emptyHint);

                return;
            }
        }
    }

    QListView::paintEvent(e);
}*/

void CustListView::resizeEvent(QResizeEvent *e)
{
    QListView::resizeEvent(e);

    int sx = (this->width() - m_busyIcon->width()) / 2;
    int sy = (this->height() - m_busyIcon->height()) / 2;
    m_busyIcon->move(sx, sy);
}
