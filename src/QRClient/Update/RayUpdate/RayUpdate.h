#pragma once

#include "http.h"
#include <qmap.h>
#include <QTemporaryDir>
#include <QDomDocument>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include <QUrl>
#include <QUrlQuery>
#include <QLocalServer>
#include <QLocalSocket>

enum class EnUpdate  {
    UPSUCCESS = 0,
    NONEED = 1,
    UPERROR = 2,
    UPFORCED_ERROR
};

enum DataType
{
    TYPE_PROGRESS,
    TYPE_STATUS
};

class file_info {
public:
    QString src_path = "/";
    QString dst_path = "/";
    QString opt = "update"; //delete update add
    QString md5;
    QString name;
    bool need_dowload = false;// 删除操作不需要下载，下载完成也不需要，MD5一致的文件也不需要
    bool download_success = false;
};

class update_info {
public:
    QString min_version;
    QString new_version;
    QString forced;
    QString type;
    QString unsued;
    QMap<QString, file_info> files;
};

struct UPDATE_TEXT {
    QString describe;
    QString notice;
};

//整包更新作为lib嵌入主程序，差量更新是一个exe
class RayUpdate : public QObject {
    Q_OBJECT

public:
    RayUpdate(const QStringList args);
    ~RayUpdate();
    bool readUpdateXml();
    void start();
    void download(QUrl url);
    void quitUpdate(EnUpdate code);
    QString genUpdateSh(const QString &exe_dir, const QString & download_dir, const QString & tarname);
    void onDownloadSuccess();
    int compareVersion(QString version, QString version2);
    void stop();
    bool isStop();
private:
    void downloadPackage();//更新
    void checkLocalFileMd5();//更新
    bool checkMd5(QString path, QString MD5);
    bool isFullPackUpdate();
    void killProcess();
    bool initLocalServer();
    void sendData(int type, const QVariant& data);
signals:
    void finished(int code);
    void dataArrived(const QString& data);
    void updateText(const QMap<int, UPDATE_TEXT>& update_text, const QString& version);
public slots:
    void onDownloadDone(const int code, const QUrlQuery query);
    void onGetCheckDone(const int code, const QString filename);
    void onSpeedArrived(const QString& data);
    void newLocalConnection();
    void onReadyRead();
private:
    //update_xml_info update_xml_info_;
    QString download_host_;
    QString update_xml_name_;
    QString os_type_;
private:
    Http http_;
    QString install_path_;

    QString process_name_;
    QString product_name_;
    QString old_version_;
    QString platform_; // test or release
    QString action_;
    QString update_type_;//update check download
    QString full_pack_url_;
    update_info update_info_;
    int retry_times_;
    bool need_restart_;
    bool is_stoped_;
private:
    int des_lang_id_;
    QLocalServer server_;
    QList<QLocalSocket*> sockets_;
};
