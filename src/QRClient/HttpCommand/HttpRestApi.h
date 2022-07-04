#pragma once
#include <QString>
#include <QObject>
#include <QHash>

enum HttpErrorCode {
    SSO_ERROR_COMMON_SUCCESS = 0,
    SSO_ERROR_COMMON_FAILED,
    SSO_ERROR_COMMON_FOBIDDEN,
    SSO_ERROR_COMMON_REQUESTLIMITEXCEEDED,
    SSO_ERROR_COMMON_INTERNAR_ERROR,
    SSO_ERROR_COMMON_PARAMETER_INVALID,
    SSO_ERROR_COMMON_PARAMETER_EMPTY,
    SSO_ERROR_COMMON_ILLEGAL_PROTOCOL,
    SSO_ERROR_VALIDATE_ERROR,
    SSO_ERROR_VALIDATE_CODE_SENDERROR,
    SSO_ERROR_VALIDATE_LIMITEXCEEDED,
    SSO_ERROR_SIGNATURE_EXPIRED,
    SSO_ERROR_SIGNATURE_INVALID,
    SSO_ERROR_ACCOUNT_EMAIL_FORMAT,
    SSO_ERROR_ACCOUNT_EMAIL_ACTIVED,
    SSO_ERROR_ACCOUNT_TOKEN_EXPIRED,
    SSO_ERROR_ACCOUNT_EMAIL_NOTFOUND,
    SSO_ERROR_ACCOUNT_PHONE_NOTFOUND,
    SSO_ERROR_ACCOUNT_UNACTIVATED,
    SSO_ERROR_ACCOUNT_LOCKED,
    SSO_ERROR_ACCOUNT_SIGNIN_FAILED,
    SSO_ERROR_ACCOUNT_SIGNIN_IPLIMIT,
    SSO_ERROR_ACCOUNT_DISABLED,
    SSO_ERROR_ACCOUNT_SIGN_GRAPHICVERIFICATIONREQUIRED,
    SSO_ERROR_ACCOUNT_PHONE_EXISTED,
    SSO_ERROR_ACCOUNT_EMAIL_EXISTED,
    SSO_ERROR_ACCOUNT_USERNAME_EXISTED,
    SSO_ERROR_ACCOUNT_THIRDPARTY_NOTFOUND,
    SSO_ERROR_ACCOUNT_THIRDPARTY_SIGNIN_FAILED,
    SSO_ERROR_ACCOUNT_THIRDPARTY_SIGNIN_TIMEOUT,
    SSO_ERROR_ACCOUNT_THIRDPARTY_AUTH_ERROR,
    SSO_ERROR_ACCOUNT_THIRDPARTY_UNIONID_EXISTED,
    SSO_ERROR_ACCOUNT_THIRDPARTY_BIND_EXISTED,
    SSO_ERROR_ACCOUNT_NOTFOUND,
    SSO_ERROR_ACCOUNT_VALIDATE_INTELLIGENTFAILED,
    SSO_ERROR_ACCOUNT_REGISTER_LOWSAFE,
    SSO_ERROR_SYSTEM_CACHE_ERROR,
    SSO_ERROR_SYSTEM_ERROR,
    SSO_ERROR_END = 99,

    HTTP_ERROR_SUCCESS                          = 200,  // 接口正常返回
    HTTP_SSO_THIRDPARTY_SIGNIN_FAILED           = 201,
    HTTP_SSO_THIRDPARTY_SIGNIN_TIMEOUT          = 202,
    HTTP_ERROR_FAIL                             = 300,  // 失败
    HTTP_ERROR_NEED_REDIRECT                    = 301,  // 老平台用户登录，需要迁移
    HTTP_ERROR_FORBIDDEN                        = 403,  // 没有权限
    HTTP_ERROR_SSO_AUTH_FAILED                  = 405,  // sso鉴权失败
    HTTP_ERROR_SSO_AUTH_REQUEST_FAILED          = 406,  // sso鉴权失败
    HTTP_ERROR_INTERNAL_HTTP_ERROR              = 500,  // 服务器处理失败
    HTTP_ERROR_PROJECT_EXIST_ERROR              = 503,  // 项目配置名已存在
    HTTP_ERROR_PLUGIN_EXIST_ERROR               = 504,  // 插件配置名已存在
    HTTP_ERROR_USER_BALANCE_NOT_ENOUGH          = 507,  // 用户余额不足
    HTTP_ERROR_USER_ACCOUNT_NOT_ACTIVE          = 508,  // 账户未激活
    HTTP_ERROR_RENDER_FRAME_ANALYZE_FAIL        = 510,  // 帧解析失败
    HTTP_ERROR_ANALYSE_TOO_MANY_FRAMES          = 511,  // 任务帧数太多
    HTTP_ERROR_USER_INFO_NOT_EXIST              = 513,  // 用户信息不存在
    HTTP_ERROR_ANALYSE_NOT_EXIST                = 520,  // 分析作业不存在
    HTTP_ERROR_RENDER_CAMARA_NOT_EXIST          = 530,  // 渲染相机不存在

    HTTP_ERROR_PARAMETER_INVALID                = 600,  // 非法参数
    HTTP_ERROR_PARAMETER_CANT_BE_EMPTY          = 601,  // 缺少必要参数
    HTTP_ERROR_NEED_USER_LOGIN                  = 602,  // 需要用户登录
    HTTP_ERROR_ILLEGAL_PROTOCOL                 = 603,  // 非法请求
    HTTP_ERROR_VALIDATE_CODE_HTTP_ERROR         = 604,  // 手机验证码错误
    HTTP_ERROR_VINSUFFICIENT_PERMISSIONS        = 605,  // 权限不足
    HTTP_ERROR_VALIDATE_COMMOM_CODE_HTTP_ERROR  = 606,  // 验证码错误
    HTTP_ERROR_VALIDATE_SEND_CODE_HTTP_ERROR    = 607,  // 验证码发送失败
    HTTP_ERROR_USER_AREA_ERROR                  = 608,  // 客户国内外区域不匹配
	HTTP_ERROR_USER_PLATFORM_ERROR              = 609,  // 用户平台错误
    HTTP_ERROR_FOX_USER_FORBIDDEN               = 627,  // 不支持fox用户登录
    HTTP_ERROR_RENDERBUS_USER_FORBIDDEN         = 628,  // 不支持renderbus用户登录

    HTTP_ERROR_DO_NOT_HAVE_ANY_MORE_RECORD      = 700,  // 没有更多记录
    HTTP_ERROE_RENDER_WAIT_TASK_COMPLETE        = 710,  // 等待光子或者主图渲染完成
    HTTP_ERROR_TASK_EXPIRE_CANT_OPERATE         = 718,  // 任务已过期，不可操作
    HTTP_ERROR_RENDER_COMMIT_PASS_ONE_MONTH     = 728,  // 任务已过期
    HTTP_ERROR_TASK_JSON_IS_INVALID             = 740,  // 读task.json失败
    HTTP_ERROR_TASK_JSON_IS_EMPTY               = 741,  // task.json文件为空
    HTTP_ERROR_STOP_TASK_ALL_COPY_PICTURE       = 748,  // 正在出图的任务不能停止，请等待出图完成后再做操作
    HTTP_ERROR_JOB_SUBMITING                    = 749,  // 任务正在提交中
    HTTP_ERROR_TEST_FRAMES_ILLEGAL              = 750,  // 测试帧不合法
    HTTP_ERROR_ENGINE_NOT_COMPLETE              = 753,  // 引擎配置不完整
	HTTP_ERROR_SUBMIT_NOT_ALLOWED               = 756,  // 不允许提交
    HTTP_ERROR_TASK_DEDUCT_ILLEGAL              = 760,  // 任务不符合要求
    HTTP_ERROR_TASK_NOT_EXIST_HARDWARE_CONFIG   = 773,  // 任务不存在硬件配置
    
