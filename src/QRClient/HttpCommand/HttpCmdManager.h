#ifndef HTTPCMDMANAGER_H
#define HTTPCMDMANAGER_H

#include <QObject>
#include <QList>
#include <QJsonObject>
#include "HttpCommand/HttpCommon.h"
#include "HttpCmdResponse.h"

class HttpCmdRequest;
class CommandServer;
class HttpCmdManager : public QObject
{
    Q_OBJECT
private:
    HttpCmdManager(QObject *parent = 0);
    ~HttpCmdManager();
public:
    static HttpCmdManager* getInstance()
    {
        static HttpCmdManager instance;
        return &instance;
    }
    void setRequestHeader(const HttpRequestHeader& header)
    {
        m_header = header;
    }
    HttpRequestHeader getRequestHeader()
    {
        return m_header;
    }
    void setRequestHost(const QString& host)
    {
        m_host = host;
    }
    QString getRequestHost()
    {
        return m_host;
    }
    void setSearchContent(const QString& content)
    {
        m_searchContent = content;
    }
    QString getSearchContent()
    {
        return m_searchContent;
    }
    void setSearchType(const QString& type)
    {
        m_searchType = type;
    }
    QString getSearchType()
    {
        return m_searchType;
    }
    void setCurTimeFilter(int curTimeFilter)
    {
        m_curTimeFilter = curTimeFilter;
    }
    int getCurTimeFilter()
    {
        return m_curTimeFilter;
    }
public:
    void ssoLogin(const QString& username, const QString& passwd, QObject* observer = Q_NULLPTR);
    void thirdLogin(const QString & userKey, int type, QObject * observer = Q_NULLPTR);
public:
    void initRequestHeader();
    void login(const QString& userKey, const QString& accessId, QObject* observer = Q_NULLPTR);
    void platformList(QObject* observer = Q_NULLPTR);
    void userInfo(QObject* observer = Q_NULLPTR);
    void realnameAuthKey(QObject* observer = Q_NULLPTR);
    void transformBid(QObject* observer = Q_NULLPTR);
    void transformConfig(QObject* observer = Q_NULLPTR);
    void userBalance(QObject* observer = Q_NULLPTR);
    void renderList(const FilterContent& content, QObject* observer = Q_NULLPTR);
    void userLogout(QObject* observer = Q_NULLPTR);

    void taskOperatorStart(QList<qint64> taskIds, QObject* observer = Q_NULLPTR);
    void taskOperatorStart(qint64 taskId, QObject* observer = Q_NULLPTR);

    void taskOperatorStop(QList<qint64> taskIds, QObject* observer = Q_NULLPTR);
    void taskOperatorStop(qint64 taskId, const QString& option = "render", QObject* observer = Q_NULLPTR);

    void taskOperatorResub(QList<int> resubStatus, QList<qint64> taskIds, QObject* observer = Q_NULLPTR);
    void taskOperatorResub(int resubStatus, qint64 taskId, QObject* observer = Q_NULLPTR);
	void taskOperatorResubAtSurvey(int resubStatus, qint64 taskId, const QString& surveyFile, QObject* observer = Q_NULLPTR);

    void taskOperatorDelete(QList<qint64> taskIds, QObject* observer = Q_NULLPTR);
    void taskOperatorDelete(qint64 taskId, QObject* observer = Q_NULLPTR);

    // ????????
    void taskOperatorFullSpeed(QList<qint64> taskIds, QObject* observer = Q_NULLPTR);
    void taskOperatorFullSpeed(qint64 taskId, QObject* observer = Q_NULLPTR);

    // ????????????
    void taskUpgradeRam(QList<qint64> taskIds, int ram, QObject* observer = Q_NULLPTR);
    void taskUpgradeRam(qint64 taskId, int ram, QObject* observer = Q_NULLPTR);

    void taskSubmit(qint64 jobId, const QString& sceneName, QObject* observer = Q_NULLPTR);
    void taskSubmitAT(qint64 jobId, const QString& sceneName, int atType, QObject* observer = Q_NULLPTR);
    void taskSubmitRebuild(qint64 jobId, const QString& sceneName, const RebuildOptionParams& params, QObject* observer = Q_NULLPTR);
    void taskSubmitATSurvey(qint64 jobId, const QString& surveyFile, const QString& sceneName, QObject* observer = Q_NULLPTR);

