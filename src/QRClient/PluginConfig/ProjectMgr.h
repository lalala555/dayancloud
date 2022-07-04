/***********************************************************************
* Module:  ProjectMgr.h
* Author:  hqr
* Modified: 2018/10/09 15:48:44
* Purpose: Declaration of the class
***********************************************************************/
#pragma once
#ifndef PROJECTMGR_H
#define PROJECTMGR_H

#include "HttpCommand/HttpCommon.h"
Q_DECLARE_METATYPE(ProjectConfigInfo)
typedef QMap<QString, ProjectConfigInfo> ProjectList;
Q_DECLARE_METATYPE(ProjectList)

class ProjectMgr
{
    ProjectMgr(void);
public:
    ~ProjectMgr(void);
    static ProjectMgr* getInstance()
    {
        static ProjectMgr instance;
        return &instance;
    }

    bool init();
    void clear();

    void addProject(int projectId, const QString& projectName);
    void removeProject(const QString& projectName);
    void setDefaultProject(const QString& projectName = DEFAULT_PROJECT_NAME);
    QString getDefaultProject();
    int getProjectId(const QString& projectName);
    QString getProjectOutput(int cgid); // 本地输出路径
    QString getProjectDir(int cgid); // 本地项目目录
    QString getProjectOutput(const QString& projectName);// 本地输出路径

    void addProject(ProjectConfigInfo* info);

    ProjectList projectList;
    ProjectList allAccountProjNames;  // 所有主子账号的项目名，只用于显示，不能用于提交使用
    bool isInitialized; // 是否已初始化数据
};

#endif // PROJECTMGR_H
