#include "stdafx.h"
#include "TransfomEngineFactory.h"
#include "EngineRaySync.h"

QString TransEngineFactory::defaultEngine()
{
    return QString("RaySync");
}

void TransEngineFactory::RegisterEngines()
{
    m_mapEngine["RaySync"] = new CRaysyncEngine;
}

void TransEngineFactory::DestoryEngines()
{
    qDeleteAll(m_mapEngine);
}

AbstractTransEngine* TransEngineFactory::createEngine(const QString &mode)
{
    QString engineName = mode;

    AbstractTransEngine* engine = nullptr;
    auto it = m_mapEngine.find(engineName);
    if (it == m_mapEngine.end()) {
        engineName = defaultEngine();
        engine = m_mapEngine[engineName];
    } else {
        engine = it.value();
    }

    return engine;
}

QVector<QString> TransEngineFactory::getSupportEngineNames(const QVector<QString> engineNames)
{
    QVector<QString> supNames;
    
    // 求交集，用于获取客户端和服务器都支持的引擎名字
    QList<QString> names = m_mapEngine.keys();
    foreach (QString name, names) {
        if (engineNames.contains(name)){
            supNames.append(name);
        }
    }

    return supNames;
}
