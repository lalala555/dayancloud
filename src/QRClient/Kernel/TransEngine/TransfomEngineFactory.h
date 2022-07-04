/***********************************************************************
* Module:  TransfomEngineFactory.h
* Author:  hqr
* Modified: 2016/11/14 15:07:21
* Purpose: Declaration of the class
***********************************************************************/
#ifndef TRANS_ENGINE_FACTORY
#define TRANS_ENGINE_FACTORY

#include "TransformEngine.h"

class AbstractTransEngineFactory
{
public:
    AbstractTransEngineFactory() {};
    virtual ~AbstractTransEngineFactory() {};

public:
    virtual AbstractTransEngine* createEngine(const QString &mode) = 0;
public:
};

class TransEngineFactory : AbstractTransEngineFactory
{
private:
    TransEngineFactory()
    {
        RegisterEngines();
    };
public:
    static TransEngineFactory* getInstance()
    {
        static TransEngineFactory instance;
        return &instance;
    }

    static void destory() { }

    ~TransEngineFactory()
    {
        DestoryEngines();
    };

public:
    QString defaultEngine();
    AbstractTransEngine* createEngine(const QString &mode);
    QVector<QString> getSupportEngineNames(const QVector<QString> engineNames);

private:
    void RegisterEngines();
    void DestoryEngines();
private:
    QMap<QString, AbstractTransEngine*> m_mapEngine;
};

#endif // TRANS_ENGINE_FACTORY

