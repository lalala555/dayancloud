#include "stdafx.h"
#include "UpdateManager.h"
#include <QApplication>
#include <QProcess>
#include <QDir>
#include "Common/QXmlReader.h"

UpdateManager::UpdateManager(QObject* parent /*= 0*/)
    : m_sink(0)
	, m_update_work(nullptr)
{
}

UpdateManager::~UpdateManager()
{
    cancelUpdate();
}

void UpdateManager::init(const QString & strUpdater, const QString & version)
{
    m_strUpdater = strUpdater;
    m_version    = version;
}

void UpdateManager::checkUpdate(int flag, QObject* sink)
{
    m_exitCode = -1;

    m_sink = sink;

    UpdateTag* ut = new UpdateTag;

    ut->action    = "check";
    ut->flag      = flag;
    ut->reciver   = this;

    this->checkAction(ut);
}

void UpdateManager::performUpdate(int flag, QObject * sink)
{
    m_exitCode = -1;

    m_sink = sink;

    UpdateTag* ut = new UpdateTag;

    ut->action    = "update";
    ut->flag      = flag;
    ut->reciver   = this;

    this->downloadAction(ut);
}

void UpdateManager::cancelUpdate()
{
    emit pauseWork();
}

void UpdateManager::waitUpdate()
{
    m_workerThread.wait();
}

int UpdateManager::exitCode()
{
    return m_exitCode;
}

void UpdateManager::checkAction(UpdateTag* ut)
{
    UpdateCheckWorker *worker = new UpdateCheckWorker(m_strUpdater, m_version);
    worker->moveToThread(&m_workerThread);
    connect(worker, &UpdateCheckWorker::updateTextArrived, this, &UpdateManager::handleUpdateText, Qt::DirectConnection);
    connect(&m_workerThread, &QThread::finished, worker, &QObject::deleteLater);

    connect(worker, &UpdateCheckWorker::resultReady, this, &UpdateManager::handleResults, Qt::DirectConnection);
    m_workerThread.start();

    QMetaObject::invokeMethod(worker,
                              "doUpdateWork",
                              Qt::AutoConnection,
                              Q_ARG(void *, ut));

}

void UpdateManager::downloadAction(UpdateTag * ut)
{
    UpdateCheckWorker *worker = new UpdateCheckWorker(m_strUpdater, m_version);
    worker->moveToThread(&m_workerThread);

    connect(this, SIGNAL(cmdArrived(const QString&)), m_sink, SLOT(onCmdArrived(const QString&)));
    connect(this, &UpdateManager::operate, worker, &UpdateCheckWorker::doUpdateWork);
    connect(this, &UpdateManager::pauseWork, worker, &UpdateCheckWorker::pauseUpdateWork);
    connect(&m_workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(worker, &UpdateCheckWorker::resultReady, this, &UpdateManager::handleResults);
    connect(worker, &UpdateCheckWorker::dataArrived, this, &UpdateManager::handleCmdArrived);

    m_workerThread.start();

    emit operate((void*)ut);
}

void UpdateManager::handleResults(int errorCode)
{
    m_exitCode = errorCode;

    m_workerThread.quit();

    emit resultReady(errorCode);
}

void UpdateManager::handleCmdArrived(const QString& data)
{
    emit cmdArrived(data);
}

void UpdateManager::handleUpdateText(const QMap<int, UPDATE_TEXT>& data,  const QString& version)
{
    m_version = version;
    for (auto i = data.begin(); i != data.end(); ++i) {
        m_langid_text[i.key()] = i.value();
    }
}

// 获取对应语言的更新信息
QString getVersionLangText(QDomElement ma, int langId)
{
    QString strData;
    QDomNodeList langs = ma.childNodes();
    for(int i = 0; i < langs.size(); i++){
        QDomNode node = langs.at(i);
        if (node.isNull()) continue;
        QDomElement lang = node.toElement();
        if(lang.isNull()) continue;

        int langid = lang.attribute("itemid").toInt();
        if(langid == langId){
            strData = lang.text();
        }
    }
    
    return strData;
}

QString UpdateManager::getVersionText(QString& strVersionInfo, int langId)
{

    if (m_langid_text.find(langId) != m_langid_text.end()) {
        strVersionInfo = m_langid_text[langId].describe;
    }
    return m_version;

    QString updateXml = QApplication::applicationDirPath() + "/update/path.xml";
    updateXml = QDir::toNativeSeparators(updateXml);
    std::string strPath = updateXml.toStdString();

    QString strVersion;

    QXmlReader xml;
    if (!xml.loadXml(updateXml))
        return "";
    
    if(!xml.getElementByTagName("update_xml").isNull()) {
        QString strUpdateXml = xml.getElementByTagName("update_xml").text();

        if(!xml.getElementByTagName("tmp_files_root").isNull()){
            QString strTmpRoot = xml.getElementByTagName("tmp_files_root").text();

            QString appdata = QProcessEnvironment::systemEnvironment().value("APPDATA");
            appdata.append("\\rayvision\\");
            appdata.append(strTmpRoot);
            QString wcsPathXml = appdata;
            wcsPathXml += "\\" + strUpdateXml;

            QXmlReader xml2;
            if (!xml2.loadXml(wcsPathXml))
                return "";

            if(!xml.getElementByTagName("new_version").isNull()){
                strVersion = xml.getElementByTagName("new_version").text();
            }

            if (!xml.getElementByTagName("description").isNull()) {
                QDomElement des = xml.getElementByTagName("description");
                strVersionInfo = getVersionLangText(des, langId);
            }
        } 
    }

    return strVersion;
}

//获取对应语言的公告信息
QString getNoticeText(QDomElement ma, int langId)
{
    QString strData;
    QDomNodeList langs = ma.childNodes();
    for (int i = 0; i < langs.size(); i++) {
        QDomNode node = langs.at(i);
        if (node.isNull()) continue;
        QDomElement lang = node.toElement();
        if (lang.isNull()) continue;

        int langid = lang.attribute("itemid").toInt();
        if (langid == langId) {
            strData = lang.text();
        }
    }
    return strData;
}

QString UpdateManager::getAnnounceText(int langId)
{
    QString updateXml = QApplication::applicationDirPath() + "\\update\\path.xml";
    updateXml = QDir::toNativeSeparators(updateXml);
    std::string strPath = updateXml.toStdString();

    QString strRet;

    QXmlReader xml;
    if (!xml.loadXml(updateXml))
        return "";

    if (!xml.getElementByTagName("update_xml").isNull()) {
        QString strUpdateXml = xml.getElementByTagName("update_xml").text();

        if (!xml.getElementByTagName("tmp_files_root").isNull()) {
            QString strTmpRoot = xml.getElementByTagName("tmp_files_root").text();

            QString appdata = QProcessEnvironment::systemEnvironment().value("APPDATA");
            appdata.append("/rayvision/");
            appdata.append(strTmpRoot);
            QString wcsPathXml = appdata;
            wcsPathXml += "/" + strUpdateXml;

            QXmlReader xml2;
            if (!xml2.loadXml(wcsPathXml))
                return "";

            if (!xml.getElementByTagName("notice").isNull()) {
                QDomElement notice = xml.getElementByTagName("notice");
                strRet = getNoticeText(notice, langId);
            }
        }
    }

    return strRet;
}
