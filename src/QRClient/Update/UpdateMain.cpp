#include "stdafx.h"
#include "UpdateMain.h"
#include "Update/UpdateManager.h"

UpdateMain::UpdateMain(QWidget *parent)
    : Dialog(parent)
    , m_isAlive(false)
{
    ui.setupUi(this);
    ui.label_logo->setPixmap(QPixmap(LOGO_ICON));

    ui.progressBar->setRange(0, 100);
    ui.progressBar->setTextVisible(false);

    ui.btnOk->setMinimumWidth(120);
    ui.btnCancel->setMinimumWidth(120);
    ui.btnRetry->setMinimumWidth(120);
    connect(ui.btnRetry, &QPushButton::clicked, this, &UpdateMain::startDownload);

    this->startDownload();
}

UpdateMain::~UpdateMain()
{

}

void UpdateMain::startDownload()
{
    m_isAlive = true;
    ui.progressBar->setValue(0);
    m_downloadTimer.restart();
    ui.btnRetry->hide();
    ui.labelProgress->setText(tr("开始下载..."));

    UpdateManagerInstance.performUpdate(UPDATE_MANUAL_TRIGGER, this);
    QObject::connect(&UpdateManagerInstance, SIGNAL(resultReady(int)), this, SLOT(onUpdateReturn(int)));
}

void UpdateMain::onCmdArrived(const QString& data)
{
    if(!m_isAlive) {
        return;
    }

    QStringList s = data.split("|");
    if (s.size() != 3) {
        Q_ASSERT(false);
        return;
    }
    unsigned long curr_size = s.at(0).toLong();
    unsigned long total_size = s.at(1).toLong();

    float per = (float)(curr_size * 100.0) / total_size;

    // calculate the download speed
    double speed = curr_size * 1000.0 / m_downloadTimer.elapsed();
    QString unit;
    if(speed < 1024) {
        unit = "BYTE/s";
    } else if(speed < 1024 * 1024) {
        speed /= 1024;
        unit = "KB/s";
    } else {
        speed /= 1024 * 1024;
        unit = "MB/s";
    }
    QString spd = QString::number(speed, 'f', 2);

    qint64 elapsedSecs = m_downloadTimer.elapsed() / 1000;
    QString tip = QString(tr("正在下载 %1% [%2 %3]"))
                  .arg(QString::number(per, 'f', 2))
                  .arg(spd)
                  .arg(unit);

    ui.labelProgress->setText(tip);
    ui.progressBar->setValue(per);
    ui.progressBar->update();

    if(m_strDownFileName != s.at(2)) {
        m_strDownFileName = s.at(2);
    }
}

void UpdateMain::onUpdateReturn(int updateCode)
{
    m_isAlive = false;

    QObject::disconnect(&UpdateManagerInstance, SIGNAL(resultReady(int)), this, SLOT(onUpdateReturn(int)));

    switch(updateCode) {
        case download_update_xml_failure:
        case download_files_failure:
            ui.btnRetry->show();
            ui.labelProgress->setText(tr("网络异常,请检查网络..."));
            break;
        case need_not_to_update:
            ui.labelProgress->setText(tr("无更新可用"));
            ui.progressBar->setValue(100);
            break;
    }
}

void UpdateMain::on_btnOk_clicked()
{
    QDesktopServices::openUrl(QUrl(m_strDownFileName));
}

void UpdateMain::closeEvent(QCloseEvent * event)
{
    UpdateManagerInstance.cancelUpdate();
}

void UpdateMain::changeEvent(QEvent *event)
{
    switch(event->type()) {
    case QEvent::LanguageChange:
        ui.retranslateUi(this);
        break;
    default:
        Dialog::changeEvent(event);
        break;
    }
}
