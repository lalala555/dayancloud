#pragma once
#include "stdafx.h"
#include <QWidget>
#include <QEvent>
#include <QOpenGLWidget>
#include <osgViewer/Viewer>

class QInputEvent;

class QOSGViewerWidget : public QOpenGLWidget, public osgViewer::Viewer
{
	Q_OBJECT

public:
	QOSGViewerWidget(QWidget *parent = 0);
	~QOSGViewerWidget();
	bool event(QEvent* event);

protected:	

//	bool eventFilter(QObject* obj, QEvent* event);

	void setKeyboardModifiers(QInputEvent* event);
	void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void mouseDoubleClickEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent* event);
	void resizeEvent(QResizeEvent *event);
	void moveEvent(QMoveEvent* event);
	void timerEvent(QTimerEvent *);


protected:
	virtual void paintGL();

public:
	void loadAtData(const string& _atDataJsonfilePath);
	void setPointCloudDataFile(const string& _atDataJsonfilePath);
	void clearData();
private:
	void initGW();

// 	osg::ref_ptr<osg::Camera> createCamera(int x, int y, int w, int h);
// 	osg::Camera* CreateCameraFrustum(osg::Node* pat);
private:
	osgViewer::GraphicsWindow* _gw;
	string m_atDataJsonfilePath;
};



