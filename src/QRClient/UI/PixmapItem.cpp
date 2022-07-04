#include "stdafx.h"
#include "PixmapItem.h"

#include <QDebug>

#include <Windows.h>

#define KEYDOWN(vk_code) ((GetKeyState(vk_code) & 0x8000) ? true : false)
#define KEYUP(vk_code) ((GetKeyState(vk_code) & 0x8000) ? false : true)

Q_DECLARE_METATYPE(QPainterPath)

PixmapItem::PixmapItem(QGraphicsItem *parent /*= nullptr*/)
	: QGraphicsPixmapItem(parent)
{
	this->setFlags(flags() | ItemIsMovable | ItemIsFocusable | ItemIsSelectable);
	setAcceptedMouseButtons(Qt::LeftButton);
	m_controlPoint = new ControlPointItem();
	m_controlPoint->hide();

	connect(m_controlPoint, &ControlPointItem::posChanged, this, [this](const QPointF &pos) {
		QPointF p = mapFromScene(pos);
		m_controlPoint->saveImagePos(p.toPoint());
		emit stabPoints(p);
	});
}

PixmapItem::~PixmapItem()
{
}

void PixmapItem::initItem()
{
	scene()->addItem(m_controlPoint);	
}

void PixmapItem::setLines(const std::vector<std::vector<QPoint>> & lines)
{
	for (auto line : m_lineItems)
		delete line;
	m_lineItems.clear();

	QPen pen;
	pen.setColor(Qt::yellow);
	pen.setWidth(2);

	//for (auto & line : lines)
	//{
	//	auto p1 = mapToScene(line.p1());
	//	auto p2 = mapToScene(line.p2());
	//	m_lineItems.push_back(new QGraphicsLineItem(p1.x(), p1.y(), p2.x(), p2.y()));
	//	m_lineItems.back()->setData(Qt::UserRole, line);
	//	m_lineItems.back()->setPen(pen);
	//	scene()->addItem(m_lineItems.back());
	//}

	for (auto &line : lines)
	{
		if (line.size() < 2)
			continue;
		QPainterPath path(line[0]);
		for (int i = 1; i < line.size(); ++i)
		{
			path.lineTo(line[i]);
		}
		
		auto item = new QGraphicsPathItem(this->mapToScene(path));
		item->setPen(pen);
		item->setFlag(QGraphicsItem::ItemClipsToShape);
		QVariant path_vardata;
		path_vardata.setValue<QPainterPath>(path);
		item->setData(Qt::UserRole, path_vardata);
		scene()->addItem(item);
		m_lineItems.push_back(item);
	}
}

void PixmapItem::setControlPoints(const std::vector<ControlPointInfo> & points)
{
	for (auto item : m_complatePoint)
		delete item;
	m_complatePoint.clear();

	for (auto & p : points)
	{
		auto cpitem = new ControlPointItem();
		cpitem->saveImagePos(p.p);
		cpitem->setName(p.name);
		cpitem->setPos(mapToScene(p.p));
		cpitem->setEdit(false);

		m_complatePoint.push_back(cpitem);
		scene()->addItem(cpitem);
	}
}

void PixmapItem::setCurrentStabPoint(const QPointF &p)
{
	if (p.isNull())
	{
		m_controlPoint->hide();
	}
	else
	{
		m_controlPoint->setPos(mapToScene(p));
		m_controlPoint->saveImagePos(p.toPoint());
	}

}

void PixmapItem::setScaleNum(double scale_num, QPointF p)
{
	QPointF preScenePos = this->mapToScene(p);

	this->setScale(scale_num);

	QPointF offset = mapToScene(p) - preScenePos;
	this->moveBy(-offset.x(), -offset.y());

	m_controlPoint->setPos(mapToScene(m_controlPoint->getImagePos()));
	for (auto & line : m_lineItems)
	{
		QPainterPath path = line->data(Qt::UserRole).value<QPainterPath>();
		line->setPath(mapToScene(path));
	}
	for (auto item : m_complatePoint)
	{
		item->setPos(mapToScene(item->getImagePos()));
	}

}

//void PixmapItem::keyPressEvent(QKeyEvent *event)
//{
//	if (event->key() == Qt::Key_Shift)
//		m_pressShift = true;
//}
//
//void PixmapItem::keyReleaseEvent(QKeyEvent *event)
//{
//	if (event->key() == Qt::Key_Shift)
//		m_pressShift = false;
//}

void PixmapItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		pressScenePos = event->scenePos();
	}
	QGraphicsPixmapItem::mousePressEvent(event);
}

void PixmapItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (KEYDOWN(VK_SHIFT))
		{
			m_controlPoint->setEdit(true);
			m_controlPoint->setPos(event->scenePos());
			m_controlPoint->saveImagePos(event->pos().toPoint());
			m_controlPoint->show();
			emit stabPoints(event->pos());//选中的图片上的坐标
		}
	}
	QGraphicsPixmapItem::mouseReleaseEvent(event);
}

void PixmapItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		QPointF offset = event->scenePos() - pressScenePos;
		pressScenePos = event->scenePos();
		this->moveBy(offset.x(), offset.y());
		m_controlPoint->setPos(mapToScene(m_controlPoint->getImagePos()));

		for (auto & line : m_lineItems)
		{
			QPainterPath path = line->data(Qt::UserRole).value<QPainterPath>();
			line->setPath(this->mapToScene(path));
		}
		for (auto item : m_complatePoint)
		{
			item->setPos(mapToScene(item->getImagePos()));
		}
	}
	QGraphicsPixmapItem::mouseMoveEvent(event);
}

void PixmapItem::wheelEvent(QGraphicsSceneWheelEvent *event)
{
	if (event->delta() > 0)
	{
		setScaleNum(scale() * 1.1, event->pos());
	}
	else
	{
		setScaleNum(scale() / 1.1, event->pos());
	}
	event->accept();
}

void PixmapItem::drawClear()
{
	m_complatePoint.clear();
	m_lineItems.clear();
}


