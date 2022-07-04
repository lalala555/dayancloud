#pragma once

#include <QGraphicsObject>

class ControlPointItem : public QGraphicsObject
{
	Q_OBJECT
public:
	ControlPointItem(QGraphicsItem *parent = nullptr);
	~ControlPointItem();

	void setEdit(bool isEdit);
	void setName(const QString &);

	void saveImagePos(const QPoint &);
	QPoint getImagePos();

	QRectF boundingRect() const override;

signals:
	void posChanged(const QPointF &);

protected:
	//QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = Q_NULLPTR */) override;

private:
	static QImage m_controlEditImg;
	static QImage m_controlOkImg;
	QString m_name;
	QPoint m_pos;
	bool m_isEdit{ true };
	QPointF m_prePos;
};

