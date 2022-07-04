#pragma once
#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include <QObject>
#include <QThread>
#include <QMap>
#include <QtConcurrent/QtConcurrent>
#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "Basement/log4z/log4z.h"

enum QueuePopResult {
    POP_OK,
    POP_STOP,
    POP_UNEXPECTED
};

template<class Worker, typename Container, typename Cmp>
class PQueue
{
public:
    std::mutex _mutex;
    std::condition_variable _condi;
    bool _stopFlag = false;
    std::priority_queue<Worker, Container, Cmp> _pqueue;
    QMutex m_mutex;
    QWaitCondition m_cond;

    virtual ~PQueue() = default;

    void push(const Worker& value)
    {
        QMutexLocker locker(&m_mutex);
        _pqueue.push(value);
        m_cond.wakeOne();
    }

    void push(Worker&& value)
    {
        QMutexLocker locker(&m_mutex);
        _pqueue.push(std::move(value));
        m_cond.wakeOne();
    }

    QueuePopResult pop(Worker& out)
    {
        QMutexLocker locker(&m_mutex);
        if (_stopFlag) // ֹͣ
            return POP_STOP;
        if (_pqueue.empty())
            m_cond.wait(&m_mutex);
        if (_stopFlag) // ֹͣ
            return POP_STOP;
        if (_pqueue.empty()) // ���⻽��
            return POP_UNEXPECTED;

        out = _pqueue.top();
        _pqueue.pop();
        return POP_OK;
    }

    void stop()
    {
        QMutexLocker locker(&m_mutex);
        _stopFlag = true;
        m_cond.wakeAll();
    }

    void clear()
    {
        QMutexLocker locker(&m_mutex);
        while (!_pqueue.empty())
        {
            _pqueue.top();
            _pqueue.pop();
        }
    }

    bool empty()
    {
        QMutexLocker locker(&m_mutex);
        return _pqueue.empty();
    }
};

template<typename Worker>
class PriorityTaskQueue : public QThread
{
public:
    PriorityTaskQueue(QObject *parent = nullptr) {}
    ~PriorityTaskQueue() {
        qDebug() << "~PriorityTaskQueue";
    }

public:
    void init()
    {
    }

    void setMaxRunTask(int max)
    {
        m_maxRunTask = max;
    }

    void startQueue()
    {
        m_mutex = new QMutex();
        m_runTaskQueue = true;
        m_reserve = false;

#if 0
        taskHandle();
#else
        start();
#endif    
    }

    void stopQueue()
    {
        {
            QMutexLocker locker(m_mutex);
            m_runTaskQueue = false;
        }
        stopAllTask();

#if 0
        m_future.cancel();
        m_future.waitForFinished();
#else
        quit();
        wait(100);
#endif
    }

    void startTask(qint64 taskId, Worker worker, bool batch = false)
    {  
        if (!checkTaskExist(taskId)) {
            QMutexLocker locker(m_mutex);
            if (worker->_packetIndex < 0) {
                m_taskList.push_back(worker);
            } else {
                m_taskList.insert(worker->_packetIndex, worker);
            }   
        }  
        // sortWorkerList(m_taskList);
    }

    bool stopTask(qint64 taskId)
    {
        removeTask(taskId);
        return true;
    }

    void removeTask(qint64 taskId)
    {
        QMutexLocker locker(m_mutex);
        for (int i = 0; i < m_taskList.size(); i++) {
            Worker w = m_taskList.at(i);
            if (w != nullptr && w->getTaskId() == taskId) {
                w->stopTask();
                m_taskList.removeAt(i);
                break;
            }
        }
    }

    void stopAllTask()
    {
        QMutexLocker locker(m_mutex);
        for (int i = 0; i < m_taskList.size(); i++) {
            Worker w = m_taskList.at(i);
            if(w != nullptr)
                w->stopTask();
        }

        m_taskList.clear();
    }

    void removeAllTask()
    {
        stopAllTask();
    }

    void startAllWaittingTask()
    {
        /*std::lock_guard<std::mutex> locker(m_mutex);
        auto it = m_waitingTasks.begin();
        while(it != m_waitingTasks.end()) {
            m_taskQueue.push(it->second);
            it++;
        }*/
    }

