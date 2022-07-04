/***********************************************************************
* Module:  EngineRaySync.h
* Author:  hqr
* Created: 2019/05/15 11:17:18
* Modifier: hqr
* Modified: 2019/05/15 11:17:18
* Purpose: Declaration of the class
***********************************************************************/
#ifndef ENGINE_RAYSYNC_H_
#define ENGINE_RAYSYNC_H_

#include "TransformEngine.h"

typedef struct tagRaysyncInitParam {
    IResolveSink * resolve_sink;
    std::vector<server_node_info_t> server_node_array;
    size_t server_node_size;
    size_t max_uploader_count;
    size_t max_downloader_count;
    std::string m_log_path;
    size_t m_remote_type;
    bool m_use_reset;
    char m_user_data[256];
    bool m_auto_check;

    tagRaysyncInitParam(IResolveSink * r_sink, const std::vector<server_node_info_t> server_array, size_t nodesize, bool reconnect, const string & log_ini, size_t remote_type, int up_count, int down_count, const char *user_data, bool atuo_check)
        : resolve_sink(r_sink)
        , server_node_array(server_array)
        , server_node_size(nodesize)
        , m_log_path(log_ini)
        , m_remote_type(remote_type)
        , max_uploader_count(up_count)
        , max_downloader_count(down_count)
        , m_use_reset(reconnect)
        , m_auto_check(atuo_check)
    {
        memset(m_user_data, 0, 256);
        strncpy(m_user_data, user_data, 256);
    }

    void operator=(const tagRaysyncInitParam& udxParam)
    {
        resolve_sink         = udxParam.resolve_sink;
        server_node_array    = udxParam.server_node_array;
        server_node_size     = udxParam.server_node_size;
        m_log_path           = udxParam.m_log_path;
        m_remote_type        = udxParam.m_remote_type;
        max_uploader_count   = udxParam.max_uploader_count;
        max_downloader_count = udxParam.max_downloader_count;
        m_use_reset          = udxParam.m_use_reset;
        m_auto_check         = udxParam.m_auto_check;
        strncpy(m_user_data, udxParam.m_user_data, 256);
    }

    bool isValid()
    {
        return server_node_array.size() > 0;
    }
} RaysyncInitParam;

class CRaysyncEngine : public AbstractTransEngine
{
public:
    CRaysyncEngine();
    virtual ~CRaysyncEngine();
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
    void set_max_upload_speed(uint64_t max_upload_speed); // Mbps
    void set_max_download_speed(uint64_t max_download_speed); // Mbps
    void switch_server(const std::string& server);
    bool initialized();
    size_t get_upload_speed();
    size_t get_download_speed();
    size_t get_lost_rate(); // ÕÚ∑÷÷Æ
    size_t get_latency();
    void get_status(engine_status_t& status);
    bool push_download_task2(const download_info_t &download_info);
    bool start_download_task2(int task_id);

public:
    IRaysyncEngine* m_engine;
};

#endif // _ENGINE_RAYSYNC_H_
