#include "stdafx.h"
#include "AboutWindow.h"
#include "ui_AboutWindow.h"

AboutWindow::AboutWindow(QWidget *parent)
    : Dialog(parent)
    , ui(new Ui::AboutWindowClass)
{
    ui->setupUi(this);
    ui->label_logo->setPixmap(QPixmap(LOGO_ICON));

    QString strVersion = QString("%1 %2").arg(STRFILEVERSION).arg(BUILD_TIME);
    ui->labelVersion->setText(strVersion);

    QString transEngine = LocalSetting::getInstance()->getTransEngine();
    QString hostname = LocalSetting::getInstance()->getNetWorkName();
    if(hostname.isEmpty())
        hostname = TransConfig::getInstance()->GetDefaultNetServerName(transEngine);
    QString transEngineName = QString("%1[%2]").arg(transEngine)
                              .arg(hostname);
    ui->label_engine->setText(transEngineName);

    // QString strHost = CConfigDb::Inst()->getHost();
    QString strHost = WSConfig::getInstance()->get_link_address("url_official_website_dayan");
    if (!strHost.isEmpty()) {
        QString strUrl = QString("<a href='%1'><span style='color:#999999;text-decoration:none;'>%2</span></a>").arg(strHost).arg(strHost);
        ui->labelWetsite->setText(strUrl);
    }
}

AboutWindow::~AboutWindow()
{
    delete ui;
}

void AboutWindow::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    } else
        Dialog::changeEvent(event);
}

void AboutWindow::on_btnChangelog_clicked()
{
    QString strVersionFile = QApplication::applicationDirPath() + "/version.txt";
    QDesktopServices::openUrl(QUrl::fromLocalFile(strVersionFile));
}
