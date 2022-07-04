/***********************************************************************
* Module:  UserConfig.h
* Author:  hqr
* Created: 2018/07/19 15:07:15
* Modifier: hqr
* Modified: 2018/07/19 15:07:15
* Purpose: 用户资料相关
***********************************************************************/
#pragma once
#ifndef USER_CONFIG_H
#define USER_CONFIG_H
#include <QObject>
#include <QDateTime>
#include "kernel/msg.h"
#include "kernel/macro.h"

enum enUserType {
    MAIN_ACCOUNT = 1,
    CHILD_ACCOUNT = 2,
};

enum enLoginChannel {
    LOGIN_CHANNEL_0,        // 内部客户端登录
    LOGIN_CHANNEL_UNKOWN,   // 三方登录
};

struct UserSet {
    bool isChildAccount()
    {
        return this->accountType == CHILD_ACCOUNT;
    }
    qint32 channel; //渠道编号 请求渠道 。1：web国内；2：web国外；3：国内windows客户端；4：国外windonws客户端 ；5：微信公众号；
    qint32 platform; //平台号
    QString signature; //签名
    QString userKey; //用户唯一的key，判断用户有无登录的标志
    QString userAuthKey; //用户实名认证KEY
    QString version; //系统版本

    qint32 id;
    QString userName;
    qint32 accountType; //账号类型(1主账号 2子账号)
    QString loginType = "normal"; // normal/WeChat/QQ
    QString ip; // WAN IP
    QString raySyncUserKey; // 雷速user key
};

struct AccountSet {
    qint32 userId; //用户id
    QString userName; //用户名
    qint32 platform; //平台号
    QString phone; //手机
    QString email; //邮箱
    QString company; //公司
    QString name; //名称
    QString job; //职位
    QString country;
    QString city;
    QString address;
    QString communicationNumber; //qq或微信交流的号码
    int zone; //国内外区域
    qint32 softType; //软件编号
    qint32 softStatus; //是否已选软件 1已选择 0未选择
    qint32 businessType; //业务类型 0效果图 1动画
    qint32 status; //用户状态(4和5：被禁用 0：未激活 1：激活)
    qint32 infoStatus; // 是否已完善资料(0忽略1未完善2已完善)
    qint32 authStatus; // 认证状态
    qint32 realnameStatus; // 实名状态
    qint32 accountType; //账号类型 1主账号 2子账号
    qint32 userType; // 用户类型
    qint32 mainUserId; //主账号id
    qint32 level; //级别
    float rmbbalance; //人民币余额
    float usdbalance; //美元余额
    float rmbCumulative; //人民币总充值金额
    float usdCumulative; //美元总充值金额
    float credit; //信用积分
    float coupon; //优惠券
    QString description; //描述
    float cpuPrice; //cpu价格
    float gpuPrice; //gpu价格
    bool shareMainCapital; //使用主账号资产
    bool subDeleteTask; //子账号删除作业
    bool useMainBalance; //使用主账号余额
    bool hideBalance; //隐藏余额
    bool hideJobCharge; //隐藏作业扣费
    bool useLevelDirectory; //保持用户本地目录
    qint32 downloadDisable; //禁用下载 0正常,1 禁止下载,2 欠费停止下载
    bool picturePro; //是否是效果图专业版
    QString pictureProExpire; //效果图专业版是否过期
    QString accessId; // 设备绑定id
    QString password; // 账号密码
    int reLoginTimes; // 重新登录次数 max = 1
    int couponCount;  // 优惠券数量
    float qyCoupon;   // 青云券
    int qyCouponCount;// 青云券数量
    int enableNodeDetails; // 是否显示节点信息
    double gpuSingleDiscount; // 单卡GPU折扣
    int taskNodeLimitPermission; // 是否允许修改节点机数量
    int groupTaskLevel;// 是否开启任务优先级调节
    int openRenderRam; //是否显示物理内存
    qint64 picMemberValidity; // 效果图会员到期时间
    int pictureLever;
    quint64 delayAuthTime; // 延迟认证时间

