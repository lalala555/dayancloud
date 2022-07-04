/***********************************************************************
* Module:  TransformEngine.h
* Author:  hqr
* Modified: 2016/11/14 15:07:27
* Purpose: Declaration of the class
***********************************************************************/
#ifndef TRANSFORM_ENGINE
#define TRANSFORM_ENGINE
#include "IPacket.h"
#include "kernel/trans_engine_define.h"

class AbstractTransEngine
{
public:
    AbstractTransEngine() : once_initialize(false) {};
    virtual ~AbstractTransEngine() {};
public:
    virtual bool init() = 0;
    virtual void exit() = 0;
    virtual bool pause_upload() = 0;
    virtual bool resume_upload() = 0;
    virtual bool pause_download() = 0;
    virtual bool resume_download() = 0;
    virtual bool initialized() = 0;
    virtual bool push_upload_task(const upload_info_t & upload_info) = 0;
    virtual bool stop_upload_task(int task_id) = 0;
    virtual bool push_download_task(const download_info_t & download_info) = 0;
    virtual bool stop_download_task(int task_id) = 0;
    virtual void set_max_upload_speed(uint64_t max_upload_speed) = 0;
    virtual void set_max_download_speed(uint64_t max_download_speed) = 0;
    virtual void switch_server(const std::string& server) = 0;
    virtual std::string logpath() { return log_path; };
    virtual size_t get_upload_speed() = 0;
    virtual size_t get_download_speed() = 0;
    virtual size_t get_lost_rate() = 0;
    virtual size_t get_latency() = 0;
    virtual void get_status(engine_status_t& status) = 0;
    virtual bool push_download_task2(const download_info_t &download_info) = 0;
    virtual bool start_download_task2(int task_id) = 0;
public:
    bool once_initialize;
    std::string log_path;
};

#endif
