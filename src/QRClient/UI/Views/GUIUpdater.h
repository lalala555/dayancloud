/***********************************************************************
* Module:  GUIUpdater.h
* Author:  hqr
* Modified: 2016/07/03 17:41:12
* Purpose: Declaration of the class
***********************************************************************/
#ifndef GUIUPDATER_H
#define GUIUPDATER_H

#include <QObject>
class GUIUpdater : public QObject
{
    Q_OBJECT

public:
    explicit GUIUpdater(QObject *parent = 0) : QObject(parent) {}
    void handleCommandType(intptr_t wParam, intptr_t lParam)
    {
        emit commandTypeSignal(wParam, lParam);
    }
    void updateTasklistUi(intptr_t wParam, intptr_t lParam)
    {
        emit updateTasklistUiSignal(wParam, lParam);
    }
    void infolistUi(intptr_t wParam, intptr_t lParam)
    {
        emit infolistUiSignal(wParam, lParam);
    }
    void smallpicRet(intptr_t wParam, intptr_t lParam)
    {
        emit smallpicRetSignal(wParam, lParam);
    }
    void loginRet(intptr_t wParam, intptr_t lParam)
    {
        emit loginRetSignal(wParam, lParam);
    }
    void deleteTaskUi(intptr_t wParam, intptr_t lParam)
    {
        emit deleteTaskUiSignal(wParam, lParam);
    }
    void downloadComplete(intptr_t wParam, intptr_t lParam)
    {
        emit downloadCompleteSignal(wParam, lParam);
    }
    void changeTransMode(intptr_t wParam, intptr_t lParam)
    {
        emit changeTransModeSignal(wParam, lParam);
    }
    void appendLogInfo(intptr_t wParam, intptr_t lParam)
    {
        emit appendLogInfoSignal(wParam, lParam);
    }
    void command(intptr_t wParam, intptr_t lParam)
    {
        emit commandSignal(wParam, lParam);
    }
    void logout(intptr_t wParam, intptr_t lParam)
    {
        emit logoutSignal(wParam, lParam);
    }
    void badgeNumber(intptr_t wParam, intptr_t lParam)
    {
        emit badgeNumberSignal(wParam, lParam);
    }
    void updateProjectList(intptr_t wParam, intptr_t lParam)
    {
        emit updateProjectListSignal(wParam, lParam);
    }
    void viewStateChanged(intptr_t wParam, intptr_t lParam)
    {
        emit viewStateChangedSignal(wParam, lParam);
    }
    void startUploadTask(intptr_t wParam, intptr_t lParam)
    {
        emit startUploadTaskSignal(wParam, lParam);
    }
    void showMsgBox(intptr_t wParam, intptr_t lParam)
    {
        emit showMsgBoxSignal(wParam, lParam);
    }
    void showMsgError(intptr_t wParam, intptr_t lParam)
    {
        emit showMsgErrorSignal(wParam, lParam);
    }
    void creatProjectError(intptr_t wParam, intptr_t lParam)
    {
        emit creatProjectErrorSignal(wParam, lParam);
    }
    void relaunch(intptr_t wParam, intptr_t lParam)
    {
        emit relaunchSignal(wParam, lParam);
    }
    void showLoading(intptr_t wParam, intptr_t lParam)
    {
        emit showLoadingSignal(wParam, lParam);
    }
    void cloneCfgRet(intptr_t wParam, intptr_t lParam)
    {
        emit cloneCfgRetSignal(wParam, lParam);
    }
    void cloneCfgFail(intptr_t wParam, intptr_t lParam)
    {
        emit cloneCfgFailSignal(wParam, lParam);
    }
    void cloneCreateTaskRet(intptr_t wParam, intptr_t lParam)
    {
        emit cloneCreateTaskRetSignal(wParam, lParam);
    }
    void downloadUpdateRet(intptr_t wParam, intptr_t lParam)
    {
        emit downloadUpdateRetSignal(wParam, lParam);
    }
    void checkUpdateRet(intptr_t wParam, intptr_t lParam)
    {
        emit checkUpdateRetSignal(wParam, lParam);
    }
    void checkUpdate(intptr_t wParam, intptr_t lParam)
    {
        emit checkUpdateSignal(wParam, lParam);
    }
    void quit4Update(intptr_t wParam, intptr_t lParam)
    {
        emit quit4UpdateSignal(wParam, lParam);
    }
    void submitTaskRet(intptr_t wParam, intptr_t lParam)
    {
        emit submitTaskRetSignal(wParam, lParam);
    }
    void continueDropInfo(intptr_t wParam, intptr_t lParam)
    {
        emit continueDropInfoSignal(wParam, lParam);
    }
    void batchSetParam(intptr_t wParam, intptr_t lParam)
    {
        emit batchSetParamSignal(wParam, lParam);
    }
    void setReddot(intptr_t wParam, intptr_t lParam)
    {
        emit setReddotSignal(wParam, lParam);
    }
    void searchtoTaskList(intptr_t wParam, intptr_t lParam)
    {
        emit searchtoTaskListSignal(wParam, lParam);
    }
    void uploadAndDownLoadError(intptr_t wParam, intptr_t lParam)
    {
        emit uploadAndDownLoadErrorSignal(wParam, lParam);
    }
    void netStateChanged(intptr_t wParam, intptr_t lParam)
    {
        emit netStateChangedSignal(wParam, lParam);
    }
    void batchCancel(intptr_t wParam, intptr_t lParam)
    {
        emit batchCancelSignal(wParam, lParam);
    }
    void showPluginList(intptr_t wParam, intptr_t lParam)
    {
        emit showPluginListSignal(wParam, lParam);
    }
    void showConfigList(intptr_t /*wParam*/, intptr_t /*lParam*/)
    {
        emit updateConfigListSignal();
    }
    void deleteActionSettingPage(intptr_t wParam, intptr_t lParam)
    {
        emit deleteActionSettingPageSignal(wParam, lParam);
    }
    void updateNetworkData(intptr_t wParam, intptr_t lParam)
    {
        emit updateNetworkSignal(wParam, lParam);
    }
    void finishNetwork(intptr_t wParam, intptr_t lParam)
    {
        emit finishNetworkSignal(wParam, lParam);
    }
    void dependentDeleteTaskId(intptr_t wParam, intptr_t lParam)
    {
        emit dependentDeleteSignal(wParam, lParam);
    }
    void speedLimit(intptr_t wParam, intptr_t lParam)
    {
        emit speedLimitSignal(wParam, lParam);
    }
    void operatingPluginConfig(intptr_t wParam, intptr_t lParam)
    {
        emit operatingPluginConfigSignal(wParam, lParam);
    }
    void updateProjectLocalPath(intptr_t wParam, intptr_t lParam)
    {
        emit updateProjectLocalPathSignal(wParam, lParam);
    }
    void execScriptFinished(intptr_t wParam, intptr_t lParam)
    {
        emit execScriptFinishedSignal(wParam, lParam);
    }
    void loadFileFinished(intptr_t wParam, intptr_t lParam)
    {
        emit loadFileFinishedSignal(wParam, lParam);
    }
    void messageCompleted(intptr_t wParam, intptr_t lParam)
    {
        emit messageCompletedSignal(wParam, lParam);
    }
    void updateClient(intptr_t wParam, intptr_t lParam)
    {
        emit updateClientSignal(wParam, lParam);
    }
    void updateScripts(intptr_t wParam, intptr_t lParam)
    {
        emit updateScriptsSignal(wParam, lParam);
    }
    void updateIssues(intptr_t wParam, intptr_t lParam)
    {
        emit updateIssuesSignal(wParam, lParam);
    }
    void projectSetting(intptr_t wParam, intptr_t lParam)
    {
        emit projectSettingSignal(wParam, lParam);
    }
    void dependTaskCompleted(intptr_t wParam, intptr_t lParam)
    {
        emit dependTaskCompletedSignal(wParam, lParam);
    }
    void triggerSchedule(intptr_t wParam, intptr_t lParam)
    {
        emit triggerScheduleSignal(wParam, lParam);
    }
    void transModeChanged(intptr_t wParam, intptr_t lParam)
    {
        emit transModeChangedSignal(wParam, lParam);
    }
    void transServerChanged(intptr_t wParam, intptr_t lParam)
    {
        emit transServerChangedSignal(wParam, lParam);
    }
    void newsContentResponse(intptr_t wParam, intptr_t lParam)
    {
        emit newsContentResponseSignal(wParam, lParam);
    }
    void needUserLogin(intptr_t wParam, intptr_t lParam)
    {
        emit needUserLoginSignal(wParam, lParam);
    }
    void accountUsernamePasswordFail(intptr_t wParam, intptr_t lParam)
    {
        emit accountUsernamePasswordFailSignal(wParam, lParam);
    }
    void updateUserNameListResponse(intptr_t wParam, intptr_t lParam)
    {
        emit updateUserNameListSignal(wParam, lParam);
    }
    void updateTransSpeedDisplay(intptr_t wParam, intptr_t lParam)
    {
        emit updateTransSpeedDisplaySignal(wParam, lParam);
    }
    void fileCheckExceptionRespone(intptr_t wParam, intptr_t lParam)
    {
        emit fileCheckExceptionResponeSignal(wParam, lParam);
    }
    void updatePlatformStatus(intptr_t wParam, intptr_t lParam)
    {
        emit updatePlatformStatusSignal(wParam, lParam);
    }
    void updateAllProjNames(intptr_t wParam, intptr_t lParam)
    {
        emit updateAllProjNamesSignal(wParam, lParam);
    }
    void updateDownloadListUi(intptr_t wParam, intptr_t lParam)
    {
        emit updateDownloadListUiSignal(wParam, lParam);
    }
    void updateMainUi(intptr_t wParam, intptr_t lParam)
    {
        emit updateMainUiSignal(wParam, lParam);
    }
    void updateMainUiBtnState(intptr_t wParam, intptr_t lParam)
    {
        emit updateMainUiBtnStateSignal(wParam, lParam);
    }
    void showProcessView(intptr_t wParam, intptr_t lParam)
    {
        emit showProcessViewSignal(wParam, lParam);
    }
    void locateProjectPage(intptr_t wParam, intptr_t lParam)
    {
        emit locateProjectPageSignal(wParam, lParam);
    }
    void resubmitTaskRet(intptr_t wParam, intptr_t lParam)
    {
        emit resubmitTaskRetSignal(wParam, lParam);
    }
private:

signals:
    void commandTypeSignal(intptr_t, intptr_t);
    void updateTasklistUiSignal(intptr_t, intptr_t);
    void infolistUiSignal(intptr_t, intptr_t);
    void smallpicRetSignal(intptr_t, intptr_t);
    void loginRetSignal(intptr_t, intptr_t);
    void deleteTaskUiSignal(intptr_t, intptr_t);
    void downloadCompleteSignal(intptr_t, intptr_t);
    void changeTransModeSignal(intptr_t, intptr_t);
    void appendLogInfoSignal(intptr_t, intptr_t);
    void commandSignal(intptr_t, intptr_t);
    void logoutSignal(intptr_t, intptr_t);
    void badgeNumberSignal(intptr_t, intptr_t);
    void updateProjectListSignal(intptr_t, intptr_t);
    void viewStateChangedSignal(intptr_t, intptr_t);
    void startUploadTaskSignal(intptr_t, intptr_t);
    void showMsgBoxSignal(intptr_t, intptr_t);
    void showMsgErrorSignal(intptr_t, intptr_t);
    void creatProjectErrorSignal(intptr_t, intptr_t);
    void relaunchSignal(intptr_t, intptr_t);
    void showLoadingSignal(intptr_t, intptr_t);
    void cloneCfgRetSignal(intptr_t, intptr_t);
    void cloneCfgFailSignal(intptr_t, intptr_t);
    void cloneCreateTaskRetSignal(intptr_t, intptr_t);
    void downloadUpdateRetSignal(intptr_t, intptr_t);
    void checkUpdateRetSignal(intptr_t, intptr_t);
    void checkUpdateSignal(intptr_t, intptr_t);
    void quit4UpdateSignal(intptr_t, intptr_t);
    void submitTaskRetSignal(intptr_t, intptr_t);
    void continueDropInfoSignal(intptr_t, intptr_t);
    void batchSetParamSignal(intptr_t, intptr_t);
    void setReddotSignal(intptr_t, intptr_t);
    void searchtoTaskListSignal(intptr_t, intptr_t);
    void uploadAndDownLoadErrorSignal(intptr_t, intptr_t);
    void netStateChangedSignal(intptr_t, intptr_t);
    void batchCancelSignal(intptr_t, intptr_t);
    void showPluginListSignal(intptr_t, intptr_t);
    void updateConfigListSignal();
    void deleteActionSettingPageSignal(intptr_t, intptr_t);
    void updateNetworkSignal(intptr_t, intptr_t);
    void finishNetworkSignal(intptr_t, intptr_t);
    void dependentDeleteSignal(intptr_t, intptr_t);
    void speedLimitSignal(intptr_t, intptr_t);
    void operatingPluginConfigSignal(intptr_t, intptr_t);
    void updateProjectLocalPathSignal(intptr_t, intptr_t);
    void execScriptFinishedSignal(intptr_t, intptr_t);
    void loadFileFinishedSignal(intptr_t, intptr_t);
    void messageCompletedSignal(intptr_t, intptr_t);
    void updateClientSignal(intptr_t, intptr_t);
    void updateScriptsSignal(intptr_t, intptr_t);
    void updateIssuesSignal(intptr_t, intptr_t);
    void projectSettingSignal(intptr_t, intptr_t);
    void dependTaskCompletedSignal(intptr_t, intptr_t);
    void triggerScheduleSignal(intptr_t, intptr_t);
    void transModeChangedSignal(intptr_t, intptr_t);
    void transServerChangedSignal(intptr_t, intptr_t);
    void newsContentResponseSignal(intptr_t, intptr_t);
    void needUserLoginSignal(intptr_t, intptr_t);
    void accountUsernamePasswordFailSignal(intptr_t, intptr_t);
    void updateUserNameListSignal(intptr_t, intptr_t);
    void updateTransSpeedDisplaySignal(intptr_t, intptr_t);
    void fileCheckExceptionResponeSignal(intptr_t, intptr_t);
    void updatePlatformStatusSignal(intptr_t, intptr_t);
    void updateAllProjNamesSignal(intptr_t, intptr_t);
    void updateDownloadListUiSignal(intptr_t, intptr_t);
    void updateMainUiSignal(intptr_t, intptr_t);
    void updateMainUiBtnStateSignal(intptr_t, intptr_t);
    void showProcessViewSignal(intptr_t, intptr_t);
    void locateProjectPageSignal(intptr_t, intptr_t);
    void resubmitTaskRetSignal(intptr_t, intptr_t);
};

class MessageDispatcher: public QObject
{
    Q_OBJECT
public:
    void sendSignal(int msg, intptr_t wParam, intptr_t lParam)
    {
        emit handleMessage(msg, wParam, lParam);
    };
signals:
    void handleMessage(int msg, intptr_t wParam, intptr_t lParam);
};

class CmdResponser : public QObject
{
    Q_OBJECT
public:
    CmdResponser() {};

signals:
    void cmdResponse(int sid, void* resp);
};

#endif // GUIUPDATER_H
