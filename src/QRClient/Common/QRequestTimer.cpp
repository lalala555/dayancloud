#include "stdafx.h"
#include "QRequestTimer.h"

QRequestTimer::QRequestTimer(QObject *parent)
    : QObject(parent)
    , m_reply(Q_NULLPTR)
    , m_timer(new QTimer)
{
    connect(m_timer.data(), SIGNAL(timeout()), this, SLOT(onRequestTimeout()));
}

QRequestTimer::~QRequestTimer()
{
}

void QRequestTimer::setTimeout(QNetworkReply* reply, int delayTime)
{
    if(reply && reply->isRunning()) {
        m_reply = reply;
        m_timer->stop();
        m_timer->start(delayTime);
    }
}

void QRequestTimer::onRequestTimeout()
{
    if(!m_timer->isActive())
        return;

    if(m_reply && m_reply->isRunning()) {
        m_reply->abort();
        m_reply->close();
        LOGFMTE("[onRequestTimeout] download content timeout!!");
    }
}
