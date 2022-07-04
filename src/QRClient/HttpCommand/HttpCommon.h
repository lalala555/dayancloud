#ifndef COMMON_H
#define COMMON_H

#include <QObject>
#include <QString>
#include <QNetworkRequest>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QCryptographicHash>
#include <QTimer>
#include <QNetworkReply>
#include <QApplication>
#include <QMetaType>
#include <QScopedPointer>
#include "HttpRestApi.h"
#include "UI/Views/NewJobs/JobState.h"

class CommandServer;

#define REQUEST_TIMEOUT 1000 * 30 //请求超时时间
#define HEADER_CONFIG "header_config.ini"
static QString INFO_SALT[3] = {"vaBci","l6AF8","vM9vH"};

struct RequestHeader {
    QString version; // 版本号: 1.0.0
    QString signature; // 签名: rayvision2017
    RequestHeader()
    {
        signature = "rayvision2017";
        version = "1.0.0";
    }
};

struct HttpRequestHeader : RequestHeader {
    int     channel;      // 请求渠道 。1：web国内；2：web国外；3：国内windows客户端；4：国外windonws客户端 ；5：微信公众号；
    int     platform;     // 请求平台。 2：2平台；5：5平台；8：8平台；9：9平台。
    int     languageFlag; // 语言标识. 0: 代表中文, 1: 代表英文
    QString userKey;      // 用户唯一的key，由服务端生成。
    QString host;         // 访问地址
    QString sTraceId;     // 会话ID

    HttpRequestHeader()
    {
        channel = CHANNEL_MODELLING;
        platform = 1;
        languageFlag = LANG_CN;
        signature = "rayvision2017";
        version = "1.0.0";
        userKey = "";
        host = "http://dev.renderbus.com";
    }
};

// sso default header
struct SSORequestHeader : RequestHeader {
};

struct PlatformInfo {
    int id;
    int platform;
    QString name;
    int status;
};

struct MaxCommitCommonInfo {
    QString element_active;
    QString element_type;
    QString frames;
    QString height;
    QString width;
    QString kg;
    QString output_file;
    QString output_file_basename;
    QString output_file_type;
    QStringList all_camera;
    QStringList element_list;
    QStringList renderable_camera;
};

struct MaxCommitVrayInfo {
    QString displacement;
    QString filter_kernel;
    QString filter_on;
    QString gi;
    QString gi_frames;
    QString gi_height;
    QString gi_width;
    QString image_sampler_type;
    QString irradiance_map_mode;
    QString irrmap_file;
    QString light_cache_file;
    QString light_cache_mode;
    QString onlyphoton;
    QString photonnode;
    QString primary_gi_engine;
    QString reflection_refraction;
    QString secbounce;
    QString secondary_gi_engine;
    QString subdivs;
};

struct MaxCommitParams {
    qint64   taskId;
    qint64   timeOut;
    int     isMaxRender;
    int     projectId;
    int     renderNum;
    int     kernel;
    int     nodeBlock;
    QString preFrames;
    QString projectName;
    MaxCommitCommonInfo* common;
    MaxCommitVrayInfo*   vray;
};

struct RecommitFailFrameInfo {
    int    taskStatus;
    QString munuTaskId;
};

struct ProjectItem {
    int projectId;
    QString projectName;
};

struct RenderingTaskItem {
    qint64 id;
    qint64 userId;
    int taskStatus;
    int channel;
    int platform;
    int isAutoCommit;
    int projectId;
    int isDelete;
    int preTaskStatus;
    qint64 lastestUpdateDate;
    qint64 submitDate;
    qint64 completedDate;
    double renderConsume;
    qint64 renderDuration;
    int totalFrames;
    int doneFrames;
    int failedFrames;
    QString munuTaskId;
    QString projectName;
    QString projectPath;
    QString sceneName;
    QString renderCamera;
    QString userName;
    QString producer;
    QString outputFilePath;
    QString inputProjectPath;
    QString editName;
    QString outputFileName;
    QString layerName;
    QString framesRange;
    QString statusText;
    QString executingFrames;
    QString preStatusText;
};

