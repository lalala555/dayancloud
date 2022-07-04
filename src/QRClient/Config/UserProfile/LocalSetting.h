#pragma once
#ifndef LOCAL_SETTING_H
#define LOCAL_SETTING_H

#include <string>
#include <QSettings>
#include "ISetting.h"

/************************************************************************/
/* 本地配置保存类，用于替代ConfigDb                                     */
/************************************************************************/
class LocalSetting : public ISetting
{
    explicit LocalSetting();

public:
    ~LocalSetting();
    static LocalSetting* getInstance()
    {
        static LocalSetting instance;
        return &instance;
    }

    bool init();
    int getPlatformIdForLocal();
    int getDefaultsite()
    {
        return _platform;
    }
    void setDefaultSite(int siteId)
    {
        _platform = siteId;

        m_settings->setValue("APP-SETTING/Platform", _platform);
        save();
    }
    int getLanguageId()
    {
        return _languageId;
    }
    void setLanguageId(int langId)
    {
        _languageId = langId;

        m_settings->setValue("APP-SETTING/Language", _languageId);
        save();
    }
    void setUserName(const QString & userName)
    {
        _userName = userName;

        QString encrypted = QString::fromStdString(CToken::SimpleEncrypt(_userName.toStdString().c_str()));

        m_settings->setValue("APP-SETTING/UserName", encrypted);
        save();
    }
    QString getUserName()
    {
        return _userName;
    }
    void setPassword(const QString & password)
    {
        _password = password;

        QString encrypted = QString::fromStdString(CToken::SimpleEncrypt(_password.toStdString().c_str()));

        m_settings->setValue("APP-SETTING/Password", encrypted);
        save();
    }
    QString getPassword()
    {
        return _password;
    }
    void setAutoLogin(bool autologin)
    {
        _autoLogin = autologin;

        m_settings->setValue("APP-SETTING/AutoLogin", _autoLogin);
        save();
    }
    bool getAutoLogin()
    {
        return _autoLogin;
    }
    void setRememberPassword(bool rememberPassword)
    {
        _rememberPassword = rememberPassword;

        m_settings->setValue("APP-SETTING/RememberPassword", _rememberPassword);
        save();
    }
    bool getRememberPassword()
    {
        return _rememberPassword;
    }
    void setTransEngine(const QString &engine)
    {
        _transEngineName = engine;

        m_settings->setValue("TRANS-SETTING/TransEngine", _transEngineName);
        save();
    }
    QString getTransEngine()
    {
        return _transEngineName;
    }
    void setLocalSavePath(const QString & path)
    {
        _localSavePath = path;

        m_settings->setValue("APP-SETTING/LocalSavePath", _localSavePath);
        save();
    }
    QString getLocalSavePath()
    {
        return _localSavePath;
    }
    void setClientProjectPath(const QString & path)
    {
        _clientProjectPath = path;

        m_settings->setValue("APP-SETTING/ClientProjectPath", _clientProjectPath);   
        save();
    }
    QString getClientProjectPath()
    {
        return _clientProjectPath;
    }
    void setDownloadThreadCount(int count)
    {
        _downloadThreadCount = count;

        m_settings->setValue("TRANS-SETTING/DownloadThreadCount", _downloadThreadCount);
        save();
    }
    int getDownloadThreadCount()
    {
        _downloadThreadCount = _downloadThreadCount > DOWNLOAD_THREAD_MAX_COUNT ? DOWNLOAD_THREAD_MAX_COUNT : _downloadThreadCount;
        return _downloadThreadCount;
    }
    void setUploadThreadCount(int count)
    {
        _uploadThreadCount = count;

        m_settings->setValue("TRANS-SETTING/UploadThreadCount", _uploadThreadCount);
        save();
    }
    int getUploadThreadCount()
    {
        _uploadThreadCount = _uploadThreadCount > UPLOAD_THREAD_MAX_COUNT ? UPLOAD_THREAD_MAX_COUNT : _uploadThreadCount;
        return _uploadThreadCount;
    }
    int getTyphoonChannelCount()
    {
        _typhoonChannelCount = _typhoonChannelCount > CHANNEL_MAX_COUNT ? CHANNEL_MAX_COUNT : _typhoonChannelCount;
        return _typhoonChannelCount;
    }
    void setTransEncrypt(bool isEncrypt)
    {
        _transEncrypt = isEncrypt;

        m_settings->setValue("TRANS-SETTING/TransEncrypt", isEncrypt);
        save();
    }
    bool getTransEncrypt()
    {
        return _transEncrypt;
    }
    bool getUseReset()
    {
        return _usereset;
    }
    void setNetChannel(const QString &strchannel)
    {
        _strNetChannel = strchannel;

        m_settings->setValue("TRANS-SETTING/Channel", _strNetChannel);
        save();
    }
    QString getNetChannel()
    {
        return _strNetChannel;
    }
    void setTaskColHeader(const QString &strHeader)
    {
        _taskColHeader = strHeader;

        m_settings->setValue("VIEW-SETTING/TaskHeader", _taskColHeader);  
        save();
    }
    QString getTaskColHeader()
    {
        return _taskColHeader;
    }
    void setNetWorkName(const QString &strname)
    {
        _strNetWorkName = strname;

        m_settings->setValue("TRANS-SETTING/HostName", _strNetWorkName); 
        save();
    }
    QString getNetWorkName()
    {
        return _strNetWorkName;
    }
    void setLimitUpload(int nlimit)
    {
        _limitUpload = nlimit;

        m_settings->setValue("TRANS-SETTING/LimitUpload", _limitUpload);
        save();
    }
    int getLimitUpload()
    {
        return _limitUpload;
    }
    void setLimitDownload(int nlimit)
    {
        _limitDownload = nlimit;

        m_settings->setValue("TRANS-SETTING/LimitDownload", _limitDownload);
        save();
    }
    int getLimitDownload()
    {
        return _limitDownload;
    }
    void setFileCheckMode(int mode)
    {
        _fileCheckMode = mode;

        m_settings->setValue("TRANS-SETTING/FileCheckMode", _fileCheckMode);
        save();
    }
    int getFileCheckMode()
    {
        return _fileCheckMode;
    }
    void setProtocolType(int type)
    {
        _protocolType = type;

        m_settings->setValue("TRANS-SETTING/ProtocolType", _protocolType);
        save();
    }
    int getProtocolType()
    {
        return _protocolType;
    }
    void setSparseCheckMode(int check)
    {
        _sparseCheckMode = check;

        m_settings->setValue("TRANS-SETTING/SparseCheck", _sparseCheckMode);
        save();
    }
    int getSparseCheckMode()
    {
        return _sparseCheckMode;
    }
    void resetTransEngineConfig();

