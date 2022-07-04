#include "stdafx.h"
#include "Process.h"
#include <QProcess>

Process::Process(QObject *parent)
    : QObject(parent)
{
    QObject::connect(&m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(handleFinished(int, QProcess::ExitStatus)));
    QObject::connect(&m_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(handleError(QProcess::ProcessError)));
    QObject::connect(&m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(readyReadStandardOutput()));
    QObject::connect(&m_process, SIGNAL(readyReadStandardError()), this, SLOT(readyReadStandardError()));
}

int Process::startSync(const QString &program, const QStringList &arguments)
{
    m_arguments = arguments;

    m_process.start(program, m_arguments);
    m_process.waitForFinished(-1);

    return m_process.exitCode();
}

void Process::start(const QString &program, const QStringList &arguments)
{
    m_arguments = arguments;

    m_process.start(program, m_arguments);
}

void Process::stop()
{
    m_process.close();
}

int Process::exitCode()
{
    return m_process.exitCode();
}

void Process::handleFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    emit finished(exitCode);
}

void Process::handleError(QProcess::ProcessError error)
{
    emit finished(0xFFEE);
}

void Process::readyReadStandardError()
{
    QProcess *p = (QProcess *)sender();
    QByteArray bytes = p->readAllStandardError();

    emit dataArrived(QString::fromLocal8Bit(bytes));
}

void Process::readyReadStandardOutput()
{
    QProcess *p = (QProcess *)sender();
    QByteArray bytes = p->readAllStandardOutput();

    emit dataArrived(QString::fromLocal8Bit(bytes));
}

