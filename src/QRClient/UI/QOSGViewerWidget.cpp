#include "stdafx.h"
#include <QMenu>
#include <osg/LightModel>
#include <osgViewer/Renderer>
#include <osg/ValueObject>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#pragma execution_character_set("utf-8")

#include <iostream>
#include "QOSGViewerWidget.h"
#include <QInputEvent>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgGA/MultiTouchTrackballManipulator>
#include <osgGA/StateSetManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/CameraManipulator>
#include <QApplication>
#include <QMessageBox>
#include <osgUtil/Optimizer>
#include <osg/CoordinateSystemNode>
#include <osg/PositionAttitudeTransform>
#include <osgUtil/DelaunayTriangulator>
// #include "DataLoad.h"
 #include "osgTest.h"
// #include "osgCommon.h"




QOSGViewerWidget::QOSGViewerWidget(QWidget *parent) 
    : QOpenGLWidget(parent)
    , _gw(nullptr)
{

	setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow);
	QSurfaceFormat format = QSurfaceFormat::defaultFormat();
	format.setRenderableType(QSurfaceFormat::OpenGL);
	format.setSamples(16);
	setFormat(format);
	setMouseTracking(true);
	setFocusPolicy(Qt::StrongFocus);
	initGW();	
}


QOSGViewerWidget::~QOSGViewerWidget()
{
	setCamera(NULL);
	setSceneData(NULL);
	_gw->close();

}

bool QOSGViewerWidget::event(QEvent *event)
{
    if (_gw == nullptr)
        return false;

	switch (event->type()) {
	case QEvent::TouchBegin:
	case QEvent::TouchEnd:
	case QEvent::TouchUpdate: {
		QList<QTouchEvent::TouchPoint> touchPoints = static_cast<QTouchEvent *>(event)->touchPoints();
		unsigned int id = 0;
		unsigned int tapCount = touchPoints.size();

		osg::ref_ptr<osgGA::GUIEventAdapter> osgEvent(NULL);
		osgGA::GUIEventAdapter::TouchPhase phase = osgGA::GUIEventAdapter::TOUCH_UNKNOWN;
		foreach(const QTouchEvent::TouchPoint& touchPoint, touchPoints) {
			if (!osgEvent) {
				if (event->type() == QEvent::TouchBegin) {
					phase = osgGA::GUIEventAdapter::TOUCH_BEGAN;
					osgEvent = _gw->getEventQueue()->touchBegan(id, osgGA::GUIEventAdapter::TOUCH_BEGAN, touchPoint.pos().x(), touchPoint.pos().y());
				}
				else if (event->type() == QEvent::TouchEnd) {
					phase = osgGA::GUIEventAdapter::TOUCH_ENDED;
					osgEvent = _gw->getEventQueue()->touchEnded(id, osgGA::GUIEventAdapter::TOUCH_ENDED, touchPoint.pos().x(), touchPoint.pos().y(), tapCount);
				}
				else if (event->type() == QEvent::TouchUpdate) {
					phase = osgGA::GUIEventAdapter::TOUCH_MOVED;
					osgEvent = _gw->getEventQueue()->touchMoved(id, osgGA::GUIEventAdapter::TOUCH_MOVED, touchPoint.pos().x(), touchPoint.pos().y());
				}
			}
			else {
				osgEvent->addTouchPoint(id, osgGA::GUIEventAdapter::TOUCH_ENDED, touchPoint.pos().x(), touchPoint.pos().y());
				osgEvent->addTouchPoint(id, phase, touchPoint.pos().x(), touchPoint.pos().y());
			}
			id++;
		}
		break;
	}
	default:
		break;
	}
	return QOpenGLWidget::event(event);
}


// bool QOSGViewerWidget::eventFilter(QObject* obj, QEvent* event)
// {
// 	if (obj == (QObject*)this) 
// 	{
// 		if (event->type() == QEvent::MouseButtonPress) {
//  		bool a = QOSGViewerWidget::event(event);
// 		return a;
// 		}
// 		return true;
// 	}	
// 	return true;
// }

