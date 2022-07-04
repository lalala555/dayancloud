#ifndef NETSETTING_H
#define NETSETTING_H

#include <QWidget>
#include "ui_NetSetting.h"

class NetSetting : public QWidget
{
    Q_OBJECT
public:
    NetSetting(IResponseSink* sink, QWidget *parent = 0);
    ~NetSetting();

    void initDialog();
    void finishPingIP();
    void updateNetworkData();
    void setSelectStyle(bool isSelect);
    void installWidgetOpt();

private:
    void initUI();

    //加载引擎列表
    void loadEngineList();
    void addEngineInfoTableWidget(const QString& engine);
    void updateTransferRoute(const QString & engine);
    void updateLimitNetwork();

    void setShowEditUpload(bool bshow);
    void setShowEditDownload(bool bshow);

    void saveNetworkName(const QString& engine, const QString& serverName);
    void loadOtherTransSetting();

private slots:
    void slotSaveNetworkSetting();
    void slotTransferModeChange(const QString & text);
    void slotDBClickedNetworkWidget(int row, int column);
    void slotClickedLimitUpload();
    void slotClickedLimitDownload();
    void slotEditFinishLimitUpload();
    void slotEditFinishLimitDownload();
    void slotUpdateWidgetRect(const QRect&);
    void slotChangedDownloadCount(int count);
    void slotChangedUploadCount(int count);
signals:
    void noticeWidgetOpt(int state = 0);

private:
    Ui::NetSetting ui;
    IResponseSink*  m_pSink;
    QString m_preTransEngine;
	QVector<QPair<bool,QString>> m_needCheckLine;//is_upload , line
	QString  m_currentCheckEngine;
	QPair<bool,QString>  m_currentCheckLine;
	QTime  m_transCheckTime;
	QTimer* m_transCheckTimer;
	size_t m_upload_tranChecksize;
	QString m_transGoodLine;
	QPair<QString, size_t> m_pairEngineUploadSpeed; // engine - <upload >
	QPair<QString, size_t> m_pairEngineDownloadSpeed; // engine - <download >
};

#endif // NETSETTING_H
