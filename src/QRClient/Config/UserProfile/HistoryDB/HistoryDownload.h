#pragma once
#include "HistoryDbBase.h"
class HistoryDownload : public CHistoryDbBase
{
public:
    enum FilterDate
    {
        downloaded_all = 0,
        downloaded_3_days_ago = 3,
        downloaded_7_days_ago = 7,
        downloaded_15_days_ago = 15,
        downloaded_one_month_ago = 30,
        downloaded_three_month_ago = 3 * 30,
        downloaded_six_month_ago = 6 * 30,
        downloaded_one_year_ago = 12 * 30,
    };
public:
    HistoryDownload();
    ~HistoryDownload();

    virtual bool addTask(int taskId, const DownloadHistoryInfo& info);
    virtual bool removeTask(int taskId);
    virtual bool updateTaskStatus(int taskId, int status, int checking, const std::string& completeTime, const std::string& downloadList);
    virtual bool updateTaskPriority(int taskId, int priority, int priorityTime, int prorityType);
    virtual bool updateTaskScene(int taskId, const std::string &strScene);
    virtual bool updateTaskParentInfo(int taskId, int parentId, const std::string &taskAlias);
    virtual bool updateTaskOutputSize(int taskId, const std::string &outputsize);

    virtual int getUnfinishCount(int filterDate);
    bool readTasklist(const SQLiteConnection& connection);

    void getUnstoppedTaskList(std::map<int, DownloadHistoryInfo>& list, int filterDate);
    void getUnfinishedTaskList(std::list<int>& list, int filterDate);
    bool updateDownloadTaskSavePath(int taskId, const std::string &path);
    void getTaskListIds(std::list<int>& list, int filterDate);

    bool checkColumnExist(const SQLiteConnection& conn, const std::string& colName);
    bool addNewStrColumn(const SQLiteConnection& conn, const std::string& colName, int strLen);
    bool updateTableStruct(const SQLiteConnection& conn);
    bool addNewIntColumn(const SQLiteConnection& conn, const std::string& colName);
public:
    //�������ͬ�������ݿ�
    bool postAsyncAddTask(int taskId, const DownloadHistoryInfo& info);
    //�޸�����״̬ͬ�������ݿ�
    bool postAsyncUpdateTaskStatus(int taskId, int status, int checking, const std::string& completeTime, const std::string& downloadList);
    //�޸��������ȼ�ͬ�������ݿ�
    bool postAsyncUpdateTaskPriority(int taskId, int priority, int priorityTime, int prorityType);
    //�޸����񳡾���ͬ�������ݿ�
    bool postAsyncUpdateTaskScene(int taskId, const std::string &strScene);
    //ɾ������ͬ�������ݿ�
    bool postAsyncDelTask(int taskId);
    // ���¸�����ID�ͱ���
    bool postAsyncUpdateParentInfo(int taskId, int parentId, const std::string &parentAlias);
    // ��������·��
    bool postAsyncUpdateTaskSavePath(int taskId, const std::string &path);
    // �������������С
    bool postAsyncTaskOutputSize(int taskId, const std::string &outputsize);
public:
    bool onAddTask(const SQLiteConnection& conn, int taskId, const SyncData* data);
    bool onRemoveTask(const SQLiteConnection& conn, int taskId, const SyncData* data);
    bool onUpdateTaskStatus(const SQLiteConnection& conn, int taskId, const SyncData* data);
    bool onUpdateTaskPriority(const SQLiteConnection& conn, int taskId, const SyncData* data);
    bool onUpdateTaskScene(const SQLiteConnection& conn, int taskId, const SyncData* data);
    bool onUpdateTaskParentInfo(const SQLiteConnection& conn, int taskId, const SyncData* data);
    bool onUpdateTaskSavePath(const SQLiteConnection& conn, int taskId, const SyncData* data);
    bool onUpdateTaskOutputSize(const SQLiteConnection& conn, int taskId, const SyncData* data);
public:
    void getTaskList(std::map<int, DownloadHistoryInfo>& list, int filterDate);
    bool findTask(int taskId);
    int getTaskStatus(int taskId);
    static bool isDateContains(const std::string& time, int filterDate);

private:
    std::map<int, DownloadHistoryInfo> m_dhi;
};

