#include "stdafx.h"
#include "TextMovingWidget.h"

TextMovingWidget::TextMovingWidget(QWidget *parent)
    : QWidget(parent)
    , m_x_pos(0)
    , m_direction(1)
    , m_curtText("")
    , m_curtIndex(0)
    , m_vol(20)
{
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onUpdateTextPosition()));
    this->setMouseTracking(true);
}

TextMovingWidget::~TextMovingWidget()
{
    m_timer->stop();
    m_timer->deleteLater();
}

void TextMovingWidget::startAnimation()
{
    m_curtIndex = 0;
    int lang = LocalSetting::getInstance()->getLanguageId();
    if(m_timer!= nullptr) {
        if(!m_newsList.isEmpty()) {
            m_curtText = m_newsList.at(m_curtIndex).msg[lang];
        }
        m_x_pos = this->geometry().width();
        m_timer->stop();
        m_timer->start(m_vol);
    }
}

void TextMovingWidget:: stopAnimation()
{
    if(m_timer != nullptr)
        m_timer->stop();
    m_contentList.clear();
    m_newsList.clear();
    m_curtIndex = 0;
    m_curtText = "";
}

void TextMovingWidget::addMovingText(const QString& zh, const QString& en, const QString& jp, const QString& url)
{
    News news;
    news.msg[LANG_EN] = en;
    news.msg[LANG_CN] = zh;
    news.msg[LANG_JP] = jp;
    news.url = url;
    m_newsList.append(news);
}

void TextMovingWidget::addMovingContents(const QList<NoticeContent> contents)
{
    if(checkContents(contents)) {
        return;
    }

    m_contentList.clear();
    m_contentList = contents;

    m_newsList.clear();
    foreach(NoticeContent content, contents) {
        addMovingText(content.notifyMessageZh,
                      content.notifyMessageEn,
                      content.notifyMessageJp,
                      "");
    }

    this->startAnimation();
}

bool TextMovingWidget::checkContents(const QList<NoticeContent>& contents)
{
    if(m_contentList.isEmpty() ||
            m_contentList.count() != contents.count()) {
        return false;
    }

    int sameCount = 0;
    foreach(NoticeContent content, contents) {
        if(m_contentList.contains(content)) {
            sameCount ++;
        }
    }
    return sameCount == (m_contentList.count());
}

void TextMovingWidget::removeMovingText(const QString& text)
{
    m_textList.remove(text);
}

void TextMovingWidget::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    QPainter p(this);
    p.setPen(Qt::white);
    QFontMetrics metric(font());
    int y = (height()- metric.height()) / 2;
    p.drawText(m_x_pos, y, metric.width(m_curtText), metric.height(), Qt::AlignLeft, m_curtText);
}

void TextMovingWidget::enterEvent(QEvent *e)
{
    if(m_newsList.isEmpty())
        return;

    m_timer->stop();
    this->setCursor(Qt::PointingHandCursor);
    update();
}

void TextMovingWidget::leaveEvent(QEvent *)
{
    if(m_newsList.isEmpty())
        return;

    this->setCursor(Qt::CrossCursor);
    m_timer->start(m_vol);
    update();
}

void TextMovingWidget::changeEvent(QEvent *event)
{
    switch(event->type()) {
    case QEvent::LanguageChange: {
        this->startAnimation();
    }
    break;
    default:
        QWidget::changeEvent(event);
        break;
    }
}

int TextMovingWidget::getDirection(const QString& text)
{
    QFontMetrics metrics(font());
    int tw = metrics.width(text);
    if(tw <= width()) {
        if(m_x_pos + tw> width())
            return 0;
        else if(m_x_pos <= 0)
            return 1;
        return m_direction;
    } else {
        if(m_x_pos == 10)
            return 0;
        else if(m_x_pos <= width() - tw -10)
            return 1;
        return m_direction;
    }
}

void TextMovingWidget::onUpdateTextPosition()
{
    if(m_newsList.isEmpty())
        return;

    QFontMetrics metric(font());
    int lang = LocalSetting::getInstance()->getLanguageId();
    int tw= metric.width(m_curtText);
    /*if(tw > width()) //If the text width greater than widget width
    {
        if(m_direction) //right
            m_x_pos++;
        else          //left
            m_x_pos--;
    }
    else
    {
        if(m_direction) //right
            m_x_pos++;
        else           //left
            m_x_pos--;
    }
    m_direction = getDirection(m_curtText);*/
    m_x_pos--;
    if(m_x_pos < 0 && abs(m_x_pos) > tw) {
        m_curtIndex++;
        if(m_curtIndex > m_newsList.count() - 1) {
            m_curtIndex = 0;
        }
        m_x_pos = this->geometry().width();
        m_curtText = m_newsList.at(m_curtIndex).msg[lang];
    }

    update();
}
