#pragma once

#include <QGraphicsPixmapItem>

#include "ControlPointItem.h"

#include "SurveyImageViewer.h"

class PixmapItem : public QObject, public QGraphicsPixmapItem
{
	Q_OBJECT
public:
	PixmapItem(QGraphicsItem *parent = nullptr);
	~PixmapItem();

	void initItem();

	void setLines(const std::vector<std::vector<QPoint>> & lines);

	void setControlPoints(const std::vector<ControlPointInfo> & points);

	void setCurrentStabPoint(const QPointF &);

	void setScaleNum(double scale_num, QPointF p = QPointF());

	void drawClear();

signals:
	// ´Ìµã×ø±ê
	void stabPoints(const QPointF &);

protected:
	//virtual void keyPressEvent(QKeyEvent *event) override;
	//virtual void keyReleaseEvent(QKeyEvent *event) override;
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	virtual void wheelEvent(QGraphicsSceneWheelEvent *event) override;

private:
	//double m_scale{1.0};
	//bool m_pressShift{ false };

	QPointF pressScenePos;

	std::vector<ControlPointItem *> m_complatePoint;

	ControlPointItem *m_controlPoint{ nullptr };
	std::vector<QGraphicsPathItem *> m_lineItems;
};

