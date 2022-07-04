#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QSplashScreen>
#include <QProgressBar>

class SplashScreen : public QSplashScreen
{
    Q_OBJECT

public:
    SplashScreen(const QPixmap & pixmap = QPixmap());
    ~SplashScreen();

    void setProgressBarPos(int x, int y, int w, int h);
    void setProgressValue(int v);
    void setProgressBarRange(int min, int max);
    void setProgressBarTextVisible(bool visible);
    void setProgressBarStytle(QString style);
    bool isGifFinished();
    void waitForFinished();
    void setGifScreen(const QString& gifPath, int speed = 100);

private slots:
    void repaintScreen(int);
    void onFrameChanged(int);

signals:
    void movieFinished();

protected:
    virtual void showEvent(QShowEvent *event);

private:
    QProgressBar *m_progress;
    QLabel *m_label;
    QMovie *m_movie;
    int m_totalFrame;
};

#endif // SPLASHSCREEN_H
