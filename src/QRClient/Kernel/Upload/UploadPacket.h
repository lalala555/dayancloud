#pragma once
#include "IPacket.h"
#include "TransEngine/TransfomEngineFactory.h"
#include "UploadFileListByDb.h"
/*====================================================================
**class CUploadPacket

**描述：
**每个上传任务id对应一个UploadPacket,上传完不会删除，任然会保存在内存中
**UploadPacket的产生有两种情况：
**1、创建新任务时，根据工程文件的cfg文件的描述创建
**2、加载已有任务时，程序LoadUncomplele时会根据xml描述，为历史任务创建一个对应的packet

=======================================================================*/
class CUploadPacket : public IPacket, public IUploadSink
{

public:
    CUploadPacket();
    ~CUploadPacket(void);
public:
    virtual void on_upload(const upload_callback_info_t & callback_info) override;
    int translateRayvisionCallback(const upload_callback_info_t & callback_info);
    //初始化上传数据，初始化失败将导致任务无法被成功开启
    //成功返回true，失败返回false
    bool init(const UploadInfo* info, UploadFileListByDb* fileList);
    //启动回调队列，每个任务都要成功开启回调队列才能接受传输引擎的回调消息
    //已经开启成功的任务，无法二次开启
    //开启成功返回ture，失败返回false
    bool start();
    //任务完成和取消时都要停止回调队列，否则将无法再次开启，对象释放时自动停止
    void stop();
    bool doWork(AbstractTransEngine* engine);

    inline t_taskid getTaskId()
    {
        return m_taskId;
    };
    inline t_uid getUserId()
    {
        return m_uid;
    };
    inline bool isRunning()
    {
        return m_runStatus == PACKET_RUNNING;
    }
    inline bool isIdle()
    {
        return m_runStatus == PACKET_FREE;
    }
    int getStorageId()
    {
        return m_storageId;
    }
    void clearPacket();
    void startTask();
    void stopTask();
    void updatePriority(int priority);
    void updateTimePriority(unsigned int timePriority);
    void setTransEngine(AbstractTransEngine* engine) { m_engine = engine; }
    void setPacketIndex(int index) { _packetIndex = index; }
private:
    bool m_bVip;
    t_taskid m_taskId;
    t_uid m_uid;

    int m_storageId;
    int m_total; // 需要传输的总文件数目

    AbstractTransEngine* m_engine;

    UploadFileListByDb* m_fileList;
    QFuture<void> m_future;
private:
    bool uploadFile(const UncompleteFile& fileElem);
};