void QOSGViewerWidget::setKeyboardModifiers(QInputEvent *event)
{
    if (_gw == nullptr)
        return;

	int modkey = event->modifiers() & (Qt::ShiftModifier | Qt::ControlModifier | Qt::AltModifier);
	unsigned int mask = 0;
	if (modkey & Qt::ShiftModifier) {
		mask |= osgGA::GUIEventAdapter::MODKEY_SHIFT;
	}
	if (modkey & Qt::ControlModifier) {
		mask |= osgGA::GUIEventAdapter::MODKEY_CTRL;
	}
	if (modkey & Qt::AltModifier) {
		mask |= osgGA::GUIEventAdapter::MODKEY_ALT;
	}
	_gw->getEventQueue()->getCurrentEventState()->setModKeyMask(mask);
	update();
	event->accept();
}

void QOSGViewerWidget::keyPressEvent(QKeyEvent *event)
{
    if (_gw == nullptr)
        return;

	setKeyboardModifiers(event);
	_gw->getEventQueue()->keyPress(event->key());
	QOpenGLWidget::keyPressEvent(event);
	
	update();
	event->accept();
}

void QOSGViewerWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (_gw == nullptr)
        return;

	setKeyboardModifiers(event);
	_gw->getEventQueue()->keyRelease(event->key());
	QOpenGLWidget::keyReleaseEvent(event);
	update();
	event->accept();
}

void QOSGViewerWidget::mousePressEvent(QMouseEvent *event)
{
    if (_gw == nullptr)
        return;

	int button = 0;
	switch (event->button()) {
	case Qt::LeftButton: button = 1; break;
	case Qt::MidButton: button = 2; break;
	case Qt::RightButton: button = 3; break;
	case Qt::NoButton: button = 0; break;
	default: button = 0; break;
	}
	setKeyboardModifiers(event);
	_gw->getEventQueue()->mouseButtonPress(event->x(), event->y(), button);
	QOpenGLWidget::mousePressEvent(event);
	update();
	event->accept();
}

void QOSGViewerWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (_gw == nullptr)
        return;

	int button = 0;
	switch (event->button()) {
	case Qt::LeftButton: button = 1; break;
	case Qt::MidButton: button = 2; break;
	case Qt::RightButton: button = 3; break;
	case Qt::NoButton: button = 0; break;
	default: button = 0; break;
	}
	setKeyboardModifiers(event);
	_gw->getEventQueue()->mouseButtonRelease(event->x(), event->y(), button);

	QOpenGLWidget::mouseReleaseEvent(event);
	update();
	event->accept();
}

void QOSGViewerWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (_gw == nullptr)
        return;
     
	int button = 0;
	switch (event->button()) {
	case Qt::LeftButton: button = 1; break;
	case Qt::MidButton: button = 2; break;
	case Qt::RightButton: button = 3; break;
	case Qt::NoButton: button = 0; break;
	default: button = 0; break;
	}
	setKeyboardModifiers(event);
	_gw->getEventQueue()->mouseDoubleButtonPress(event->x(), event->y(), button);

	QOpenGLWidget::mouseDoubleClickEvent(event);
	update();
	event->accept();
}

void QOSGViewerWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (_gw == nullptr)
        return;

	setKeyboardModifiers(event);
	_gw->getEventQueue()->mouseMotion(event->x(), event->y());
	QOpenGLWidget::mouseMoveEvent(event);
	update();
	event->accept();
}

void QOSGViewerWidget::wheelEvent(QWheelEvent *event)
{
    if (_gw == nullptr)
        return;

	setKeyboardModifiers(event);
	_gw->getEventQueue()->mouseScroll(
		event->orientation() == Qt::Vertical ?
		(event->delta() > 0 ? osgGA::GUIEventAdapter::SCROLL_UP : osgGA::GUIEventAdapter::SCROLL_DOWN) :
		(event->delta() > 0 ? osgGA::GUIEventAdapter::SCROLL_LEFT : osgGA::GUIEventAdapter::SCROLL_RIGHT));
	QOpenGLWidget::wheelEvent(event);
	update();
	event->accept();
}

void QOSGViewerWidget::resizeEvent(QResizeEvent *event)
{
    if (_gw == nullptr)
        return;

	const QSize& size = event->size();
	_gw->resized(x(), y(), size.width(), size.height());
	_gw->getEventQueue()->windowResize(x(), y(), size.width(), size.height());
	_gw->requestRedraw();

	QOpenGLWidget::resizeEvent(event);
	event->accept();
}

