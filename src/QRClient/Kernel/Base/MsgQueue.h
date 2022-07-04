#pragma once
#ifndef MSGQUEUE_H
#define MSGQUEUE_H

#include <QThread>
#include <QtConcurrent/QtConcurrent>

template<class T>
class MsgBlockingQueue
{
public:
    virtual ~MsgBlockingQueue() = default;

    void push(const T& value)
    {
        {
            QMutexLocker locker(&m_mutex);
            m_queue.enqueue(value);
        }
        m_cond.wakeOne();
    }

    T pop()
    {
        QMutexLocker locker(&m_mutex);
        while (!m_stopFlag && m_queue.empty())
            m_cond.wait(&m_mutex);

        if(m_stopFlag)
            return nullptr;

        T out = m_queue.dequeue();

        return out;
    }

    void stop()
    {
        m_stopFlag = true;
        m_cond.wakeAll();
    }

    bool empty()
    {
        QMutexLocker locker(&m_mutex);
        return m_queue.empty();
    }

private:
    QQueue<T> m_queue;
    QMutex m_mutex;
    QWaitCondition m_cond;
    volatile bool m_stopFlag = false;
};

// 对外接口
template<typename MsgType>
class MsgQueue : public QThread
{
public:
    MsgQueue(QObject *parent = nullptr) {}
    ~MsgQueue() {
    }

public:
    void init()
    {
    }

    void startQueue()
    {
#if 0
        msgHandle();
#else
        start();
#endif  
    }

    void stopQueue()
    {
        m_runMsgQueue = false;
        m_msgQueue.stop();

#if 0
        m_future.cancel();
        m_future.waitForFinished();
#else
        quit();
        wait(100); 
#endif 
    }

    // 处理消息
    void post_msg(MsgType msg)
    {
        m_msgQueue.push(msg);
    }

protected:
    void run() {
        while (m_runMsgQueue) {
            MsgType msg = m_msgQueue.pop();
            if (msg != nullptr) {
                msg->sendMsg();
                msg->release();
            }
        }
    }

    // 消息处理
    void msgHandle()
    {
        m_future = QtConcurrent::run([this] {
            while (m_runMsgQueue) {
                MsgType msg = m_msgQueue.pop();
                if (msg != nullptr) {
                    msg->sendMsg();
                    msg->release();
                }
            }
        });
    }

private:
    MsgBlockingQueue<MsgType> m_msgQueue;
    bool m_runMsgQueue = true;
    QFuture<void> m_future;
};

#endif // !MSGQUEUE_H


