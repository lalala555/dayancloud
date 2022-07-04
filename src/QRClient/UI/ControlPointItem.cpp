#include "stdafx.h"
#include "ControlPointItem.h"

QImage ControlPointItem::m_controlEditImg;

QImage ControlPointItem::m_controlOkImg;

ControlPointItem::ControlPointItem(QGraphicsItem *parent)
	: QGraphicsObject(parent)
{
	this->setFlags(flags() | ItemIsSelectable);
	setAcceptedMouseButtons(Qt::LeftButton);
}


ControlPointItem::~ControlPointItem()
{
}

void ControlPointItem::setEdit(bool isEdit)
{
	m_isEdit = isEdit;
	update();
}

void ControlPointItem::setName(const QString &name)
{
	m_name = name;
}

void ControlPointItem::saveImagePos(const QPoint &p)
{
	m_pos = p;
}

QPoint ControlPointItem::getImagePos()
{
	return m_pos;
}

QRectF ControlPointItem::boundingRect() const
{
	return QRectF(-20, -20, 40, 40);
}

//QVariant ControlPointItem::itemChange(GraphicsItemChange change, const QVariant &value)
//{
//	if (change == ItemPositionChange && scene()) {
//		// value is the new position.
//		QPointF newPos = value.toPointF();
//		QRectF rect = scene()->sceneRect();
//		if (!rect.contains(newPos)) {
//			// Keep the item inside the scene rect.
//			newPos.setX(qMin(rect.right(), qMax(newPos.x(), rect.left())));
//			newPos.setY(qMin(rect.bottom(), qMax(newPos.y(), rect.top())));
//
//			emit posChanged(newPos);
//
//			return newPos;
//		}
//	}
//	return QGraphicsItem::itemChange(change, value);
//}

void ControlPointItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (m_isEdit && event->button() == Qt::LeftButton && (GetKeyState(VK_SHIFT) & 0x8000))
	{
		m_prePos = event->scenePos();
	}
	else
	{
		event->ignore();
	}
}

void ControlPointItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if (m_isEdit && event->button() == Qt::LeftButton && (GetKeyState(VK_SHIFT) & 0x8000))
	{
		emit posChanged(pos());
	}
	else
	{
		event->ignore();
	}
}

void ControlPointItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	if (m_isEdit && (event->buttons() & Qt::LeftButton) && (GetKeyState(VK_SHIFT) & 0x8000))
	{
		auto curPos = event->scenePos();
		auto offset = curPos - m_prePos;
		m_prePos = curPos;

		moveBy(offset.x(), offset.y());

		//posChanged(pos());
	}
	else
	{
		event->ignore();
	}
}

void ControlPointItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = Q_NULLPTR */)
{
	if (m_controlEditImg.isNull())
		m_controlEditImg.load(":/view/images/position.png");
	if (m_controlOkImg.isNull())
		m_controlOkImg.load(":/view/images/position_ok.png");

	if (m_isEdit)
		painter->drawImage(-20, -20, m_controlEditImg.scaled(40, 40));
	else
		painter->drawImage(-20, -20, m_controlOkImg.scaled(40, 40));

	if (!m_name.isEmpty())
	{
		QPen pen;
		pen.setColor(QColor(233,143,54));
		QFont font;
		font.setBold(true);
		painter->setFont(font);
		painter->setPen(pen);
		painter->drawText(20, 20, m_name);
	}
}
