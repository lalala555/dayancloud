/***********************************************************************
* Module:  RBHelper.h
* Author:  hqr
* Modified: 2017/04/18 17:00:13
* Purpose: RenderBus helper
***********************************************************************/
#pragma once
#ifndef RB_HELPER_H
#define RB_HELPER_H

#include <QString>
#include "kernel/msg.h"
#include "kernel/macro.h"
#include <QRect>

namespace RBHelper
{
    QString currentDateTime();
    QString timeCostToString(qint64 timeCost, int format = EN_FORMAT_WORDS);
    QString getVipDisplay(int level);
    QString getLocaleName();
    QString generateMsgId();
    int versionCompare(const QString& lhs, const QString& rhs);
    bool makeDirectory(const QString& folder);
    QString makePathFromConfigPath(const QString& name = "");
    QString makePathFromTempPath(const QString& name = "");
    QString getClientProjectPath(); // 获取工程配置文件路径
    void deleteUserDirectory(t_taskid taskId);
    QString getTempPath(const QString& folder = "");
    QString getAppDataPath();
    QString getAppDataLocalPath();
    QString getProgramDataPath();
    QString getAllUserProfilePath();
    QString getUserProfilePath();
    QString getProgramDataLocalPath();
    QString getOutputPath(t_taskid taskId, t_taskid parentId, const QString& scene, const QString& projectSavePath);
    QString getScriptRootPath();
    QString getPyScriptRootPath();
    QString getDefaultDownloadDir();
    QString getDefaultProjectDir();
    SCENE_TYPE typeOfScene(const QString& scene);
    CG_ID cgidOfScene(const QString& scene);
    bool loadJsonFile(const QString& jsonFile, QJsonObject& jsonObject);
    bool saveJsonFile(const QString& jsonFile, const QJsonObject& jsonObject);
    QString getUserStorageFolder(enAssetType type);
    QIcon getServerLoadIcon(enPlatformStatus load);
    QString getOutputStorageFolder();
    bool isNumber(const QString& text);
    bool isFloatValid(float speed);
    QString getOutputPathForThread(t_taskid taskId, t_taskid parentId, const QString& scene, const QString& projectSavePath, const QString& pypath);
    // 可以开放给开发
    bool isDeveloper();
    QString getTransmitErrorDisplay(int ec);
    QString getTransmitErrorUrl(int ec);
    QUrl getSimpleLinkUrl(const QString& uri);
    QUrl getLinkUrl(const QString& uri, const QString& extra = ""); // 获取带token的链接
    QUrl getRaysyncLinkUrl(const QString& uri); // 获取带token的链接
    float convertTransSpeed(float speed, int unit = SPEED_UNIT_KB);
    QString convertTransSpeedToStr(float speed, int display = DISPLAY_UNIT_BYTE);
    int convertSpeedUnit(float speed, int display = DISPLAY_UNIT_BYTE);
    QString getFileDeleteLogPath(t_taskid taskid);
    QString getReportPath(const QString& folder);
    QString getReportPathBatch(const QString& folder, t_taskid taskid, int type = 0, const QString& suffix = ".log");
    QString getSelectedFolder();
    QStringList getSelectedFiles(const QStringList& filters);
    QString getStatusText(qint32 status);
    QString getPlatformStatusText(qint32 status);
    void startSpeedTest(const QString& url, const QString& transmitUrl = "");
    double second2hour(qint32 sec, int decimals = 2);
    qint32 hour2second(double hour);
    QStringList getOutputParams(t_taskid taskId, t_taskid parentId, const QString& scene, const QString& projectSavePath);
    QString getOutputCmd(t_taskid taskId, t_taskid parentId, const QString& scene, const QString& projectSavePath);
    QString frameStatusToString(int status);
    QString ramUsageToString(qint64 value);
    QString pixelsToString(qint64 value);
    QString cpuUsageToString(qint32 value);
    QString feeCostToString(double couponFee, double arrearsFee, double feeAmount);
    QString UtcToDateTimeString(qint64 time);
    QString getWindowGrabPicPath(QWidget *widget, qint64 taskId, const QString& page, const QRect& rect = QRect()); // 窗口截图并获取路径
    QString getUploadCachePath();
    QString getLocalCgSofts();
    qint64  getJsonContainsValueCount(const QString& filePath, const QString& key);
    QString getFilterTime(int filterTime);
    QString getScriptHelperApp();
    QString getCgProductName(const QString& name);
    bool unavailableTime(const QString& time, int checkYear = 2000);
    QRegion roundRect(const QRect& rect, int r, bool topLeft, bool topRight, bool bottomLeft, bool bottomRight);
    QString encodingUrl(const QString& url, const QString& include = "!@#$;'");
    QString getJsonMakerPath();
    QString getSplitChar(int index);
    QString getFieldOrderString(int index);
    int getFieldOrderIndex(const QString& order);
    int getSplitCharIndex(const QString& ch);
    bool checkProjectExist(qint64 taskId, const QString& fileName = PROJECT_DATA_JSON);
    QString getOutputTypeString(OutputType type);
    QString getOutputTypeDisplay(OutputType type);
    QString getOutputFileName(OutputType type);
}

#endif // RB_HELPER_H
