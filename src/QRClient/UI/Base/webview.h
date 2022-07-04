/***********************************************************************
* Module:  WebView.h
* Author:  hqr
* Modified: 2021/05/18 17:41:53
* Purpose: Declaration of the class
***********************************************************************/
#ifndef QWEBVIEW_H
#define QWEBVIEW_H

#include "Window.h"

class QWebEngineView;

#define TIMEOUTSEC (1000 * 30)  //30s³¬Ê±

class WebView : public QWidget
{
    Q_OBJECT

public:
    WebView(QWidget *parent = 0);
    ~WebView();

    void loadUrl(const QUrl& url);
    void reloadUrl();

private slots:
    void onLoadFinished(bool ok);
    void onLoadTimeout();
    void onRefresh(bool checked);
    void onWebGLReport(bool checked);
    void onOpen(bool checked);
    void loadReportUrl(const QUrl& url);

private:
    QWebEngineView *m_webView;
    QWebEngineView *m_webViewLoading;

    QUrl m_detailUrl;
    QUrl m_loadingUrl;
    QUrl m_loadingUrl404;
    bool m_loadSuccess;
};

#endif // QWEBVIEW_H
