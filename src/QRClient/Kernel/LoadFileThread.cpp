#include "stdafx.h"
#include "LoadFileThread.h"
#include "Common/StringUtil.h"
#include "Common/FileUtil.h"
#include "Kernel/IPacket.h"

LoadFileThread::LoadFileThread(void)
    : m_filesDb(nullptr)
{
}

LoadFileThread::~LoadFileThread(void)
{
    m_filesDb = nullptr;
}

void LoadFileThread::run()
{
    if (m_uploadJsonPath.isEmpty()) {
        LOGFMTE("[run] Upload josn file is not exsit!");
        return;
    }
        
    // block to load files
    add_texture_file_list();

    CGlobalHelp::Response(UM_LOADFILE_FINISHED, m_taskId, 0);
}

bool LoadFileThread::add_texture_file_list()
{
    bool bret = false;

    QJsonObject upload_json;
    bret = RBHelper::loadJsonFile(m_uploadJsonPath, upload_json);
    if(!bret) return false;

    if (m_filesDb == nullptr) {
        LOGFMTE("[add_texture_file_list] File Database is NULL!");
        return false;
    } 
    if (!m_filesDb->execSqlPrepare()) {
        LOGFMTE("[add_texture_file_list] Database exec sql prepare failed!");
        return false;
    } 
    m_filesDb->operationBegin();

    if(upload_json.contains("asset") && upload_json["asset"].isArray()) {
        QJsonArray asset_array = upload_json["asset"].toArray();

        int count = 0;
        foreach (const QJsonValue& v, asset_array) {
            QVariantMap file_map = v.toObject().toVariantMap();

            QString local_path = QDir::toNativeSeparators(file_map.value("local").toString()).trimmed();
            QString remote_path = QDir::toNativeSeparators(file_map.value("server").toString()).trimmed();

            QString userDir = RBHelper::getUserStorageFolder(TYPE_TEXT);
            QString fixed_remote_path = QString("%1%2").arg(userDir).arg(remote_path);

            if (!local_path.isEmpty() && !remote_path.isEmpty()) {
                add_texture_to_filelist(local_path, fixed_remote_path);
            } 
        }
    }

    if(upload_json.contains("scene")) {
        if (upload_json["scene"].isArray()) {
            QJsonArray files_array = upload_json["scene"].toArray();

            foreach(const QJsonValue& v, files_array)
            {
                QVariantMap file_map = v.toObject().toVariantMap();
                QString local_path = QDir::toNativeSeparators(file_map.value("local").toString()).trimmed();
                QString remote_path = QDir::toNativeSeparators(file_map.value("server").toString()).trimmed();

                QString userDir = RBHelper::getUserStorageFolder(TYPE_SCENE);
                QString fixed_remote_path = QString("%1%2").arg(userDir).arg(remote_path);

                if (!local_path.isEmpty() && !remote_path.isEmpty()) {
                    add_texture_to_filelist(local_path, fixed_remote_path, TYPE_SCENE);
                }    
            }
        } else if (upload_json["scene"].isObject()) {
            QJsonObject scene_obj = upload_json["scene"].toObject();
            QString local_path = QDir::toNativeSeparators(scene_obj["local"].toString()).trimmed();
            QString remote_path = QDir::toNativeSeparators(scene_obj["server"].toString()).trimmed();

            QString userDir = RBHelper::getUserStorageFolder(TYPE_SCENE);
            QString fixed_remote_path = QString("%1%2").arg(userDir).arg(remote_path);

            if (!local_path.isEmpty() && !remote_path.isEmpty()) {
                add_texture_to_filelist(local_path, fixed_remote_path, TYPE_SCENE);
            }   
        }
    }

    if (upload_json.contains("contrl_point") && upload_json["contrl_point"].isArray()) {
        QJsonArray coord_array = upload_json["contrl_point"].toArray();

        int count = 0;
        foreach(const QJsonValue& v, coord_array) {
            QVariantMap file_map = v.toObject().toVariantMap();

            QString local_path = QDir::toNativeSeparators(file_map.value("local").toString()).trimmed();
            QString remote_path = QDir::toNativeSeparators(file_map.value("server").toString()).trimmed();

            QString userDir = RBHelper::getUserStorageFolder(TYPE_TEXT);
            QString fixed_remote_path = QString("%1%2").arg(userDir).arg(remote_path);

            if (!local_path.isEmpty() && !remote_path.isEmpty()) {
                add_texture_to_filelist(local_path, fixed_remote_path);
            }
        }
    }

    if (upload_json.contains("pos_file") && upload_json["pos_file"].isArray()) {
        QJsonArray coord_array = upload_json["pos_file"].toArray();

        int count = 0;
        foreach(const QJsonValue& v, coord_array) {
            QVariantMap file_map = v.toObject().toVariantMap();

            QString local_path = QDir::toNativeSeparators(file_map.value("local").toString()).trimmed();
            QString remote_path = QDir::toNativeSeparators(file_map.value("server").toString()).trimmed();

            QString userDir = RBHelper::getUserStorageFolder(TYPE_TEXT);
            QString fixed_remote_path = QString("%1%2").arg(userDir).arg(remote_path);

            if (!local_path.isEmpty() && !remote_path.isEmpty()) {
                add_texture_to_filelist(local_path, fixed_remote_path);
            }
        }
    }

    if (upload_json.contains("kml_file") && upload_json["kml_file"].isArray()) {
        QJsonArray coord_array = upload_json["kml_file"].toArray();

        int count = 0;
        foreach(const QJsonValue& v, coord_array) {
            QVariantMap file_map = v.toObject().toVariantMap();

            QString local_path = QDir::toNativeSeparators(file_map.value("local").toString()).trimmed();
            QString remote_path = QDir::toNativeSeparators(file_map.value("server").toString()).trimmed();

            QString userDir = RBHelper::getUserStorageFolder(TYPE_TEXT);
            QString fixed_remote_path = QString("%1%2").arg(userDir).arg(remote_path);

            if (!local_path.isEmpty() && !remote_path.isEmpty()) {
                add_texture_to_filelist(local_path, fixed_remote_path);
            }
        }
    }

    if (upload_json.contains("block_files") && upload_json["block_files"].isArray()) {
        QJsonArray coord_array = upload_json["block_files"].toArray();

        int count = 0;
        foreach(const QJsonValue& v, coord_array) {
            QVariantMap file_map = v.toObject().toVariantMap();

            QString local_path = QDir::toNativeSeparators(file_map.value("local").toString()).trimmed();
            QString remote_path = QDir::toNativeSeparators(file_map.value("server").toString()).trimmed();

            QString userDir = RBHelper::getUserStorageFolder(TYPE_TEXT);
            QString fixed_remote_path = QString("%1%2").arg(userDir).arg(remote_path);

            if (!local_path.isEmpty() && !remote_path.isEmpty()) {
                add_texture_to_filelist(local_path, fixed_remote_path);
            }
        }
    }

    m_filesDb->operationCommit();

    return true;
}