struct MaxTaskItem {
    int  abortFrames;
    int  cgId;
    int  cloneOriginalId;
    int  cloneParentId;
    int  doneFrames;
    int  executingFrames;
    int  failedFrames;
    int  totalFrames;
    int  layerParentId;
    int  taskLevel;
    int  taskLimit;
    int  taskStatus;
    int  isOpen;
    int  preTaskStatus;
    int  taskUserLevel;
    int  taskRam;
    int  shareMainCapital;
    int  isDelete;
    int  channel;
    int  isOverTime;
    int  stopType;
    int  operable;
    int  jobType;
    int progress;
    int  renderNum;

    double userAccountConsume;
    double couponConsume;
    double renderConsume;
    double taskArrears;
    double qyCouponConsume;

    qint64 id;
    qint64 userId;
    qint64 renderDuration;
    qint64 taskOverTime;
    qint64 overTimeStop;
    qint64 submitDate;
    qint64 completedDate;
    qint64 startTime;

    QString keyValue;
    QString layerName;
    QString framesRange;
    QString munuTaskId;
    QString outputFileName;
    QString producer;
    QString projectName;
    QString renderCamera;
    QString sceneName;
    QString statusText;
    QString taskType;
    QString taskTypeText;
    QString userName;
    QString taskAlias;
    QString preStatusText;
    QString locationOutput;
    QString remark;
    QString sceneLocalPath;
    QString tiles;
    QString differenceOfRenderConsume;
    QList<MaxTaskItem*> respRenderingTaskList;
    QString ctrlPointCount; // 控制点数量
    QString picCount; // 照片数量
    QStringList outputType;
    QString totalPixel; // 总像素
    ~MaxTaskItem()
    {
        qDeleteAll(respRenderingTaskList);
    }
};

//发布列表结构
struct PublishItem {
	enum modelType {
		MODEL_OTHER, //其他
	    MODEL_CITY,  //智慧城市
		MODEL_WORK,  //工地测量
		MODEL_BUIDING,//古建筑/景区
		MODEL_RELICS, //考古/文物
		MODEL_SAFETY, //公共安防
		MODEL_PRO,    //建筑工程
		MODEL_FACTORY,//智慧工厂
	} type;
	qint32 id; //发布链接id；
	qint32 userId; //用户id
	qint32 taskId; //任务id
	qint32 platform; //平台id
	qint32 channel; //通道id
	qint32 zone; //区域
	QString publishName; //任务名称;
	QString coverImgUrl; //封面图片url(服务器);
	QString localImgUrl; //下载到本地的url
	QString des;        //描述；
	int isNeedKey;    //是否需要密钥；
	QString secretKey;  //密钥；
	qint32 concurrentCount; //链接打开并发树
	qint64 publishTime; //发布时间
	qint64 expiredTime;  //失效事件
	qint32 isDelete;    //0/删除,1/未删除
	QString url;    //发布链接
	qint32 status; //0未过期，1已过期

};

//发布任务结构
struct PubTask {
	qint32 publishId; //共享作业id
	qint32 publishJobId; //共享id
	QString publishJobMunuId;//共享作业的munu id
	qint64 startTime; //开始时间(任务开始渲染的时间)
	qint64 submitTime;//提交时间（提交到munu的时间）
	qint64 completedTime; //完成时间
	qint32 status;//状态 0/发布完成 5/发布完成(有警告) 10/发布完成(有错误) 15/发布失败 20/正在发布 25/等待发布 45/放弃 48/超时停止
	qint32 duration;// 运行时间（s）
	double consume; //应付费用
	double couponConsume; //大雁券支付
	double unitPrice; //单价
};
// 错误码列表结构
struct ErrorCodeItem {
    enum Type {
        MODE_WARN,
        MODE_ERROR,
    };
    qint32 id;
    QString code;
    int type;
    qint32 languageFlag;
    QString desDescription;
    QString desSolution;
    QString solutionPath;
    bool isRepair;
    bool isDelete;
    bool isOpen;
    QString lastModifyAdmin;
    qint64 updateTime;

