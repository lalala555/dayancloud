#include "stdafx.h"
#include "PwdMismatchWindow.h"
#include "ui_PwdMismatchWindow.h"

PwdMismatchWindow::PwdMismatchWindow(QWidget *parent)
    : Dialog(parent)
    , ui(new Ui::PwdMismatchWindowClass)
{
    ui->setupUi(this);
    ui->label_logo->setPixmap(QPixmap(LOGO_ICON));
    this->setStyleSheet("QLabel{color:#eeeeee;}");
}

PwdMismatchWindow::~PwdMismatchWindow()
{
    delete ui;
}

void PwdMismatchWindow::on_btnPwdLink_clicked()
{
    QString product = WSConfig::getInstance()->get_link_address("path_from");  //LocalConfig::getInstance()->defaultProduct.toLower();
    QString strUrl = QString("%1/%2?from=%3&lang=%4&from=%5").arg(CConfigDb::Inst()->getHost()).arg(FORGET_PASSWORD).arg(LINK_REFER).arg(RBHelper::getLocaleName()).arg(product);
    QDesktopServices::openUrl(QUrl(strUrl));
}

void PwdMismatchWindow::changeEvent(QEvent *event)
{
    switch(event->type()) {
    case QEvent::LanguageChange: {
        ui->retranslateUi(this);
    }
    break;
    default:
        QWidget::changeEvent(event);
        break;
    }
}
