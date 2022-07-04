#include "stdafx.h"
#include "MessageBoxRestart.h"
#include "ui_MessageBoxRestart.h"

MessageBoxRestart::MessageBoxRestart(QWidget *parent)
    : Dialog(parent)
    , ui(new Ui::MessageBoxRestart)
{
    ui->setupUi(this);
    ui->label_logo->setPixmap(QPixmap(LOGO_ICON));

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setStyleSheet("QLabel{color:#eeeeee;}");
    this->installEventFilter(this);
}

MessageBoxRestart::~MessageBoxRestart()
{
    delete ui;
}

void MessageBoxRestart::on_btnOk_clicked()
{
    done(enIDOK);
}