    QStringList details; // 本地错误使用
};

class ResponseHead
{
public:
    bool result;
    int  code;
    QString scode;
    int  cmdType;
    qint64 serverTime;
    QString version;
    QString msg;
    QString option;
    QByteArray rawData;
    ResponseHead(): result(false), code(HTTP_ERROR_UNKNOWN), cmdType(RESTAPI_END), serverTime(0), version("1.0.0") {}
    virtual ~ResponseHead() {}
};
Q_DECLARE_METATYPE(QSharedPointer<ResponseHead>)

class PlatformResponse : public ResponseHead
{
public:
    QList<PlatformInfo> Platforms;
};

class UserLoginResponse : public ResponseHead
{
public:
    int channel;
    int platform;
    int userId;
    int zone;
    int loginStatus;
    int accountType;
    int shareMainCapital;
    int subDeleteTask;
    int useMainBalance;
    int authStatus;
    int realnameStatus;
    quint64 delayAuthTime; // 延迟认证时间
    QString signature;
    QString _version;
    QString userKey;
    QString userName;
    QString phone;
    QString email;
    QString loginTime;
    QString raySyncUserKey;
    QString accessId;
};

class UserLogoutResponse : public ResponseHead
{
public:
    QString data;
};

class BalanceResponse : public ResponseHead
{
public:
    int vipLevel;
    int downloadDisable;
    bool isPicture;
    bool isPicturePro;
    qint64  pictureProExpiredDate;
    qint64  user_id;
    QString rmbbalance;
    QString usdbalance;
    QString coupon;
    QString qyCoupon;
    int couponCount;
    int qyCouponCount;
};

class UserInfoResponse : public ResponseHead
{
public:
    qint64 userId;
    qint64 mainUserId;
    qint64 studentEndTime;
    qint64 picMemberValidity; // 效果图会员到期时间
    quint64 delayAuthTime;

    bool picturePro;
    bool downloadDisable;

    int status;
    int accountType; 
    int authStatus;
    int realnameStatus;
    int softType;
    int businessType;
    int zone;
    int userType;
    int level;
    int blenderFlag;
    int c4dFlag;
    int houdiniFlag;
    int keyshotFlag;
    int displaySubaccount;
    int hideBalance; // 隐藏余额
    int hideJobCharge; // 隐藏作业扣费
    int infoStatus;
    int pictureLever;
    int platform;
    int softStatus;
    int subDeleteTask; // 子账号删除作业
    int subDeleteCapital; // 子账号删除资产 0不允许 1 允许
    int useLevelDirectory; // 保持用户本地目录
    int useMainBalance; // 使用主账号余额
    int enableNodeDetails; // 是否开启渲染节点预览
    int taskNodeLimitPermission; // 是否开启调整节点机数量
    int groupTaskLevel;  // 是否开启任务优先级调节
    int openRenderRam; //是否显示物理内存

    double rmbbalance; // 人民币余额
    double usdbalance; // 美元余额
    double coupon; // 优惠券
    double cpuPrice; // cpu价格
    double credit; // 信用积分
    double gpuPrice; // gpu价格
    double rmbCumulative; // 人民币总充值金额
    double usdCumulative; // 美元总充值金额

    double commonCoupon; // 普通券（指出青云券以外的券）金额
    double qyCoupon; // 青云券金额
    double commonCouponCount; // 普通券数量
    double qyCouponCount; // 青云券数量
    double gpuSingleDiscount; //gpu单卡折扣

    QString userName;
    QString phone;
    QString email;
    QString trueName;
    QString job;
    QString communicationNumber;
    QString address;
    QString city;
    QString country;
    QString company;
    QString pictureProExpire;
    QString description;
    
    BalanceResponse balance;

    QList<memberHistory> memberHistoryVOS;
};
class UserAuthKeyResponse : public ResponseHead
{
public:
    QString data;
};

class UserInfoUpdateResponse : public ResponseHead
{
};

