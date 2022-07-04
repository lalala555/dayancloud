/***********************************************************************
* Module:  TaskWriter.h
* Author:  hqr
* Created: 2021/01/13 14:24:26
* Modifier: hqr
* Modified: 2021/01/13 14:24:26
* Purpose: Declaration of the class
***********************************************************************/
#pragma once
#ifndef TASKWRITER_H
#define TASKWRITER_H

#include "HttpCommand/HttpCommon.h"
#include "kernel/render_struct.h"

class TaskCCWriter
{
public:
    TaskCCWriter(QSharedPointer<TaskInfo> task);
    ~TaskCCWriter(void) {};

    bool dump_taskjson(QSharedPointer<RenderParamConfig> param, int version = 1);
    bool dump_uploadjson(QSharedPointer<RenderParamConfig> param);
    bool dump_tipsjson(QSharedPointer<RenderParamConfig> param);
    bool dump_assetjson(QSharedPointer<RenderParamConfig> param);
    bool dump_argument_json(QSharedPointer<RenderParamConfig> param, QString& argumentFile, int version = 0);
    bool dump_groupinfo_json(QSharedPointer<RenderParamConfig> param);

    bool update_cc_task(const TaskInfoCC & taskInfo);
    bool update_task_json_cc(QJsonObject& task_json, const TaskInfoCC& taskInfo);

    void init_project_path();
    void backup_and_remove_josn_file();

    QString cg_file; // 必填
    qint64 job_id; // 必填
    QString job_id_alias; // 必填
    PluginConfigInfo plugin;
    ProjectConfigInfo project;
    QString ram;
    QString test_frames;
    int action_after_test;
    QString mode; // 分析模式：normal/fast/debug

    QString json_file;
    QString client_project_dir;

    QString frameTimeoutRemind;
    QString frameTimeoutStop;

    QString muiltFrames;
    QString submitLayer;
    QString gpuCount;
    QString submitNoded;

    QString argument_file;
    QString client_tmp_path;
private:
    QJsonObject create_object(int version);
    QSharedPointer<TaskInfo> m_task;
};

#endif // TASKWRITER_H
