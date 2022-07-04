#include "stdafx.h"
#include <QMetaType>

struct MetaTypeRegistration {
    MetaTypeRegistration()
    {
        qRegisterMetaType<intptr_t>("intptr_t");
        qRegisterMetaType<QJsonObject>("QJsonObject");
    }
};

static MetaTypeRegistration staticInstance;

Q_DECLARE_METATYPE(intptr_t)