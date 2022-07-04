#include "stdafx.h"
#include "EngineRayvision.h"
#include "TaskCenterManager.h"

static RayvisionInitParam getRayvisionParam()
{
    size_t remote_type      = remote_type_t::transmit_server;
    QString strchannel = LocalSetting::getInstance()->getNetChannel();
    auto server_name = LocalSetting::getInstance()->getNetWorkName();

	std::vector<server_node_info_t> server_array;
	std::vector<server_node_info_t> server_array_back;

    // 获取服务器列表
	auto  server_list = TransConfig::getInstance()->getTransConfig();
	bool bcheck = false;

	for (auto info = server_list.engines.begin();info != server_list.engines.end();info++) {
		if (info->engine_name.toLower() == "rayvision") {
			for (auto line = info->lines.begin();line!=info->lines.end();line++) {
				server_node_info_t server_info;
				sprintf(server_info.host, "%s", line->server.toStdString().c_str());
				sprintf(server_info.name, "%s", line->name.toStdString().c_str());
				server_info.port = line->port;
				if (server_name == line->name) {
					server_array.push_back(server_info);
				} else {
					server_array_back.push_back(server_info);
				}
			}

			bcheck = info->automatic_check;
		}
	}

	for (auto i = server_array_back.begin();i!=server_array_back.end();i++) {
		server_array.push_back(*i);
	}


    bool bUseReset = LocalSetting::getInstance()->getUseReset();
    const QString log_path = RBHelper::getTempPath("udxer_engine_log");
    RBHelper::makeDirectory(log_path);

    char user_data[256]   = {0};
    sprintf_s(user_data, 256, "{\"source\":\"RayvisionVFXDesktop\",\"platform\":\"PC\",\"engine\":\"udxer\",\"uid\":%d,\"name\":\"%s\"}",
        MyConfig.userSet.id, qPrintable(MyConfig.userSet.userName));

    //bool bcheck = TransConfig::getInstance()->GetAutoMaticCheck("Rayvision");

    RayvisionInitParam rayvisionParam(CTaskCenterManager::Inst(),
                                      server_array,
                                      server_array.size(),
                                      log_path.toStdString(),
                                      LocalSetting::getInstance()->getUploadThreadCount(),
                                      LocalSetting::getInstance()->getDownloadThreadCount(),
                                      LocalSetting::getInstance()->getTyphoonChannelCount(),
                                      remote_type,
                                      bcheck,
                                      user_data);

    return rayvisionParam;
}

RayvisionEngine::RayvisionEngine()
    : m_engine(NULL)
{

}

RayvisionEngine::~RayvisionEngine()
{

}

bool RayvisionEngine::init()
{
#ifdef USE_UDX
	m_engine = create_udx_engine();
#else
	m_engine = create_udxer_engine();
#endif
    if(!m_engine)
        return false;
    RayvisionInitParam initParam = getRayvisionParam();
    if(!m_engine->init(initParam.resolve_sink
                       , &initParam.m_server_node_info_array[0]
                       , initParam.m_server_node_info_count
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

void RayvisionEngine::exit()
{
    m_engine->cancel_callback();
#if 0
    m_engine->exit();
    destroy_aspera_engine(m_engine);
    m_engine = NULL;
#endif
}

bool RayvisionEngine::pause_upload()
{
    if(m_engine == nullptr)
        return false;
    return m_engine->pause_upload();
}

bool RayvisionEngine::resume_upload()
{
    if(m_engine == nullptr)
        return false;
    return m_engine->resume_upload();
}

bool RayvisionEngine::pause_download()
{
    if(m_engine == nullptr)
        return false;
    return m_engine->pause_download();
}

bool RayvisionEngine::resume_download()
{
    if(m_engine == nullptr)
        return false;
    return m_engine->resume_download();
}

bool RayvisionEngine::initialized()
{
    return once_initialize;
}

size_t RayvisionEngine::get_upload_speed()
{
    if (m_engine == nullptr)
        return 0;
    return 0;
}

size_t RayvisionEngine::get_download_speed()
{
    if (m_engine == nullptr)
        return 0;
    return 0;
}

size_t RayvisionEngine::get_lost_rate()
{
    if (m_engine == nullptr)
        return 0;
    return 0;
}

size_t RayvisionEngine::get_latency()
{
    if (m_engine == nullptr)
        return 0;
    return 0;
}

bool RayvisionEngine::push_upload_task(const upload_info_t & upload_info)
{
    if(m_engine == nullptr)
        return false;
    return m_engine->push_upload_task(upload_info);
}

bool RayvisionEngine::stop_upload_task(int task_id)
{
    if(m_engine == nullptr)
        return false;
    return m_engine->stop_upload_task(task_id);
}

bool RayvisionEngine::push_download_task(const download_info_t & download_info)
{
    if(m_engine == nullptr)
        return false;
    return m_engine->push_download_task(download_info);
}

bool RayvisionEngine::stop_download_task(int task_id)
{
    if(m_engine == nullptr)
        return false;
    return m_engine->stop_download_task(task_id);
}

size_t RayvisionEngine::get_running_upload_task_count()
{
    if(m_engine == nullptr)
        return 0;
    return m_engine->get_running_upload_task_count();
}

size_t RayvisionEngine::get_running_download_task_count()
{
    if(m_engine == nullptr)
        return 0;
    return m_engine->get_running_download_task_count();
}

void RayvisionEngine::set_max_upload_speed(uint64_t max_upload_speed)
{
    if(m_engine == nullptr)
        return;
    return m_engine->set_max_upload_speed(max_upload_speed);
}

void RayvisionEngine::set_max_download_speed(uint64_t max_download_speed)
{
    if(m_engine == nullptr)
        return;
    return m_engine->set_max_download_speed(max_download_speed);
}

void RayvisionEngine::switch_server(const std::string& server)
{
    if(m_engine == nullptr)
        return;

    m_engine->switch_server(server.c_str());
}