bool LoadFileThread::add_texture_to_filelist(const QString& localPath, const QString& remotePath, enAssetType type)
{
    UncompleteFile file;
    file.status = FILE_NOT_START;
    file.status += FILE_CHECK_SUCCESS;
    file.projectid       = m_projectId;
    file.projectsymbol   = m_projectSymbol;
    file.local_file      = localPath;
    file.remote_file     = remotePath;
    file.taskid          = m_taskId;
    file.rootid          = QString::number(m_mapUploadRoot[type]);
    file.pathMd5         = String::getStringMd5(localPath + remotePath);
    file.loadtexturefile = 1;
    file.scene           = m_sencePath;
    file.framename       = m_senceName;
    file.camera          = m_camera;
    file.updatetime      = QString::number(QDateTime::currentDateTime().toTime_t());

    qint64 fileSize = 0;
    QFileInfo fileInfo(localPath);
    if(fileInfo.exists()) {
        fileSize = fileInfo.size();
    }
    file.filesize = fileSize;

    // 单个文件属性(主要是第一次保存的时候有用)
    if(type == TYPE_TEXT) {
        file.filetype = TYPE_SCENE;
    } else if(type == TYPE_CFG) {
        file.filetype = TYPE_CFG;
    }

    if(m_filesDb != nullptr) {
        m_filesDb->appendFile(file);
    }

    return true;
}

void LoadFileThread::setAddFilesParam(const QString& strfullpath, t_taskid taskId, int projectId,
                                      const QString& projectSymbol)
{
    m_taskId = taskId;
    m_projectId = projectId;
    m_projectSymbol = projectSymbol;
    m_uploadJsonPath = strfullpath;
}

void LoadFileThread::setUploadRootMap(QMap<int, int> mapUploadRoot)
{
    m_mapUploadRoot = mapUploadRoot;
}

void LoadFileThread::setSeneceInfo(const QString& senceName, const QString& sencePath, const QString& camera)
{
    m_senceName = senceName;
    m_sencePath = sencePath;
    m_camera    = camera;
}
