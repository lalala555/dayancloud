#include "stdafx.h"
#include "ParameterCustom.h"
#include "ui_ParameterCustom.h"
#include "HttpCommand/HttpCmdManager.h"
#include "Common/FileUtil.h"
#include "NewJobs/JobStateMgr.h"

ParameterCustom::ParameterCustom(QWidget *parent)
    : Dialog(parent)
    , ui(new Ui::ParameterCustomClass)
    , m_oldTaskId(0)
{
    ui->setupUi(this);
    ui->label_logo->setPixmap(QPixmap(LOGO_ICON));

    m_busyIcon = new QProgressIndicator(ui->btnOk);
    m_busyIcon->setAttribute(Qt::WA_TransparentForMouseEvents);
}

ParameterCustom::~ParameterCustom()
{
    delete ui;
}

void ParameterCustom::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    } else
        Dialog::changeEvent(event);
}

void ParameterCustom::showEvent(QShowEvent *e)
{
    Dialog::showEvent(e);

    int sx = (ui->btnOk->width() - m_busyIcon->width()) / 2;
    int sy = (ui->btnOk->height() - m_busyIcon->height()) / 2;
    m_busyIcon->move(sx, sy);
}

void ParameterCustom::on_btnOk_clicked()
{
    QString jobIdText = ui->lineEdit->text();
    if(jobIdText.isEmpty())
        return;

    m_oldTaskId = jobIdText.toLongLong();

    HttpCmdManager::getInstance()->createJobIds(1, REND_CMD_TASK_ID, this);

    showLoading(true);
}

void ParameterCustom::onRecvResponse(QSharedPointer<ResponseHead> response)
{
    if (response == Q_NULLPTR)
        return;

    int type = response->cmdType;
    switch (type) {
    case RESTAPI_JOB_CREATE: {
        QSharedPointer<JobCreateResponse> jobs = qSharedPointerCast<JobCreateResponse>(response);
        if (jobs->createType == REND_CMD_TASK_ID) {
            if (jobs->code == HTTP_ERROR_SUCCESS) {
                if (jobs->taskids.count() > 0) {
                    qint64  jobId = jobs->taskids.at(0);
                    QString jobIdAlias = jobs->taskIdAlias.at(0);
                    if (!getNewTaskIdSuccess(jobId, jobIdAlias)) {
                        new ToastWidget(this, QObject::tr("作业创建失败, 请重试！"), QColor("#e25c59"));
                        showLoading(false);
                    }

                }
            } else if (jobs->code == HTTP_ERROR_USER_BALANCE_NOT_ENOUGH) {
                new ToastWidget(this, QObject::tr("余额不足, 请充值后再试！"), QColor("#e25c59"));
                showLoading(false);
            } else if (jobs->code == HTTP_ERROR_USER_ACCOUNT_NOT_ACTIVE) {
                new ToastWidget(this, QObject::tr("账户未激活，请联系客服！"), QColor("#e25c59"));
                showLoading(false);
            } else if (jobs->code == HTTP_ERROR_USER_INFO_NOT_EXIST) {
                new ToastWidget(this, QObject::tr("用户信息不存在，请联系客服！"), QColor("#e25c59"));
                showLoading(false);
            } else {
                new ToastWidget(this, QObject::tr("作业创建失败, 请重试！"), QColor("#e25c59"));
                showLoading(false);
            }
        }
    }
    break; 
    }
}

void ParameterCustom::showLoading(bool loading)
{
    if (loading) {
        m_busyIcon->show();
        m_busyIcon->startAnimation();
        ui->btnOk->setEnabled(false);
    } else {
        m_busyIcon->hide();
        m_busyIcon->stopAnimation();
        ui->btnOk->setEnabled(true);
    }
}

bool ParameterCustom::getNewTaskIdSuccess(qint64 taskId, const QString& taskIdAlias)
{
    // 1. 创建新的文件夹
    // 2. 将就任务的文件拷贝到新文件夹
    // 3. 修改文件
    // 4. 提交
    QString oldTask = QString("%1/%2").arg(RBHelper::getDefaultProjectDir()).arg(QString::number(m_oldTaskId));
    QString newTask = QString("%1/%2").arg(RBHelper::getDefaultProjectDir()).arg(QString::number(taskId));

    // 拷贝文件
    if (!File::copyRecursively(oldTask.toLower(), newTask.toLower())) {
        LOGFMTE("[ParameterCustom] %lld copy cloned task json failed!", m_oldTaskId);
        return false;
    }

    // 修改文件
    // 修改task.json文件
    QString task_json_path = QString("%1/task.json").arg(newTask);
    QJsonObject task_json;
    bool bret = RBHelper::loadJsonFile(task_json_path, task_json);
    if (!bret) {
        LOGFMTE("[ParameterCustom] %lld load cloned task json error!", taskId);
        return false;
    }

    // 系统信息
    QJsonObject task_info;
    if (task_json.contains("task_info") && task_json["task_info"].isObject()) {
        task_info = task_json["task_info"].toObject();
    }
    if (task_info.isEmpty()) {
        LOGFMTE("[TaskCenter] %lld task_info is empty!", taskId);
        return false;
    }
    task_info["job_id"] = QString::number(taskId);
    task_info["job_id_alias"] = taskIdAlias;
    // task_info["clone_parent_id"]      = QString::number(oldId);
    task_info["clone_original_id"] = QString::number(m_oldTaskId);
    task_info["clone_client_version"] = STRFILEVERSION;

    task_json["task_info"] = task_info;

    RBHelper::saveJsonFile(task_json_path, task_json);

    // 上传任务
    stSubTask* task = new stSubTask();
    task->type = CMD_SUBMIT_SCRIPT;
    task->jobId = taskId;
    CGlobalHelp::Response(UM_COMMAND_TYPE, REND_CMD_UPLOAD_TASK, (intptr_t)task);

    showLoading(false);
    this->close();

    int nShow = CConfigDb::Inst()->getShowTaskType();
    JobStateMgr::Inst()->SendRefreshTasklist(-1, nShow);

    return true;
}