    void updateTaskPriority(qint64 taskId, uint priority, uint updateTime)
    {
        QMutexLocker locker(m_mutex);
        Worker w = nullptr;
        for (int i = 0; i < m_taskList.size(); i++) {
            w = m_taskList.at(i);
            if (w != nullptr && w->getTaskId() == taskId) {
                m_taskList.at(i)->updateTimePriority(updateTime);
                m_taskList.at(i)->updatePriority(priority);
                break;
            }
        }

        sortWorkerList(m_taskList, w); 
    }

    void freeTask(qint64 taskId)
    {
        removeTask(taskId);
    }

    // �������񲢷���ʱ
    void setTaskInterval(int interval)
    {
        m_taskInterval = interval;
    }

    void updateTaskTable(const QVector<qint64>& table) {
        QMutexLocker locker(m_mutex);
        m_taskTable.clear();
        m_taskTable.append(table);
    }

    void startAddTask() {
        m_startTask = false;
    }

    void addTaskEnd() {
        m_startTask = true;
    }

    void setReserve(bool reserve) {
        m_reserve = reserve;
    }

private:
    // ��������Ƿ��Ѿ����Ƴ�
    bool checkTaskExist(qint64 taskId)
    {
        QMutexLocker locker(m_mutex);
        for (int i = 0; i < m_taskList.size(); i++) {
            Worker w = m_taskList.at(i);
            if (w != nullptr && w->getTaskId() == taskId) {
                return true;
            }
        }
        return false;
    }

    bool cmpWorker(Worker a, Worker b) {
        /*if (a->_priority < b->_priority) {
            return true;
        }
        if (a->_priority == b->_priority && a->_priorityTime > b->_priorityTime) {
            return true;
        }
        if (a->_priority == b->_priority && a->_priorityTime == b->_priorityTime
            && a->_packetIndex > b->_packetIndex) {
            return true;
        }*/
        if (a->_priorityTime > b->_priorityTime) {
            return true;
        } else if (a->_priorityTime == b->_priorityTime) { // ���ʱ����ȣ���ʹ��index
            if (a->_packetIndex < b->_packetIndex) {// ���indexС������Ҫ����λ��
                return true;
            }
        }
        return false;
    }

    void sortWorkerList(QList<Worker>& list, Worker exchange) {
        // if (exchange == nullptr)return;
        QList<Worker> temp = list;

        bool bIsDone = false;
        for (int i = temp.size() - 1; i > 0 && !bIsDone; --i){
            bIsDone = true;
            for (int j = 0; j < i; ++j){
                if (cmpWorker(temp[j], temp[j + 1])){
                    bIsDone = false;
                    Worker tmp = temp[j + 1];
                    temp[j + 1] = temp[j];
                    temp[j] = tmp;
                }
            }
        }
        list.clear();
        list.swap(temp);
    }

    bool isPriorityTask(Worker w) {
        if (w->_priority > 0 && w->_priorityTime > 0)
            return true;
        return false;
    }

    QVector<qint64> vectorReserve(const QVector<qint64>& vct)
    {
        QVector<qint64> tvect;
        if (vct.isEmpty())
            return tvect;

        tvect.resize(vct.size());
        for (int i = vct.length() - 1; i >= 0; i--) {
            tvect[vct.length() - 1 - i] = vct[i];
        }

        return tvect;
    }

