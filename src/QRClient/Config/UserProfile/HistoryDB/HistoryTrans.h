#pragma once
#ifndef HISTORYTRANS_H
#define HISTORYTRANS_H
#include "HistoryDbBase.h"

class HistoryTrans : public CHistoryDbBase
{
public:
    HistoryTrans(void);
    ~HistoryTrans(void);

    // ��������
public:
    bool addTask(int taskid, const TransHistoryInfo& taskinfo);
    bool removeTask(int taskid);
    bool updateTask(int taskid, const TransHistoryInfo& taskinfo);
    bool updateTaskTransState(int taskid, int state);   // ���������״̬������״̬��
    int  getUnfinishedCount(); //��ȡδ��ɵ�����
    bool addOldTask(const SQLiteConnection& connection, const TransHistoryInfo& taskinfo);
    bool readOldData(const SQLiteConnection& connection);
    bool saveOldData(const SQLiteConnection& connection);
    bool readTasklist(const SQLiteConnection& connection);

    void getAllTaskList(std::map<int, TransHistoryInfo>& alltask);
    bool findTask(int taskid);
    TransHistoryInfo findTaskInfo(int taskid);

    // �ϴ�����
public:
    int  getUploadUnfinishedCount(std::map<int, TransHistoryInfo>& unuploadtasks); //��ȡ�ϴ�δ��ɵ�����
    bool updateTaskScenePath(int taskid, const std::string& path);
    void getUploadTaskList(std::map<int, TransHistoryInfo>& uploadtasks);

    // ���ز���
public:
    int  getDownloadUnfinishedCount(std::map<int, TransHistoryInfo>& undownloadtasks); //��ȡ����δ��ɵ�����
    bool updateTaskDownloadPath(int taskid, const std::string& downpath);
    void getDownloadTaskList(std::map<int, TransHistoryInfo>& downloadtasks);
    int  getDownloadTaskStatus(int taskId);
    void getDownloadUnstoppedTaskList(std::map<int, TransHistoryInfo>& list);
    // ���±�ṹ
    bool updateTableStruct(const SQLiteConnection& conn);
    bool checkColumnExist(const SQLiteConnection& conn, const std::string& colName);
    bool addNewColumnInt(const SQLiteConnection& conn, const std::string& colName, int defaultValue);
    bool addNewColumnString(const SQLiteConnection& conn, const std::string& colName);
public:
    //�������ͬ�������ݿ�
    bool postAsyncAddTask(int taskId,const TransHistoryInfo& info);
    //�޸�����ͬ�������ݿ�
    bool postAsyncUpdateTask(int taskId, const TransHistoryInfo& info);
    //ɾ������ͬ�������ݿ�
    bool postAsyncDelTask(int taskId);
    //�޸Ĵ���״̬ ͬ�������ݿ�
    bool postAsyncUpdateTaskTransState(int taskId, int state);
    //�޸�����ı��س���·��
    bool postAsyncUpdateLocalScenePath(int taskId, const std::string& localpath);
    //�޸�����ĳ�������·��
    bool postAsyncUpdateDownloadPath(int taskId, const std::string& downloadPath);
    //�޸����Ϊ·��
    bool postAsyncUpdateLocalSavePath(int taskId, const std::string& localSavePath);
public:
    bool onAddTask(const SQLiteConnection& conn, int taskId, const SyncData* data);
    bool onRemoveTask(const SQLiteConnection& conn, int taskId, const SyncData* data);
    bool onUpdateTask(const SQLiteConnection& conn, int taskId, const SyncData* data);
    bool onUpdateTaskTransState(const SQLiteConnection& conn, int taskId, const SyncData* data);
    bool onUpdateTaskScenePath(const SQLiteConnection& conn, int taskId, const SyncData* data);
    bool onUpdateTaskDownloadPath(const SQLiteConnection& conn, int taskId, const SyncData* data);
    bool onUpdateTaskLoaclSavePath(const SQLiteConnection& conn, int taskId, const SyncData* data);

private:
    CLock m_lock;
    std::map<int, TransHistoryInfo> m_translist;
};



#endif // _HISTORYTRANS_H