    bool getBeginnerGuide()
    {
        return _beginnerGuideFlag;
    }
    void setBeginnerGuide(bool show)
    {
        _beginnerGuideFlag = show;

        m_settings->setValue("APP-SETTING/BeginnerGuide", _beginnerGuideFlag);
        save();
    }

    bool getHasCheckedLocalSoftFlag()
    {
        return _hasCheckedLocalSoftFlag;
    }
    void setHasCheckedLocalSoftFlag(bool checked)
    {
        _hasCheckedLocalSoftFlag = checked;

        m_settings->setValue("APP-SETTING/HasCheckedLocalSoftFlag", _hasCheckedLocalSoftFlag); 
        save();
    }

    int getLoginChoiceMode()
    {
        return _loginChoiceMode;
    }
    void setLoginChoiceMode(int mode)
    {
        _loginChoiceMode = mode;

        m_settings->setValue("APP-SETTING/LoginChoiceMode", _loginChoiceMode); 
        save();
    }

    // 分析设置
    QString getFrameTest()
    {
        return _frameTest;
    }
    void setFrameTest(const QString& test)
    {
        _frameTest = test;

        m_settings->setValue("ANALYSIS-SETTING/FrameTest", _frameTest);     
        save();
    }
    bool getIsSubmitLayer()
    {
        return _isSubmitLayer;
    }
    void setIsSubmitLayer(bool isLayered)
    {
        _isSubmitLayer = isLayered;

        m_settings->setValue("ANALYSIS-SETTING/LayerSubmit", _isSubmitLayer);
        save();
    }
    int getMuiltFrames()
    {
        return _mutilFrame;
    }
    void setMuiltFrames(int muiltframes)
    {
        _mutilFrame = muiltframes;

        m_settings->setValue("ANALYSIS-SETTING/MuiltFrames", _mutilFrame);
        save();
    }
    int getRenderRam()
    {
        return _renderRam;
    }
    void setRenderRam(int ram)
    {
        _renderRam = ram;

        m_settings->setValue("ANALYSIS-SETTING/RenderRam", _renderRam);
        save();
    }
    bool showSpeedStat()
    {
        return _showSpeedStat;
    }
    bool showSpeedChart()
    {
        return _showSpeedChart;
    }
    int getDownloadShowInDay()
    {
        if(_downloadShowInDay < 0)
            return 20;

        return _downloadShowInDay;
    }
    int getMaxUploadRetryTimes()
    {
        _uploadRetryTimes = _uploadRetryTimes > MAX_UPLOAD_RETRY_TIMES ? MAX_UPLOAD_RETRY_TIMES : _uploadRetryTimes;
        return _uploadRetryTimes;
    }
    void setMaxUploadRetryTimes(int times)
    {
        _uploadRetryTimes = times;

        m_settings->setValue("TRANS-SETTING/UploadRetryTimes", _uploadRetryTimes);
        save();
    }
    int getMaxUploadJobCount()
    {
        _uploadJobCount = _uploadJobCount > MAX_UPLOAD_JOB_COUNT ? MAX_UPLOAD_JOB_COUNT : _uploadJobCount;
        return _uploadJobCount;
    }
    void setMaxUploadJobCount(int jobCount)
    {
        _uploadJobCount = jobCount;

        m_settings->setValue("TRANS-SETTING/UploadJobCount", _uploadJobCount);     
        save();
    }
    void setProxy(int mode, const QString& host = "127.0.0.1", quint16 port = 0);
    void getProxy(int& mode, QString& host, quint16& port);