    HTTP_ERROR_ACCOUNT_BINDING_USER_NULL        = 800,  // 账号不存在
    HTTP_ERROR_ACCOUNT_NOT_BINDING              = 801,  // 未绑定设备
    HTTP_ERROR_ACCOUNT_BINDING_FAIL             = 802,  // 设备绑定失败
    HTTP_ERROR_ACCOUNT_NOACTIVE                 = 803,  // 账号未激活
    HTTP_ERROR_ACCOUNT_LOCKED                   = 804,  // 账号已被禁用
    HTTP_ERROR_ACCOUNT_USERNAME_PASSWORD_FAIL   = 805,  // 用户名或密码错误
    HTTP_ERROR_ACCOUNT_UNIONID_FAIL             = 806,  // 账号未绑定第三方用户
    HTTP_ERROR_ACCOUNT_PHONE_FAIL               = 807,  // 手机未绑定第三方用户
    HTTP_ERROR_ACCOUNT_UNIONID_PHONE            = 808,  // 手机已绑定其他第三方用戶
    HTTP_ERROR_ACCOUNT_WEIXIN_FAIL              = 809,  // 微信登录失败
    HTTP_ERROR_ACCOUNT_WEIBO_FAIL               = 810,  // 微博登录失败
    HTTP_ERROR_ACCOUNT_LOGOUT_FAIL              = 811,  // 登出失败
    HTTP_ERROR_ACCOUNT_LOGIN_IPLIMITFAIL        = 812,  // IP被限制
    HTTP_ERROR_ACCOUNT_QQ_FAIL                  = 813,  // QQ登录失败
    HTTP_ERROR_NOREPEAT_SELECT_SOFTWARE         = 814,  // 无法重复选择常用软件
    HTTP_ERROR_ACCOUNT_UNIONID_EXISTS           = 815,  // UNIONID已存在
    HTTP_ERROR_ACCOUNT_IPLIMIT_EXISTS           = 816,  // 无法添加重复的限制ip
    HTTP_ERROR_ACCOUNT_NO_WXPUB_SUBSCRIBE       = 817,  // 未关注微信公众号
    HTTP_ERROR_ACCOUNT_LOGIN_TIMES_WARNING      = 818,  // 您还有三次机会
    HTTP_ERROR_ACCOUNT_INVALID_ADDRESSES        = 819,  // 无效的邮件地址
    HTTP_ERROR_ACCOUNT_LOGIN_TWO_TIMES_WARNING  = 820,  // 您还有两次次机会
    HTTP_ERROR_ACCOUNT_LOGIN_ONE_TIMES_WARNING  = 821,  // 您还有一次次机会
    HTTP_ERROR_ACCOUNT_DISENABLED               = 822,  // 账号已被停用
    HTTP_ERROR_USER_LOGIN_NEED_VERIFICATION     = 824,  // 密码错误次数超过3次，需要验证

    HTTP_ERROR_VALIDATE_PHONE_FAIL              = 900,  // 手机号已存在
    HTTP_ERROR_VALIDATE_EMAIL_FAIL              = 901,  // 邮箱已存在
    HTTP_ERROR_VALIDATE_USERNAME_FAIL           = 902,  // 用户名已存在
    HTTP_ERROR_ACCOUNT_EMAIL_FAIL               = 903,  // 邮箱未绑定账户
    HTTP_ERROR_CURRENCY_NOT_SUPPORT             = 904,  // 币种不支持
    HTTP_ERROR_AGENT_NOT_SUPPORT                = 905,  // 代理商不支持
    HTTP_ERROR_AMOUNT_NOT_SUPPORT               = 906,  // 请输入合理的充值范围
    HTTP_ERROR_COUPONNO_NOT_SUPPORT             = 908,  // 优惠码不支持
    HTTP_ERROR_PAYMETHOD_NOT_SUPPORT            = 909,  // 支付方式不支持
    HTTP_ERROR_NO_INVOICE_ORDER                 = 910,  // 无可用的发票订单
    HTTP_ERROR_NO_INVOICE_ADDRESS               = 911,  // 无可用的发票收件地址
    HTTP_ERROR_SUBUSER_EXISTS_TASKS             = 912,  // 无法删除一个存在执行任务的子账号
    HTTP_ERROR_SUBUSER_ARREARAGE                = 913,  // 当前子账号欠费
    HTTP_ERROR_DELSUBUSER_EXISTS_BALANCE        = 914,  // 删除子账号存在余额
    HTTP_ERROR_NO_INVOICE_TEMPLATE              = 915,  // 无可用的发票信息
    HTTP_ERROR_RECEIPT_TYPE_HTTP_ERROR          = 916,  // 发票类型不匹配
    HTTP_ERROR_NO_LATEST_VERSION                = 920,  // 暂无客户端最新版本
    HTTP_ERROR_BALANCE_NOT_ENOUGH               = 921,  // 账户余额不够此次转账
    HTTP_ERROR_ACCOUNT_PHONE_BIND_FAIL          = 922,  // 手机未绑定账户
    HTTP_ERROR_RECEIPT_TEMPLATE_EXISTS          = 923,  // 发票模板已存在
    HTTP_ERROR_UPDATE_COUPON_ERROR              = 924,  // 更新用户优惠码失败
    HTTP_ERROR_INTELLIGENT_VERIFY_FAIL          = 925,  // 智能验证失败
    HTTP_ERROR_ORGANIZATION_NAME_EXISTS         = 926,  // 部门名称已存在
    HTTP_ERROR_PAY_RECORD_NOT_EXIST             = 927,  // 订单不存在
    HTTP_ERROR_THIRD_PARTIES_PAY_VERIFY_FAIL    = 928,  // 三方支付验签失败
    HTTP_ERROR_UPDATE_USER_ACCOUNT_BALANCE_FAIL = 929,  // 更新用户账户余额信息失败
    HTTP_ERROR_UPDATE_USER_ACCOUNT_ROLE_FAIL    = 930,  // 更新用户账户等级信息失败
    HTTP_ERROR_UPDATE_PAT_RECORD_FAIL           = 931,  // 异步回调更新订单信息失败
    HTTP_ERROR_ROLE_NAME_EXISTS                 = 932,  // 权限名称已存在
    HTTP_ERROR_CREATE_USER_PROJECT_ERROR        = 933,  // 创建用户默认project失败
    HTTP_ERROR_RPC_TASKRENDERING_ERROR          = 934,  // 调用task模块dubbo服务出错
    HTTP_ERROR_COUPON_USED_OR_EXPIRED_ERROR     = 935,  // 优惠码已使用或已过期
    HTTP_ERROR_SUBUSER_QUANTITY_ENOUGH          = 936,  // 添加子账号达到上限
    HTTP_ERROR_TASK_NOT_EXIST                   = 937,  // 任务不存在

