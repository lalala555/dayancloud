#ifndef TEXTMOVINGWIDGET_H
#define TEXTMOVINGWIDGET_H

#include <QWidget>
#include "kernel/msg.h"

struct News{
    QString msg[3];
    QString url;
};

class TextMovingWidget : public QWidget
{
    Q_OBJECT

public:
    TextMovingWidget(QWidget *parent = 0);
    ~TextMovingWidget();
    void setTextMovingSpeed(int vol = 20){ m_vol = vol; } // vol 移动速度
    void addMovingText(const QString& zh, const QString& en, const QString& jp, const QString& url);
    void addMovingContents(const QList<NoticeContent> contents);
    void removeMovingText(const QString& text);
    void startAnimation();
    void stopAnimation();
    bool checkContents(const QList<NoticeContent>& contents);

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);
    inline int getDirection(const QString&);
    virtual void changeEvent(QEvent *event);

private slots:
    void onUpdateTextPosition();
private:    
    QTimer* m_timer;
    int m_direction; // 0-left, 1-right
    int m_x_pos;
    int m_vol;
    int m_curtIndex;
    QString m_curtText;
    QMap<QString, QString> m_textList;
    QList<News> m_newsList;
    QList<NoticeContent> m_contentList;
};

#endif // TEXTMOVINGWIDGET_H
