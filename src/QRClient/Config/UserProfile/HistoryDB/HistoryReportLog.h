#pragma once
#ifndef HISTORYREPORTLOG_H_
#define HISTORYREPORTLOG_H_
#include "HistoryDbBase.h"

class HistoryReportLog : public CHistoryDbBase
{
public:
    HistoryReportLog() {};
    ~HistoryReportLog() {};
public:
    LogReportHistoryInfo getReportLogInfo(const std::string& path);
    std::map<std::string, LogReportHistoryInfo> getAllReportLogInfo() { return m_dhi; }

    bool addReportLog(const LogReportHistoryInfo& info);
    bool removeReportLog(const std::string& logpath);
    bool updateReportLog(const LogReportHistoryInfo& info);

    bool onAddReportLog(const SQLiteConnection& conn, const SyncData* data);
    bool onRemoveReportLog(const SQLiteConnection& conn, const SyncData* data);

    bool postAsyncAddReportLog(const LogReportHistoryInfo& info);
    bool postAsyncDelReportLog(const std::string& logpath);
    bool postAsyncUpdateReportLog(const LogReportHistoryInfo& info);

    bool readReportLoglist(const SQLiteConnection& connection);

private:
    std::map<std::string, LogReportHistoryInfo> m_dhi;
};

#endif

