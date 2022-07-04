/***********************************************************************
* Module:  platform_win.h
* Author:  hqr
* Created: 2019/06/17 11:51:14
* Modifier: hqr
* Modified:  2019/06/17 11:51:14
* Purpose: Declaration of the class
***********************************************************************/
#pragma once
#ifndef PLATFORM_WIN_H
#define PLATFORM_WIN_H

#define SECURITY_WIN32

#include <windows.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <Psapi.h>

#include <security.h>
#pragma comment(lib, "secur32.lib")

#include <combaseapi.h>


#include <wininet.h>
#pragma comment(lib,"wininet.lib")

class platform
{
public:
    struct release_deleter {
        template <typename T>
        void operator()(T* p) const
        {
            p->Release();
        }
    };
    static std::wstring getComputerName()
    {
        TCHAR infoBuf[128] = { 0 };
        DWORD bufCharCount = 128;

        GetComputerName(infoBuf, &bufCharCount);

        return std::wstring(infoBuf);
    }

    static std::wstring getLoginUserName()
    {
        TCHAR szUserName[128] = { 0 };
        unsigned long dwNameLen = sizeof(szUserName);
        GetUserName(szUserName, &dwNameLen);

        return std::wstring(szUserName);
    }

    static std::wstring getLoginDisplayName()
    {
        TCHAR szUserName[128] = { 0 };
        unsigned long dwNameLen = sizeof(szUserName);
        GetUserNameEx(NameDisplay, szUserName, &dwNameLen);

        std::wstring userName(szUserName);
        if (userName.empty()) {
            userName = getLoginUserName();
        }

        return userName;
    }

    static bool getSpaceFromPath(const std::wstring& strPath, ULARGE_INTEGER& ulUse, ULARGE_INTEGER& ulTotal, ULARGE_INTEGER& ulFree)
    {
        std::wstring::size_type found = strPath.find_first_of(L"/\\");
        std::wstring strDisk = L"";

        if (found != -1) {
            strDisk = strPath.substr(0, found);
        }

        if (strDisk.empty() == true) {
            return false;
        }

        return (GetDiskFreeSpaceEx(strDisk.c_str(), &ulUse, &ulTotal, &ulFree) == FALSE) ? false : true;
    }

    static bool isDiskSpaceEnough(const std::wstring& strPath, DWORD minSpaceSize)
    {
        DWORD dwSize = 0; //(MB)
        ULARGE_INTEGER ulUse;
        ULARGE_INTEGER ulTotal;
        ULARGE_INTEGER ulFree;
        ulUse.QuadPart = 0;
        ulTotal.QuadPart = 0;
        ulFree.QuadPart = 0;

        // installed drive
        platform::getSpaceFromPath(strPath, ulUse, ulTotal, ulFree);
        dwSize = (DWORD)(ulFree.QuadPart / 1024 / 1024);
        if (dwSize < minSpaceSize) {
            return false;
        }

        return true;
    }

    void restart(LPCTSTR app, LPCTSTR param)
    {
        INT_PTR  result = (INT_PTR)::ShellExecute(0, _T("open"), app, param, 0, SW_SHOWNORMAL);
        if (SE_ERR_ACCESSDENIED == result) {
            // Requesting elevation
            result = (INT_PTR)::ShellExecute(0, _T("runas"), app, param, 0, SW_SHOWNORMAL);
        }
        if (result <= 32) {
            // error handling
        }
    }
};

#endif // PLATFORM_WIN_H
