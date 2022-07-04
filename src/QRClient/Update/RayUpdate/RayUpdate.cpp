#include "stdafx.h"
#include "stdafx.h"
#include "RayUpdate.h"
#include <QCryptographicHash>
#include <QUuid>
#include <QProcess>

RayUpdate::RayUpdate(const QStringList args)
{
    connect(&http_, SIGNAL(downloadDone(const int, const QUrlQuery)), SLOT(onDownloadDone(const int, const QUrlQuery)));
    connect(&http_, SIGNAL(getCheckDone(const int, const QString)), SLOT(onGetCheckDone(const int, const QString)));

    install_path_ = args[0];
    process_name_ = args[1];
    old_version_ = args[2];
    update_type_ = args[3];
    action_ = args[4];
    product_name_ = args[5];
    platform_ = args[6];
    download_host_ = args[7];
    if(download_host_.at(download_host_.size() - 1) != '/') {
        download_host_ = download_host_ + "/";
    }
    download_host_ = download_host_ + "update.php";  //下载 modify 20220418 update.php-> changelog.php
    need_restart_ = false;
    if(args.size() == 9 && args[8] == "restart") {
        need_restart_ = true;
    }
    retry_times_ = 0;
    des_lang_id_ = 0;


    if(update_type_.toInt() == 1) {
        connect(&http_, SIGNAL(speedReport(QString)), this, SLOT(onSpeedArrived(QString)));
    }

}

RayUpdate::~RayUpdate()
{

}

bool RayUpdate::readUpdateXml()
{
    //打开或创建文件
    QFile file("update.xml"); //相对路径、绝对路径、资源路径都行
    if(!file.open(QFile::ReadOnly)) {
        qDebug("open file:update.xml failed!");
        return false;
    }


    QDomDocument doc;
    if(!doc.setContent(&file)) {
        file.close();
        qDebug("set document fail!");
        return false;
    }
    file.close();

    QDomElement root = doc.documentElement(); //返回根节点
    qDebug() << root.nodeName();
    QDomNode node = root.firstChild(); //获得第一个子节点
    while(!node.isNull()) { //如果节点不空
        if(node.isElement()) { //如果节点是元素
            QDomElement e = node.toElement(); //转换为元素，注意元素和节点是两个数据结构，其实差不多
            QDomNodeList list = e.childNodes();
            for(int i = 0; i < list.count(); i++) { //遍历子元素，count和size都可以用,可用于标签数计数
                QDomNode n = list.at(i);
                qDebug() << e.tagName() << " " << e.text() << " " << e.attribute("update_xml");
                if(e.tagName() == "host") {
                    download_host_ = e.text();

                    return true;
                }
            }
        }
        node = node.nextSibling();
    }
    return false;
}


void RayUpdate::start()
{
    //if (!readUpdateXml()) {
    //    qDebug("read update.xml fail ,exit");
    //    exit(-1);
    //}
    is_stoped_ = false;
    qDebug() << "dowload_host_:" << download_host_;

    //QString dowload_xml_path = dowload_host_ + "?file=update.xml";
    ////dowload_xml_path = QDir::toNativeSeparators(dowload_xml_path);
    //qDebug() << "ss " << dowload_xml_path;

    //download(dowload_xml_path);
#ifdef Q_OS_WIN
    os_type_ = "windows";
#else Q_OS_LINUX
    os_type_ = "linux";
#endif
    QString check_type = update_type_; //检查整包，2检查增量更新
    if(check_type != "1" && check_type != "2") {
        qDebug() << "unknow check type! use -h to show usage!";
        quitUpdate(EnUpdate::UPERROR);
        return;
    }
	QString url = download_host_ + "?check=" + check_type + "&os_type=" + os_type_ + "&platform=" + platform_ + "&product=" + product_name_;
    http_.get(download_host_ + "?check=" + check_type + "&os_type=" + os_type_ + "&platform=" + platform_ + "&product=" + product_name_);

}

void RayUpdate::download(QUrl url)
{
    qDebug("begin dowload %s", url.toEncoded().constData());
    http_.doDownload(url);
}