    void cloneJobId(qint64 jobId, QObject* observer = Q_NULLPTR);
    void createJobIds(int createCount, int createType, QObject* observer = Q_NULLPTR);
    //????????
	void getPublishList(QString publishName = NULL, QString publishId = NULL, int status = 0, QObject* observer = Q_NULLPTR);
    //????????
	void createPublish(const PublishItem &item, QObject* observer = Q_NULLPTR);
	//????????
	void deletePublish(int id, QObject* observer = Q_NULLPTR);
	//????????????
	void uploadPic(const QString& filePath, QObject* observer = Q_NULLPTR);
	//????????????????
	void getPublishTask(int id, QObject* observer = Q_NULLPTR);
	//????????
	void stopPublishTask(int id, QObject* observer = Q_NULLPTR);
	//????id????????
	void getPublishById(int id, QObject* observer = Q_NULLPTR);
	//????fabu
	void updatePublish(const PublishItem &item, QObject* observer = Q_NULLPTR);
	//????taskidnew
	void renderListNew(const FilterContent& content, QObject* observer = Q_NULLPTR);
	//??????id????????????
	void checkId(int id, QObject* observer = Q_NULLPTR);
	//????????
	//void getPublishById();
    void createJobIds(socket_id sid, CommandServer* commandserver, int createCount, int createType, QObject* observer = Q_NULLPTR);
    void taskTotalFrameInfo(QObject* observer = Q_NULLPTR);
    void updateUserInfo(qint32 taskOverTime, QObject* observer = Q_NULLPTR);
    void updateFilterTime(); // ????????????
    void queryUserRenderSetting(QObject* observer = Q_NULLPTR);
    void getTaskStatusCount(QObject* observer = Q_NULLPTR);
    void getRecentTask(QObject* observer = Q_NULLPTR);
    void getUserStorageInfo(QObject* observer = Q_NULLPTR);
    void loadingProjectName(QObject* observer = Q_NULLPTR);

    void queryMajorProgress(qint64 taskid, QObject* observer = Q_NULLPTR);
    void queryPhotonProgress(qint64 taskid, QObject* observer = Q_NULLPTR);
    void queryJobFrameGrab(qint32 frameId, QObject* observer = Q_NULLPTR);
    void queryJobFrameRenderingTime(qint32 frameId, QObject* observer = Q_NULLPTR);

    void qureyClientProNotice(int channel, int lang, QObject* observer = Q_NULLPTR);
    /* ??????????-1???? ????????????????????????*/
    void qureyClientSysNotice(int channel, int lang, int noticeType = -1, QObject* observer = Q_NULLPTR);

    void loadingTaskParamters(qint64 taskid, QObject* observer = Q_NULLPTR);
    void qureyTaskFrameRenderingInfo(qint64 taskid, qint32 pageNum, qint32 pageSize, QObject* observer = Q_NULLPTR);// ??????
    void loadTaskThumbnail(qint64 frameId, int frameStatus, QObject* observer = Q_NULLPTR);
    void loadFrameRenderLog(qint64 frameId, int pageNum, const QString& renderingType, qint32 pageSize = 16384, QObject* observer = Q_NULLPTR);
    void searchTaskFrameRenderingInfo(qint64 taskid, const QString& searchKey, const QList<int>& stateList, qint32 pageNum, qint32 pageSize, QObject* observer = Q_NULLPTR);// ??????????
    void queryTaskRenderingConsume(qint64 taskid, QObject* observer = Q_NULLPTR);