class JobCreateResponse : public ResponseHead
{
public:
    QList<qint64> taskids;
    QList<QString> taskIdAlias;
    CommandServer* commandServer;
    socket_id sid;
    int createType;
    t_taskid cloneJobId;
    t_uid userId;
public:
    ~JobCreateResponse()
    {
        taskids.clear();
        taskIdAlias.clear();
    }
};

class maxHandleCommitResponse : public ResponseHead
{
public:
    int data;
};

class HistoryTaskResponse : public ResponseHead
{
public:
    QList<RenderingTaskItem*> items;
};

class MaxTaskItemsResponse : public ResponseHead
{
public:
    int pageCount;
    int pageNum;
    int size;
    int total;
    bool needFresh;
    bool freshView;
    bool isRepeat;
    QList<MaxTaskItem*> items;
    MaxTaskItemsResponse() : pageCount(0), pageNum(0), size(0), total(0), needFresh(true){}

public:
    ~MaxTaskItemsResponse()
    {
        qDeleteAll(items.begin(), items.end());
        items.clear();
    }
};
//发布列表响应
class PublishItemsResponse : public ResponseHead
{
public:
	int pageCount;
	int pageNum;
	int size;
	int total;
	QList<PublishItem*> items;
};

//创建发布响应
class CreatePublish : public ResponseHead
{
public:
	PublishItem *item;

};
//验证id已经发布响应
class CheckId : public ResponseHead
{
public:
	int isDuplicate;
};
//上传封面图片响应
class UploadPic : public ResponseHead
{
public:
	QString picurl;
};
//查找一个发布中的多个任务
class PublishTaskResponse : public ResponseHead
{
public:
	QList<PubTask*> tasks;
};
class TaskOperateResponse : public ResponseHead
{
public:
    int data;
    QList<qint64> taskIds;
public:
    ~TaskOperateResponse()
    {
        taskIds.clear();
    }
};

class TotalTaskFrameInfoResponse : public ResponseHead
{
public:
    int executingFramesTotal;
    int doneFramesTotal;
    int failedFramesTotal;
    int totalFrames;
    int waitingFramesTotal;
};

class RecommitFailFrameResponse : public ResponseHead
{
public:
    QList<RecommitFailFrameInfo> recommitFailFrames;
};

class TaskThumbnailResponse : public ResponseHead
{
public:
    qint64 frameId;
    QStringList thumbnails;
};

class ProjectNameResponse : public ResponseHead
{
public:
    QList<ProjectItem> projectNames;
};

class RenderingTaskResponse : public ResponseHead
{
public:
    int pageCount;
    int pageNum;
    int total;
    int size;
    QList<RenderingTaskItem*> items;
};

class TransBidResponse : public ResponseHead
{
public:
    QString config_bid;
    QString input_bid;
    QString output_bid;
    QString parent_input_bid; // 父帐号存储id
};

class TransConfigResponse : public ResponseHead {
public:
    QJsonObject trans_config;
};

class TransSetting : public ResponseHead
{
public:
    TransBidResponse transBid;
};

class RenderSettingResponse : public ResponseHead
{
public:
    qint32 singleNodeRenderFrames; // 是否忽略max错误贴图
    qint32 maxIgnoreMapFlag; // 是否忽略max错误贴图 0不忽略，1忽略
    qint32 autoCommit; // (网页分析用)是否启动场景参数渲染, 1不启用，2启用
    qint32 separateAccountFlag; // 主子账号分离设置
    qint32 miFileSwitchFlag; // mi文件分析风险开关
    qint32 assFileSwitchFlag; // 不分析ass文件开关标识
    qint32 manuallyStartAnalysisFlag; // 手动开启分析开关
    qint32 downloadDisable; // 禁用下载 1禁用，0不禁用
    qint32 taskOverTimeSec; // 超时时间-秒
    qint32 ignoreMapFlag; // 本地分析忽略贴图丢失
    // qint32 forceAnalysisFlag;
    qint32 isVrayLicense; // 使用付费版vray渲染
    qint32 justUploadConfigFlag; // 本地分析max只上传配置文件
    qint32 justUploadCgFlag; // maya渲染只上传cg文件
    qint32 mandatoryAnalyseAllAgent; // 本地分析强制分析所有代理