void RayUpdate::quitUpdate(EnUpdate code)
{
    qDebug() << "Application quit with:" << static_cast<int>(code);
    if(isFullPackUpdate()) {
        if(isStop()) {
            return;
        }
        enum _UpdateStatus {
            update_process_is_exist,
            invalid_argument_count,
            get_root_directory_failure,
            parse_local_config_failure,
            download_update_xml_failure,
            parse_update_xml_failure,
            download_files_failure,
            replace_files_failure,
            kill_process_failure,
            create_process_failure,
            need_not_to_update,
            need_to_update,
            forced_to_update,
            download_success,
            update_success,
            server_is_unusable
        };
        switch(code) {
        case EnUpdate::UPSUCCESS:
        {
            if(update_info_.forced == "1") {
                emit finished(forced_to_update);
            } else {
                emit finished(need_to_update);
            }
        }
        break;
        case EnUpdate::NONEED:
            emit finished(need_not_to_update);
            break;
        case EnUpdate::UPERROR:
        case EnUpdate::UPFORCED_ERROR:
            emit finished(need_not_to_update);
            break;
        default:
            break;
        }
    } else {
        int rt = static_cast<int>(code) + update_info_.forced.toInt();
        if(need_restart_) {
            QProcess* myProcess = new QProcess();
            QStringList arguments;
            bool a = myProcess->startDetached(install_path_ + "/" + process_name_, arguments);
            qDebug() << "restart ... ret:" << a;
            delete  myProcess;
        }
        qApp->exit(rt);
    }

}

bool copyDir(const QString& source, const QString& destination, bool override)
{
    QDir directory(source);
    if(!directory.exists()) {
        return false;
    }


    QString srcPath = QDir::toNativeSeparators(source);
    if(!srcPath.endsWith(QDir::separator())) {
        srcPath += QDir::separator();
    }
    QString dstPath = QDir::toNativeSeparators(destination);
    if(!dstPath.endsWith(QDir::separator())) {
        dstPath += QDir::separator();
    }


    bool error = false;
    QStringList fileNames = directory.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
    for(QStringList::size_type i = 0; i != fileNames.size(); ++i) {
        QString fileName = fileNames.at(i);
        QString srcFilePath = srcPath + fileName;
        QString dstFilePath = dstPath + fileName;
        QFileInfo fileInfo(srcFilePath);
        if(fileInfo.isFile() || fileInfo.isSymLink()) {
            if(override) {
                QFile::setPermissions(dstFilePath, QFile::WriteOwner);
            }
            if(QFile::remove(dstFilePath)) {
                qDebug() << "remove " << dstFilePath << "success";
            } else {
                qDebug() << "remove " << dstFilePath << "fail";
            }
            QFile::copy(srcFilePath, dstFilePath);
        } else if(fileInfo.isDir()) {
            QDir dstDir(dstFilePath);
            dstDir.mkpath(dstFilePath);
            if(!copyDir(srcFilePath, dstFilePath, override)) {
                error = true;
            }
        }
    }


    return !error;
}

QString RayUpdate::genUpdateSh(const QString &exe_dir, const QString & download_dir, const QString & tarname)
{

    QString shname = download_dir + "/update.sh";
    std::cout << "update dir:" << download_dir.toStdString() << std::endl;
    QFile file(shname);
    if(!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug() << "open file update.sh faied!";
        return "";
    }
    QString sh = QString(
        "#!/bin/bash\n"
        "killall %1\n"
        "tar cf %2/%3_back.tar %4\n"
        "if [ $? -ne 0 ]; then\n"
        "   exit\n"
        "fi\n"
        "rm -rf %5/*\n"
        "tar zxf %6 -C %7\n"
        "if [ $? -ne 0 ]; then\n"
        "   tar xf %8/%9_back.tar %10\n"
        "fi\n"
        "sleep 2\n"
        //       "rm -rf %11\n"
    ).arg(process_name_).arg(download_dir).arg(process_name_).arg(install_path_).arg(install_path_).arg(download_dir + "/" + tarname).arg(install_path_)
        .arg(download_dir).arg(process_name_).arg(install_path_).arg(download_dir);
    QTextStream in(&file);
    in << sh;
    if(tarname.toLower().contains("renderbus")) {
        std::cout << (install_path_ + "/" + "renderbus.sh").toStdString();
        in << (install_path_ + "/" + "renderbus.sh").toLatin1();
    } else {
        std::cout << (install_path_ + "/" + "foxrenderfarm.sh").toStdString();
        in << (install_path_ + "/" + "foxrenderfarm.sh").toLatin1();
    }
    file.close();
    QString ch = "chmod 770 " + shname;
    system(ch.toStdString().c_str());
    return shname;
}