    HTTP_ERROR_REDIS_CACHE_FAIL                 = 1000,  // redis缓存异常
	HTTP_ERROR_DIFFERENT_HARDWARE_CONFIG        = 1022, // 不同的硬件配置
	HTTP_ERROR_USER_AUTH_STATUS_INVALID         = 1206,  // 用户未实名

    HTTP_ERROR_UNKNOWN                          = 0x1000
};

/* 用户认证状态 */
enum enHttpAuthStatus {
    AUTHSTATUS_AUTH_NONE          = 0, // 未认证
    AUTHSTATUS_AUTH_DONE          = 1, // 已认证
    AUTHSTATUS_AUTH_AUTHING       = 2, // 审核中
    AUTHSTATUS_AUTH_FAILED        = 3, // 认证失败
    AUTHSTATUS_AUTH_WAITING       = 4, // 待审核
    AUTHSTATUS_AUTH_CHANGING      = 5, // 变更中
    AUTHSTATUS_AUTH_PERSONAL_OK   = 6, // 个人认证成功
    AUTHSTATUS_AUTH_ENTERPRISE_OK = 7, // 企业认证成功
};

/* 实名认证状态 */
enum enRealnameStatus {
    REALNAME_STATUS_DONE = 0,  // 0 已实名，可提交作业
    REALNAME_STATUS_DELAY = 1, // 1 (老用户未实名，时间小于21/11/01, 可提交作业，需提示)
    REALNAME_STATUS_NONE = 2,  // 2 未实名（新用户或老用户21/11/01后），不能提交作业
};

enum enSubmitTaskErrorType {
    SUBMIT_TASK_OK                   = 200, //任务提交成功
    SUBMIT_TASK_FAILED               = 100, //提交失败
    SUBMIT_TASK_INTERNAL_ERROR       = 500, //系统错误
    SUBMIT_TASK_MONEY_NOT_ENOUGH     = 507, //余额不足
    SUBMIT_TASK_FRAME_ANALYSE_FAILED = 595, //帧解析失败
    SUBMIT_TASK_SUBMIT_DENIED        = 749, //重复提交
    SUBMIT_TASK_FORMAT_ERROR         = 999, //文件格式错误595
    SUBMIT_TASK_NETWORK_ERROR        = 4096 //网络错误
};

//API URL
#define RESTAPI_URI_PLATFORM_LIST               "/api/rendering/task/common/queryPlatforms"
#ifdef FOXRENDERFARM
#define RESTAPI_URI_USER_LOGIN                  "/api/rendering/user/abroadUserLogin"
#else
#define RESTAPI_URI_USER_LOGIN                  "/api/rendering/user/userLogin"
#endif
#define RESTAPI_URI_ATTENTION_QRCODE            "/api/rendering/user/wxbind/attentionQRcode"
#define RESTAPI_URI_INVITE_QRCODE               "/api/rendering/user/wxbind/inviteQRcode"
#define RESTAPI_URI_USER_INFO                   "/api/rendering/user/queryUser"    
#define RESTAPI_URI_USER_BALANCE                "/api/rendering/user/queryUser"
#define RESTAPI_URI_GENERATE_REALNAME_AUTH      "/api/rendering/user/generateRealNameAuth"
#define RESTAPI_URI_TASK_SUMMARY                "/api/rendering/task/renderingTask/getTaskStatusCount"
#define RESTAPI_URI_RECENT_TASK                 "/api/rendering/task/renderingTask/getRecentTask"
#define RESTAPI_URI_USER_STORAGE                "/api/rendering/task/transfer/getUserStorageInfo"
#define RESTAPI_URI_USER_RENDER_SETTING         "/api/rendering/user/client/queryUserSetting"
#define RESTAPI_URI_USER_TRANS_BID              "/api/rendering/task/plugin/getPluginTransferBid"
#define RESTAPI_URI_USER_TRANS_CONFIG           "/api/rendering/task/plugin/getPluginTransferConfig"
#define RESTAPI_URI_UPDATE_USER_INFO            "/api/rendering/user/client/updateUserSetting"
#define RESTAPI_URI_USER_PRO_NOTICE             "/api/rendering/user/notice/clientProNotice"
#define RESTAPI_URI_USER_SYS_NOTICE             "/api/rendering/user/notice/clientSysNotice"
#define RESTAPI_URI_USER_PLUGIN_CONFIG          "/api/rendering/task/common/getUserPluginConfig"
#define RESTAPI_URI_USER_ADD_RENDER_PLUGIN      "/api/rendering/task/common/addUserRenderPlugin"
#define RESTAPI_URI_USER_EDIT_RENDER_PLUGIN     "/api/rendering/task/common/editUserRenderPlugin"
#define RESTAPI_URI_USER_DEFAULT_RENDER_PLUGIN  "/api/rendering/task/common/delUserPluginConfigOrSetDefault"
#define RESTAPI_URI_USER_REMOVE_RENDER_PLUGIN   "/api/rendering/task/common/delUserPluginConfigOrSetDefault"
#define RESTAPI_URI_USER_RENDER_SOFT_VERSION    "/api/rendering/task/common/getRenderSoftVersion"
#define RESTAPI_URI_ADVANCED_PARAM_SETTING      "/api/rendering/task/commit/advanceParameterSetting"
#define RESTAPI_URI_JOB_CREATE                  "/api/rendering/task/plugin/pluginCreateTaskId"
#define RESTAPI_URI_TASK_QUERYPUBLISH           "/api/rendering/task/publish/queryPublishList"
#define RESTAPI_URI_TASK_CREATEPUBLISH          "/api/rendering/task/publish/createPublish"
#define RESTAPI_URI_TASK_DELETEPUBLISH          "/api/rendering/task/publish/delPublish"
#define RESTAPI_URI_TASK_QUERYPUBTASK           "/api/rendering/task/publish/queryPublishTask"
#define RESTAPI_URI_TASK_UPLOADPIC              "/api/rendering/task/publish/uploadCoverImg"
#define RESTAPI_URI_TASK_STOPTASK               "/api/rendering/task/publish/stopPublishJob"
#define RESTAPI_URI_TASK_GETPUBLISHBYID         "/api/rendering/task/publish/getPublishById"
#define RESTAPI_URI_TASK_UPDATEPUB              "/api/rendering/task/publish/updatePublish"
#define RESTAPI_URI_TASK_CHECKID                "/api/rendering/task/publish/checkPublishDuplicate"