    int getGpuCount()
    {
        return _gpuCount;
    }
    void setGpuCount(int gpuCount)
    {
        _gpuCount = gpuCount;

        m_settings->setValue("ANALYSIS-SETTING/GpuCount", _gpuCount);
        save();
    }

    void setDownloadShowInDay(int day)
    {
        _downloadShowInDay = day;

        m_settings->setValue("VIEW-SETTING/DownloadShowInDay", _downloadShowInDay);
        save();
    }

    int getAnalyzeInterval()
    {
        return _analyzeInterval;
    }
    void setAnalyzeInterval(int interval)
    {
        _analyzeInterval = interval;

        m_settings->setValue("ANALYSIS-SETTING/AnalyzeInterval", _analyzeInterval);
        save();
    }

    bool getAnalyzeReport()
    {
         return _sendAnalyzeReport;
    }
    bool getTransportReport()
    {
        return _sendTransportReport;
    }
    float getUploadSpeedFactor() 
    {
        if (_uploadSpeedFactor > 1.0f || _uploadSpeedFactor < 0) {
            _uploadSpeedFactor = 1.0f;
        }
        return _uploadSpeedFactor;
    }
    void setUploadSpeedFactor(float factor)
    {
        _uploadSpeedFactor = factor;

        m_settings->setValue("TRANS-SETTING/UploadSpeedFactor", QString::number(_uploadSpeedFactor));
        save();
    }
    float getDownloadSpeedFactor()
    {
        if (_downloadSpeedFactor > 1.0f || _downloadSpeedFactor < 0) {
            _downloadSpeedFactor = 1.0f;
        }
        return _downloadSpeedFactor;
    }
    void setDownloadSpeedFactor(float factor)
    {
        _downloadSpeedFactor = factor;

        m_settings->setValue("TRANS-SETTING/DownloadSpeedFactor", QString::number(_downloadSpeedFactor));
        save();
    }
    int getCacheCleanLimitDatys()
    {
        if (_cacheCleanLimitDatys <= 0) {
            _cacheCleanLimitDatys = 7;
        }
        return _cacheCleanLimitDatys;
    }
    void setCacheCleanLimitDatys(int day)
    {
        _cacheCleanLimitDatys = day;

        m_settings->setValue("APP-SETTING/CacheCleanLimitDatys", QString::number(_cacheCleanLimitDatys));
        save();
    }
    bool getIsSubmitNoded()
    {
        return _isNodeSubmit;
    }
    void setIsSubmitNoded(bool isnoded)
    {
        _isNodeSubmit = isnoded;

        m_settings->setValue("ANALYSIS-SETTING/NodeSubmit", _isNodeSubmit);
        save();
    }
    bool getIsAnalyzeTaskReserve()
    {
        return _isAnalyzeTaskReserve;
    }
    void setIsAnalyzeTaskReserve(bool isReserve)
    {
        _isAnalyzeTaskReserve = isReserve;

        m_settings->setValue("APP-SETTING/AnalyzeTaskReserve", _isAnalyzeTaskReserve);
        save();
    }
    bool getIsUploadTaskReserve()
    {
        return _isUploadTaskReserve;
    }
    void setIsUploadTaskReserve(bool isReserve)
    {
        _isUploadTaskReserve = isReserve;

        m_settings->setValue("APP-SETTING/UploadTaskReserve", _isUploadTaskReserve);
        save();
    }
    bool getIsDownloadTaskReserve()
    {
        return _isDownloadTaskReserve;
    }
    void setIsDownloadTaskReserve(bool isReserve)
    {
        _isDownloadTaskReserve = isReserve;

        m_settings->setValue("APP-SETTING/DownloadTaskReserve", _isDownloadTaskReserve);
        save();
    }
    bool getEnableSelectGupCount()
    {
        return _enableSelectGupCount;
    }
    void setEnableSelectGupCount(bool enable)
    {
        _enableSelectGupCount = enable;

        m_settings->setValue("ANALYSIS-SETTING/EnableSelectGupCount", _enableSelectGupCount);
        save();
    }
    bool isDownloadMkSenceNameDir()
    {
        return _bDownloadMkSenceNameDir;
    }
    void setDownloadMkSenceNameDir(bool isMakeDir)
    {
        _bDownloadMkSenceNameDir = isMakeDir;
        m_settings->setValue("APP-SETTING/DownloadMkSenceNameDir", isMakeDir);
        save();
    }
    void setAutoStart(bool autologin)
    {
        _bAutoStart = autologin;

        m_settings->setValue("APP-SETTING/AutoStart", _bAutoStart);
        save();
    }
    bool getAutoStart()
    {
        return _bAutoStart;
    }
    void setCleanCache(bool clean)
    {
        _bCleanCache = clean;

        m_settings->setValue("APP-SETTING/CleanCache", _bCleanCache);
        save();
    }
    bool getCleanCache()
    {
        return _bCleanCache;
    }

