/***********************************************************************
* Module:  version.h
* Author:  hqr
* Modified: 2016/11/14 15:04:02
* Purpose: Declaration of the class
***********************************************************************/
#pragma once
#define FILEVER             5,0,4,8
#define PRODUCTVER          FILEVER
#define STRFILEVERSION      "5.0.4.8"
#define SHORTVERSION        "5.0"
#define STRPRODUCTVERSION   STRFILEVERSION
#define BUILD_TIME          "Build 2022-6-30"

#ifdef FOXRENDERFARM
    #define STRGREENAPPLEICON   "foxCRClient.ico"
    #define STRCOMPANYNAME      "Shenzhen Rayvision Technology Co., Ltd\0"
    #define STRFILEDESCRIPTION  "AvicaCloud Desktop\0"
    #define STRINTERNALNAME     "AvicaCloud.exe\0"
    #define STRLEGALCOPYRIGHT   "Copyright (C) 2021\0"
    #define STRORIGINALFILENAME "AvicaCloud.exe\0"
    #define STRPRODUCTNAME      "FoxRenderfarm Application\0"
#else
    #define STRGREENAPPLEICON   "CRClient.ico"
    #define STRCOMPANYNAME      "深圳市瑞云科技有限公司\0"
    #define STRFILEDESCRIPTION  "大雁云客户端\0"
    #define STRINTERNALNAME     "DayanCloud.exe\0"
    #define STRLEGALCOPYRIGHT   "Copyright (C) 2021\0"
    #define STRORIGINALFILENAME "DayanCloud.exe\0"
    #define STRPRODUCTNAME      "DayanCloud Application\0"
#endif