#define RESTAPI_URI_JOB_SUBMIT                  "/api/rendering/task/plugin/pluginSubmitTask"
#define RESTAPI_URI_JOB_RESUBMIT_CCAT           "/api/rendering/task/plugin/recommitCCAT"
#define RESTAPI_URI_JOB_SUBMIT_CLONE_TASK       "/api/rendering/task/commit/submitCloneTask"
#define RESTAPI_URI_JOB_UPDATE                  ""
#define RESTAPI_URI_JOB_LIST                    "/api/rendering/task/renderingTask/getRenderingTask/v2"
#define RESTAPI_URI_JOB_NEWLIST                 "/api/rendering/task/renderingTask/getRenderingTaskList"
#define RESTAPI_URI_JOB_TASK_LIST               "/api/rendering/task/historyTask/getHistoryTask"
#define RESTAPI_URI_JOB_TASK_UPDATE             ""
#define RESTAPI_URI_JOB_TASK_STOP               "/api/rendering/task/renderingTask/stopTask"
#define RESTAPI_URI_JOB_TASK_START              "/api/rendering/task/renderingTask/startTask"
#define RESTAPI_URI_JOB_TASK_RECOMMIT           "/api/rendering/task/renderingTask/restartTask"
#define RESTAPI_URI_JOB_TASK_ABORT              "/api/rendering/task/renderingTask/abandonTask"
#define RESTAPI_URI_JOB_TASK_DELETE             "/api/rendering/task/renderingTask/deleteTask"
#define RESTAPI_URI_JOB_TASK_FULL_SPEED         "/api/rendering/task/commit/commitMainPictureRender"
#define RESTAPI_URI_PROJECT_LIST                "/api/rendering/task/common/operateUserLabel"
#define RESTAPI_URI_PROJECT_NAME                "/api/rendering/task/renderingTask/loadingProjectName"
#define RESTAPI_URI_ERROR_LIST_CODE             "/api/rendering/task/common/getErrorCodeByCode"
#define RESTAPI_URI_ERROR_LIST_ALL              "/api/rendering/task/common/getAllErrorCodes"
#define RESTAPI_URI_JOB_FRAME_INFO              "/api/rendering/task/renderingTask/taskFrameRenderingInfo"
#define RESTAPI_URI_JOB_THUMBNAIL               "/api/rendering/task/renderingTask/loadingTaskThumbnail"
#define RESTAPI_URI_JOB_TOTAL_INFO              "/api/rendering/task/renderingTask/getTotalTaskFrameInfo"
#define RESTAPI_URI_JOB_FAILED_FRAME_RECOMMIT   "/api/rendering/task/renderingTask/recommitFailFrame"
#define RESTAPI_URI_JOB_TASK_PROCESS            "/api/rendering/task/renderingTask/loadingTaskProcessImg"
#define RESTAPI_URI_JOB_FRAME_GRAB              "/api/rendering/task/renderingTask/loadingTaskGrab"
#define RESTAPI_URI_JOB_FRAME_RENDERING_TIME    "/api/rendering/task/frame/renderingFrameTime"
#define RESTAPI_URI_JOB_LOAD_TASK_PARAMTER      "/api/rendering/task/renderingTask/loadingTaskParameter"
#define RESTAPI_URI_JOB_FRAME_RENDER_INFO       "/api/rendering/task/renderingTask/taskFrameRenderingInfo"
#define RESTAPI_URI_JOB_FRAME_RENDER_CONSUME    "/api/rendering/task/renderingTask/taskRenderingConsume"
#define RESTAPI_URI_JOB_FRAME_RENDER_LOG        "/api/rendering/task/renderingTask/showLog"
#define RESTAPI_URI_JOB_TASK_UPGRADE_TASK_RAM   "/api/rendering/task/renderingTask/upgradeTaskRam"
#define RESTAPI_URI_JOB_FRAME_RECOMMIT          "/api/rendering/task/renderingTask/recommitTaskFrame"
#define RESTAPI_URI_JOB_FRAME_START             "/api/rendering/task/renderingTask/startTaskFrame"
#define RESTAPI_URI_JOB_FRAME_STOP              "/api/rendering/task/renderingTask/stopTaskFrame"
#define RESTAPI_URI_LOADING_USER_NAME           "/api/rendering/task/renderingTask/loadingUserName"
#define RESTAPI_URI_JOB_QUERY_TASK_INFO         "/api/rendering/task/renderingTask/queryTaskInfo"
#define RESTAPI_URI_JOB_UPDATE_TASK_REMARK      "/api/rendering/task/renderingTask/updateTaskRemark"
#define RESTAPI_URI_JOB_UPDATE_TASK_LEVEL       "/api/rendering/task/renderingTask/updateTaskUserLevel"
#define RESTAPI_URI_JOB_UPDATE_TASK_OVERTIME    "/api/rendering/task/renderingTask/updateTaskOverTime"
#define RESTAPI_URI_JOB_UPDATE_TASK_TIMEOUT     "/api/rendering/task/renderingTask/setOverTimeStop"
#define RESTAPI_URI_JOB_LOAD_PRODUCER           "/api/rendering/task/renderingTask/loadingProducer"
#define RESTAPI_URI_JOB_NODE_DETAILS            "/api/rendering/task/renderingTask/getNodeDetails"
#define RESTAPI_URI_JOB_UPDATE_TASK_LIMIT       "/api/rendering/task/renderingTask/updateTaskLimit"
#define RESTAPI_URI_CUSTOM                      ""
#define RESTAPI_URI_SEND_VERIFY_CODE            "/api/rendering/user/validate/SendVerifyCode"
#define RESTAPI_URI_USER_LOGOUT                 "/api/rendering/user/userLogout"
#define RESTAPI_URI_USER_FEEDBACK               "/api/rendering/user/feedback/addUserFeedback"
#define RESTAPI_URI_TASK_PLATFORM_STATUS        "/api/rendering/task/common/getPlatformStatus"
#define RESTAPI_URI_BIND_WX_TO_USER             "/api/rendering/user/wxbind/bindWxToUser"
#define RESTAPI_URI_QUERY_BINDING_WX            "/api/rendering/user/wxbind/queryBindingWx"
#define RESTAPI_URI_DOWNLOAD_START              "/api/rendering/task/countdownload/startDownload"
#define RESTAPI_URI_DOWNLOAD_COMPLETE           "/api/rendering/task/countdownload/downloadComplete"
#define RESTAPI_URI_DOWNLOAD_COUNT              "/api/rendering/task/countdownload/queryTaskDownloadCount"
#define RESTAPI_URI_DOWNLOAD_INFO               "/api/rendering/task/countdownload/getTaskDownloadInfo"
#define RESTAPI_URI_UPLOAD_FILE                 "/api/rendering/upload/uploadfile"
#define RESTAPI_URI_LOADING_PROJECT_NAME        "/api/rendering/task/renderingTask/loadingProjectName"
#define RESTAPI_URI_TRANSPORTINFO               "/api/rendering/upload/transport/transportinfo"
#define RESTAPI_URI_FRAME_RECOMMIT_INFO         "/api/rendering/task/renderingTask/frameConsumeInfo" // 帧重提信息
#define RESTAPI_URI_OUTPUT_GETSIZE              "/api/rendering/task/output/getSize" // 任务下output的大小
#define RESTAPI_URI_PAYMENT_USE_VOUCHER         "/api/rendering/task/common/useVoucher" // 使用免单券
#define RESTAPI_URI_PAYMENT_QUERY_VOUCHER       "/api/rendering/pay/recharge/queryUserVouchers" // 查询免单券
#define RESTAPI_URI_PICTURE_PRICE               "/api/rendering/task/common/queryPicturePrice" // 查询效果图价格
#define RESTAPI_URI_AT_REPORT_DATA_URL          "/api/rendering/task/renderingTask/loadAtTaskReportDataUrl" // 空三报告获取路径
#define RESTAPI_URI_SHARE_REBUILD_RESULT_LINK   "/api/rendering/task/renderingTask/shareRebuildResultLink"  // 分享链接
#define RESTAPI_URI_AT_POINTCLOUD_DATA_URL      "/api/rendering/task/renderingTask/loadTaskPointCloudDataUrl" //定义空三点云数据获取路径
#define RESTAPI_URI_UPLOAD_JSON_FILE_URL        "/api/rendering/task/plugin/taskJsonFile" // 上传json文件
#define RESTAPI_URI_CHECK_IMAGE_POS_MD5         "/api/rendering/task/renderingTask/checkTaskPointCloudData"   //检查image_pos.json MD5值是否改变
 