    // qint32 renderMayaByPass;
    // qint32 renderMaxBypass;
    qint32 downloadLimit; // 限制下载（1--不限制， 0--限制下载20天内的任务）
    qint32 downloadLimitDay; // 限制下载天数，默认20天
    qint32 cloneLimitDay; // 限制克隆天数，默认30天
    qint32 submitDisable; // 提交是否禁止
    qint32 ignoreAnalyseWarn; // 忽略分析警告提示 1:开启 0:不开启
};

class UserRenderSettingResponse : public ResponseHead
{
public:
    RenderSettingResponse renderSetting;
    TransSetting transSetting;
};

struct resubOptData {
    QString munuTaskId;
    int taskStatus;
};

class FailFrameResubOperateResponse : public ResponseHead
{
public:
    QList<resubOptData> datas;
    QList<qint64> taskIds;
    QList<int> resubStatus;
};

class TaskSubmitResponse : public ResponseHead
{
public:
    QString data;
    qint64 taskId;
};

class UpdateUserInfoResponse : public ResponseHead
{
public:
    QString data;
};

class UserFeedbackResponse : public ResponseHead
{
public:
    QString data;
};

class ErrorListResponse : public ResponseHead
{
public:
    QList<ErrorCodeItem> errorCodes;
};

class AdvancedParamSetResponse : public ResponseHead
{
public:
    struct Ram {
        QString name;
        qint32 value;
        qreal rate;
        bool isDefault;
    };

    struct timeOut {
        double minTimeOut;
        double maxTimeOut;
        double defaultTimeOut;

        int max;
        int min;
        int defaultSecond;
    } timeout;

    struct timeoutStop {
        double minTimeoutStop;
        double maxTimeoutStop;
        double defaultTimeoutStop;

        int max;
        int min;
        int defaultSecond;
    } timeoutstop;

    struct renderNum {
        qint32 minRenderNum;
        qint32 maxRenderNum;
        qint32 defaultRenderNum;
    } rendernum;

    struct renderBlocker {
        qint32 maxBlocks;
        qint32 defaultBlocks;
    } renderblocks;

    struct Gpu {
        QString name;
        qint32 value;
        qreal rate;
        bool isDefault;
    };

    QList<Ram> rams;
    QList<Gpu> gpus;
};

enum RenderTaskType {
    enRenderMainPhoto = 2,
    enRenderPhoton    = 5
};

class TaskProcessImgResponse : public ResponseHead
{
public:
    bool isRenderPhoton;
    int  width;
    int  height;
    int  block;
    QString currentTaskType;
    QString sceneName;
    QString startTime;
    QString completedTime;
    QList<TaskGrabInfo*> processImgList;
public:
    ~TaskProcessImgResponse()
    {
        //qDeleteAll(processImgList);
        //processImgList.clear();
    }
};

struct FrameGrabInfo {
    QString renderUrl;
    QString progressUrl;
};
class JobFrameGrabResponse : public ResponseHead
{
public:
    QList<FrameGrabInfo*> processImgList;
public:
    ~JobFrameGrabResponse()
    {
        qDeleteAll(processImgList);
        processImgList.clear();
    }
};

class JobFrameRenderingTimeResponse : public ResponseHead
{
public:
    QString progressText;
};

enum ClientNoticeChannel {
    CLIENT_ANIMATE        = 0,
    CLIENT_PICTURE        = 1,
    CLIENT_WEB_RENDERBUS  = 2,
    CLIENT_WEB_FOXRENDER  = 3,
    CLIENT_RENDERBUS_ANIM = 4,
    CLIENT_FOXRENDER_ANIM = 5,

  /*CLIENT_AGENT_3D66    = 4,
    CLIENT_AGENT_OUMO    = 5,
    CLIENT_AGENT_LINECG  = 6,
    CLIENT_AGENT_YUNWU   = 7*/
    CLIENT_RENDERBUS_DAYAN = 9,
    CLIENT_FOXRENDER_DAYAN = 10,
};

