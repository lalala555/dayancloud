#include "stdafx.h"
#include "CommonSetting.h"
//#include "Project/ProjectManager.h"
#include "Common/LangTranslator.h"
#include "HttpCommand/HttpCmdManager.h"

CommonSetting::CommonSetting(IResponseSink * sink, QWidget *parent)
    : QWidget(parent)
    , m_pSink(sink)
{
    ui.setupUi(this);
    this->initUI();
}

CommonSetting::~CommonSetting()
{

}

void CommonSetting::initData()
{
}

void CommonSetting::initUI()
{
    ui.checkBox_autologin->setChecked(LocalSetting::getInstance()->getAutoLogin());
    connect(ui.checkBox_autologin, SIGNAL(stateChanged(int)), this, SLOT(slotAutoLogin(int)));
    ui.checkBox_autoLaunch->setChecked(LocalSetting::getInstance()->getAutoStart());
    connect(ui.checkBox_autoLaunch, SIGNAL(stateChanged(int)), this, SLOT(slotAutoLunch(int)));
    connect(ui.checkBox_autologin, SIGNAL(stateChanged(int)), this, SIGNAL(noticeWidgetOpt(int)));
    connect(ui.checkBox_autoLaunch, SIGNAL(stateChanged(int)), this, SIGNAL(noticeWidgetOpt(int)));
    connect(ui.checkBox_autoDownload, SIGNAL(stateChanged(int)), this, SIGNAL(noticeWidgetOpt(int)));
    connect(ui.checkBox_cleanCache, SIGNAL(stateChanged(int)), this, SIGNAL(noticeWidgetOpt(int)));
    connect(ui.checkBox_enable_exact, SIGNAL(stateChanged(int)), this, SIGNAL(noticeWidgetOpt(int)));
    connect(ui.checkBox_update, SIGNAL(stateChanged(int)), this, SIGNAL(noticeWidgetOpt(int)));

#ifdef FOXRENDERFARM
	ui.label_3->setFixedWidth(130);
	ui.label_4->setFixedWidth(130);
	ui.label_9->setFixedWidth(130);
#endif

    // 下载设置
    bool isMkDir = LocalSetting::getInstance()->isDownloadMkSenceNameDir();
    ui.checkBox_newDir->setChecked(isMkDir);
    connect(ui.checkBox_newDir, SIGNAL(stateChanged(int)), this, SLOT(slotChangeDownloadSet(int)));

    // 自动下载
    bool isAutoDownload = CConfigDb::Inst()->getAutoDownload();
    ui.checkBox_autoDownload->setChecked(false); // 默认false
    CConfigDb::Inst()->setAutoDownload(false);
    connect(ui.checkBox_autoDownload, SIGNAL(stateChanged(int)), this, SLOT(slotAutoDownload(int)));

    // 清理缓存
    bool isCleanCache = LocalSetting::getInstance()->getCleanCache();
    ui.checkBox_cleanCache->setChecked(isCleanCache);
    connect(ui.checkBox_cleanCache, SIGNAL(stateChanged(int)), this, SLOT(slotCleanCache(int)));

    // 上传进度精准度
    bool exact = LocalSetting::getInstance()->getEnableExactProgress() > 0 ? true : false;
    ui.checkBox_enable_exact->setChecked(exact);
    connect(ui.checkBox_enable_exact, SIGNAL(stateChanged(int)), this, SLOT(slotEnableExactProgress(int)));

    // 是否自动更新
    bool update = LocalSetting::getInstance()->getEnableAutoUpdate() > 0 ? true : false;
    ui.checkBox_update->setChecked(update);
    connect(ui.checkBox_update, SIGNAL(stateChanged(int)), this, SLOT(slotAutoUpdate(int)));

    // 语言设置
    QButtonGroup* group = new QButtonGroup(this);
    group->addButton(ui.radioButton_cn);
    ui.radioButton_cn->setChecked(true);

    // 用户自定义坐标
    bool userCoord = LocalSetting::getInstance()->getEnableUserCoord() > 0 ? true : false;
    ui.checkBox_user_coord->setChecked(userCoord);
    connect(ui.checkBox_user_coord, SIGNAL(stateChanged(int)), this, SLOT(slotEnableUserCoord(int)));

    // 隐藏控件
    ui.widget_auto_download->hide();
    ui.widget_download->hide();
    ui.widget_clean_cache->hide();
    ui.widget_exact_progress->hide();
    ui.widget_user_coord_sys->hide();
	ui.widget_lan->hide();

#if USE_SCROLL_PAGE
#else
    ui.groupBox->setTitle("");
#endif
}

void CommonSetting::slotAutoLogin(int state)
{
    bool bCheck = (state == Qt::Checked ? true : false);
    if(bCheck)
        LocalSetting::getInstance()->setRememberPassword(true);
    LocalSetting::getInstance()->setAutoLogin(bCheck);
}

void CommonSetting::slotAutoLunch(int state)
{
    bool bCheck = (state == Qt::Checked ? true : false);

    // 当前用户使用自启动
    QSettings  reg("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",QSettings::NativeFormat);
    if (bCheck) {
        QString strAppPath=QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
        reg.setValue("wirtepad",strAppPath);
    } else {
        reg.setValue("wirtepad","");
    }
    LocalSetting::getInstance()->setAutoStart(bCheck);
}

void CommonSetting::slotChangeDownloadSet(int state)
{
    bool bCheck = (state == Qt::Checked ? true : false);
    LocalSetting::getInstance()->setDownloadMkSenceNameDir(bCheck);
}

void CommonSetting::slotAutoDownload(int state)
{
    bool bCheck = (state == Qt::Checked ? true : false);
    CConfigDb::Inst()->setAutoDownload(bCheck);
}

void CommonSetting::slotCleanCache(int state)
{
    bool bCheck = (state == Qt::Checked ? true : false);
    LocalSetting::getInstance()->setCleanCache(bCheck);
}

void CommonSetting::slotEnableExactProgress(int state)
{
    bool bCheck = (state == Qt::Checked ? true : false);
    LocalSetting::getInstance()->setEnableExactProgress(state);
}

void CommonSetting::slotAutoUpdate(int state)
{
    bool bCheck = (state == Qt::Checked ? true : false);
    LocalSetting::getInstance()->setEnableAutoUpdate(bCheck);
    CConfigDb::Inst()->setAutoUpdate(bCheck);
}

void CommonSetting::setSelectStyle(bool isSelect)
{
    if (isSelect) {
        ui.groupBox->setStyleSheet("QGroupBox{border-width:1px; border-style:solid; border-color:#675FFE;}"
            "QGroupBox:title{color:#675FFE;}");
    } else {
        ui.groupBox->setStyleSheet("QGroupBox{border-width:1px;border-style:solid;border-color:#333333;}"
            "QGroupBox:title{color:#675FFE;}");
    }
}

void CommonSetting::slotEnableUserCoord(int state)
{
    bool bCheck = (state == Qt::Checked ? true : false);
    LocalSetting::getInstance()->setEnableUserCoord(bCheck);
}