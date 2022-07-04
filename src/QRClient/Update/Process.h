/***********************************************************************
* Module:  Process.h
* Author:  hqr
* Modified: 2016/11/14 15:03:13
* Purpose: Declaration of the class
***********************************************************************/
#pragma once
#ifndef PROCESS_H_
#define PROCESS_H_

#include <QObject>
#include <QProcess>

class Process : public QObject
{
    Q_OBJECT

public:
    explicit Process(QObject *parent = 0);
    void start(const QString &program, const QStringList &arguments);
    int startSync(const QString &program, const QStringList &arguments);
    void stop();
    int exitCode();

signals:
    void dataArrived(const QString& data);
    int finished(int exitCode);

public slots:
    void handleFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void handleError(QProcess::ProcessError error);
    void readyReadStandardOutput();
    void readyReadStandardError();

private:
    QProcess m_process;
    QStringList m_arguments;
};


#endif // PROCESS_H_
