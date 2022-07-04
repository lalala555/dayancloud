#include "stdafx.h"
#include "SurveyImageViewer.h"

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QWheelEvent>
#include <QDebug>

#include "ControlPointItem.h"
#include "PixmapItem.h"

SurveyImageViewer::SurveyImageViewer(QWidget *parent)
{
	//setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	//setDragMode(QGraphicsView::ScrollHandDrag);

	m_scenebackground = new QGraphicsScene(this);
	setStyleSheet("QGraphicsView{border:none;background-color:transparent;}");
	setScene(m_scenebackground);

	m_item = std::make_unique<PixmapItem>();
	scene()->addItem(m_item.get());
	m_item->initItem();
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	connect(m_item.get(), SIGNAL(stabPoints(const QPointF &)), this, SIGNAL(stabPoints(const QPointF &)));

}


SurveyImageViewer::~SurveyImageViewer()
{
}

void SurveyImageViewer::showImage(const QImage & img, bool scroll_center)
{
	setSceneRect(img.rect());
	m_item->setPixmap(QPixmap::fromImage(img));
	m_item->setPos(0, 0);

	double scale = height()*1.0 / img.height();
	m_item->setScaleNum(scale);

	verticalScrollBar()->setValue(verticalScrollBar()->minimum());
	horizontalScrollBar()->setValue(horizontalScrollBar()->minimum());

	//if (scroll_center)
	//{
	//	verticalScrollBar()->setValue(verticalScrollBar()->maximum() / 2);
	//	horizontalScrollBar()->setValue(horizontalScrollBar()->maximum() / 2);
	//}
}

void SurveyImageViewer::setControlPoints(const std::vector<ControlPointInfo> & points)
{
	m_item->setControlPoints(points);
}

void SurveyImageViewer::setLines(const std::vector<std::vector<QPoint>> & lines)
{
	m_item->setLines(lines);
}

void SurveyImageViewer::setCurrentStabPoint(const QPointF &p)
{
	m_item->setCurrentStabPoint(p);
}

void SurveyImageViewer::PixmapItemDataClear()
{
	if (m_item) 
	{
		m_item.reset();
		m_item = std::make_unique<PixmapItem>();
		scene()->addItem(m_item.get());
		m_item->initItem();
		setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

		connect(m_item.get(), SIGNAL(stabPoints(const QPointF &)), this, SIGNAL(stabPoints(const QPointF &)));
	}

}
