#include "stdafx.h"
#include "HeaderView.h"

void CameraHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    if(nullptr == painter)
        return;

    painter->save();
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();

    // ¸ÐÓ¦Æ÷³ß´ç¡¢½¹¾à
    if(COLUMN_RENDER_SENSOR_SIZE == logicalIndex || COLUMN_RENDER_FOCAL_LENGTH == logicalIndex) {
        QPixmap pixmap(":/view/images/view/edit_normal.png");
        int x = rect.right() - pixmap.width();
        int y = rect.top() + (rect.height() - pixmap.height()) / 2;

        painter->save();
        painter->drawPixmap(QPoint(x, y), pixmap);
        painter->restore();
    }
}

void DetailHeaderView::paintSection(QPainter * painter, const QRect & rect, int logicalIndex) const
{
    if(nullptr == painter)
        return;

    painter->save();
    CheckedHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();

    if(COLUMN_RENDER_STATUS == logicalIndex) {
        QPixmap pixmap(":/images/down_normal.png");
        int x = rect.right() - pixmap.width();
        int y = rect.top() + (rect.height() - pixmap.height()) / 2;

        painter->save();
        painter->drawPixmap(QPoint(x, y), pixmap);
        painter->restore();
    }
}

void DetailHeaderView::mousePressEvent(QMouseEvent *event)
{
    int pressedIndex = this->visualIndexAt(event->pos().x());
    if(COLUMN_RENDER_STATUS == pressedIndex) {
        emit showFilter(pressedIndex);
        return;
    }

    CheckedHeaderView::mousePressEvent(event);
}