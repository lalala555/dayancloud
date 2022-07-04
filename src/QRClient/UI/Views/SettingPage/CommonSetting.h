#ifndef COMMONSETTING_H
#define COMMONSETTING_H

#include <QWidget>
#include "ui_CommonSetting.h"

class CommonSetting : public QWidget
{
    Q_OBJECT

public:
    CommonSetting(IResponseSink * sink, QWidget *parent = 0);
    ~CommonSetting();
    void initData();
    void setSelectStyle(bool isSelect);

private:
    void initUI();

signals:
    void noticeWidgetOpt(int state = 0);

private slots:
    void slotAutoLogin(int state);
    void slotAutoLunch(int state);
    void slotChangeDownloadSet(int state);
    void slotAutoDownload(int state);
    void slotCleanCache(int state);
    void slotEnableExactProgress(int state);
    void slotAutoUpdate(int state);
    void slotEnableUserCoord(int state);
private:
    Ui::CommonSetting ui;
    IResponseSink*   m_pSink;
};

#endif // COMMONSETTING_H
