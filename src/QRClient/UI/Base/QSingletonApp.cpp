#include "stdafx.h"
#include "QSingletonApp.h"
#include <QLocalSocket>

QSingletonApp::QSingletonApp(const QString& guid)
    : m_serverName(guid)
    , m_localServer(Q_NULLPTR)
    , m_isRunning(false)
    , m_widget(Q_NULLPTR)
{
    initLocalConnection();
}

QSingletonApp::~QSingletonApp()
{
    delete m_localServer;
}

void QSingletonApp::exit()
{
    if(m_localServer) {
        if(m_localServer->isListening()) {
            m_localServer->close();
            m_localServer->deleteLater();
        }
    }
    /*if(m_widget){
        m_widget->deleteLater();
    }*/
    m_serverName.clear();
}

bool QSingletonApp::isAppRunning()
{
    return m_isRunning;
}

void QSingletonApp::setWidget(QWidget* widget)
{
    m_widget = widget;
}

void QSingletonApp::initLocalConnection()
{
    m_isRunning = false;
    QLocalSocket socket;
    socket.connectToServer(m_serverName);
    if(socket.waitForConnected(500)) {
        m_isRunning = true;
        QTextStream stream(&socket);
        QStringList args =QApplication::arguments();
        if(args.count() > 1)
            stream << args.last();
        else
            stream << QString();
        stream.flush();
        socket.waitForBytesWritten(2000);
        return;
    }
    // 连不上就重新new一个
    newLocalServer();
    socket.deleteLater();
}

void QSingletonApp::newLocalServer()
{
    m_localServer = new QLocalServer;
    m_localServer->setSocketOptions(QLocalServer::OtherAccessOption);
    connect(m_localServer, SIGNAL(newConnection()), this, SLOT(newLocalConnection()));
    if(!m_localServer->listen(m_serverName)) {
        if(m_localServer->serverError() == QAbstractSocket::AddressInUseError) {
            QLocalServer::removeServer(m_serverName);
            m_localServer->listen(m_serverName);
        }
    }
}

void QSingletonApp::newLocalConnection()
{
    QLocalSocket* socket = m_localServer->nextPendingConnection();
    if(socket == Q_NULLPTR) return;

    socket->waitForReadyRead(1000);
    QTextStream stream(socket);
    delete socket;

    if(m_widget != Q_NULLPTR) {
        m_widget->raise();
        m_widget->activateWindow();
        m_widget->setWindowState((m_widget->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        m_widget->show();
    }
}