void RayUpdate::onDownloadSuccess()
{
    if(update_info_.type == "1") {
        auto file_info_pack = update_info_.files.constBegin();

#if defined(Q_OS_UNIX)
        if(file_info_pack->name.endsWith(".tar.gz")) {
            auto sh = genUpdateSh(install_path_, http_.download_path_, file_info_pack->name).toLatin1();
            std::cout << "update done.........\n";

            QProcess* myProcess = new QProcess();
            QStringList arguments;
            bool a = myProcess->startDetached(sh, arguments);
            delete  myProcess;
            exit(0);
        }
#endif
        //启动安装包
        QProcess* myProcess = new QProcess();
        QStringList arguments;
        bool a = myProcess->startDetached(http_.download_path_ + file_info_pack->dst_path + file_info_pack->name, arguments);
        delete  myProcess;
        killProcess();
        if(a) {
            quitUpdate(EnUpdate::UPSUCCESS);
        } else {
            quitUpdate(EnUpdate::UPERROR);
        }


    } else if(update_info_.type == "2") {
        QString tmp = http_.download_path_;
        QDir tmpdir(tmp);
        killProcess();
        for(auto i : update_info_.files) {
            if(i.opt == "delete") {
                if(QFile::remove(QDir::toNativeSeparators(install_path_ + i.dst_path + i.name))) {
                    qDebug() << "remove " + install_path_ + i.dst_path + i.name << " success!";
                } else {
                    qDebug() << "remove " + install_path_ + i.dst_path + i.name << " false!";
                }

            }

        }
        if(copyDir(tmp, install_path_, true)) {
            qDebug() << "copy dir:" << tmp << " to " << install_path_ << " success";
            quitUpdate(EnUpdate::UPSUCCESS);
        } else {
            qDebug() << "copy dir:" << tmp << " to " << install_path_ << " failed";
            quitUpdate(EnUpdate::UPERROR);
        }
    }
}

int RayUpdate::compareVersion(QString version, QString version2)
{

    if(version == version2) {
        return 0;
    }

    QStringList vers = version.split(".");
    QStringList vers2 = version2.split(".");


    if(vers.size() != vers2.size()) {
        qDebug() << "versio format error,not equit version1:" << version << " version2:" << version2;
        quitUpdate(EnUpdate::UPERROR);
    }
    for(int i = 0; i < vers.size(); i++) {
        if(vers.at(i) > vers2.at(i)) {
            return 1;
        } else if(vers.at(i) < vers2.at(i)) {
            return -1;
        }
    }
    return 0;//nerver go to here,disable warning
}

void RayUpdate::stop()
{
    disconnect();
    is_stoped_ = true;
    http_.stop();
}

bool RayUpdate::isStop()
{
    return is_stoped_;
}




void RayUpdate::downloadPackage()
{
    bool need_download = false;
    for(auto it = update_info_.files.begin(); it != update_info_.files.end(); it++) {
        if(!it->need_dowload) {
            continue;
        }
        it->need_dowload = false;
        need_download = true;
        if(isFullPackUpdate()) {
            full_pack_url_ = download_host_ + "?file=" + it->name + "&src_path=" + it->src_path + "&platform=" + platform_ + "&product=" + product_name_ + "&os_type=" + os_type_ + "&check=" + update_type_;
        }
        download(download_host_ + "?file=" + it->name + "&src_path=" + it->src_path + "&platform=" + platform_ +
                 "&product=" + product_name_ +
                 "&os_type=" + os_type_ + "&check=" + update_type_ +
                 "&dst_path=" + it->dst_path + "&uid=" + it.key());
    }

    if(!need_download) {
        qDebug() << "No File Need to Download";
        quitUpdate(EnUpdate::UPSUCCESS);
    }

}

void RayUpdate::checkLocalFileMd5()
{
    for(auto& i : update_info_.files) {
        if(i.opt == "update") {
            if(!checkMd5(install_path_ + i.dst_path + i.name, i.md5)) {
                i.need_dowload = true;
            } else {
                i.need_dowload = false;
            }
        } else if(i.opt == "add") {
            i.need_dowload = true;
        }
    }
}


bool RayUpdate::checkMd5(QString path, QString MD5)
{
    QFile sourceFile(path);
    qint64 fileSize = sourceFile.size();
    const qint64 bufferSize = 10240;

    if(sourceFile.open(QIODevice::ReadOnly)) {
        QByteArray ba = QCryptographicHash::hash(sourceFile.readAll(), QCryptographicHash::Md5);
        sourceFile.close();
        QString filemd5 = QString(ba.toHex());
        qDebug() << "file :" << path << endl << " MD5:" << filemd5 << "target MD5:" << MD5;
        return filemd5 == MD5;
    } else {
        qDebug() << "check MD5 open file:" << path << "error ";
    }

    return false;
}

