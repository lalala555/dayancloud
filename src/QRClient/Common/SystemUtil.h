/***********************************************************************
* Module:  SystemUtil.h
* Author:  hqr
* Created: 2017/05/22 12:18:42
* Modified: 2017/05/22 12:18:42
* Purpose: Declaration of the class
***********************************************************************/
#ifndef SYSTEMUTIL_H_
#define SYSTEMUTIL_H_

#include <QCoreApplication>
#include <QNetworkInterface>
#include <QHostInfo>
#include "compat/platform_win.h"
#include "kernel/macro.h"
#include "Resource/version.h"
#include "Common/RBHelper.h"
#include <QDesktopWidget>
#include <QApplication>


class System
{
public:
    static bool isDiskSpaceEnough(int32_t minSpaceSize)
    {
        std::wstring strPath = QCoreApplication::applicationDirPath().toStdWString();
        return platform::isDiskSpaceEnough(strPath, minSpaceSize);
    }

    static QString getSysProductName()
    {
        QString osProductName = QSysInfo::prettyProductName();

        return osProductName;
    }

    static QString getUserAgent()
    {
        QString osProductName = QSysInfo::prettyProductName();
        QString buildCpuArch = QSysInfo::buildCpuArchitecture();
        QString userAgent = QString("%1/%2 (%3; %4) ")
            .arg(USERAGENT_NAME).arg(STRFILEVERSION).arg(osProductName).arg(buildCpuArch);

        return userAgent;
    }

    static QString getLoginDisplayName()
    {
        return QString::fromStdWString(platform::getLoginDisplayName()).toUtf8();
    }

    static QString getLoginUserName()
    {
        return QString::fromStdWString(platform::getLoginUserName());
    }

    static QString getComputerName()
    {
        return QString::fromStdWString(platform::getComputerName().c_str());
    }

    static QString getMacAddress()
    {
        foreach(QNetworkInterface netInterface, QNetworkInterface::allInterfaces())
        {
            // Return only the first non-loopback MAC Address
            if (!(netInterface.flags() & QNetworkInterface::IsLoopBack))
                return netInterface.hardwareAddress();
        }
        return QString();
    }

    static QString getLanIP()
    {
        const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
        for (const QHostAddress &address : QNetworkInterface::allAddresses()) {
            if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
                return address.toString();
        }

        return QString();
    }

    static QString getSystemDrive()
    {
#ifdef Q_OS_WIN
        return QStorageInfo::root().rootPath();
#else
        return qgetenv("HOME");
#endif
    }

    static int getScreenHeight()
    {
        QDesktopWidget* d = QApplication::desktop();
        if (d == NULL)
            return 0;
        return d->height();
    }

    static QString getCpuInfo()
    {
#ifdef Q_OS_WIN
        QSettings settings("HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", QSettings::NativeFormat);
        QString cpuInfo = settings.value("ProcessorNameString").toString();
#else
        QString cpuInfo;
#endif

        return cpuInfo;
    }
};

#endif // SYSTEMUTIL_H_
