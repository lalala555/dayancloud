#include "stdafx.h"
#include "UpdateForm.h"
#include "UpdateMain.h"
#include "Update/UpdateManager.h"

UpdateForm::UpdateForm(QWidget *parent)
    : Dialog(parent)
{
    ui.setupUi(this);
    ui.label_logo->setPixmap(QPixmap(LOGO_ICON));

    this->setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);

    this->setVisible(false);

    this->initUi();

    connect(ui.btnOk, SIGNAL(clicked()), this, SLOT(slotButtonOK()));
    connect(ui.btnClose, SIGNAL(clicked()), this, SLOT(slotCancel()));
    connect(ui.btnCancel, SIGNAL(clicked()), this, SLOT(slotCancel()));
}

UpdateForm::~UpdateForm()
{

}

void UpdateForm::showUpdate()
{
    this->initUi();
    this->show();
}

void UpdateForm::initUi()
{
    ui.widget_3->hide();

    QString strVersionInfo;
    QString strVersion = UpdateManagerInstance.getVersionText(strVersionInfo, LocalSetting::getInstance()->getLanguageId());

    ui.textEdit_notices->setText(strVersionInfo);
    QString strBuf = tr("发现新版本") + strVersion + tr("可用。是否现在升级?");
    ui.label_newnotice->setText(strBuf);
    ui.textEdit_notices->setEnabled(false);
}

void UpdateForm::slotButtonOK()
{
    this->hide();
    // 升级主程序
    QPointer<UpdateMain> m_updateMain(new UpdateMain());
    m_updateMain->exec();

    done(enDialogYes);
}

void UpdateForm::slotCancel()
{
    done(enDialogYes);
}

void UpdateForm::changeEvent(QEvent *event)
{
    switch(event->type()) {
    case QEvent::LanguageChange: {
        ui.retranslateUi(this);
    }
    break;
    default:
        Dialog::changeEvent(event);
        break;
    }
}
