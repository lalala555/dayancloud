#include "stdafx.h"
#include "EngineRaySync.h"
#include "TaskCenterManager.h"

static RaysyncInitParam getRaysyncParam()
{
    QString strchannel = LocalSetting::getInstance()->getNetChannel();

    auto server_name = LocalSetting::getInstance()->getNetWorkName();
    //获取服务器列表
    auto  server_list = TransConfig::getInstance()->getTransConfig();

    std::vector<server_node_info_t> server_array;
    std::vector<server_node_info_t> server_array_back;

    bool bcheck = false;

    for (auto info : server_list.engines) {
        if (info.engine_name.toLower() == "raysync") {
            for (auto line : info.lines) {
                server_node_info_t server_info;
                sprintf(server_info.host, "%s", line.server.toStdString().c_str());
                sprintf(server_info.name, "%s", line.name.toStdString().c_str());
                server_info.port = line.port;
                if (server_name == line.name) {
                    server_array.push_back(server_info);
                } else {
                    server_array_back.push_back(server_info);
                }
            }

            bcheck = info.automatic_check;
        }
    }

    for (auto i : server_array_back) {
        server_array.push_back(i);
    }

    bool bUseReset = LocalSetting::getInstance()->getUseReset();
    const QString log_path = RBHelper::getTempPath("raysync_engine_log");
    RBHelper::makeDirectory(log_path);

    char user_data[256]   = {0};
    sprintf(user_data, "{\"source\":\"DayanDesktop\",\"platform\":\"PC\",\"engine\":\"RaySync\",\"uid\":%d,\"name\":\"%s\"}",
              MyConfig.userSet.id, qPrintable(MyConfig.userSet.userName));


    RaysyncInitParam param(CTaskCenterManager::Inst(),
                           server_array,
                           server_array.size(),
                           bUseReset,
                           log_path.toUtf8().constData(),
                           remote_type_t::transmit_server,
                           LocalSetting::getInstance()->getUploadThreadCount(),
                           LocalSetting::getInstance()->getDownloadThreadCount(),
                           user_data,
                           bcheck);

    return param;
}

CRaysyncEngine::CRaysyncEngine()
    : m_engine(nullptr)
{

}

CRaysyncEngine::~CRaysyncEngine()
{

}

bool CRaysyncEngine::init()
{
    m_engine = create_raysync_engine();
    if(m_engine == nullptr)
        return false;

    RaysyncInitParam initParam = getRaysyncParam();
    if (!initParam.isValid())
        return false;
    log_path = initParam.m_log_path;

    m_engine->set_socks5("127.0.0.1", 0, true , MyConfig.storageSet.commonBid, 1200);

    int protoType = LocalSetting::getInstance()->getProtocolType();
    m_engine->set_protocol_type(protoType);
    if(!m_engine->init(initParam.resolve_sink
                       , &initParam.server_node_array[0]
                       , initParam.server_node_size
                       , initParam.max_uploader_count
                       , initParam.max_downloader_count
                       , initParam.m_log_path.c_str()
                       , initParam.m_remote_type
                       , initParam.m_user_data
                       , initParam.m_auto_check
                       , true
                      )) {
        return false;
    }

    once_initialize = true;

    return true;
}

void CRaysyncEngine::exit()
{
    m_engine->cancel_callback();

#ifdef Q_OS_LINUX
    m_engine->exit();
    destroy_raysync_engine(m_engine);
    m_engine = nullptr;
#endif
}

bool CRaysyncEngine::pause_upload()
{
    if(m_engine == nullptr)
        return false;
    return m_engine->pause_upload();
}

bool CRaysyncEngine::resume_upload()
{
    if(m_engine == nullptr)
        return false;
    return m_engine->resume_upload();
}

bool CRaysyncEngine::pause_download()
{
    if(m_engine == nullptr)
        return false;
    return m_engine->pause_download();
}

bool CRaysyncEngine::resume_download()
{
    if(m_engine == nullptr)
        return false;
    return m_engine->resume_download();
}

bool CRaysyncEngine::initialized()
{
    return once_initialize;
}

bool CRaysyncEngine::push_upload_task(const upload_info_t & upload_info)
{
    if(m_engine == nullptr)
        return false;
    return m_engine->push_upload_task(upload_info);
}

bool CRaysyncEngine::stop_upload_task(int task_id)
{
    if(m_engine == nullptr)
        return false;
    return m_engine->stop_upload_task(task_id);
}

bool CRaysyncEngine::push_download_task(const download_info_t & download_info)
{
    if(m_engine == nullptr)
        return false;
    return m_engine->push_download_task(download_info);
}

bool CRaysyncEngine::stop_download_task(int task_id)
{
    if(m_engine == nullptr)
        return false;
    return m_engine->stop_download_task(task_id);
}

void CRaysyncEngine::set_max_upload_speed(uint64_t max_upload_speed)
{
    if(m_engine == nullptr)
        return;

    int maxKBps = max_upload_speed * 1024 / 8;

    return m_engine->set_max_upload_speed(maxKBps);
}

void CRaysyncEngine::set_max_download_speed(uint64_t max_download_speed)
{
    if(m_engine == nullptr)
        return;

    int maxKBps = max_download_speed * 1024 / 8;

    return m_engine->set_max_download_speed(maxKBps);
}

void CRaysyncEngine::switch_server(const std::string& server)
{
    if(m_engine == nullptr || server.empty())
        return;

    m_engine->switch_server(server.c_str());
}

size_t CRaysyncEngine::get_upload_speed()
{
    if(m_engine == nullptr)
        return 0;

    return m_engine->get_transmit_status(transmit_status_upload_speed);
}

size_t CRaysyncEngine::get_download_speed()
{
    if(m_engine == nullptr)
        return 0;

    return m_engine->get_transmit_status(transmit_status_upload_rtt);
}

size_t CRaysyncEngine::get_lost_rate()
{
    if(m_engine == nullptr)
        return 0;

    return m_engine->get_transmit_status(transmit_status_upload_drop_rate);
}

size_t CRaysyncEngine::get_latency()
{
    if(m_engine == nullptr)
        return 0;

    return m_engine->get_transmit_status(transmit_status_upload_rtt);
}

void CRaysyncEngine::get_status(engine_status_t& status)
{
    if(m_engine == nullptr)
        return;

    return m_engine->get_status(status);
}

bool CRaysyncEngine::push_download_task2(const download_info_t &download_info)
{
    if (m_engine == nullptr)
        return false;
    return m_engine->push_download_task2(download_info);
}

bool CRaysyncEngine::start_download_task2(int task_id)
{
    if (m_engine == nullptr)
        return false;
    return m_engine->start_download_task2(task_id);
}