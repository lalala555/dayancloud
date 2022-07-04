#include "stdafx.h"
#include "WSConfig.h"
#include "Common/StringUtil.h"
#include "Common/QXmlReader.h"

bool WSConfig::init()
{
    QString appPath = QCoreApplication::applicationDirPath().append("/").append(WSCONFIG_FILE);
    QString config_path = appPath.toLocal8Bit();

    QXmlReader qdoc;
    if(!qdoc.loadXml(appPath)){
        return false;
    }

    if(!qdoc.getElementByTagName("Version").isNull()){
        m_wsVersion = qdoc.getElementByTagName("Version").text();
    }

    if (!qdoc.getElementByTagName("Version").isNull()) {
        m_wsHostUrl = qdoc.getElementByTagName("HostUrl").text();
    }

    return true;
}

QString WSConfig::getWSHostUrl() const
{
    return m_wsHostUrl;
}

QString WSConfig::getWSVersion() const
{
    return m_wsVersion;
}

bool WSConfig::parse_site(const QString &server_list_xml)
{
    QXmlReader doc;
    if (!doc.loadXml(server_list_xml))
        return false;

    QDomElement root = doc.getRootElement();
    if (root.isNull())
        return false;

    QDomNode node = root.firstChild();
    while(!node.isNull()) {
        QDomElement elem = node.toElement();
        if(elem.isNull()) continue;

        if(elem.tagName() == "platforms") {
            site.host = elem.attribute("host");
            site.accept_version = elem.attribute("accept_version");
            site.content_digest = elem.attribute("hash");

            QDomNodeList platList = elem.childNodes();
            for(int i = 0; i < platList.size(); i++){
                QDomNode plat = platList.at(i);
                if (plat.isNull()) continue;
                
                QDomElement info = plat.toElement();
                if(info.isNull()) continue;

                platform_info platform;
                platform.id = info.attribute("id").toInt();
                platform.name = info.attribute("name");
                platform.alias = info.attribute("alias");
                platform.tooltip = info.attribute("tooltip");
                platform.system_status = (enPlatformStatus)info.attribute("system_status").toInt();
                platform.gpu_flag = info.attribute("gpu") == "true";
                platform.qingyun_flag = info.attribute("qingyun") == "true";
                platform.is_default = info.attribute("default") == "true";
                platform.argument_mode = info.attribute("argument_mode").toInt();
                site.platforms.push_back(platform);
            }
        } else if(elem.tagName() == "softwares") {
            QDomNodeList softwares = elem.childNodes();
            for (int i = 0; i < softwares.size(); i++) {
                QDomNode softnode = softwares.at(i);
                if (softnode.isNull()) continue;
                
                QDomElement soft = softnode.toElement();
                if (soft.isNull()) continue;

                CGSoftware software;
                software.id = soft.attribute("id").toInt();
                software.name = soft.attribute("name");
                software.suffix = soft.attribute("suffix");
                software.script = soft.attribute("script");
                software.logo = soft.attribute("logo");
                software.linux_support = soft.attribute("linux_support").toInt();

                bool enabled = soft.attribute("enabled") == "true";
                if (enabled) {
                    cg_softwares.push_back(software);
                    cg_support_softwares[software.id] = software;

                    QList<QString> types = software.suffix.split(",");
                    for (auto it = types.begin(); it != types.end(); ++it) {
                        cg_support_types[*it] = software;
                    }
                }
            }
        } else if (elem.tagName() == "links") {
            QDomNodeList links = elem.childNodes();
            for (int i = 0; i < links.size(); i++) {
                QDomNode linknode = links.at(i);
                if (linknode.isNull()) continue;

                QDomElement link = linknode.toElement();
                if (link.isNull()) continue;

                m_url_map[link.tagName()] = link.text();
            }
        } else if(elem.tagName() == "keymap") {
            QDomNodeList keys = elem.childNodes();
            for(int i = 0; i < keys.size(); i++) {
                QDomNode keynode = keys.at(i);
                if(keynode.isNull()) continue;

                QDomElement key = keynode.toElement();
                if(key.isNull()) continue;

                m_dict[key.tagName()] = key.text();
            }
        }
        node = node.nextSibling();
    }

    return true;
}

int WSConfig::get_default_platform(int ptid)
{
    foreach (auto platform, site.platforms) {
        if(platform.id == ptid)
            return ptid;
    }

    return site.platforms[0].id;
}

platform_info WSConfig::get_platform(int ptid)
{
    platform_info pi;

    foreach (auto platform, site.platforms) {
        if(ptid == platform.id) {
            pi = platform;
            break;
        }
    }

    return pi;
}

QString WSConfig::get_host_url()
{
    return site.host;
}

QString WSConfig::get_accept_version()
{
    return site.accept_version;
}

void WSConfig::get_content_degist(QString& content, QString& degist)
{
    content = site.content;
    degist = site.content_digest;
}

bool WSConfig::is_gpu_platform(int ptid)
{
    bool is_gpu = false;

    platform_info platform = get_platform(ptid);

    return platform.gpu_flag;
}

int WSConfig::get_argument_mode(int ptid)
{
    platform_info platform = get_platform(ptid);

    return platform.argument_mode;
}

bool WSConfig::is_qingyun_platform(int ptid)
{
    bool is_gpu = false;

    platform_info platform = get_platform(ptid);

    return platform.qingyun_flag;
}

QString WSConfig::get_link_address(const QString& key)
{
    return m_url_map.value(key, QString());
}

QString WSConfig::get_key_value(const QString& key)
{
    return m_dict.value(key, QString());
}
