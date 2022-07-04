#include "stdafx.h"
#include "AppInstance.h"
#include "Resource/resource.h"
#include "UI/MainWindow.h"
#include "Kernel/TaskCenterManager.h"
#include "UI/Base/Widgets/SplashScreen.h"
#include "Common/SystemUtil.h"
#include "HttpCommand/HttpCmdManager.h"
#include "Config/UserProfile/TransConfig.h"
#include "UI/LoginWindow.h"
// 记录加载进度
int g_splashProgress = 0;

CAppInstance::CAppInstance(void)
    : QSingletonApp(GLOBAL_APPINSTANCE_NAME)
    , m_logout(false)
    , m_relaunch(false)
{
}

CAppInstance::~CAppInstance(void)
{
}

CAppInstance* CAppInstance::GetApp()
{
    static CAppInstance theApp;
    return &theApp;
}

bool CAppInstance::InitClient()
{
    bool ret = false;

    ret = this->InitLocalSetting();
    if(ret == false) {
        LOGFMTE("[CAppInstance] WSConfig was corrupted!");
        return false;
    }

    ret = AutoLogin();

    return ret;
}

bool CAppInstance::ProcessCmdLine()
{
    QCommandLineParser parser;
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

    QCommandLineOption updateOption("update");
    parser.addOption(updateOption);
    QCommandLineOption reloadOption("reload");
    parser.addOption(reloadOption);
    QCommandLineOption relaunchOption("relaunch");
    parser.addOption(relaunchOption);
    QCommandLineOption hideOption("hide");
    parser.addOption(hideOption);

    parser.parse(qApp->arguments());

    m_isUpdate   = parser.isSet(updateOption);
    m_isReload   = parser.isSet(reloadOption);
    m_isRelaunch = parser.isSet(relaunchOption);
    m_isHidden   = parser.isSet(hideOption);

    return true;
}

bool CAppInstance::Startup()
{
    bool bResult = false;

    this->ProcessCmdLine();

    LOGFMTI("========================STARTUP=============================");
    LOGFMTI("=Verson:%s %s Login=%s Computer=%s=",
            STRFILEVERSION,
            BUILD_TIME,
            qPrintable(System::getLoginDisplayName()),
            qPrintable(System::getComputerName()));

    bResult = this->InitClient();

    return bResult;
}

bool CAppInstance::Cleanup()
{
    this->exit();

    LOGFMTI("========================SHUTDOWN=============================");

    return true;
}

// 自动登录
bool CAppInstance::AutoLogin()
{
    QScopedPointer<LoginWindow> loginWindow(new LoginWindow());
    int codeRet = loginWindow->exec();
    if(codeRet == enDialogLogin) {
        

#ifdef FOXRENDERFARM
		QString pixPath = QApplication::applicationDirPath() + "/resource/splash_avica.png";
#else
		QString pixPath = QApplication::applicationDirPath() + "/resource/splash.png";
#endif // FOXRENDERFARM

        QPixmap pixmap(pixPath);
        m_splashWnd.reset(new SplashScreen(pixmap));
        m_splashWnd->setProgressBarPos(190, 230, 320, 8);
        m_splashWnd->setProgressBarRange(1, 5);
        m_splashWnd->setEnabled(false);
        m_splashWnd->show();
        qApp->processEvents();

        HttpCmdManager::getInstance()->userInfo(this);

        return true;
    }

    return false;
}

bool CAppInstance::InitLocalSetting()
{
    LOGFMTI("[CAppInstance] init webservice in...");

    bool ret = true;

    ret = WSConfig::getInstance()->init();
    if(!ret) return false;

    LOGFMTI("[CAppInstance] init webservice out...");

    return ret;
}

void CAppInstance::onRecvResponse(QSharedPointer<ResponseHead> response)
{
    int type = response->cmdType;
    if(type == RESTAPI_USER_INFO) {
        m_splashWnd->setProgressValue(++g_splashProgress);
        qApp->processEvents();

        if(response->code == HTTP_ERROR_SUCCESS) {
            QSharedPointer<UserInfoResponse> userInfo = qSharedPointerCast<UserInfoResponse>(response);
            MyConfig.userSet.id          = userInfo->userId;
            MyConfig.userSet.userName    = userInfo->userName;
            MyConfig.userSet.accountType = userInfo->accountType;
            MyConfig.accountSet.userId   = userInfo->userId;
            MyConfig.accountSet.authStatus = userInfo->authStatus;
            MyConfig.accountSet.realnameStatus = userInfo->realnameStatus;
            MyConfig.accountSet.delayAuthTime = userInfo->delayAuthTime;
        }

        CConfigDb::Inst()->start();
        HttpCmdManager::getInstance()->transformConfig(this);

    } else if (type == RESTAPI_USER_TRANS_CONFIG) {
        m_splashWnd->setProgressValue(++g_splashProgress);
        qApp->processEvents();

        if (response->code != HTTP_ERROR_SUCCESS) {
            QString msg = QString("%1[code:%2]").arg(QObject::tr("获取传输配置信息错误")).arg(response->code);
            FatalExit(msg);
            return;
        }
        QSharedPointer<TransConfigResponse> configInfo = qSharedPointerCast<TransConfigResponse>(response);
        auto keys = configInfo->trans_config.keys();
        if (TransConfig::getInstance()->setTransConfig(QJsonDocument(configInfo->trans_config))) {
            m_splashWnd->setProgressValue(++g_splashProgress);
            qApp->processEvents();

            LocalSetting::getInstance()->resetTransEngineConfig();

            LOGFMTI("[RESTAPI_USER_TRANS_CONFIG] Start to load init task center...");
            if (CTaskCenterManager::Inst()->init() && CTaskCenterManager::Inst()->beepOn()) {
                m_splashWnd->setProgressValue(++g_splashProgress);
                qApp->processEvents();

                LOGFMTI("[onRecvResponse] Start to create mainwindow object");
                QPointer<CMainWindow> mainWindow(new CMainWindow());            
                LOGFMTI("[onRecvResponse] End to create mainwindow object");

                // 等待动图播放完毕
                if (!m_splashWnd->isGifFinished()) {
                    m_splashWnd->waitForFinished();
                    m_splashWnd->setProgressValue(++g_splashProgress);
                    qApp->processEvents();
                }   
                mainWindow->init();
                m_splashWnd->setProgressValue(++g_splashProgress);
                qApp->processEvents();
                 
                m_splashWnd->finish(mainWindow);
                if(this->IsHidden())
                    mainWindow->hide();
                else
                    mainWindow->show();
                this->setWidget(mainWindow);
            } else {
                FatalExit(QObject::tr("初始化失败，此问题一般是由于配置文件损坏或丢失导致"));
            }
        } else {
            FatalExit(QObject::tr("初始化传输配置数据失败!"));
        }
    }
}

void CAppInstance::FatalExit(const QString& message)
{
    m_splashWnd->close();
    g_splashProgress = 0;
    Util::ShowMessageBoxError(message);
    QApplication::exit(-1);
    this->Logout();
}