    int getCheckIsDownloadFinishedTimes()
    {
        if (_checkIsDownloadFinishTimes < 0 || _checkIsDownloadFinishTimes > 10) {
            _checkIsDownloadFinishTimes = 2;
        }
        return _checkIsDownloadFinishTimes;
    }
    void setCheckIsDownloadFinishedTimes(int times)
    {
        _checkIsDownloadFinishTimes = times;

        m_settings->setValue("APP-SETTING/CheckIsDownloadFinishedTimes", times);
        save();
    }
    int getCheckIsDownloadFinishedTimesSkipDays()
    {
        if (_downloadFinishTimesDaysSkipped < 1) {
            _downloadFinishTimesDaysSkipped = 7;
        }
        return _downloadFinishTimesDaysSkipped;
    }
    void setCheckIsDownloadFinishedTimesSkipDays(int skipped)
    {
        _downloadFinishTimesDaysSkipped = skipped;

        m_settings->setValue("APP-SETTING/DownloadFinishTimesDaysSkipped", skipped);
        save();
    }

    int getEnableExactProgress()
    {
        return _bEnableExactProgress;
    }
    void setEnableExactProgress(int enable)
    {
        _bEnableExactProgress = enable;

        m_settings->setValue("APP-SETTING/EnableExactProgress", enable);
        save();
    }