    bool displayTaskLevel() {
        // 主账号一直显示
        if (isMainAccount()) {
            return true;
        }
        return groupTaskLevel == 1;
    }
    bool displayTaskLimit() {
        return taskNodeLimitPermission == 1;
    }
    bool isMainAccount() {
        return accountType == MAIN_ACCOUNT;
    }
    bool isChildAccount() {
        return accountType == CHILD_ACCOUNT;
    }
    bool displayRenderRam() {
        return openRenderRam == 1;
    }

    QList<memberHistory> memberHistoryVOS;
    bool isMemberTask(qint64 startTime) {
        foreach (memberHistory history, memberHistoryVOS){
            if (startTime >= history.startTime && startTime <= history.endTime) {
                return true;
            }
        }
        return false;
    }
};

struct RenderSet {
    qint32 singleNodeRenderFrames; // 是否忽略max错误贴图
    qint32 maxIgnoreMapFlag; // 是否忽略max错误贴图 0不忽略，1忽略
    qint32 autoCommit; // (网页分析用)是否启动场景参数渲染, 1不启用，2启用
    qint32 separateAccountFlag; // 主子账号分离设置
    qint32 miFileSwitchFlag; // 是否启用mi文件分析风险开关
    qint32 assFileSwitchFlag; // 是否启用ass文件分析风险开关
    qint32 manuallyStartAnalysisFlag; // 手动开启分析开关
    qint32 downloadDisable; // 禁用下载 1禁用，0不禁用
    qint32 ignoreMapFlag; // 本地分析忽略贴图丢失
    qint32 ignoreAnalyseWarn; // 忽略分析警告
    qint32 isVrayLicense; // 使用付费版vray渲染
    qint32 justUploadConfigFlag; // 本地分析max只上传配置文件
    qint32 justUploadCgFlag; // maya渲染只上传cg文件
    qint32 forceAnalysisFlag; // 本地分析强制分析所有代理

    qint32 taskOverTimeSec; // 超时提醒-秒
    qint32 jobTimeout; // 帧超时停止
    qint32 taskTimeout; // 任务超时停止

    qint32 downloadLimit;     // 限制下载
    qint32 downloadLimitDay;  // 限制下载天数
    qint32 cloneLimitDay;     // 克隆限制天数
    qint32 submitDisable;     // 提交是否禁止
};

struct ParamSet {
    struct Ram {
        QString name;
        qint32 value;
        qreal rate;
        bool isDefault;
    };
    QList<Ram> rams;
    struct TimeOut {
        qint32 minTimeOut;
        qint32 maxTimeOut;
        qint32 defaultTimeOut;
    } overtime; // 超时提醒

    struct timeoutStop {
        qint32 minTimeoutStop;
        qint32 maxTimeoutStop;
        qint32 defaultTimeoutStop;
    } timeoutstop; // 超时停止

    struct renderNum {
        qint32 minRenderNum;
        qint32 maxRenderNum;
        qint32 defaultRenderNum;
    } rendernum; // 一机多帧

    struct renderBlocker {
        qint32 maxBlocks;
        qint32 defaultBlocks;
    } renderblocks; // 分块

    struct Gpu {
        QString name;
        qint32 value;
        qreal rate;
        bool isDefault;
    };
    QList<Gpu> gpus; //gpu

    // ram
    int indexOfRAM(qint32 ramValue)
    {
        for (int i = 0; i < rams.size(); i++) {
            if(rams.at(i).value == ramValue)
                return i;
        }
        return 0;
    }
    qint32 getRAM(int index)
    {
        if (rams.isEmpty() || index >= rams.size())
            return 32;
        
        int ram = rams.at(index).value;
        if (ram >= 32) {
            return 32;
        }
        return ram;
    }
    qint32 getMaxRAM()
    {
        int Max = 0;
        for (int i = 0; i < rams.size(); i++) {
            if(rams.at(i).value > Max) {
                Max = rams.at(i).value;
            }
        }
        return Max;
    }

