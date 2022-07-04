#pragma once

#include <QObject>
#include "Common/QDownloadManager.h"

class LoadCoordSystemConfig : public QObject
{
    Q_OBJECT
private:
    explicit LoadCoordSystemConfig(QObject *parent = 0);
public:
    static LoadCoordSystemConfig* getInstance()
    {
        static LoadCoordSystemConfig instance;
        return &instance;
    }
    ~LoadCoordSystemConfig();

    void initConfig();
    void loadConfig();
    QStringList getSystemConfig();

private:
    bool loadLocalConfig();
    bool loadOnlineConfig();
    bool readConfigJson(const QString& filePath);

private slots:
    void onDownloadFinished(int code, const QString& data);

private:
    QStringList m_configs;
    QDownloadManager m_downloadCfg;
};