struct ProContent {
    qint64 contentId;
    qint64 noticeId;
    qint64 noticeLanFlag;
    QString noticeTitle;
    QString noticeText;
};

struct ProNoticeContent {
    qint64 id;
    qint64 adminId;
    qint64 channel;
    qint64 noticeType;
    qint64 isDelete;
    QString noticeUrl;
    QString startTime;
    QString endTime;
    QString createTime;
    QString updateTime;
    QList<qint64> noticeLanFlag;
    QList<ProContent> contents;
};

class ClientProNoticeResponse : public ResponseHead
{
public:
    QList<ProNoticeContent*> noticeContents;
public:
    ~ClientProNoticeResponse()
    {
        qDeleteAll(noticeContents);
        noticeContents.clear();
    }
};

struct SysNoticeContent {
    qint64 id;
    qint64 adminId;
    qint64 noticeType;
    qint64 channel;
    qint64 isDelete;
    QString contentZh;
    QString contentJp;
    QString contentEn;
    QString startTime;
    QString endTime;
    QString createTime;
    QString updateTime;
    QList<int> platform;
};

class ClientSysNoticeResponse : public ResponseHead
{
public:
    QList<SysNoticeContent*> noticeContents;
public:
    ~ClientSysNoticeResponse()
    {
        qDeleteAll(noticeContents);
        noticeContents.clear();
    }
};

class TaskParamterResponse : public ResponseHead
{
public:
    qint64  taskId;
    QString sceneName;
    QString taskStatusText;
    QString cgFilePath;
    QList<QString> taskParam;
};

class TaskFrameRenderingInfoResponse : public ResponseHead
{
public:
    qint32  pageCount;
    qint32  pageNum;
    qint32  size;
    qint32  total;
    qint64  taskId;
    QString searchKey;
    QList<int> stateList;
    QList<JobState::DetailInfo*> detailItems;
public:
    ~TaskFrameRenderingInfoResponse()
    {
        qDeleteAll(detailItems);
        detailItems.clear();
    }
};

class TaskRenderLogResponse : public ResponseHead
{
public:
    qint64 frameId;
    int    pageCount;
    int    pageNum;
    qint64 total;
    qint32 size;
    QStringList renderLogList;
};

struct ProjectConfigInfo {
    qint64  projectId;
    QString projectName;
    ProjectConfigInfo()
    {
        projectId = 0;
    }
};

class ProjectListResponse : public ResponseHead
{
public:
    int optType;
    QList<ProjectConfigInfo*> projectList;
public:
    ~ProjectListResponse()
    {
        qDeleteAll(projectList);
        projectList.clear();
    }
};

class ProjectNamesResponse : public ResponseHead
{
public:
    int optType;
    QList<ProjectConfigInfo*> projectList;
public:
    ~ProjectNamesResponse()
    {
        qDeleteAll(projectList);
        projectList.clear();
    }
};

struct OperateUserRenderPlugin {
    qint64 cgId;
    qint64 renderSystem;
    qint64 renderLayerType;
    qint32 optType; // 主要针对删除和设置默认时使用
    QString cgName;
    QString cgVersion;
    QString editName;
    QString projectPath;
    QString isMainUserId;
    QList<int> pluginIds;
};

class OperateUserRenderPluginResponse : public ResponseHead
{
public:
    QString data;
};

class UpgradeTaskRamResponse : public ResponseHead
{
public:
    QString data;
    QList<qint64> taskIds;
};

struct PluginInfoVos {
    qint32 pluginId;
    QString pluginName;
    QString pluginVersion;
};
Q_DECLARE_METATYPE(PluginInfoVos)

struct PluginConfigInfo {
    int isDefault;
    QString editName;
    qint32 cgId;
    QString cgName;
    QString cgVersion;
    qint32 osName;
    qint32 renderLayerType;
    QString projectPath;
    QString isMainUserId;
    QList<PluginInfoVos> infoVos;
};

class PluginConfigResponse : public ResponseHead
{
public:
    QList<qint64> cgIds;
    QList<PluginConfigInfo*> pluginConfigs;
public:
    ~PluginConfigResponse()
    {
        qDeleteAll(pluginConfigs);
        pluginConfigs.clear();
    }
};

