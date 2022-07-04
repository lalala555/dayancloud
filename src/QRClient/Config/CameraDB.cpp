#include "stdafx.h"
#include "CameraDB.h"
#include "Common/csv.h"

CameraDB::CameraDB(QObject *parent)
    : QObject(parent)
{
}

CameraDB::~CameraDB()
{
}

void CameraDB::init()
{
    QList<QStringList> sensors = CSV::parseFromFile(":/Resource/sensor_database.csv");
    for(int i = 0; i < sensors.count(); i++) {
        auto sensor = sensors.at(i);
        m_sensorDatabase[sensor[0] + sensor[1]] = sensor[2].toFloat();
    }
}

QString CameraDB::getSensorSize(const QString & camera)
{
    float sensor_size = 0.0f;
    if(m_sensorDatabase.find(camera) != m_sensorDatabase.end()) {
        sensor_size = m_sensorDatabase[camera];
    }

    return QString::number(sensor_size, 'f', 2);
}
