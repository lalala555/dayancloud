#include "stdafx.h"
#include "UpdateWorker.h"
#include <QCoreApplication>
#include <QFileInfo>

UpdateCheckWorker::UpdateCheckWorker(QString strUpdater, QString version)
    : m_updater(0)
    , m_strUpdater(strUpdater)
    , m_version(version)
{

}

UpdateCheckWorker::~UpdateCheckWorker()
{
    delete m_updater, m_updater = nullptr;
}

void UpdateCheckWorker::doUpdateWork(void* parameter)
{
    int exitCode = 0;

    QMutexLocker lock(&mutex);

    UpdateTag* ut    = (UpdateTag*)parameter;
    QString action   = ut->action;
    int flag         = ut->flag;
    QObject* reciver = ut->reciver;
    QFileInfo fileUpdater(m_strUpdater);

    QString appPath = QCoreApplication::applicationDirPath();

    QStringList strArguList;
    strArguList << appPath << QCoreApplication::applicationName() + APP_SUFFIX << STRFILEVERSION << QString::number(1) << action << LocalConfig::getInstance()->defaultProduct
                << WSConfig::getInstance()->getWSVersion() << WSConfig::getInstance()->getWSHostUrl() + CRCLIENT2_URL << "restart";//
    if(m_updater == nullptr) {
        m_updater = new RayUpdate(strArguList);
    }

    connect(m_updater, &RayUpdate::dataArrived, this, &UpdateCheckWorker::onDataArrived);//汇报进度
    connect(m_updater, &RayUpdate::finished, this, &UpdateCheckWorker::resultReady); //告诉你现在这个任务已经完了
    connect(m_updater, &RayUpdate::updateText, this, &UpdateCheckWorker::onUpdateTextArrived);//告诉你升级的详情，更新了什么东东

    m_updater->start();

    delete ut, ut = 0;
}


void UpdateCheckWorker::pauseUpdateWork()
{
    m_updater->stop();
    //delete m_updater;
    //m_updater = nullptr;
}

void UpdateCheckWorker::onUpdateTextArrived(const QMap<int, UPDATE_TEXT>& data, const QString& version)
{
    emit updateTextArrived(data, version);
}

/***********************************************************/
/***********    <cmd><code>[\s\S]+?</cmd>       ************/
/***********************************************************/
// strip
int ExtractCommand(const QString& strCmd, QStringList& arr)
{
    QRegExp rx("<cmd><code>(.+)</cmd>");
    rx.setMinimal(true);
    int count = 0;
    int pos = 0;
    while((pos = rx.indexIn(strCmd, pos)) != -1) {
        ++count;
        pos += rx.matchedLength();
        arr << rx.cap(0);
    }

    return arr.size();
}

void UpdateCheckWorker::onDataArrived(const QString& data)
{
    emit dataArrived(data);
}

