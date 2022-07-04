/***********************************************************************
* Module:  HeaderView.h
* Author:  hqr
* Modified: 2021/06/11 14:42:34
* Purpose: Declaration of the class
***********************************************************************/
#pragma once

#include <QHeaderView>

class CameraHeaderView : public QHeaderView
{
    enum COLUMN {
        COLUMN_RENDER_SENSOR_SIZE = 4,
        COLUMN_RENDER_FOCAL_LENGTH = 5
    };
public:
    CameraHeaderView(Qt::Orientation orientation, QWidget *parent = Q_NULLPTR) : QHeaderView(orientation, parent) {};
    ~CameraHeaderView(void) {};

protected:
    virtual void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;
};

#include "CheckedHeaderView.h"
class DetailHeaderView : public CheckedHeaderView
{
    Q_OBJECT

    enum COLUMN {
        COLUMN_RENDER_STATUS = 1
    };
public:
    DetailHeaderView(Qt::Orientation orientation, QWidget *parent) : CheckedHeaderView(orientation, parent) {};
    ~DetailHeaderView() {};

protected:
    virtual void paintSection(QPainter * painter, const QRect & rect, int logicalIndex) const;
    virtual void mousePressEvent(QMouseEvent *event);

signals:
    void showFilter(int logicalIndex);

private:
    mutable QRect m_rect;
};