    // gpu
    int indexOfGPU(qint32 gpuValue)
    {
        for (int i = 0; i < gpus.size(); i++) {
            if (gpus.at(i).value == gpuValue)
                return i;
        }
        return 0;
    }
    qint32 getGPU(int index)
    {
        if (gpus.isEmpty() || index >= gpus.size())
            return 2;

        return gpus.at(index).value;
    }
    qint32 getMaxGPU()
    {
        int Max = 2;
        for (int i = 0; i < gpus.size(); i++) {
            if (gpus.at(i).value > Max) {
                Max = gpus.at(i).value;
            }
        }
        return Max;
    }
    bool isDefaultGpuCard(qint32 gpuValue)
    {
        for (int i = 0; i < gpus.size(); i++) {
            if (gpus.at(i).value == gpuValue)
                return gpus.at(i).isDefault;
        }
        return false;
    }
    bool hasGpuCountConfig()
    {
        return gpus.count() > 0;
    }
    int getDefaultGpuCard() 
    {
        int defCard = 2;
        for (int i = 0; i < gpus.size(); i++) {
            if (gpus.at(i).isDefault) {
                defCard = gpus.at(i).value;
                break;
            }
        }
        return defCard;
    }
};

struct ChildTransBidInfo {
    QString userId;
    QString outputBid;
};

struct StorageSet {
    qint32 commonBid;
    qint32 uploadBid;
    qint32 downloadBid;
    qint32 cloneBid;
    QMap<qint64, ChildTransBidInfo> childrenBid;

    qint32 getChildOutputBid(qint64 childUserId){
        auto it = childrenBid.find(childUserId);
        if (it == childrenBid.end()) {
            return 0;
        }
        return it.value().outputBid.toInt();
    }
};

// 客户端内存临时数据存放
struct CacheDataSet {
    qint32 loginChannel;
    qint32 platform;
    QString platformName;
    QString userName; // 客户端显示用
    QString password;
    SHOW_TASKTPE showType; // 获取选中状态类型
    qint32 clientType;
    QString agentName;
    QString version;
    qint32 platformStatus; // 平台状态
    CacheDataSet()
    {
        showType = TYPE_SHOW_ALLD;
        platformStatus = PLATFORM_STATUS_NORMAL;
    };
};

// 统计需要
struct TaskStatus {
    int waiting;
    int execute;
    int done;
    int failed;
    int total;
};
struct DownloadStatus {
    int waiting;
    int execute;
    int done;
    int total;
};
struct FrameStatus {
    int waiting;
    int execute;
    int done;
    int failed;
    int abandon;
    int total;
};
typedef struct RenderStatusSet {
    TaskStatus ts;
    DownloadStatus ds;
    FrameStatus fs;
} RenderStatus;

typedef struct VoucherSet{
    QList<VoucherInfo> effectiveVouchers; // 可用的免单券
    QList<VoucherInfo> invalidVouchers;   // 不可用的券

    bool hasEffectiveVoucher() {
        return effectiveVouchers.size() > 0;
    }

    int voucherCount() {
        return effectiveVouchers.size();
    }
}VoucherSet;

typedef struct PicturePrice {
    double memberRate;
    double commonRate;
    double maxMemberPrice;
    double maxCommonPrice;
}PicturePrice;

class UserConfig
{
public:
    UserConfig() {};
    ~UserConfig() {};

public:
    UserSet userSet;
    AccountSet accountSet;
    RenderSet renderSet;
    ParamSet paramSet;
    StorageSet storageSet;
    CacheDataSet client;
    RenderStatusSet renderStatusSet;
    QDateTime currentDateTime;
    VoucherSet voucherSet;
    PicturePrice picPrice;
};

extern UserConfig MyConfig;

#endif // _USER_CONFIG_H_