    int getEnableAutoUpdate()
    {
        return _isEnableAutoUpdate;
    }
    void setEnableAutoUpdate(bool enable)
    {
        _isEnableAutoUpdate = enable ? 1 : 0;

        m_settings->setValue("APP-SETTING/EnableAutoUpdate", _isEnableAutoUpdate);
        save();
    }

    int getEnableUserCoord()
    {
        return _isEnableUserCoord;
    }
    void setEnableUserCoord(bool enable)
    {
        _isEnableUserCoord = enable ? 1 : 0;

        m_settings->setValue("APP-SETTING/EnableUserCoord", _isEnableUserCoord);
        save();
    }
private:
    int _languageId;
    int _platform;
    bool _transEncrypt;
    bool _usereset;
    int _downloadThreadCount;
    int _uploadThreadCount;
    int _typhoonChannelCount;
    int _limitUpload;
    int _limitDownload;
    int _fileCheckMode;  // 0-关闭校验，1-标准校验，2-严格校验
    int _sparseCheckMode; // 0-基础校验 1-稀疏校验 3-严格校验
    int _protocolType; // 0-自动 1-UDP 2-TCP
    QString _transEngineName;
    QString _strNetWorkName;
    QString _strNetChannel;
    QString _strLimitUpload;
    QString _taskColHeader;
    QString _userName;
    QString _password;
    bool _autoLogin;
    bool _rememberPassword;
    QString _localSavePath;
    QString _clientProjectPath;
    bool _beginnerGuideFlag;
    bool _hasCheckedLocalSoftFlag;
    int  _loginChoiceMode; // 登录模式，0 -- 账号密码登录，其他值 -- 三方登录或代理登录
    bool _showSpeedStat;
    bool _showSpeedChart;
    int _downloadShowInDay;

    QString _frameTest;
    bool _isSubmitLayer;
    int  _renderRam;
    int  _mutilFrame;
    int  _gpuCount;

    int  _uploadRetryTimes; // 上传重试次数
    int  _uploadJobCount;   // 同时上传任务的数量

    int _analyzeInterval; // 并发分析的时间间隔，单位 ms

    bool _sendAnalyzeReport;
    bool _sendTransportReport;

    float _uploadSpeedFactor;   // 上传速度限制因子
    float _downloadSpeedFactor; // 下载速度限制因子

    int _cacheCleanLimitDatys;  // 缓存清理时限， 默认7天以前的要清理
    bool _isNodeSubmit; // 是否启动分节点提交
    bool _isAnalyzeTaskReserve;  // 分析任务是否逆序
    bool _isUploadTaskReserve;   // 上传任务是否逆序
    bool _isDownloadTaskReserve; // 下载任务是否逆序

    bool _enableSelectGupCount;  // 是否允许选择GPU卡数
    bool _bDownloadMkSenceNameDir; // 下载时是否创建场景名文件夹

    bool _bAutoStart; // 开机自启

    bool _bCleanCache; // 清理缓存

    int _checkIsDownloadFinishTimes; // 检查是否下载完成的次数
    int _downloadFinishTimesDaysSkipped; // 超过多少天的任务跳过下载完成检查

    int _bEnableExactProgress; // 是否开启精准上传进度

    int _isEnableAutoUpdate;

    int _isEnableUserCoord;
};

#endif // _LOCAL_SETTING_H