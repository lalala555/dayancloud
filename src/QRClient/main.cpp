#include "stdafx.h"
#include "AppInstance.h"
#include "Update/UpdateManager.h"
#include "Common/SystemUtil.h"
#include <gdiplus.h>
#include <QScreen>
#include <QGuiApplication>

#if 1
#include <iostream>
#include <cstdlib> 

#pragma push_macro("slots")  //解决Python 和 Qt 的关键词 slots 冲突
#undef slots
#include <Python.h>
#pragma pop_macro("slots")
#include <Python.h>
#endif


int main(int argc, char *argv[])
{
//	AllocConsole();
//	freopen("CONOUT$", "w", stdout);
//	freopen("CONOUT$", "w", stderr);
#ifdef Q_OS_WIN
    // Initialize GDI+
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
#endif

    QGlobalHelp::init();

	//适应高分辨率屏幕
#if (QT_VERSION >= QT_VERSION_CHECK(5,9,0))
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling,true);
#endif
	
    QApplication app(argc, argv);
    QApplication::setWindowIcon(QIcon(APP_ICON));


    QFile file(":/themes/green/green.css");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    app.setStyleSheet(styleSheet);

    QTranslator translator;
#ifdef FOXRENDERFARM
	translator.load(QLocale(), QLatin1String("dayancloud_en.qm"));
#else
	translator.load(QLocale(), QLatin1String("qrenderbus_zh.qm"));
#endif // FOXRENDERFARM
    
    app.installTranslator(&translator);

    // 空间大小检查
    if (!System::isDiskSpaceEnough(100)) {
        Util::ShowMessageBoxError(QObject::tr("磁盘已满，请清理磁盘空间后再运行"));
        return 0;
    }

    if(CAppInstance::GetApp()->isAppRunning())
        return 0;

    CGlobalHelp::BeepOn();

    bool bResult = CAppInstance::GetApp()->Startup();
    if(bResult) {
        app.exec();

        if(CAppInstance::GetApp()->IsLogout()) {
            QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
        } else if(CAppInstance::GetApp()->IsRelaunch()) {
            QProcess::startDetached(qApp->arguments()[0], QStringList() << "-reload");
        } else if(CAppInstance::GetApp()->IsUpdate()) {
            UpdateManagerInstance.performUpdate(UPDATE_AUTO_TRIGGER);
        }
        CAppInstance::GetApp()->Cleanup();
    }
    CGlobalHelp::BeepOff();

    QGlobalHelp::uninit();

    Gdiplus::GdiplusShutdown(gdiplusToken);

    return 0;
}
