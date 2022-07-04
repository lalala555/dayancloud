#include "stdafx.h"
#include "UpdateNotify.h"
#include "UpdateMain.h"
#include "Update/UpdateManager.h"
#include "ui_UpdateNotify.h"

UpdateNotify::UpdateNotify(QWidget *parent) :
    Notification(parent, false),
    ui(new Ui::UpdateNotify)
{
    ui->setupUi(this);
    ui->label_logo->setPixmap(QPixmap(LOGO_ICON));

    this->resize(350, 250);
    this->move((m_desktop.screenGeometry().width() - this->width()), m_desktop.screenGeometry().height());
    QObject::connect(ui->labelUpdateNow, &Label::clicked, this, &UpdateNotify::updateNow);

    QString strVersionInfo;
    QString strVersion = UpdateManagerInstance.getVersionText(strVersionInfo, LocalSetting::getInstance()->getLanguageId());

    ui->textEdit->setText(strVersionInfo);
}

UpdateNotify::~UpdateNotify()
{
    delete ui;
}

void UpdateNotify::showUpdate()
{
    QString strVersionInfo;
    QString strVersion = UpdateManagerInstance.getVersionText(strVersionInfo, LocalSetting::getInstance()->getLanguageId());

    ui->textEdit->setText(strVersionInfo);

    this->show();
    this->showAnimation();
}

void UpdateNotify::updateNow()
{
    this->close();

    QPointer<UpdateMain> m_updateMain(new UpdateMain());
    m_updateMain->exec();
}

void UpdateNotify::closeEvent(QCloseEvent * event)
{
    this->stopAnimation();
}

void UpdateNotify::changeEvent(QEvent *event)
{
    switch(event->type()) {
    case QEvent::LanguageChange: {
        ui->retranslateUi(this);
    }
    break;
    default:
        Notification::changeEvent(event);
        break;
    }
}