void QOSGViewerWidget::moveEvent(QMoveEvent *event)
{
    if (_gw == nullptr)
        return;

	const QPoint& pos = event->pos();
	_gw->resized(pos.x(), pos.y(), width(), height());
	_gw->getEventQueue()->windowResize(pos.x(), pos.y(), width(), height());

	QOpenGLWidget::moveEvent(event);
	event->accept();
}

void QOSGViewerWidget::timerEvent(QTimerEvent *)
{
	update();
}

void QOSGViewerWidget::paintGL()
{
	if (isVisibleTo(QApplication::activeWindow())) {
		frame();
	}
}
void QOSGViewerWidget::setPointCloudDataFile(const string& _atDataJsonfilePath)
{
	m_atDataJsonfilePath = _atDataJsonfilePath;
}

void QOSGViewerWidget::initGW()
{
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	traits->windowDecoration = false;
	traits->x = 0;
	traits->y = 0;
	traits->width = width();
	traits->height = height();
	traits->doubleBuffer = true;
	traits->sharedContext = 0;
	traits->setUndefinedScreenDetailsToDefaultScreen();
	_gw = new osgViewer::GraphicsWindowEmbedded(traits);

	auto camera = getCamera();
	camera->setGraphicsContext(_gw);

	double fovy, aspectRatio, zNear, zFar;
	getCamera()->getProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);
	double newAspectRatio = double(traits->width) / double(traits->height);

	double aspectRatioChange = newAspectRatio / aspectRatio;
	if (aspectRatioChange != 1.0)  //设置相机投影矩阵的缩放，使xy显示比例一致
	{
		getCamera()->getProjectionMatrix() *= osg::Matrix::scale(1.0 / aspectRatioChange, 1.0, 1.0);
	}
	camera->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
	camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	camera->setClearColor(osg::Vec4(0.25, 0.286, 0.314, 0));

	this->setCamera(camera);
}

void QOSGViewerWidget::loadAtData(const string& _atDataJsonfilePath)
{
 	osg::ref_ptr<osg::Group> root = new osg::Group;
	double ori_x, ori_y, ori_z;
	osg::Node *pnode = LoadPointCloudFormJson(_atDataJsonfilePath, ori_x, ori_y, ori_z);
	if ( !pnode)
	{
		return;
	}
	
#if 0  //test
	osg::Vec3 cp1(354046.870, 2056514.813, 87.506);
	osg::Vec3 cp2(354022.112, 2056567.422, 87.514);
	osg::Vec3 cp3(354015.604, 2056501.074, 87.579);
	osg::Vec3 cpo(ori_x, ori_y, ori_z);

	osg::Node *pnodecp1 = createSphereR(cp1 - cpo,3);
	osg::Node *pnodecp2 = createSphereR(cp2 - cpo,3);
	osg::Node *pnodecp3 = createSphereR(cp3 - cpo,3);

	root->addChild(pnodecp1);
	root->addChild(pnodecp2);
	root->addChild(pnodecp3);
#endif  //end test

	//创建坐标轴和参考网格
	osg::BoundingSphere bs = pnode->getBound();
	osg::Vec3d ref_center = bs.center();
 	root->addChild(pnode);
 	root->addChild(createAxis(bs));

//	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
// 	mt->setMatrix(osg::Matrix::rotate(90, osg::Vec3d(1, 0, 0)) * osg::Matrix::scale(osg::Vec3(0.2, 0.2, 0.2)));
// 	mt->addChild(pnode);
// 	mt->addChild(createAxis(bs));
// 	mt->addChild(createSkyBox());

	root->addChild(createSkyBox());
	this->setSceneData(root);
	this->setThreadingModel(osgViewer::Viewer::SingleThreaded);
	osg::ref_ptr <osgGA::TrackballManipulator> tm = new osgGA::TrackballManipulator();
	tm->setVerticalAxisFixed(true);
	this->setCameraManipulator(tm);
	this->addEventHandler(new osgViewer::WindowSizeHandler());
	startTimer(20);

}

void QOSGViewerWidget::clearData()
{
	setCamera(NULL);
	setSceneData(NULL);
}