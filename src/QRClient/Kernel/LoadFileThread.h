/***********************************************************************
* Module:  LoadFileThread.h
* Author:  ftc
* Created: 2017/09/13 19:31:31
* Modifier: hqr
* Modified: 2017/09/13 19:31:31
* Purpose: Declaration of the class
***********************************************************************/
#pragma once
#include <QThread>
#include <QMutex>
#include "kernel/render_struct.h"
#include "Kernel/Upload/UploadFileListByDb.h"

class UploadFileListByDb;

class LoadFileThread: public QThread
{
public:
    LoadFileThread(void);
    ~LoadFileThread(void);
    void setAddFilesParam(const QString& strfullpath, t_taskid taskId, int projectId,
                          const QString& projectSymbol);
    void setUploadRootMap(QMap<int, int> mapUploadRoot);
    void setUploadListDb(UploadFileListByDb* db) { m_filesDb = db; }
    void setSeneceInfo(const QString& senceName, const QString& sencePath, const QString& camera);

protected:
    virtual void run();
private:
    bool add_texture_file_list();
    bool add_texture_to_filelist(const QString& localPath, const QString& remotePath, enAssetType type = TYPE_TEXT);

private:
    mutable QMutex mutex;
    int m_projectId;

    t_taskid m_taskId;
    QString m_projectSymbol;    //
    QString m_uploadJsonPath;   // upload.json路径
    QMap<int, int> m_mapUploadRoot;

    UploadFileListByDb* m_filesDb;
    QString m_senceName;
    QString m_sencePath;
    QString m_camera;
};