    Worker popTaskWorker() {
        QMutexLocker locker(m_mutex);
        Worker w = nullptr;
        QVector<qint64> table = m_taskTable;

        // �����б�
        if (m_reserve == true) {
            sortWorkerList(m_taskList, w);
            table = vectorReserve(m_taskTable); // �Ƿ񽫱��˳����������ҳ����Ҫ�����������ʼ���أ�
        }

        // ������������ȼ�����
        QMap<int, QList<Worker>> priSet;
        for (int i = 0; i < m_taskList.size(); i++) {
            Worker p = m_taskList.at(i);
            if (p != nullptr && !p->isRunning()) {
                priSet[p->_priority].append(p);
            }
        }

        // �������ȼ�����, ����ߵ��������
        QList<int> keys = priSet.keys();
        qSort(keys.begin(), keys.end(), qGreater<int>());
        
        if (!table.isEmpty()) {
            // �������ȼ�˳��ʼ����
            for (int k = 0; k < keys.size(); k++) {
                int prioVal = keys.at(k);
                QList<Worker> prioritys = priSet.value(prioVal);

                // ��˳�����
                for (int j = 0; j < table.size(); j++) {
                    qint64 taskId = table.at(j);

                    // ��Ӧ���λ�ã���������ƥ���
                    for (int i = 0; i < prioritys.size(); i++) {
                        w = prioritys.at(i);
                        if (w != nullptr && w->getTaskId() == taskId && !w->isRunning()) {
                            return w;
                        } else {
                            w = nullptr;
                        }
                    }
                }
            }
            
        } else {
            // �������ȼ�˳��ʼ����
            for (int k = 0; k < keys.size(); k++) {
                int prioVal = keys.at(k);
                QList<Worker> prioritys = priSet.value(prioVal);
                    // ��Ӧ���λ�ã���������ƥ���
                for (int i = 0; i < prioritys.size(); i++) {
                    w = prioritys.at(i);
                    if (w != nullptr && !w->isRunning()) {
                        return w;
                    } else {
                        w = nullptr;
                    }
                }
            }
        }

        return w;
    }

    int runningSize() {
        QMutexLocker locker(m_mutex);
        QList<Worker> list = m_taskList;
        int running = 0;
        for (int i = 0; i < list.size(); i++) {
            Worker w = list.at(i);
            if (w != nullptr && w->isRunning()) {
                running++;
            }
        }
        return running;
    }

protected:
    void run() {
        LOGFMTI("[PriorityTaskQueue] QThread start!");
        while (m_runTaskQueue) {

            // ����µ�����ʱ���Ȳ�����
            if (m_startTask == false) {
                QThread::msleep(50);
                continue;
            }

            if (!m_runTaskQueue)
                return;

            // �����е������Ѿ������������ => continue
            if ((m_taskList.isEmpty() || runningSize() == m_maxRunTask)) {
                // LOGFMTI("[ScriptThread] Execute Script m_maxRunTask, %d!!", m_maxRunTask);
                QThread::msleep(50);
                continue;
            }

            Worker worker = popTaskWorker();

            if (worker != nullptr && !worker->isRunning()) {
                // �����Ѿ��Ƴ������ټ���ִ��
                if (!checkTaskExist(worker->getTaskId())) {
                    continue;
                }
                worker->startTask();
            }

            QThread::msleep(m_taskInterval); // ��������һ�봥�����������ʧ�ܣ��ļ�д��ȫ�������
        }
    }

    void taskHandle()
    {
        m_future = QtConcurrent::run([this] {
            LOGFMTI("[PriorityTaskQueue] QtConcurrent start!!");
            while (m_runTaskQueue) {

                // ����µ�����ʱ���Ȳ�����
                if (m_startTask == false) {
                    QThread::msleep(50);
                    continue;
                }

                if (!m_runTaskQueue)
                    return;

                // �����е������Ѿ������������ => continue
                if ((m_taskList.isEmpty() || runningSize() == m_maxRunTask)) {
                    // LOGFMTI("[ScriptThread] Execute Script m_maxRunTask, %d!!", m_maxRunTask);
                    QThread::msleep(50);
                    continue;
                }

                Worker worker = popTaskWorker();

                if (worker != nullptr && !worker->isRunning()) {
                    // �����Ѿ��Ƴ������ټ���ִ��
                    if (!checkTaskExist(worker->getTaskId())) {
                        continue;
                    }
                    worker->startTask();
                }

                QThread::msleep(m_taskInterval); // ��������һ�봥�����������ʧ�ܣ��ļ�д��ȫ�������
            }
        });
    }

private:
    QMutex *m_mutex;
    QList<Worker> m_taskList;
    QVector<qint64> m_taskTable;

    int m_maxRunTask = 1;
    volatile bool m_runTaskQueue;
    volatile bool m_startTask;

    int m_taskInterval = 1000;
    QFuture<void> m_future;

    bool m_reserve; // �Ƿ�����
};

#endif // !TASKQUEUE_H


