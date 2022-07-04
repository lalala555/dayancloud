/***********************************************************************
* Module:  EngineRayvision.h
* Author:  hqr
* Modified: 2016/11/18 17:51:24
* Purpose: Declaration of the class
***********************************************************************/
#ifndef _ENGINE_RAYVISION_H_
#define _ENGINE_RAYVISION_H_

#include "TransformEngine.h"

typedef struct tagRayvisionInitParam {
    IResolveSink * resolve_sink;
    std::vector<server_node_info_t> m_server_node_info_array;
    size_t m_server_node_info_count;
    size_t max_uploader_count;
    size_t max_downloader_count;
    size_t max_channel_count;
    std::string m_log_path;
    size_t m_remote_type;
    char m_user_data[128];
    bool m_auto_check;

    tagRayvisionInitParam(IResolveSink * r_sink, const std::vector<server_node_info_t> node_info_list, size_t node_info_size,
                        const std::string & log_ini, int up_count, int down_count, int channel_count,
                        size_t remote_type, bool auto_check, const char *user_data)
        : resolve_sink(r_sink)
        , m_server_node_info_array(node_info_list)
        , m_server_node_info_count(node_info_size)
        , m_log_path(log_ini)
        , max_uploader_count(up_count)
        , max_downloader_count(down_count)
        , max_channel_count(channel_count)
        , m_remote_type(remote_type)
        , m_auto_check(auto_check)
    {
        memset(m_user_data, 0, 128);
        strncpy_s(m_user_data, user_data, strlen(user_data));

    }

    void operator=(const tagRayvisionInitParam& rayvisionParam)
    {
        resolve_sink             = rayvisionParam.resolve_sink;
        m_server_node_info_array = rayvisionParam.m_server_node_info_array;
        m_server_node_info_count = rayvisionParam.m_server_node_info_count;
        m_log_path               = rayvisionParam.m_log_path;
        max_uploader_count       = rayvisionParam.max_uploader_count;
        max_downloader_count     = rayvisionParam.max_downloader_count;
        max_channel_count        = rayvisionParam.max_channel_count;
        m_remote_type            = rayvisionParam.m_remote_type;
        m_auto_check             = rayvisionParam.m_auto_check;
        strncpy_s(m_user_data, rayvisionParam.m_user_data, 128);
    }
} RayvisionInitParam;


class RayvisionEngine : public AbstractTransEngine
{
public:
    RayvisionEngine();
    virtual ~RayvisionEngine();
public:
    bool init();
    void exit();
    bool pause_upload();
    bool resume_upload();
    bool pause_download();
    bool resume_download();
    bool push_upload_task(const upload_info_t & upload_info);
    bool stop_upload_task(int task_id);
    bool push_download_task(const download_info_t & download_info);
    bool stop_download_task(int task_id);
    size_t get_running_upload_task_count();
    size_t get_running_download_task_count();
    void set_max_upload_speed(uint64_t max_upload_speed);
    void set_max_download_speed(uint64_t max_download_speed);
    void switch_server(const std::string& server);
    bool initialized();
    size_t get_upload_speed();
    size_t get_download_speed();
    size_t get_lost_rate(); // ÕÚ∑÷÷Æ
    size_t get_latency();
public:
#ifdef USE_UDX
    IUdxEngine* m_engine;
#else
	IUdxerEngine* m_engine;
#endif
};

#endif // _ENGINE_RAYVISION_H_