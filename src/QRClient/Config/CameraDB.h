#pragma once

#include <QObject>

class CameraDB : public QObject
{
    Q_OBJECT
private:
    explicit CameraDB(QObject *parent = 0);

public:
    static CameraDB* getInstance()
    {
        static CameraDB instance;
        return &instance;
    }
    ~CameraDB();

    void init();
    QString getSensorSize(const QString& camera);

private:
    QMap<QString, qreal> m_sensorDatabase;
};
