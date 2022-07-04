#pragma once

#include <QThread>
#include "kernel/render_struct.h"

class LoadConfigDataThread : public QThread
{
    Q_OBJECT

public:
    LoadConfigDataThread(const ConfigAnalyzeParams& params, QObject *parent = 0);
    ~LoadConfigDataThread();
    QList<ConfigDataItem*> getConfigDataItems();
    qint64 getTaskId();
    int filePreCheck();

protected:
    virtual void run();

private slots:
    void onLoadConfigDataFinished(int code);
    int getFieldOrderIndex(const QString& fieldName);

signals:
    void loadConfigDataFinished(int code);
    void loadDataFinished(int code);

private:
    bool checkRowNumEnable(const QString& line);

private:
    QList<ConfigDataItem*> m_dataItems;
    ConfigAnalyzeParams m_params;
    QStringList m_fieldNames;
};
