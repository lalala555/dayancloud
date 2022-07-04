#include "stdafx.h"
#include "LocalSetting.h"
#include "Common/SystemUtil.h"

const QString kSetting = "Setting.conf";

LocalSetting::LocalSetting()
    : _downloadThreadCount(DOWNLOAD_THREAD_DEFAULT_COUNT)
    , _uploadThreadCount(UPLOAD_THREAD_DEFAULT_COUNT)
    , _typhoonChannelCount(CHANNEL_MAX_COUNT)
    , _transEncrypt(false)
    , _usereset(false)
    , _uploadRetryTimes(5)
    , _uploadJobCount(1)
    , _isEnableAutoUpdate(0)
    , _isEnableUserCoord(1)
{
    QString appdata_local_path = RBHelper::getAppDataLocalPath();
    QString appdata_conf_path = appdata_local_path.append(kSetting);
    if (!QFile::exists(appdata_conf_path)) {
        QString app_dir = QCoreApplication::applicationDirPath();
        QString app_config_file = app_dir.append("/").append(kSetting);
        QFile::copy(app_config_file, appdata_conf_path);
    }

    m_settings = new QSettings(appdata_conf_path, QSettings::IniFormat);
    m_settings->setIniCodec("UTF-8");
}

LocalSetting::~LocalSetting()
{
}

