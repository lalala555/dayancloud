#include "stdafx.h"
#include "LogManager.h"
#include "Common/StringUtil.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifndef WIN32    // only available on Win32
#include <dirent.h>
#include <unistd.h>
#endif

using namespace zsummer::log4z;

static bool endswith(const std::string& str, const std::string& substr)
{
    size_t i = str.rfind(substr);
    return (i != std::string::npos) && (i == (str.length() - substr.length()));
}

void rollOver(const std::string& dirname, int maxDaysToKeep)
{
    const time_t oldest = time(nullptr) - maxDaysToKeep * 60 * 60 * 24;

#ifndef WIN32
#define PATHDELIMITER "/"
#else
#define PATHDELIMITER "\\"
#endif

#ifndef WIN32    // only available on Win32
    struct dirent **entries;
    int nentries = scandir(dirname.c_str(), &entries, 0, alphasort);
    if(nentries < 0)
        return;
    for(int i = 0; i < nentries; i++) {
        struct stat statBuf;
        const std::string fullfilename = dirname + PATHDELIMITER + entries[i]->d_name;
        int res = ::stat(fullfilename.c_str(), &statBuf);
        if((res == -1) || (!S_ISREG(statBuf.st_mode))) {
            free(entries[i]);
            continue;
        }
        if(statBuf.st_mtime < oldest && strstr(entries[i]->d_name, fullfilename.c_str())) {
            if(endswith(fullfilename, ".log")) {
                std::cout << " Deleting " << fullfilename.c_str() << std::endl;
                ::unlink(fullfilename.c_str());
            }
        }
        free(entries[i]);
    }
    free(entries);
#else
    HANDLE hFind = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAA ffd;
    const std::string pattern = dirname + "\\*";

    hFind = FindFirstFileA(pattern.c_str(), &ffd);
    if(hFind != INVALID_HANDLE_VALUE) {
        do {
            struct stat statBuf;
            const std::string fullfilename = dirname + PATHDELIMITER + ffd.cFileName;
            int res = ::stat(fullfilename.c_str(), &statBuf);
            if(res != -1 && statBuf.st_mtime < oldest && !(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                if(endswith(fullfilename, ".log")) {
                    std::cout << "Deleting " << fullfilename << "\n";
                    ::unlink(fullfilename.c_str());
                }
            }
        } while(FindNextFileA(hFind, &ffd) != 0);

        if(GetLastError() != ERROR_NO_MORE_FILES) {
            // [XXX] some kind of error happened
        }
        FindClose(hFind);
        hFind = INVALID_HANDLE_VALUE;
    }
#endif
}

void LogManager::init()
{
    std::string logFilePath =  RBHelper::getAppDataPath().toLocal8Bit();
    int logLevel = LocalConfig::getInstance()->defaultLogLevel;

#ifdef _DEBUG
    logLevel = LOG_LEVEL_TRACE;
#endif
    rollOver(logFilePath, 10);
    // start log4z
    ILog4zManager::getRef().setLoggerPath(LOG4Z_MAIN_LOGGER_ID, logFilePath.c_str());
    ILog4zManager::getRef().setLoggerLevel(LOG4Z_MAIN_LOGGER_ID, logLevel);
    ILog4zManager::getRef().start();
}

void LogManager::uninit()
{
}
