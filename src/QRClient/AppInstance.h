#pragma once
#include "HttpCommand/HttpCommon.h"
#include "Common/QHttpManager.h"
#include "UI/Base/QSingletonApp.h"

class SplashScreen;

class CAppInstance : public QSingletonApp
{
    Q_OBJECT
private:
    CAppInstance();
    ~CAppInstance();
public:
    static CAppInstance* GetApp();
public:
    bool ProcessCmdLine();
    bool Startup();
    bool Cleanup();
    bool IsLogout()
    {
        return m_logout;
    }
    bool IsRelaunch()
    {
        return m_relaunch;
    }
    bool IsUpdate()
    {
        return m_update;
    }
    bool IsHidden()
    {
        return m_isHidden;
    }
    void Logout()
    {
        m_logout = true;
    }
    void Relaunch()
    {
        m_relaunch = true;
    }
    void Update()
    {
        m_update = true;
    }
private:
    bool InitClient();
    bool InitLocalSetting();
    bool AutoLogin();
    void FatalExit(const QString& message);

private slots:
    void onRecvResponse(QSharedPointer<ResponseHead> response);

public:
    bool m_logout;
    bool m_relaunch;
    bool m_update;

    bool m_isUpdate;
    bool m_isReload;
    bool m_isRelaunch;
    bool m_isHidden = false;

    QScopedPointer<SplashScreen> m_splashWnd;
    QHttpManager m_httpManager;
};