bool LocalSetting::init()
{
    _platform = m_settings->value("APP-SETTING/Platform", -1).toInt();
#ifdef FOXRENDERFARM
	_languageId = m_settings->value("APP-SETTING/Language", 1).toInt();
#else
	_languageId = m_settings->value("APP-SETTING/Language", 0).toInt();
#endif // FOXRENDERFARM


    _userName = m_settings->value("APP-SETTING/UserName", "").toString();
    _userName = QString::fromStdString(CToken::SimpleDecrypt(_userName.toStdString().c_str()));
    _password = m_settings->value("APP-SETTING/Password", "").toString();
    _password = QString::fromStdString(CToken::SimpleDecrypt(_password.toStdString().c_str()));
    _autoLogin = m_settings->value("APP-SETTING/AutoLogin", false).toBool();
    _rememberPassword = m_settings->value("APP-SETTING/RememberPassword", false).toBool();
    _localSavePath = m_settings->value("APP-SETTING/LocalSavePath", "C:/DayanCloud/download/").toString();
    _clientProjectPath = m_settings->value("APP-SETTING/ClientProjectPath", "").toString();
    _beginnerGuideFlag = m_settings->value("APP-SETTING/BeginnerGuide", false).toBool();
    _hasCheckedLocalSoftFlag = m_settings->value("APP-SETTING/HasCheckedLocalSoftFlag", false).toBool();
    _loginChoiceMode = m_settings->value("APP-SETTING/LoginChoiceMode", 0).toInt();
    _cacheCleanLimitDatys = m_settings->value("APP-SETTING/CacheCleanLimitDatys", 7).toInt();
    _isAnalyzeTaskReserve = m_settings->value("APP-SETTING/AnalyzeTaskReserve", false).toBool();
    _isUploadTaskReserve = m_settings->value("APP-SETTING/UploadTaskReserve", false).toBool();
    _isDownloadTaskReserve = m_settings->value("APP-SETTING/DownloadTaskReserve", true).toBool();
    _bDownloadMkSenceNameDir = m_settings->value("APP-SETTING/DownloadMkSenceNameDir", true).toBool();
    _bAutoStart = m_settings->value("APP-SETTING/AutoStart", false).toBool();
    _bCleanCache = m_settings->value("APP-SETTING/CleanCache", false).toBool();
    _checkIsDownloadFinishTimes = m_settings->value("APP-SETTING/CheckIsDownloadFinishedTimes", 1).toInt();
    _downloadFinishTimesDaysSkipped = m_settings->value("APP-SETTING/DownloadFinishTimesDaysSkipped", 3).toInt();
    _bEnableExactProgress = m_settings->value("APP-SETTING/EnableExactProgress", 0).toInt();
    _isEnableAutoUpdate = m_settings->value("APP-SETTING/EnableAutoUpdate", 0).toInt();
    _isEnableUserCoord = m_settings->value("APP-SETTING/EnableUserCoord", 1).toInt();

    _transEngineName = m_settings->value("TRANS-SETTING/TransEngine", "RaySync").toString();
    _downloadThreadCount = m_settings->value("TRANS-SETTING/DownloadThreadCount", DOWNLOAD_THREAD_DEFAULT_COUNT).toInt();
    _uploadThreadCount = m_settings->value("TRANS-SETTING/UploadThreadCount", UPLOAD_THREAD_DEFAULT_COUNT).toInt();
    _typhoonChannelCount = m_settings->value("TRANS-SETTING/TyphoonChannelCount", CHANNEL_DEFAULT_COUNT).toInt();
    _transEncrypt = m_settings->value("TRANS-SETTING/TransEncrypt", false).toBool();
    _usereset = m_settings->value("TRANS-SETTING/Reset", false).toBool();
    _strNetWorkName = m_settings->value("TRANS-SETTING/HostName", "").toString();
    _strNetChannel = m_settings->value("TRANS-SETTING/Channel", "transmit_server").toString();
    _limitUpload = m_settings->value("TRANS-SETTING/LimitUpload", 0).toInt();
    _limitDownload = m_settings->value("TRANS-SETTING/LimitDownload", 0).toInt();
    _fileCheckMode = m_settings->value("TRANS-SETTING/FileCheckMode", 1).toInt();  // 标准校验
    _sparseCheckMode = m_settings->value("TRANS-SETTING/SparseCheck", CHECK_MODE_SPARSE).toInt();  // 稀疏校验
    _protocolType = m_settings->value("TRANS-SETTING/ProtocolType", PROTO_AUTO).toInt();  // RaySync协议设置
    _uploadRetryTimes = m_settings->value("TRANS-SETTING/UploadRetryTimes", 5).toInt();  // 任务重试次数，默认5次
    _uploadJobCount = m_settings->value("TRANS-SETTING/UploadJobCount", 1).toInt(); // 支持同时上传的任务数，默认同时上传1个任务
    _uploadSpeedFactor = QString::number(m_settings->value("TRANS-SETTING/UploadSpeedFactor", 0.8).toFloat(), 'f', 2).toFloat();// 上传速度限制因子
    _downloadSpeedFactor = QString::number(m_settings->value("TRANS-SETTING/DownloadSpeedFactor", 0.8).toFloat(), 'f', 2).toFloat(); // 下载速度限制因子

    _isSubmitLayer = m_settings->value("ANALYSIS-SETTING/LayerSubmit", false).toBool();  // 分析参数设置
    _renderRam = m_settings->value("ANALYSIS-SETTING/RenderRam", 64).toInt();
    _mutilFrame = m_settings->value("ANALYSIS-SETTING/MuiltFrames", 1).toInt();
    _frameTest = m_settings->value("ANALYSIS-SETTING/FrameTest", "000").toString();
    _gpuCount = m_settings->value("ANALYSIS-SETTING/GpuCount", 2).toInt();
    _analyzeInterval = m_settings->value("ANALYSIS-SETTING/AnalyzeInterval", 1000).toInt();
    _isNodeSubmit = m_settings->value("ANALYSIS-SETTING/NodeSubmit", false).toBool();
    _enableSelectGupCount = m_settings->value("ANALYSIS-SETTING/EnableSelectGupCount", false).toBool();

    // 上报设置
    _sendAnalyzeReport = m_settings->value("REPORT-SETTING/AnalyzeReport", true).toBool();
    _sendTransportReport = m_settings->value("REPORT-SETTING/TransportReport", true).toBool();
    // 渲染列表表头
    _showSpeedStat = m_settings->value("VIEW-SETTING/ShowSpeedStat", true).toBool();
    _showSpeedChart = m_settings->value("VIEW-SETTING/ShowSpeedChart", false).toBool();
    _taskColHeader = m_settings->value("VIEW-SETTING/TaskHeader", "").toString();
    _downloadShowInDay = m_settings->value("VIEW-SETTING/DownloadShowInDay", 20).toInt();

    // 加载默认存储路径
    if (_localSavePath.isEmpty() || _localSavePath != RBHelper::getDefaultDownloadDir()) {
        this->setLocalSavePath(RBHelper::getDefaultDownloadDir());
    }
    RBHelper::makeDirectory(_localSavePath);

    if (_clientProjectPath.isEmpty() || _clientProjectPath != RBHelper::getDefaultProjectDir()) {
        this->setClientProjectPath(RBHelper::getDefaultProjectDir());
    }
    RBHelper::makeDirectory(_clientProjectPath);

    this->setAnalyzeInterval(_analyzeInterval);
    this->setUploadSpeedFactor(getUploadSpeedFactor());
    this->setDownloadSpeedFactor(getDownloadSpeedFactor());
    this->setCacheCleanLimitDatys(getCacheCleanLimitDatys());
    this->setIsAnalyzeTaskReserve(getIsAnalyzeTaskReserve());
    this->setIsUploadTaskReserve(getIsUploadTaskReserve());
    this->setIsDownloadTaskReserve(getIsDownloadTaskReserve());
    this->setEnableSelectGupCount(getEnableSelectGupCount());
    this->setDownloadMkSenceNameDir(isDownloadMkSenceNameDir());
    this->setCleanCache(getCleanCache());
    this->setCheckIsDownloadFinishedTimes(getCheckIsDownloadFinishedTimes());
    this->setCheckIsDownloadFinishedTimesSkipDays(getCheckIsDownloadFinishedTimesSkipDays());
    this->setEnableExactProgress(getEnableExactProgress());
    this->setEnableAutoUpdate(getEnableAutoUpdate());
    this->setEnableUserCoord(getEnableUserCoord());
    return true;
}

int LocalSetting::getPlatformIdForLocal()
{
    QString wsVersion = WSConfig::getInstance()->getWSVersion();
    int version = wsVersion.toInt();
    return (_platform + PLATFORM_MODIFY * version);
}

void LocalSetting::resetTransEngineConfig()
{
    int platform = WSConfig::getInstance()->get_default_platform(LocalSetting::getInstance()->getDefaultsite());
    this->setDefaultSite(platform);

    QString transEngineName = this->getTransEngine();
    if (!TransConfig::getInstance()->CheckUserSupportEngine(transEngineName)) {
        transEngineName = TransConfig::getInstance()->GetUserDefaultEngine();
        this->setTransEngine(transEngineName);
    }

    // 重置线路
    QString transLineHost = this->getNetWorkName();
    if (!TransConfig::getInstance()->CheckUserSupportTransLine(transEngineName, transLineHost)) {
        transLineHost = TransConfig::getInstance()->GetDefaultTransLine(transEngineName, transLineHost);
        this->setNetWorkName(transLineHost);
    }
}
