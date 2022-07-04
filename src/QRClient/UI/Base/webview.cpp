#include "stdafx.h"
#include "webview.h"
#include <QWebEngineSettings>
#include <QWebEngineView>

WebView::WebView(QWidget *parent)
    : QWidget(parent)
    , m_loadSuccess(false)
    , m_loadingUrl(QUrl("qrc:/html/html/loading.html"))
    , m_loadingUrl404(QUrl("qrc:/html/html/404.html"))
{

#ifdef FOXRENDERFARM
	m_loadingUrl404 = "qrc:/html/html/404_en.html";
#endif

    qputenv("QTWEBENGINE_REMOTE_DEBUGGING", "9233"); // 启用调试功能
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &WebView::customContextMenuRequested, [this](const QPoint & pos) {
        QMenu menu;
        QAction *refreshAction = menu.addAction(tr("刷新"));
        QObject::connect(refreshAction, SIGNAL(triggered(bool)), this, SLOT(onRefresh(bool)));

        
        QAction *openAction = menu.addAction(tr("在浏览器中打开"));
        QObject::connect(openAction, SIGNAL(triggered(bool)), this, SLOT(onOpen(bool)));

        if(RBHelper::isDeveloper()) {
            QAction *reportAction = menu.addAction(tr("WebGL兼容检测"));
            QObject::connect(reportAction, SIGNAL(triggered(bool)), this, SLOT(onWebGLReport(bool)));
        }
        menu.exec(mapToGlobal(pos));
    });

    m_webViewLoading = new QWebEngineView(this);
    m_webViewLoading->load(m_loadingUrl);

    m_webView = new QWebEngineView(this);
    m_webView->hide();
    connect(m_webView, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));

    QWebEngineSettings *settings = m_webView->settings();
    settings->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
    settings->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, true);
    settings->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
    settings->setAttribute(QWebEngineSettings::WebGLEnabled, true);
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);

    auto webLayout = new QVBoxLayout();
    webLayout->addWidget(m_webViewLoading);
    webLayout->addWidget(m_webView);
    this->setLayout(webLayout);
}

WebView::~WebView()
{
}

void WebView::loadUrl(const QUrl& url)
{
    m_loadSuccess = false;

    m_detailUrl = url;

    m_webView->page()->runJavaScript("localStorage.clear()");
    m_webView->hide();
    m_webView->load(m_detailUrl);

    m_webViewLoading->show();
    m_webViewLoading->load(m_loadingUrl);

    QTimer::singleShot(TIMEOUTSEC, this, SLOT(onLoadTimeout()));
}

void WebView::loadReportUrl(const QUrl & url)
{
    m_loadSuccess = false;

    m_webView->hide();
    m_webView->load(url);

    m_webViewLoading->show();
    m_webViewLoading->load(m_loadingUrl);

    QTimer::singleShot(TIMEOUTSEC, this, SLOT(onLoadTimeout()));
}

void WebView::onLoadFinished(bool ok)
{
    m_loadSuccess = ok;

    if(m_loadSuccess) {
        m_webViewLoading->hide();
        m_webViewLoading->load(QUrl("about://blank"));

        m_webView->show();
    } else {
        m_webViewLoading->show();
        m_webViewLoading->load(m_loadingUrl404);
    }
}

void WebView::onLoadTimeout()
{
    if(m_loadSuccess)
        return;

    m_webView->stop();
    m_webView->loadFinished(false);
}

void WebView::onRefresh(bool checked)
{
    this->loadUrl(m_detailUrl);
}

void WebView::onWebGLReport(bool checked)
{
    this->loadReportUrl(QUrl("https://webglreport.com/"));
}

void WebView::onOpen(bool checked)
{
    QDesktopServices::openUrl(m_detailUrl);
}

void WebView::reloadUrl()
{
    m_webView->reload();
}