bool RayUpdate::isFullPackUpdate()
{
    return update_type_ == "1";
}

void RayUpdate::killProcess()
{
#ifdef Q_OS_WIN
    QString cmd = "taskkill /F /im " + process_name_;
#else Q_OS_LINUX
    QString cmd = "killall " + process_name_;
#endif
    int ret = system(cmd.toStdString().c_str());
    qDebug() << "kill process " << cmd << " ret:" << ret;
}



//type 1 整包更新 2 增量更新

void RayUpdate::onGetCheckDone(const int code, const QString  reply)
{
    if(code != 0) {
        quitUpdate(EnUpdate::UPERROR);
        return;
    }
    QJsonParseError json_error;
    QJsonDocument update_json(QJsonDocument::fromJson(reply.toLocal8Bit(), &json_error));

    if(json_error.error != QJsonParseError::NoError) {
        qDebug() << "json error!";
        quitUpdate(EnUpdate::UPERROR);
        return;
    }
    QJsonObject rootObj = update_json.object();
    QStringList keys = rootObj.keys();
    if(!rootObj.contains("type")) {
        qDebug() << "json value not exist!";
        quitUpdate(EnUpdate::NONEED);
        return;
    }


    qDebug() << "type:" << rootObj["type"].toString();

    auto update_object = rootObj;
    if(update_object.contains("type") && update_object.contains("min_version") && update_object.contains("new_version")
       && update_object.contains("forced") && update_object.contains("data")) {

        update_info_.min_version = update_object["min_version"].toString();
        update_info_.new_version = update_object["new_version"].toString();
        update_info_.forced = update_object["forced"].toString();
        update_info_.type = update_object["type"].toString();

        auto des_value = update_object.value("description");
        auto notice_value = update_object.value("notice");
        if(des_value.isObject() && des_value.toObject().contains("language")
           && notice_value.isObject() && notice_value.toObject().contains("language")) {
            auto des_arr = des_value.toObject().value("language").toArray();
            auto notic_arr = notice_value.toObject().value("language").toArray();
            if(des_arr.size() == notic_arr.size()) {
                QMap<int, UPDATE_TEXT> update_text;
                for(int i = 0; i < des_arr.size(); i++) {
                    UPDATE_TEXT text;
                    text.describe = des_arr.at(i).toString();
                    text.notice = notic_arr.at(i).toString();
                    update_text[i] = text;
                }
                emit updateText(update_text, update_info_.new_version);
            }
        }

        auto data_value = update_object.value("data");

        if(data_value.isObject() && data_value.toObject().contains("folder")) {
            if(data_value.toObject().value("folder").isObject() && data_value.toObject().value("folder").toObject().contains("file")) {
                if(data_value.toObject().value("folder").toObject().contains("file")) {
                    if(data_value.toObject().value("folder").toObject()["file"].isArray()) {
                        auto file_arr = data_value.toObject().value("folder").toObject()["file"].toArray();
                        for(auto i : file_arr) {
                            if(i.toObject().value("@attributes").isObject()) {
                                auto attributes_obj = i.toObject().value("@attributes").toObject();
                                file_info info;

                                if(attributes_obj.contains("name") && attributes_obj.contains("md5")) {

                                    info.name = attributes_obj["name"].toString();
                                    info.md5 = attributes_obj["md5"].toString();

                                } else {
                                    qDebug() << "config error,not contains name md5 or other option";
                                    quitUpdate(EnUpdate::UPERROR);
                                    return;
                                }
                                if(attributes_obj.contains("src_path") && attributes_obj.contains("dst_path") && attributes_obj.contains("opt")) {

                                    info.src_path = attributes_obj["src_path"].toString();
                                    info.dst_path = attributes_obj["dst_path"].toString();
                                    info.opt = attributes_obj["opt"].toString();

                                }

                                update_info_.files[QUuid::createUuid().toString()] = info;
                            }
                        }

                    } else {//Not Array
                        auto file = data_value.toObject().value("folder").toObject()["file"];
                        if(file.toObject().value("@attributes").isObject()) {
                            auto attributes_obj = file.toObject().value("@attributes").toObject();
                            file_info info;

                            if(attributes_obj.contains("name") && attributes_obj.contains("md5")) {

                                info.name = attributes_obj["name"].toString();
                                info.md5 = attributes_obj["md5"].toString();

                            } else {
                                qDebug() << "config error,not contains name md5 or other option";
                                quitUpdate(EnUpdate::UPERROR);
                                return;
                            }
                            if(attributes_obj.contains("src_path") && attributes_obj.contains("dst_path") && attributes_obj.contains("opt")) {

                                info.src_path = attributes_obj["src_path"].toString();
                                info.dst_path = attributes_obj["dst_path"].toString();
                                info.opt = attributes_obj["opt"].toString();

                            }
                            update_info_.files[QUuid::createUuid().toString()] = info;
                        }

                    }
                }
            }
        }
    } else {
        qDebug() << "update fail,config file error!";
        quitUpdate(EnUpdate::UPERROR);
        return;
    }


    qDebug() << "dowload type:" << update_info_.type;

    //整包升级版本号一样不需要更新，差量更新版本号一样也要检查
    if(compareVersion(old_version_, update_info_.min_version) >= 0
       && (compareVersion(old_version_, update_info_.new_version) < 0
           || (!isFullPackUpdate() && compareVersion(old_version_, update_info_.new_version) == 0))) {
        if(action_ != "check") {
            checkLocalFileMd5();
            downloadPackage();
        } else {
            quitUpdate(EnUpdate::UPSUCCESS);
        }
    } else {
        qDebug() << "Do not update" << update_info_.min_version << " new version:" << update_info_.new_version;
        quitUpdate(EnUpdate::NONEED);

    }

}

