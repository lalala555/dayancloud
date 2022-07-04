/***********************************************************************
* Module:  QSingletonApp.h
* Author:  yl
* Created: 2018/07/20 9:40:02
* Modifier: hqr
* Modified: 2018/07/20 9:40:02
* Purpose: Declaration of the class
***********************************************************************/
#pragma once
#ifndef QSINGLETONAPP_H_
#define QSINGLETONAPP_H_

#include <QObject>
#include <QLocalServer>

class QSingletonApp: public QObject
{
    Q_OBJECT

public:
    QSingletonApp(const QString& guid);
    virtual ~QSingletonApp();

    bool isAppRunning();
    void exit();

protected:
    void setWidget(QWidget* widget);

private:
    void initLocalConnection();
    void newLocalServer();

private slots:
    void newLocalConnection();

private:
    QString m_serverName;
    bool m_isRunning;
    QLocalServer* m_localServer;
    QWidget* m_widget;
};

#endif // QSINGLETONAPP_H_
