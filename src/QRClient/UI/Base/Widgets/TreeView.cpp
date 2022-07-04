#include "stdafx.h"
#include "TreeView.h"

CTreeView::CTreeView(QWidget *parent)
    : QTreeView(parent)
    , m_proxyModel(Q_NULLPTR)
{
    m_busyIcon = new QProgressIndicator(this);
    m_busyIcon->setAttribute( Qt::WA_TransparentForMouseEvents );
    m_busyIcon->show();

    header()->setSortIndicatorShown(true);
    header()->setDefaultAlignment(Qt::AlignCenter);
    header()->setStretchLastSection(true);

    this->setTextElideMode(Qt::ElideMiddle);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setRootIsDecorated(false);
    this->setSortingEnabled(true);
    this->viewport()->setAttribute(Qt::WA_Hover);
    this->viewport()->setMouseTracking(true);

    this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
    this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
    this->setUniformRowHeights(true);

    this->viewport()->setAcceptDrops(true);
    this->setDragEnabled(true);
    this->setDragDropMode(QAbstractItemView::InternalMove);
    this->setAcceptDrops(true);
}

CTreeView::~CTreeView()
{
}

void CTreeView::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    if(this->model()->rowCount() == 0) {
        QTreeView::paintEvent(e);
        QPainter painter(viewport());
        QRect rect = painter.viewport();

        int gx = 0;
        int gy = 0;

        if(!m_emptyHint.isEmpty()) {
            //如果是文件
            QFileInfo info(m_emptyHint);
            if(info.isFile()) {
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

    QTreeView::paintEvent(e);
}

void CTreeView::mousePressEvent(QMouseEvent *e)
{
    // this->update();
    QTreeView::mousePressEvent(e);
}

void CTreeView::mouseReleaseEvent(QMouseEvent *e)
{
    // this->update();
    QTreeView::mouseReleaseEvent(e);
}

void CTreeView::showEvent(QShowEvent *e)
{
    QTreeView::showEvent(e);
}

void CTreeView::resizeEvent(QResizeEvent *e)
{
    QTreeView::resizeEvent(e);

    int sx = (this->width() - m_busyIcon->width()) / 2;
    int sy = (this->height() - m_busyIcon->height()) / 2;
    m_busyIcon->move(sx, sy);
}

void CTreeView::keyPressEvent(QKeyEvent* e)
{
    if(e->type() == QKeyEvent::KeyPress) {
        if(e->matches(QKeySequence::Copy)) {
            QApplication::clipboard()->setText(this->currentIndex().data().toString());
        }
    }
    QTreeView::keyPressEvent(e);
}

void CTreeView::enterEvent(QEvent *e)
{
    this->verticalScrollBar()->show();
    this->horizontalScrollBar()->show();
}

void CTreeView::leaveEvent(QEvent *e)
{
    this->verticalScrollBar()->hide();
    this->horizontalScrollBar()->hide();
}

void CTreeView::showLoading(bool bShow)
{
    if(bShow) {
        m_busyIcon->show();
        m_busyIcon->startAnimation();
    } else {
        m_busyIcon->stopAnimation();
        m_busyIcon->hide();
    }
}

void CTreeView::setEmptyHint(const QString& hint)
{
    m_emptyHint = hint;
}

int CTreeView::getSelectedCount()
{
    QModelIndexList indexList = selectionModel()->selectedRows();
    return indexList.size();
}

void CTreeView::setFilterRegExp(const QString & pattern)
{
    if(m_proxyModel)
        m_proxyModel->setFilterRegExp(pattern);
}

QRect CTreeView::headerSectionRect(int logicIndex)
{
    QRect rct = header()->frameRect();
    QRect colRect;

    int x = rct.x() + header()->sectionPosition(logicIndex);
    colRect.setX(x);
    colRect.setY(rct.y());
    colRect.setWidth(header()->sectionSize(logicIndex));
    colRect.setHeight(rct.height());

    return colRect;
}

int CTreeView::getScrollBarCurValue()
{
    QScrollBar* scroll = this->horizontalScrollBar();
    if(scroll == nullptr)
        return 0;
    return scroll->value();
}