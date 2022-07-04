#pragma once

#include <QThread>

class LoadBlockFileThread : public QThread
{
    Q_OBJECT

public:
    LoadBlockFileThread(const QString& filePath, qint64 taskId, QObject *parent = 0);
    ~LoadBlockFileThread();
    qint64 getTaskId();
    int startLoadFile();
    BLOCK_TASK::BlockExchange* getBlockExchangeInfo();

protected:
    virtual void run();

signals:
    void loadBlockFileFinished(int);

private:
    bool unzipXmlzFile(const QString& src, const QString& dest);
    QString unzipXmlFileTempPath();
    QString getUnzipToolPath();
    int loadXmlFileByDOM();
    int loadXmlFileByXAS();
	bool checkTiePoints();


private:
    QString m_filePath;
    qint64 m_taskId;
    BLOCK_TASK::BlockExchange* m_blockExchange;
	bool m_tilePoints;
};