#define RESTAPI_URI_THIRD_USER_LOGIN            "/api/rendering/user/third/login"
#define SSOAPI_URI_THIRD_TOKEN                  "/api/sso/thirdParty/getToken"
#define SSOAPI_URI_THIRD_TOKEN_LOGIN            "/api/sso/thirdParty/login"
#define SSOAPI_URI_SIGNIN                       "/api/sso/sign/signIn"

// API INDEX
enum {
    SSOAPI_SIGNIN                          = 10,
    SSOAPI_THIRD_TOKEN                     = 11,
    SSOAPI_THIRD_TOKEN_LOGIN               = 12,
    RESTAPI_USER_LOGIN                     = 100,
    RESTAPI_THIRD_USER_LOGIN               = 101,
    RESTAPI_USER_INFO                      = 102,
    RESTAPI_GENERATE_REALNAME_AUTH         = 103,
    RESTAPI_PLATFORM_LIST                  = 200,
    RESTAPI_TRANSPORTINFO                  = 201,
    RESTAPI_USER_BALANCE                   = 202,
    RESTAPI_TASK_SUMMARY                   = 203,
    RESTAPI_FRAME_SUMMARY                  = 204,
    RESTAPI_RECENT_TASK                    = 205,
    RESTAPI_USER_STORAGE                   = 206,
    RESTAPI_USER_RENDER_SETTING            = 207,
    RESTAPI_USER_TRANS_BID                 = 208,
    RESTAPI_UPDATE_USER_INFO               = 209,
    RESTAPI_USER_SEND_VERIFY_CODE          = 210,
    RESTAPI_USER_LOGOUT                    = 211,
    RESTAPI_USER_PRO_NOTICE                = 212,
    RESTAPI_USER_SYS_NOTICE                = 213,
    RESTAPI_USER_PLUGIN_CONFIG             = 214,
    RESTAPI_USER_ADD_RENDER_PLUGIN         = 215,
    RESTAPI_USER_EDIT_RENDER_PLUGIN        = 216,
    RESTAPI_USER_DEFAULT_RENDER_PLUGIN     = 217,
    RESTAPI_USER_REMOVE_RENDER_PLUGIN      = 218,
    RESTAPI_USER_RENDER_SOFT_VERSION       = 219,
    RESTAPI_ADVANCED_PARAM_SETTING         = 220,
    RESTAPI_USER_FEEDBACK                  = 221,
    RESTAPI_ATTENTION_QRCODE               = 222,
    RESTAPI_INVITE_QRCODE                  = 223,
    RESTAPI_BIND_WX_TO_USER                = 224,
    RESTAPI_QUERY_BINDING_WX               = 225,
    RESTAPI_USER_TRANS_CONFIG              = 226,
    RESTAPI_UPLOAD_FILE                    = 227,
	RESTAPI_TASK_QUERYPUBLISH              = 228,
	RESTAPI_TASK_CREATEPUBLISH             = 229,
	RESTAPI_TASK_QUERYPUBTASK              = 230,
	RESTAPI_TASK_DETELEPUBLISH             = 231,
	RESTAPI_TASK_UPLOADPIC                 = 232,
	RESTAPI_TASK_STOPTASK                  = 233,
	RESTAPI_TASK_GETPUBLISHBYID            = 234,
	RESTAPI_TASK_UPDATEPUB                 = 235,
	RESTAPI_JOB_NEWLIST                    = 236,
	RESTAPI_TASK_CHECKID                   = 237,
    RESTAPI_JOB_CREATE                     = 300,
    RESTAPI_JOB_SUBMIT                     = 301,
    RESTAPI_JOB_UPDATE                     = 302,
    RESTAPI_JOB_LIST                       = 303,
    RESTAPI_JOB_TASK_LIST                  = 304,
    RESTAPI_JOB_TASK_UPDATE                = 305,
    RESTAPI_JOB_TASK_STOP                  = 306,
    RESTAPI_JOB_TASK_START                 = 307,
    RESTAPI_JOB_TASK_RECOMMIT              = 308,
    RESTAPI_JOB_TASK_ABORT                 = 309,
    RESTAPI_JOB_TASK_FULL_SPEED            = 310,
    RESTAPI_JOB_TASK_DELETE                = 311,
    RESTAPI_JOB_RESUBMIT_CCAT              = 312,
    RESTAPI_JOB_SUBMIT_CLONE_TASK          = 313,
    RESTAPI_PROJECT_NAME                   = 401,
    RESTAPI_PROJECT_LIST                   = 402,
    RESTAPI_OPERATE_USER_LABEL             = 403,
    RESTAPI_JOB_TASK_FRAME_INFO            = 404,
    RESTAPI_JOB_TASK_THUMBNAIL             = 405,
    RESTAPI_JOB_TASK_TOTAL_INFO            = 406,
    RESTAPI_JOB_TASK_FAILED_FRAME_RECOMMIT = 407,
    RESTAPI_JOB_TASK_PROCESS               = 408,
    RESTAPI_JOB_LOAD_TASK_PARAMTER         = 409,
    RESTAPI_JOB_FRAME_RENDER_INFO          = 410,
    RESTAPI_JOB_FRAME_RENDER_LOG           = 411,
    RESTAPI_JOB_TASK_UPGRADE_TASK_RAM      = 412,
    RESTAPI_JOB_FRAME_RECOMMIT             = 413,
    RESTAPI_JOB_FRAME_START                = 414,
    RESTAPI_JOB_FRAME_STOP                 = 415,
    RESTAPI_JOB_QUERY_TASK_INFO            = 416,
    RESTAPI_JOB_UPDATE_TASK_REMARK         = 417,
    RESTAPI_JOB_UPDATE_TASK_LEVEL          = 418,
    RESTAPI_JOB_UPDATE_TASK_OVERTIME       = 419,
    RESTAPI_JOB_UPDATE_TASK_TIMEOUT        = 420,
    RESTAPI_JOB_LOAD_PRODUCER              = 421,
    RESTAPI_JOB_FRAME_RENDER_CONSUME       = 422,
    RESTAPI_JOB_UPDATE_TASK_LIMIT          = 423,
    RESTAPI_JOB_FRAME_RECOMMIT_INFO        = 424,
    RESTAPI_JOB_FRAME_RENDERING_TIME       = 425,
    RESTAPI_JOB_FRAME_NODE_DETAILS         = 426,
    RESTAPI_ERROR_LIST_CODE                = 500,
    RESTAPI_ERROR_LIST_ALL                 = 501,
    RESTAPI_LOADING_USER_NAME              = 601,
    RESTAPI_JOB_FRAME_GRAB                 = 602,
    RESTAPI_TASK_PLATFORM_STATUS           = 603,
    RESTAPI_LOADING_PROJECT_NAME           = 604,
    RESTAPI_DOWNLOAD_START                 = 700,
    RESTAPI_DOWNLOAD_COMPLETE              = 701,
    RESTAPI_DOWNLOAD_COUNT                 = 702,
    RESTAPI_DOWNLOAD_INFO                  = 703,
    RESTAPI_OUTPUT_GETSIZE                 = 800,
    RESTAPI_PAYMENT_USE_VOUCHER            = 810,
    RESTAPI_PAYMENT_QUERY_VOUCHER          = 811,
    RESTAPI_QUERY_PICTURE_PRICE            = 812,
    RESTAPI_AT_REPORT_DATA_URL             = 813,
    RESTAPI_SHARE_REBUILD_RESULT_LINK      = 814,
	RESTAPI_AT_POINTCLOUD_DATA_URL         = 815,
    RESTAPI_UPLOAD_JSON_FILE_URL           = 816,
	RESTAPI_CHECK_IMAGE_POS_MD5            = 817,
    RESTAPI_CUSTOM                         = 900,
    RESTAPI_END                            = 999,
};

