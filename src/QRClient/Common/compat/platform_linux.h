/***********************************************************************
* Module:  platform_linux.h
* Author:  hqr
* Created: 2019/07/03 11:11:14
* Modifier: hqr
* Modified:  2019/07/03 11:11:14
* Purpose: Declaration of the class
***********************************************************************/
#pragma once
#ifndef PLATFORM_LINUX_H
#define PLATFORM_LINUX_H

#include <string>
#include <QString>
#include <QStorageInfo>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <QString>

class platform
{
public:
    static std::wstring getComputerName()
    {
        char hostname[1024];
        gethostname(hostname, 1024);
        return  QString(hostname).toStdWString();
    }

    static std::wstring getLoginUserName()
    {
        QByteArray user_name = qgetenv("USER");
        if (user_name.isEmpty())
            user_name = qgetenv("USERNAME");

        return std::wstring(QString(user_name).toStdWString());
    }

    static std::wstring getLoginDisplayName()
    {
        return getLoginUserName();
    }

    static size_t getMemorySize()
    {
        return 0;
    }

    static bool isDiskSpaceEnough(const std::wstring& strPath, uint32_t minSpaceSize)
    {
        QStorageInfo storage = QStorageInfo::root();
        QString root = storage.rootPath();
        qint64 free = storage.bytesAvailable();
        return (free >= (minSpaceSize * 1024 * 1024));
    }

    static std::vector<std::wstring> getGpuCards()
    {
        return{};
    }

    static int64_t getPhysicallyRamSize()
    {
        std::string token;
            std::ifstream file("/proc/meminfo");
            while(file >> token) {
                if(token == "MemTotal:") {
                    unsigned long mem;
                    if(file >> mem) {
                        return mem;
                    } else {
                        return 0;
                    }
                }
                // ignore rest of the line
                file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
            return 0;
    }
};

#endif // PLATFORM_LINUX_H