void RayUpdate::onSpeedArrived(const QString& data)
{
    emit dataArrived(data + "|" + full_pack_url_);
}


void RayUpdate::onDownloadDone(const int code, const QUrlQuery query)
{
    qDebug() << "Download file done " << code;
    if(code < 0) {
        qDebug() << "error,download file error!";
        quitUpdate(EnUpdate::UPERROR);
        return;
    }
    if(code == 0) {
        qDebug() << "All file dowload End";
        onDownloadSuccess();
        return;
    }

    QString m_uid = query.queryItemValue("uid");
    if(m_uid.size() == 0) {
        return;
    }
    auto& i = update_info_.files[m_uid];

    if(checkMd5(http_.download_path_ + i.dst_path + i.name, i.md5)) {
        qDebug() << "check file:" << i.name << " done";
        i.download_success = true;
    } else {
        qDebug() << "check Md5 [" << i.dst_path + i.name << "] error" << retry_times_;
        retry_times_++;
        i.need_dowload = true;
        if(retry_times_ > 5) {
            qDebug() << "too many times:" << retry_times_ << " dowload,exit!";
            // exit(-1);
            return;
        } else {
            downloadPackage();
        }
        return;
    }

}

bool RayUpdate::initLocalServer()
{
    QString serverName = QCoreApplication::applicationName();
    QByteArray byte_array;
    byte_array.append(serverName);
    QByteArray hash_byte_array = QCryptographicHash::hash(byte_array, QCryptographicHash::Md5);
    QString sha1 = hash_byte_array.toHex();
    serverName = serverName + sha1;

    server_.setSocketOptions(QLocalServer::OtherAccessOption);
    connect(&server_, SIGNAL(newConnection()), this, SLOT(newLocalConnection()));
    if(!server_.listen(serverName)) {
        if(server_.serverError() == QAbstractSocket::AddressInUseError) {
            QLocalServer::removeServer(serverName);
            server_.listen(serverName);
        }
    }

    return true;
}

void RayUpdate::newLocalConnection()
{
    QLocalSocket* m_socket = server_.nextPendingConnection();
    if(m_socket == NULL) {
        return;
    }

    sockets_.append(m_socket);
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

void RayUpdate::onReadyRead()
{
    // todo
}

void RayUpdate::sendData(int type, const QVariant& data)
{
    QString datatype = "status";
    switch(type) {
    case TYPE_PROGRESS:
        datatype = "progress";
        break;
    case TYPE_STATUS:
        datatype = "status";
        break;
    default:
        break;
    }
    QJsonObject object;
    object.insert("type", datatype);
    object.insert("data", data.toString());
    QByteArray jsondata = QJsonDocument(object).toJson(QJsonDocument::Compact);

    foreach(QLocalSocket* m_socket, sockets_)
    {
        if(m_socket->isWritable()) {
            m_socket->write(jsondata.constData());
        }
    }
}
