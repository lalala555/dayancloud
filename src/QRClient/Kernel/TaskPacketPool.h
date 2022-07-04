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
**author��pengwei
**create date��2014-11-16
**�������ϴ�����أ���������ʱ��ʼ��һ����������ʹ������Զ����ݣ��̰߳�ȫ��
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
**author��pengwei
**createtime��2014-11-16
**���������ض���أ���������ʱ��ʼ��һ����������ʹ������Զ�����,�̰߳�ȫ��
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
**author��
**createtime��2019-10-30
**�����������������أ���������ʱ��ʼ��һ����������ʹ������Զ�����,�̰߳�ȫ��
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