struct PluginVersions {
    qint64  pluginId;
    QString pluginName;
    QString pluginVersion;
};
Q_DECLARE_METATYPE(PluginVersions)

struct CgPlugin {
    qint64 cvId;
    QString pluginName;
    QList<PluginVersions> pluginVersions;
};

struct CgVersion {
    qint64  id;
    qint64  cgId;
    QString cgName;
    QString cgVersion;
};

class RenderSoftVersionResponse : public ResponseHead
{
public:
    QList<CgPlugin*> cgPlugins;
    QList<CgVersion*> cgVersions;
    QString osName;
    qint32  cgId;
public:
    ~RenderSoftVersionResponse()
    {
        qDeleteAll(cgPlugins); cgPlugins.clear();
        qDeleteAll(cgVersions); cgVersions.clear();
    }
};

struct RecentTaskData {
    QString date;
    qint32  taskCount;
};

class RecentTaskDataResponse : public ResponseHead
{
public:
    QList<RecentTaskData> recentDatas;
};

class UserStorageResponse : public ResponseHead
{
public:
    qint64 allCapacity;
};

class TaskSummaryResponse : public ResponseHead
{
public:
    qint32 doneTask;
    qint32 failureTask;
    qint32 renderingTask;
    qint32 waitingTask;
};

class TaskFrameOperateResponse : public ResponseHead
{
public:
    QString data;
    QList<qint64> frameIds;
    qint64 taskId;
public:
    ~TaskFrameOperateResponse()
    {
        frameIds.clear();
    }
};

struct UserName {
    qint64  userId;
    QString userName;
};

class LoadUserNameResponse : public ResponseHead
{
public:
    QList<UserName*> userNames;
public:
    ~LoadUserNameResponse()
    {
        qDeleteAll(userNames);
        userNames.clear();
    }
};

class UpdateTaskRemarkResponse : public ResponseHead
{
public:
    int taskId;
    QString data;
    QString remark;
};

class UpdateTaskLevelResponse : public ResponseHead
{
public:
    int taskId;
    QString data;
    QString level;
};

class UpdateTaskOvertimeResponse : public ResponseHead
{
public:
    int taskId;
    int overtime;
    QString data;
};

class LoadProducerResponse : public ResponseHead
{
public:
    QList<QString> producers;
};

class JobConsumeResponse : public ResponseHead
{
public:
    qint64 taskId;
    qreal couponConsume;
    qreal userAccountConsume;
    qint64 frameTimeConsumingAve;
    qint64 taskArrearage;
    qint64 taskTimeConsuming;
    qint64 totalFrames;
};

class QRCodeResponse : public ResponseHead
{
public:
    QByteArray qrCodeData;
};

class QueryBindinWxResponse : public ResponseHead
{
public:
    bool isWxScanCode;  // 用于标识微信是否扫码
    QString headimgurl;
    QString nickname;
    qint64  id;

    qint64 userId;
    qint64 updateDate;
    qint64 createDate;
    qint32 fromWhere;
    qint32 isAdmin;
    qint32 isBind;
    qint32 isDelete;
    QString openId;
    QString shareSource;
    QString unionId;
    QString wechatId;
};

class WxBindingToUserResponse : public ResponseHead
{
public:
    QString data;
};

struct PlatformStatus
{
    qint32 platform;
    qint32 status;
    qint32 type;
    QString name;
    QString taskPrefix;
    qint64 updateTime;
};

class PlatformStatusResponse : public ResponseHead
{
public:
    QList<PlatformStatus*> platforms;

public:
    ~PlatformStatusResponse(){
        qDeleteAll(platforms);
        platforms.clear();
    }
};

class NodeDetailsResponse : public ResponseHead
{
public:
    int heNum;
    int memUsedRate;
    QString date;
    QString ip;
    QString totalMem;
    QString restMem;
    QString usedMem;
    QString restRom;
    QStringList cpuRateArr;

public:
    ~NodeDetailsResponse() {
        cpuRateArr.clear();
    }
};

