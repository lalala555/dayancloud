#pragma once

class CGlobalHelp
{
public:
    static void BeepOn();
    static void BeepOff();

    static bool Response(int type, intptr_t wParam = 0, intptr_t lParam = 0);
    static void InitEnv(const QString& appdataDir);
    static void DestoryEnv(const QString& appdataDir);
    static void InitLanguageCfg();

    static IResponseSink* g_mainSink;
};