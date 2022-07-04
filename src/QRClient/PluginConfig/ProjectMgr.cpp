#include "stdafx.h"
#include "ProjectMgr.h"

ProjectMgr::ProjectMgr(void)
    : isInitialized(false)
{
}

ProjectMgr::~ProjectMgr(void)
{
}

bool ProjectMgr::init()
{
    return true;
}

void ProjectMgr::clear()
{
    projectList.clear();
}

void ProjectMgr::addProject(int projectId, const QString& projectName)
{
    ProjectConfigInfo config;
    config.projectId = projectId;
    config.projectName = projectName;
    projectList[projectName] = config;
}

void ProjectMgr::removeProject(const QString& projectName)
{
    projectList.remove(projectName);
    allAccountProjNames.remove(projectName);
}

int ProjectMgr::getProjectId(const QString& projectName)
{
    foreach(auto project, projectList) {
        if(project.projectName == projectName)
            return project.projectId;
    }
    return 0;
}

void ProjectMgr::setDefaultProject(const QString& projectName)
{
    int projectId = getProjectId(projectName);
    CConfigDb::Inst()->setProject(projectId, projectName.toStdString());
}

QString ProjectMgr::getDefaultProject()
{
    QString projectName = QString::fromStdString(CConfigDb::Inst()->getProjectName());
    return projectName;
}

QString ProjectMgr::getProjectOutput(int cgid)
{
    CGConfigItem cg_config = CConfigDb::Inst()->getCGConfig(cgid);
    return QString::fromStdString(cg_config.project_output);
}

QString ProjectMgr::getProjectDir(int cgid)
{
    CGConfigItem cg_config = CConfigDb::Inst()->getCGConfig(cgid);
    return QString::fromStdString(cg_config.project_dir);
}

QString ProjectMgr::getProjectOutput(const QString& projectName)
{
    qint64 userId = MyConfig.accountSet.userId;
    QString path = CProfile::Inst()->get_proj_download_path(userId, projectName);

    if(!CConfigDb::Inst()->isEnableProjectDownloadPath() || path.isEmpty()){
        path = LocalSetting::getInstance()->getLocalSavePath();
    }
    return path;
}

void ProjectMgr::addProject(ProjectConfigInfo* info)
{
    if (info == NULL) return;
    allAccountProjNames[info->projectName] = *info;
}