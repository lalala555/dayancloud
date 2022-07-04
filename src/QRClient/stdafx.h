#ifndef STDAFX_H__
#define STDAFX_H__


/* Add C includes here */

#if defined __cplusplus
/* Add C++ includes here */
#include <qtglobal.h>


#ifdef Q_OS_WIN
#pragma execution_character_set("utf-8")

#pragma warning(disable : 4996)
#pragma warning(disable : 4099)

#define WIN32_LEAN_AND_MEAN
#include <tchar.h>
#include <commdlg.h>
#include <commctrl.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <aclapi.h>
#include <shlobj.h>
#include <tlhelp32.h>
#include <windows.h>
#include <windowsx.h>
#include <psapi.h>


#pragma comment(lib,"psapi.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "comdlg32.lib")

#endif // Q_OS_WIN32
#include <io.h>
#include <cstdio>
#include <clocale>
#include <string>
#include <fstream>
#include <sstream>
#include <memory>
#include <regex>
#include <algorithm>

#include "kernel/compat.h"
#include "kernel/macro.h"
#include "kernel/msg.h"
#include "kernel/rendercmd_define.h"
#include "kernel/trans_engine_define.h"
#include "kernel/request_interface.h"
#include "Resource/version.h"

#include "Common/GlobalHelp.h"
#include "Common/RBHelper.h"
#include "common_interface.h"
#include "basement_interface.h"

#include "UserConfig.h"
#include "UserProfile/WSConfig.h"
#include "UserProfile/Profile.h"
#include "UserProfile/ConfigDb.h"
#include "UserProfile/LocalConfig.h"
#include "UserProfile/LocalSetting.h"
#include "UserProfile/TransConfig.h"
#include "UserProfile/HistoryDB/HistoryDbBase.h"
#include "UserProfile/HistoryDB/HistoryDbMgr.h"

#include "UI/Base/Util.h"
#include "UI/Base/Widgets/ToastWidget.h"
#include "UI/Views/NewJobs/JobState.h"


#endif

#endif // _STDAFX_H__