    // ????????????????
    void queryRenderSoftVersion(qint64 cgId, const QString& os = "windows", QObject* observer = Q_NULLPTR); // ????????????????
    void queryUserPluginConfig(QList<qint64> cgId = QList<qint64>(), QObject* observer = Q_NULLPTR); // ????????????
    void addUserPluginConfig(OperateUserRenderPlugin* opt, QObject* observer = Q_NULLPTR); // ????????????
    void removeUserPluginConfig(OperateUserRenderPlugin* opt, QObject* observer = Q_NULLPTR); // ????????????
    void setDefaultUserPluginConfig(OperateUserRenderPlugin* opt, QObject* observer = Q_NULLPTR); // ????????????????
    void editUserPluginConfig(OperateUserRenderPlugin* opt, QObject* observer = Q_NULLPTR); // ????????????
    // ????????????
    void queryUserProjectList(QObject* observer = Q_NULLPTR);
    void addUserProject(const QString& projName, QObject* observer = Q_NULLPTR);
    void deleteUserProject(const QString& projName, QObject* observer = Q_NULLPTR);
    void loadingAccountsProjNames(QObject* observer = Q_NULLPTR);
    // ?????? ????
    void frameOperatorStart(qint64 taskId, QList<qint64> frameIds, int selectAll, QObject* observer = Q_NULLPTR);
    void frameOperatorStart(qint64 taskId, qint64 frameId, int selectAll,QObject* observer = Q_NULLPTR);
    // ?????? ????
    void frameOperatorRecommit(qint64 taskId, QList<qint64> frameIds, int selectAll, QObject* observer = Q_NULLPTR);
    void frameOperatorRecommit(qint64 taskId, qint64 frameId, int selectAll,QObject* observer = Q_NULLPTR);
    // ?????? ????
    void frameOperatorStop(qint64 taskId, QList<qint64> frameIds, int selectAll, QObject* observer = Q_NULLPTR);
    void frameOperatorStop(qint64 taskId, qint64 frameId, int selectAll,QObject* observer = Q_NULLPTR);
    // ??????????????
    void querySubAccountNameList(QObject* observer = Q_NULLPTR);
    // ??????????????
    void queryAllErrorCodes(QObject* observer = Q_NULLPTR);
    // ??????????????????????????????????????
    void queryAdvancedParamSetting(QObject* observer = Q_NULLPTR);
    // ??????????????????????????????
    bool queryTaskInfo(QList<qint64> taskIds, bool freshView = false, QObject* observer = Q_NULLPTR);
    // ????????
    void userFeedback(const QString& userName, const QString& detail,
                      const QString& contactWay, int type, QObject* observer = Q_NULLPTR);
    // ????????
    void updateTaskRemark(qint64 taskId, const QString& remark, QObject* observer = Q_NULLPTR);
    // ????????????????
    void updateTaskOverTime(qint64 taskId, int overtime, QObject* observer = Q_NULLPTR);
    // ????????????????
    void updateTaskOverTimeStop(qint64 taskId, int overtime, QObject* observer = Q_NULLPTR);
    // ??????????????
    void updateTaskLevel(qint64 taskId, const QString& level, QObject* observer = Q_NULLPTR);
    // ??????????????
    void queryJobProducerList(QObject* observer = Q_NULLPTR);
    // ????????????????
    void queryQRCode(QObject* observer = Q_NULLPTR);
    // ????????????
    void queryBindWx(QObject* observer = Q_NULLPTR);
    // ????????????
    void bindWxToUser(qint64 id, qint32 bindtype, QObject* observer = Q_NULLPTR);
    // ????????????
    void queryPlatformStatus(int zone, QObject* observer = Q_NULLPTR);
    // ????????????
    void queryNodeDetails(qint64 frameId, QObject* observer = Q_NULLPTR);
    // ????????
    void startDownload(qint64 taskId, QObject* observer = Q_NULLPTR);
    // ????????
    void downloadComplete(qint64 taskId, const QString& downloadRequestId, QObject* observer = Q_NULLPTR);
    // ????????????
    void queryTaskDownloadCount(QList<qint64> taskIds, QObject* observer = Q_NULLPTR);
    // ????????????
    void getTaskDownloadInfo(qint64 taskId, QObject* observer = Q_NULLPTR);
    // ????????????
    void uploadReport(qint64 taskId, const QString& filePath, QObject* observer = Q_NULLPTR);
    void uploadReport(qint64 taskId, const QJsonObject& jsonObject, QObject* observer = Q_NULLPTR);
    // ??????????????
    void updateTaskLimit(qint64 taskId, const QString& count, QObject* observer = Q_NULLPTR);
    // ??????????
    void queryFrameRecommitInfo(qint64 frameId, int pageNum = 1, qint64 pageSize = 65535, QObject* observer = Q_NULLPTR);
    // ????????output??????
    void getTaskOutputSize(qint64 taskId, int fromPage, QObject* observer = Q_NULLPTR);
    void getTasksOutputSize(const QList<qint64>& taskIds, int fromPage, QObject* observer = Q_NULLPTR);
    // ??????
    void paymentVoucher(qint64 taskId, QObject* observer = Q_NULLPTR);
    void queryVoucher(QObject* observer = Q_NULLPTR);
    // ??????????
    void queryPicturePrice(QObject* observer = Q_NULLPTR);
    // ????????????
    void queryAtReportInfo(qint64 userId, qint64 taskId, QObject* observer = Q_NULLPTR);
    // ????????
    void submitCloneTask(qint64 taskId, const QString& projectName, const RebuildOptionParams& params, QObject* observer = Q_NULLPTR);
    // ????????????
    void queryShareResultLink(qint64 taskId, int days = 7, QObject* observer = Q_NULLPTR);
	//????????????????
	void queryAtCloudPointInfo(qint64 userId, qint64 taskId, QObject* observer = Q_NULLPTR);
    // ????????????????
    void uploadTaskSurveyFile(qint64 taskId, const QString& filePath, QObject* observer = Q_NULLPTR);
	// ????image_pos.json????MD5
	void checkImagePosJsonMD5(qint64 userId, qint64 taskId, QString md5,QObject* observer = Q_NULLPTR);

private:
    void post(HttpCmdRequest* req, QObject* observer);
    void get(HttpCmdRequest* req, QObject* observer);
    void upload(HttpCmdRequest* req, QObject* observer);
    bool isReSubmitTask(qint64 taskId);

private slots:
    void exceptionResponse();

private:
    QString m_host;
    HttpRequestHeader m_header;
    HttpCmdResponse m_httpResponse;
    QString m_searchContent;
    QString m_searchType;
    int m_curTimeFilter;
public:
    FilterContent filterContent;
};

#endif // HTTPCMDMANAGER_H
