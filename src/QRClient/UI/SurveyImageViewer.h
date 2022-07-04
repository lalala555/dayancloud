#pragma once

#include <memory>

#include <QGraphicsView>
#include <QImage>

class PixmapItem;


struct ControlPointInfo
{
	QPoint p;
	QString name;
};

class SurveyImageViewer : public QGraphicsView
{
	Q_OBJECT
public:
	SurveyImageViewer(QWidget *parent = nullptr);
	~SurveyImageViewer();

	void showImage(const QImage & img, bool scroll_center = false);

	void setControlPoints(const std::vector<ControlPointInfo> & points);

	void setLines(const std::vector<std::vector<QPoint>> & lines);

	void setCurrentStabPoint(const QPointF &);

	void PixmapItemDataClear();

signals:
	// �̵�����
	void stabPoints(const QPointF &);

private:
	std::unique_ptr<PixmapItem> m_item;

	QPointer<QPushButton> m_btsurveySure; // ����ȷ��

	QGraphicsScene* m_scenebackground;

};
