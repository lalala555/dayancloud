#include "stdafx.h"
#include "LangTranslator.h"

QTranslator* LangTranslator::translator = Q_NULLPTR;
QJsonObject  LangTranslator::g_translatorJson;