typedef struct _rest_uri {
    int idx;
    QString name;
    QString uri;
} rest_uri;

static rest_uri rest_uri_set[] = {
    { SSOAPI_SIGNIN, "用户单点登录", SSOAPI_URI_SIGNIN },
    { RESTAPI_THIRD_USER_LOGIN, "三方用户登录", RESTAPI_URI_THIRD_USER_LOGIN },
    { SSOAPI_THIRD_TOKEN, "获取三方交互TOKEN", SSOAPI_URI_THIRD_TOKEN },
    { SSOAPI_THIRD_TOKEN_LOGIN, "使用三方交互TOKEN登录", SSOAPI_URI_THIRD_TOKEN_LOGIN },
    {RESTAPI_USER_LOGIN, "用户登录", RESTAPI_URI_USER_LOGIN},
    {RESTAPI_USER_INFO, "用户信息", RESTAPI_URI_USER_INFO},
    { RESTAPI_PLATFORM_LIST, "平台列表", RESTAPI_URI_PLATFORM_LIST },
	{ RESTAPI_GENERATE_REALNAME_AUTH, "生成实名认证KEY", RESTAPI_URI_GENERATE_REALNAME_AUTH },
    { RESTAPI_TRANSPORTINFO, "传输上报", RESTAPI_URI_TRANSPORTINFO },
    {RESTAPI_INVITE_QRCODE, "小程序邀请码", RESTAPI_URI_INVITE_QRCODE },
    {RESTAPI_ATTENTION_QRCODE, "小程序二维码", RESTAPI_URI_ATTENTION_QRCODE},
    {RESTAPI_BIND_WX_TO_USER, "是否同意绑定微信到该账户", RESTAPI_URI_BIND_WX_TO_USER},
    {RESTAPI_QUERY_BINDING_WX, "查询绑定到微信", RESTAPI_URI_QUERY_BINDING_WX},
    {RESTAPI_USER_BALANCE, "用户余额", RESTAPI_URI_USER_BALANCE},
    {RESTAPI_USER_RENDER_SETTING, "渲染设置", RESTAPI_URI_USER_RENDER_SETTING},
    {RESTAPI_USER_TRANS_BID, "传输BID", RESTAPI_URI_USER_TRANS_BID},
    {RESTAPI_USER_TRANS_CONFIG, "传输配置", RESTAPI_URI_USER_TRANS_CONFIG },
    {RESTAPI_UPDATE_USER_INFO, "更新用户设置", RESTAPI_URI_UPDATE_USER_INFO},
    {RESTAPI_USER_SEND_VERIFY_CODE, "设备验证", RESTAPI_URI_SEND_VERIFY_CODE},
    {RESTAPI_USER_PRO_NOTICE, "推广消息", RESTAPI_URI_USER_PRO_NOTICE},
    {RESTAPI_USER_SYS_NOTICE, "系统通知", RESTAPI_URI_USER_SYS_NOTICE},
    {RESTAPI_USER_ADD_RENDER_PLUGIN, "添加插件", RESTAPI_URI_USER_ADD_RENDER_PLUGIN},
    {RESTAPI_USER_REMOVE_RENDER_PLUGIN, "删除插件", RESTAPI_URI_USER_REMOVE_RENDER_PLUGIN},
    {RESTAPI_USER_DEFAULT_RENDER_PLUGIN, "默认插件", RESTAPI_URI_USER_DEFAULT_RENDER_PLUGIN},
    {RESTAPI_USER_EDIT_RENDER_PLUGIN, "修改插件", RESTAPI_URI_USER_EDIT_RENDER_PLUGIN},
    {RESTAPI_USER_PLUGIN_CONFIG, "插件列表", RESTAPI_URI_USER_PLUGIN_CONFIG},
    {RESTAPI_USER_RENDER_SOFT_VERSION, "渲染器版本", RESTAPI_URI_USER_RENDER_SOFT_VERSION},
    {RESTAPI_ADVANCED_PARAM_SETTING, "高级参数设置", RESTAPI_URI_ADVANCED_PARAM_SETTING},
    {RESTAPI_USER_FEEDBACK, "用户反馈", RESTAPI_URI_USER_FEEDBACK},
    {RESTAPI_USER_STORAGE, "用户总存储", RESTAPI_URI_USER_STORAGE},
    {RESTAPI_TASK_SUMMARY, "任务统计", RESTAPI_URI_TASK_SUMMARY},
    {RESTAPI_USER_LOGOUT, "用户退出", RESTAPI_URI_USER_LOGOUT},
    {RESTAPI_JOB_CREATE, "创建任务号", RESTAPI_URI_JOB_CREATE},
    {RESTAPI_JOB_SUBMIT, "提交任务", RESTAPI_URI_JOB_SUBMIT},
    {RESTAPI_JOB_SUBMIT_CLONE_TASK, "克隆任务", RESTAPI_URI_JOB_SUBMIT_CLONE_TASK },
    {RESTAPI_JOB_RESUBMIT_CCAT, "提交空三任务", RESTAPI_URI_JOB_RESUBMIT_CCAT},
    {RESTAPI_JOB_UPDATE, "更新任务", RESTAPI_URI_JOB_UPDATE},
    {RESTAPI_JOB_LIST, "任务列表", RESTAPI_URI_JOB_LIST},
    {RESTAPI_JOB_TASK_LIST, "详情列表", RESTAPI_URI_JOB_TASK_LIST},
    {RESTAPI_JOB_TASK_UPDATE, "任务更新", RESTAPI_URI_JOB_TASK_UPDATE},
    {RESTAPI_JOB_TASK_STOP, "任务停止", RESTAPI_URI_JOB_TASK_STOP},
    {RESTAPI_JOB_TASK_START, "任务开始", RESTAPI_URI_JOB_TASK_START},
    {RESTAPI_JOB_TASK_RECOMMIT, "任务重提", RESTAPI_URI_JOB_TASK_RECOMMIT},
    {RESTAPI_JOB_TASK_ABORT, "放弃任务", RESTAPI_URI_JOB_TASK_ABORT},
    {RESTAPI_JOB_TASK_DELETE, "删除任务", RESTAPI_URI_JOB_TASK_DELETE},
    {RESTAPI_JOB_TASK_FULL_SPEED, "全速任务", RESTAPI_URI_JOB_TASK_FULL_SPEED},
    {RESTAPI_PROJECT_LIST, "项目列表", RESTAPI_URI_PROJECT_LIST},
    {RESTAPI_JOB_TASK_FRAME_INFO, "帧详情", RESTAPI_URI_JOB_FRAME_INFO},
    {RESTAPI_JOB_TASK_THUMBNAIL, "缩略图", RESTAPI_URI_JOB_THUMBNAIL},
    {RESTAPI_JOB_TASK_TOTAL_INFO, "任务统计", RESTAPI_URI_JOB_TOTAL_INFO},
    {RESTAPI_JOB_TASK_FAILED_FRAME_RECOMMIT, "重提失败帧", RESTAPI_URI_JOB_FAILED_FRAME_RECOMMIT},
    {RESTAPI_JOB_TASK_PROCESS, "任务进度图", RESTAPI_URI_JOB_TASK_PROCESS},
    {RESTAPI_JOB_FRAME_GRAB, "帧进度图", RESTAPI_URI_JOB_FRAME_GRAB},
    {RESTAPI_JOB_FRAME_RENDERING_TIME, "帧进度耗时", RESTAPI_URI_JOB_FRAME_RENDERING_TIME },
    {RESTAPI_JOB_LOAD_TASK_PARAMTER, "任务详情", RESTAPI_URI_JOB_LOAD_TASK_PARAMTER},
    {RESTAPI_JOB_FRAME_RENDER_INFO, "帧详情", RESTAPI_URI_JOB_FRAME_RENDER_INFO},
    {RESTAPI_JOB_FRAME_RENDER_CONSUME, "作业消费", RESTAPI_URI_JOB_FRAME_RENDER_CONSUME},
    {RESTAPI_JOB_FRAME_RENDER_LOG, "渲染日志", RESTAPI_URI_JOB_FRAME_RENDER_LOG},
    {RESTAPI_JOB_TASK_UPGRADE_TASK_RAM, "升级渲染内存", RESTAPI_URI_JOB_TASK_UPGRADE_TASK_RAM},
    {RESTAPI_JOB_FRAME_START, "开始帧任务", RESTAPI_URI_JOB_FRAME_START},
    {RESTAPI_JOB_FRAME_STOP, "停止帧任务", RESTAPI_URI_JOB_FRAME_STOP},
    {RESTAPI_JOB_FRAME_RECOMMIT, "重提帧任务", RESTAPI_URI_JOB_FRAME_RECOMMIT},
    {RESTAPI_JOB_QUERY_TASK_INFO, "查询下载列表壳任务状态", RESTAPI_URI_JOB_QUERY_TASK_INFO},
    {RESTAPI_JOB_UPDATE_TASK_REMARK, "设置备注", RESTAPI_URI_JOB_UPDATE_TASK_REMARK},
    {RESTAPI_JOB_UPDATE_TASK_LEVEL, "设置优先级", RESTAPI_URI_JOB_UPDATE_TASK_LEVEL},
    {RESTAPI_JOB_UPDATE_TASK_OVERTIME, "设置超时提醒", RESTAPI_URI_JOB_UPDATE_TASK_OVERTIME},
    {RESTAPI_JOB_UPDATE_TASK_TIMEOUT, "设置停止提醒", RESTAPI_URI_JOB_UPDATE_TASK_TIMEOUT },
    {RESTAPI_JOB_LOAD_PRODUCER, "加载制作人", RESTAPI_URI_JOB_LOAD_PRODUCER},
    {RESTAPI_PROJECT_NAME, "加载项目名", RESTAPI_URI_PROJECT_NAME},
    {RESTAPI_ERROR_LIST_CODE, "错误码", RESTAPI_URI_ERROR_LIST_CODE},
    {RESTAPI_ERROR_LIST_ALL, "错误码列表", RESTAPI_URI_ERROR_LIST_ALL},
    {RESTAPI_LOADING_USER_NAME, "获取提交账号名", RESTAPI_URI_LOADING_USER_NAME},
    {RESTAPI_TASK_PLATFORM_STATUS, "获取平台状态", RESTAPI_URI_TASK_PLATFORM_STATUS},
    {RESTAPI_JOB_FRAME_NODE_DETAILS, "渲染节点详情", RESTAPI_URI_JOB_NODE_DETAILS},
    {RESTAPI_DOWNLOAD_START, "开始下载", RESTAPI_URI_DOWNLOAD_START },
    {RESTAPI_DOWNLOAD_COMPLETE, "完成下载", RESTAPI_URI_DOWNLOAD_COMPLETE },
    {RESTAPI_DOWNLOAD_COUNT, "任务下载统计", RESTAPI_URI_DOWNLOAD_COUNT },
    {RESTAPI_DOWNLOAD_INFO, "任务下载详情", RESTAPI_URI_DOWNLOAD_INFO },
    {RESTAPI_UPLOAD_FILE, "上报日志文件", RESTAPI_URI_UPLOAD_FILE},
	{RESTAPI_TASK_QUERYPUBLISH, "查找任务列表", RESTAPI_URI_TASK_QUERYPUBLISH },
	{RESTAPI_TASK_CREATEPUBLISH, "创建发布", RESTAPI_URI_TASK_CREATEPUBLISH },
	{RESTAPI_TASK_DETELEPUBLISH, "删除发布", RESTAPI_URI_TASK_DELETEPUBLISH },
	{RESTAPI_TASK_QUERYPUBTASK, "查找发布中的任务", RESTAPI_URI_TASK_QUERYPUBTASK },
	{RESTAPI_TASK_UPLOADPIC, "上传封面图片", RESTAPI_URI_TASK_UPLOADPIC },
	{RESTAPI_TASK_STOPTASK, "停止任务", RESTAPI_URI_TASK_STOPTASK },
	{RESTAPI_TASK_GETPUBLISHBYID, "通过发布id找详情", RESTAPI_URI_TASK_GETPUBLISHBYID },
	{ RESTAPI_TASK_UPDATEPUB, "通过发布id更新", RESTAPI_URI_TASK_UPDATEPUB},
	{ RESTAPI_JOB_NEWLIST, "发布功能寻找taskid", RESTAPI_URI_JOB_NEWLIST },
	{ RESTAPI_TASK_CHECKID, "验证id是否已经发布过任务", RESTAPI_URI_TASK_CHECKID },

    {RESTAPI_LOADING_PROJECT_NAME, "获取项目名", RESTAPI_URI_LOADING_PROJECT_NAME},
    {RESTAPI_JOB_UPDATE_TASK_LIMIT, "更新节点机数量", RESTAPI_URI_JOB_UPDATE_TASK_LIMIT},
    {RESTAPI_JOB_FRAME_RECOMMIT_INFO, "帧重提信息", RESTAPI_URI_FRAME_RECOMMIT_INFO},
    {RESTAPI_OUTPUT_GETSIZE, "获取output的大小", RESTAPI_URI_OUTPUT_GETSIZE},
    { RESTAPI_PAYMENT_USE_VOUCHER, "使用免单券", RESTAPI_URI_PAYMENT_USE_VOUCHER },
    { RESTAPI_PAYMENT_QUERY_VOUCHER, "查询免单券", RESTAPI_URI_PAYMENT_QUERY_VOUCHER },
    { RESTAPI_QUERY_PICTURE_PRICE, "查询效果图价格", RESTAPI_URI_PICTURE_PRICE },
    { RESTAPI_AT_REPORT_DATA_URL, "空三质量报告", RESTAPI_URI_AT_REPORT_DATA_URL },
    { RESTAPI_SHARE_REBUILD_RESULT_LINK, "空三成果分享", RESTAPI_URI_SHARE_REBUILD_RESULT_LINK },
	{ RESTAPI_AT_POINTCLOUD_DATA_URL,"空三成果显示",RESTAPI_URI_AT_POINTCLOUD_DATA_URL },
    { RESTAPI_UPLOAD_JSON_FILE_URL,"上传配置文件",RESTAPI_URI_UPLOAD_JSON_FILE_URL },
	{ RESTAPI_CHECK_IMAGE_POS_MD5,"检查文件MD5",RESTAPI_URI_CHECK_IMAGE_POS_MD5 },
    {RESTAPI_CUSTOM, "自定义请求", RESTAPI_URI_CUSTOM}
};

