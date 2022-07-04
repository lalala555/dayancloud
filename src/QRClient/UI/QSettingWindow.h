#ifndef QSETTINGWINDOW_H
#define QSETTINGWINDOW_H

#include <QWidget>
#include <QMouseEvent>
#include <QScrollBar>
#include <QScrollArea>
#include <QPoint>
#include "ui_QSettingWindow.h"
#include "GUIUpdater.h"

class CommonSetting;
class NetSetting;
class ScriptInstall;

class QSettingWindow : public Dialog , public IResponseSink
{
    Q_OBJECT

public:
    QSettingWindow(QWidget *parent = 0);
    ~QSettingWindow();

    void initUI();
    void initData();
    void ShowWnd();
    void switchToProjectSetPage(int cgType = CG_ID_3DS_MAX);

private slots:
    void slotCurrentChanged(int index);
    void slotUpdateNetwork(int wParam, int lParam);
    void slotFinishPingIP(int wParam, int lParam);
    void onSlotValueChanged(int);
    void changeGroupboxStyle(int index);
    void onPartWidgetOpted();
protected:
    virtual void changeEvent(QEvent *event);
    virtual bool Response(int type, intptr_t wParam, intptr_t lParam);
    virtual void showEvent(QShowEvent *e);

    void setupEventListeners();
private:
    Ui::QSettingWindowClass ui;
    GUIUpdater     m_guiUpdater;
    CommonSetting* m_commonSetting;
    NetSetting*    m_netSetting;
    bool           m_signFlag;
};

#endif // QSETTINGWINDOW_H
