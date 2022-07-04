#pragma once

#include <QWidget>
#include "Dialog.h"
#include "ui_ResultsShareWindow.h"
#include "HttpCommand/HttpCommon.h"

class ResultsShareWindow : public Dialog
{
    Q_OBJECT

public:
    ResultsShareWindow(qint64 taskId, QWidget *parent = Q_NULLPTR);
    ~ResultsShareWindow();
    void initUi();

private slots:
    void on_btnCopyCode_clicked();
    void on_btnCopyQRCode_clicked();
    void onRecvResponse(QSharedPointer<ResponseHead> data);

private:
    QPixmap makeShareQRCode(QString shareUrl, QString code);
    void fillShareLinkContent(QString shareUrl, QString code);
    void showLoading(bool show);

private:
    Ui::ResultsShareWindow ui;
    qint64 m_taskId;
    QProgressIndicator* m_busyIcon;
};