inline QString queryCmdName(int cmdType)
{
    QString cmdName = "undefined";

    for(int i = 0; i < (sizeof(rest_uri_set)/sizeof(_rest_uri)); i++) {
        if(cmdType == rest_uri_set[i].idx) {
            cmdName = rest_uri_set[i].name;
            break;
        }
    }

    return cmdName;
}

typedef struct _sso_error {
    HttpErrorCode errorCode;
    QString code;
    QString message;
    QString desc;
} sso_error;

static sso_error sso_error_set[] = {
    { SSO_ERROR_COMMON_SUCCESS                          , "Common.Success", "Success", "成功" },
    { SSO_ERROR_COMMON_FAILED                           , "Common.Failed", "Failed", "失败" },
    { SSO_ERROR_COMMON_FOBIDDEN                         , "Common.Fobidden", "Forbidden", "没有权限" },
    { SSO_ERROR_COMMON_REQUESTLIMITEXCEEDED             , "Common.RequestLimitExceeded", "Request Limit Exceeded", "操作频繁，请稍后再试！" },
    { SSO_ERROR_COMMON_INTERNAR_ERROR                   , "Common.Internar.Error", "Internal Error", "服务器处理失败" },
    { SSO_ERROR_COMMON_PARAMETER_INVALID                , "Common.Parameter.Invalid", "parameter_invalid", "非法参数" },
    { SSO_ERROR_COMMON_PARAMETER_EMPTY                  , "Common.Parameter.Empty", "parameter_cant_be_empty", "缺少必要参数" },
    { SSO_ERROR_COMMON_ILLEGAL_PROTOCOL                 , "Common.Illegal.Protocol", "illegal_protocol", "非法请求" },
    { SSO_ERROR_VALIDATE_ERROR                          , "Validate.Code.Error", "validate_code_error", "手机验证码错误" },
    { SSO_ERROR_VALIDATE_CODE_SENDERROR                 , "Validate.Code.SendError", "validate send code error", "验证码发送失败" },
    { SSO_ERROR_VALIDATE_LIMITEXCEEDED                  , "Validate.LimitExceeded", "verification_code_send_forbidden", "今日已不能发送验证码" },
    { SSO_ERROR_SIGNATURE_EXPIRED                       , "Signature.Expired", "signature expired", "签名过期" },
    { SSO_ERROR_SIGNATURE_INVALID                       , "Signature.Invalid", "signature invalid", "非法签名" },
    { SSO_ERROR_ACCOUNT_EMAIL_FORMAT                    , "Account.Email.Format", "wrong_email_format", "邮箱格式错误" },
    { SSO_ERROR_ACCOUNT_EMAIL_ACTIVED                   , "Account.Email.Actived", "email_have_actived", "邮箱已激活" },
    { SSO_ERROR_ACCOUNT_TOKEN_EXPIRED                   , "Account.Token.Expired", "token_expired", "token已过期" },
    { SSO_ERROR_ACCOUNT_EMAIL_NOTFOUND                  , "Account.Email.NotFound", "email_not_exist", "邮箱不存在" },
    { SSO_ERROR_ACCOUNT_PHONE_NOTFOUND                  , "Account.Phone.NotFound", "phone_number_not_exist", "手机号不存在" },
    { SSO_ERROR_ACCOUNT_UNACTIVATED                     , "Account.UnActivated", "account_no_active", "账号未激活" },
    { SSO_ERROR_ACCOUNT_LOCKED                          , "Account.Locked", "account_locked", "账号已被锁定" },
    { SSO_ERROR_ACCOUNT_SIGNIN_FAILED                   , "Account.SignIn.Failed", "account_username_password_fail", "用户名或密码错误" },
    { SSO_ERROR_ACCOUNT_SIGNIN_IPLIMIT                  , "Account.SignIn.IpLimit", "account_login_iplimitfail", "ip被限制登录" },
    { SSO_ERROR_ACCOUNT_DISABLED                        , "Account.Disabled", "account_disabled", "账号已被停用" },
    { SSO_ERROR_ACCOUNT_SIGN_GRAPHICVERIFICATIONREQUIRED, "Account.Sign.GraphicVerificationRequired", "account_login_need_graphic_verification", "需要图形验证码验证" },
    { SSO_ERROR_ACCOUNT_PHONE_EXISTED                   , "Account.Phone.Existed", "validate_phone_fail", "手机号已存在" },
    { SSO_ERROR_ACCOUNT_EMAIL_EXISTED                   , "Account.Email.Existed", "validate_email_fail", "邮箱已存在" },
    { SSO_ERROR_ACCOUNT_USERNAME_EXISTED                , "Account.UserName.Existed", "validate_username_fail", "用户名已存在" },
    { SSO_ERROR_ACCOUNT_THIRDPARTY_NOTFOUND             , "Account.ThirdParty.NotFound", "Account ThirdParty NotFound,Please Binding", "账号未绑定第三方用户" },
    { SSO_ERROR_ACCOUNT_THIRDPARTY_SIGNIN_FAILED        , "Account.ThirdParty.SignIn.Failed", "account_not_login", "账号未登录" },
    { SSO_ERROR_ACCOUNT_THIRDPARTY_SIGNIN_TIMEOUT       , "Account.ThirdParty.SignIn.Timeout", "account_login_timeout", "账号登录超时" },
    { SSO_ERROR_ACCOUNT_THIRDPARTY_AUTH_ERROR           , "Account.ThirdPart.Auth.Error", "third_no_openid", "获取三方帐号信息失败" },
    { SSO_ERROR_ACCOUNT_THIRDPARTY_UNIONID_EXISTED      , "Account.ThirdParty.UnionId.Existed", "account_unionid_existed", "UnionId已存在" },
    { SSO_ERROR_ACCOUNT_THIRDPARTY_BIND_EXISTED         , "Account.ThirdParty.Bind.Existed", "account_phone_bind_existed", "手机已绑定其他第三方用戶" },
    { SSO_ERROR_ACCOUNT_NOTFOUND                        , "Account.NotFound", "user_account_not_exist", "用户账号不存在" },
    { SSO_ERROR_ACCOUNT_VALIDATE_INTELLIGENTFAILED      , "Account.Validate.IntelligentFailed", "intelligent_verify_fail", "智能验证失败" },
    { SSO_ERROR_ACCOUNT_REGISTER_LOWSAFE                , "Account.Register.LowSafe", "REGISTER_LOW_SAFE", "您的手机号或邮箱安全等级太低，请换号注册，或者联系客服进行人工注册" },
    { SSO_ERROR_SYSTEM_CACHE_ERROR                      , "System.Cache.Error", "redis_cache_fail", "redis缓存异常" },
    { SSO_ERROR_SYSTEM_ERROR                            , "System.Error", "db_cud_fail", "数据库CUD失败" }
};

inline HttpErrorCode querySSOErrorCode(const QString& code)
{
    HttpErrorCode errorCode = HTTP_ERROR_UNKNOWN;

    for (int i = 0; i < (sizeof(sso_error_set) / sizeof(sso_error)); i++) {
        if (code == sso_error_set[i].code) {
            errorCode = sso_error_set[i].errorCode;
            break;
        }
    }

    return errorCode;
}