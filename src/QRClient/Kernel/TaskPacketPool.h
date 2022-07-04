/***********************************************************************
* Module:  TaskPacketPool.h
* Author:  pengwei
* Created: 2017/09/13 19:31:52
* Modifier: hqr
* Modified: 2017/09/13 19:31:52
* Purpose: Declaration of the class
***********************************************************************/
#pragma once

#include "Upload/UploadPacket.h"
#include "Download/DownloadPacket.h"
// #include "ScriptPacket.h"

/*=====================================================
**class UploadPacketPool
**author：pengwei
**create date：2014-11-16
**描述：上传对象池，程序启动时初始化一定数量，当使用完后自动扩容，线程安全。
=====================================================*/
class UploadPacketPool
{
public:
    UploadPacketPool(void);
    ~UploadPacketPool(void);
    void init(int size);
    void free();
    CUploadPacket* popPacket();
    void freePacket(CUploadPacket* packet);
private:
    CLock m_lock;
    std::list<CUploadPacket*> m_freePacketPool;
    void allocMemory(int size);
};


/*=====================================================
**class UploadPacketPool
**author：pengwei
**createtime：2014-11-16
**描述：下载对象池，程序启动时初始化一定数量，当使用完后自动扩容,线程安全。
=====================================================*/
class DownloadPacketPool
{
public:
    DownloadPacketPool(void);
    ~DownloadPacketPool(void);
    void init(int size);
    void free();
    CDownloadPacket* popPacket();
    void freePacket(CDownloadPacket* packet);
private:
    CLock m_lock;
    std::list<CDownloadPacket*> m_freePacketPool;
    void allocMemory(int size);
};

/*=====================================================
**class ScriptPacketPool
**author：
**createtime：2019-10-30
**描述：任务分析对象池，程序启动时初始化一定数量，当使用完后自动扩容,线程安全。
=====================================================*/
/*class ScriptPacketPool
{
public:
    ScriptPacketPool(void);
    ~ScriptPacketPool(void);
    void init(int size);
    void free();
    ScriptPacket* popPacket();
    void freePacket(ScriptPacket* packet);
private:
    CLock m_lock;
    std::list<ScriptPacket*> m_freePacketPool;
    void allocMemory(int size);
};*/