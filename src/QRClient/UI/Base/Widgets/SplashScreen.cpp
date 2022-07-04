#include "stdafx.h"
#include "SplashScreen.h"

SplashScreen::SplashScreen(const QPixmap & pixmap)
    : QSplashScreen(pixmap, Qt::WindowStaysOnTopHint)
    , m_totalFrame(0)
{
    m_label = new QLabel(this);
    m_movie = new QMovie();
    m_progress = new QProgressBar(this);

    // 默认值
    m_progress->setValue(0);
    m_progress->setTextVisible(false);

#ifdef FOXRENDERFARM
//     m_progress->setStyleSheet(" QProgressBar {border: 1px solid #f3512b;}"
//                               "QProgressBar::chunk {background-color: #f3512b;}");
	m_progress->setStyleSheet(" QProgressBar {border: 1px solid #ffffff;}"
								"QProgressBar::chunk {background-color: qlineargradient(spread: pad, x1:0, y1:1, x2:1, y2:1, stop:0 rgb(103,95,254), stop:1 rgb(255,255,255))}");

#else
    m_progress->setStyleSheet(" QProgressBar {border: 1px solid #675ffe;}"
                              "QProgressBar::chunk {background-color: #675ffe;}");
#endif

    connect(m_progress, SIGNAL(valueChanged(int)), this, SLOT(repaintScreen(int)));
    connect(m_movie, &QMovie::frameChanged, this, &SplashScreen::onFrameChanged);
}

SplashScreen::~SplashScreen()
{

}

void SplashScreen::setProgressBarPos(int x, int y, int w, int h)
{
    m_progress->setGeometry(x, y, w, h);
}

void SplashScreen::setProgressValue(int v)
{
    m_progress->setValue(v);
}

void SplashScreen::repaintScreen(int)
{
    this->repaint();
}

void SplashScreen::setProgressBarRange(int min, int max)
{
    m_progress->setRange(min, max);
}

void SplashScreen::setProgressBarTextVisible(bool visible)
{
    m_progress->setTextVisible(visible);
}

void SplashScreen::setProgressBarStytle(QString style)
{
    m_progress->setStyleSheet(style);
}

void SplashScreen::showEvent(QShowEvent *event)
{
    QSplashScreen::showEvent(event);
}

void SplashScreen::setGifScreen(const QString& gifPath, int speed)
{
    m_label->setFixedSize(this->size());
    m_label->setMovie(m_movie);
    m_label->setScaledContents(true);
    m_movie->setScaledSize(m_label->size());
    
    m_movie->setSpeed(speed);
    m_movie->setFileName(gifPath);
    m_movie->start();

    m_totalFrame = m_movie->frameCount();

    this->setStyleSheet("{background-color:#eeeeee;}");
}

bool SplashScreen::isGifFinished()
{
    if (m_totalFrame == 0) return true;

    int curFrameNum = m_movie->currentFrameNumber();
    return curFrameNum >= m_totalFrame - 1;
}

void SplashScreen::waitForFinished()
{
    QEventLoop loop;
    connect(this, &SplashScreen::movieFinished, &loop, &QEventLoop::quit);
    loop.exec();

    // m_progress->hide();
    // m_label->hide();
    m_movie->stop();
}

void SplashScreen::onFrameChanged(int frame)
{
    if (frame >= m_totalFrame - 1) {
        emit movieFinished();
    }
}