class DownloadStartResponse : public ResponseHead
{
public:
    qint64 taskId;
    QString downloadRequestId;
};

class DownloadCompleteResponse : public ResponseHead
{
public:
};

struct DownloadCountItem {
    qint64 taskId;
    qint32 downloadCount;
    bool downloading;
    qint64 creatTime;
    qint64 updateTime;
};

class DownloadCountResponse : public ResponseHead
{
public:
    qint32 pageCount;
    qint32 pageNum;
    qint32 total;
    qint32 size;
    QList<DownloadCountItem> items;
};

struct DownloadInfoItem {
    qint32 downloadStatus;
    QString machineName;
    QString ip;
    qint64 startTime;
    qint64 endTime;
    qint64 updateTime;
};

class DownloadInfoResponse : public ResponseHead
{
public:
    qint32 pageCount;
    qint32 pageNum;
    qint32 total;
    qint32 size;
    QList<DownloadInfoItem> items;
};

class ReportLogResponse : public ResponseHead
{
public:
    int data;
    qint64  taskId;
    QString filePath;
};

class UpdateTaskLimitResponse : public ResponseHead
{
public:
    qint64 taskId;
    QString data;
    QString limitCount;
};

struct FrameRecommitInfo
{
    double framePrice;
    double couponFee; 
    double feeAmount;

    qint64 taskId;
    qint64 userId;
    qint64 startTime;
    qint64 frameExecuteTime;
    qint64 endTime;  
    qint64 jobId;

    QString frameName;
};

class FrameRecommitInfoResponse : public ResponseHead
{
public:
    qint64  frameId;
    qint64  pageCount;
    qint64  pageNum;
    qint64  size;
    qint64  total;
    QList<FrameRecommitInfo*> recommitItems;
public:
    ~FrameRecommitInfoResponse()
    {
        qDeleteAll(recommitItems);
        recommitItems.clear();
    }
};

struct TaskOutputSize{
    qint64 taskId;
    qint64 outputSize;
};

class TaskOutputSizeResponse : public ResponseHead 
{
public:
    qint64  pageCount;
    qint64  pageNum;
    qint64  size;
    qint64  total;
    int     fromPage; // 来自哪个页面请求 jobview/downloadview
    QMap<qint64, TaskOutputSize> tasksSize;
};

class PaymentUseVoucherResponse : public ResponseHead
{
public:
    qint64  taskId;
    QString data;
};

class PaymentQueryVoucherResponse : public ResponseHead
{
public:
    QList<VoucherInfo> effectiveVouchers; // 可用的抵扣券
    QList<VoucherInfo> invalidVouchers;   // 不可用的券
};

class TaskSubmitCCATResponse : public ResponseHead
{
public:
    QString data;
    qint64 taskId;
};

class TaskCloneResponse : public ResponseHead
{
public:
    QString data;
    qint64 taskId;
};

class UploadJsonFileResponse : public ResponseHead
{
public:
    int data;
    qint64  taskId;
    QString filePath;
};

////////////////////////////////////////////

class SSOResponse : public ResponseHead {
public:
    int httpCode = 0;
    QString redirect;
    QString rsThirdToken; // 交互凭证
    QString rsAuthToken; // sso鉴权rsAuthToken
    bool isVerified;
};

// 效果图价格
class PicturePriceResponse : public ResponseHead
{
public:
    double memberRate;
    double commonRate;
    double maxMemberPrice;
    double maxCommonPrice;
};

class AtReportUrlResponse : public ResponseHead {
public:
    QString data;
    qint64 taskId;
};

class AtPointCloudDatatUrlResponse : public ResponseHead {
public:
	QString dataUrl;
	QString imagePrefix;
	QString controlPointDataUrl;
	qint64 taskId;
};

// 分享链接
class ShareResultLinkResponse : public ResponseHead {
public:
    QString shareUrl;
    QString shareCode;
    qint64 taskId;
};
// 空三结果md5
class AtDataVerifyResponse : public ResponseHead
{
public:
    int resultCode;
    qint64 taskId;
};

#endif // COMMON_H
