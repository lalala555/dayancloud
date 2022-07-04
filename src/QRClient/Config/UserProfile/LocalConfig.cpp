#include "stdafx.h"
#include "LocalConfig.h"
#include "Common/QXmlReader.h"

LocalConfig::LocalConfig()
{
}

LocalConfig::~LocalConfig()
{
}

void LocalConfig::init()
{
    QString configPath = QCoreApplication::applicationDirPath().append("/").append(WSCONFIG_FILE);

    try {
        QXmlReader doc;
        if (!doc.loadXml(configPath)) {
            LOGFMTW("[LocalConfig] load  '%s' failed!", qPrintable(configPath));
            return;
        }

        QDomElement root = doc.getRootElement();
        if (root.isNull()) return;
        QDomElement client = doc.getElementByTagName("Client");
        if (client.isNull()) return;

        QDomNodeList nodes = client.childNodes();
        for (int i = 0; i < nodes.size(); i++) {
            QDomNode node = nodes.at(i);
            if (node.isNull()) continue;
            QDomElement child = node.toElement();
            if (child.isNull()) continue;

            if (child.tagName() == "product") {
                defaultProduct = child.text();
            } else if (child.tagName() == "partner") {
                defaultPartner = child.text();
            } else if (child.tagName() == "loglevel") {
                defaultLogLevel = child.text().toInt();
            } else if (child.tagName() == "cmdport") {
                defaultCmdPort = child.text().toInt();
            } else if (child.tagName() == "cmdportmax") {
                defaultCmdPortMax = child.text().toInt();
            } else if (child.tagName() == "detail_interval") {
                detailInterval = child.text().toInt();
            } else if (child.tagName() == "request_timeout") {
                requestTimeout = child.text().toInt();
            } else if (child.tagName() == "speedtest_timeout") {
                speedTestTimeout = child.text().toInt();
            } else if(child.tagName() == "update_time_interval") {
                upgradeTimeInterval = child.text().toInt();
                if(upgradeTimeInterval < 1) upgradeTimeInterval = 30;
            } else if(child.tagName() == "refresh_time_interval") {
                refreshTimeInterval = child.text().toInt();
                if(refreshTimeInterval < 1) refreshTimeInterval = 2;
            } else if (child.tagName() == "base_product") {
                baseProduct = child.text();
            }
        }
    } catch(std::exception & e) {
        LOGFMTE("[LocalConfig] read %s failed(%s)!", WSCONFIG_FILE, e.what());
    }
}

void LocalConfig::uninit()
{
    removeConfigXml();
}

void LocalConfig::removeConfigXml()
{
    QString config_file  = getConfingXml();
    QFile::remove(config_file);
}

QString LocalConfig::getConfingXml()
{
    QString appdataPath = QProcessEnvironment::systemEnvironment().value("APPDATA");
    appdataPath.append("/rayvision/");
    appdataPath.append(defaultProduct);
    appdataPath.append("/");
    appdataPath = QDir::toNativeSeparators(appdataPath);

    RBHelper::makeDirectory(appdataPath);

    QString newName  = appdataPath.append(IPC_FILE);

    return newName;
}

void LocalConfig::writeServerPort(quint16 port)
{

    QString config_path = getConfingXml();

    QDomDocument doc;
    QFile file(config_path);

    QString errmsg;
    if (!doc.setContent(&file, &errmsg)) {
        // 不存在就创建
        QDomProcessingInstruction instruction;
        instruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
        doc.appendChild(instruction);

        QDomElement cfg = doc.createElement("config");

        QDomElement cmdport = doc.createElement("cmdport");
        QDomText text = doc.createTextNode(QString::number(port));
        cfg.appendChild(cmdport);
        cmdport.appendChild(text);

        doc.appendChild(cfg);

    } else {
        // 更新端口号
        QDomElement root = doc.documentElement();
        QDomNodeList list = root.elementsByTagName("cmdport");
        QDomNode node = list.at(list.size() - 1).firstChild();
        QDomNode oldnode = list.at(list.size() - 1).firstChild();
        node.setNodeValue(QString::number(port));
        QDomNode newnode = node;
        list.at(list.size() - 1).firstChild().replaceChild(newnode, oldnode);
    }

    QFile writeFile(config_path);
    if (!writeFile.open(QFile::WriteOnly | QFile::Truncate))
        return;

    QTextStream outstream(&writeFile);
    doc.save(outstream, 4);
    writeFile.close();
}
