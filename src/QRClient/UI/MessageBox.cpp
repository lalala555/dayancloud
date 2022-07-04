#include "stdafx.h"
#include "MessageBox.h"
#include "ui_MessageBox.h"

CMessageBox::CMessageBox(QWidget *parent)
    : Dialog(parent)
    , ui(new Ui::CMessageBox)
{
    ui->setupUi(this);
    ui->label_logo->setPixmap(QPixmap(LOGO_ICON));

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setStyleSheet("QLabel{color:#eeeeee;}");
    this->installEventFilter(this);

    connect(ui->labInfo, SIGNAL(linkActivated(const QString&)), this, SIGNAL(linkClicked(const QString&)));
}

CMessageBox::~CMessageBox()
{
    delete ui;
}

bool CMessageBox::eventFilter(QObject* obj, QEvent* event)
{
    if(event->type() == QEvent::KeyPress) {
        QKeyEvent* key = static_cast<QKeyEvent*>(event);
        if((key->key() == Qt::Key_Enter) || (key->key() == Qt::Key_Return)) {
            done(enIDOK);
        } else {
            return Dialog::eventFilter(obj, event);
        }
        return true;
    } else {
        return Dialog::eventFilter(obj, event);
    }
    return false;
}

void CMessageBox::setMessage(const QString &msg, int type)
{
    switch(type) {
    case MB_INFO:
        ui->btnCancel->setVisible(false);
        break;
    case MB_ASK:
        break;
    case MB_ERROR:
        ui->btnCancel->setVisible(false);
        break;
    default:
        break;
    }

    ui->labInfo->setText(msg);
#ifdef Q_OS_LINUX
    int wd = 240;
    QFontMetrics f = ui->labInfo->fontMetrics();
    QRect r = f.boundingRect(msg);
    if(r.width() >= wd){
        int h = ((r.width() / wd) - 1) * f.height();
        this->resize(this->width(), this->height() + h);
    }
#endif
}

void CMessageBox::on_btnOk_clicked()
{
    done(enIDOK);
}

void CMessageBox::setTitle(const QString &title)
{
    ui->lab_title->setText(title);
    if (!title.isEmpty())ui->label_logo->hide(); // 有title的时候，隐藏logo
}

void CMessageBox::setOkBtnText(const QString& text)
{
    ui->btnOk->setText(text);

    QFont font_title = ui->btnOk->font();
    QFontMetrics titleMet(font_title);
    int width = titleMet.width(text) + 20;
    if (width > ui->btnOk->width()) {
        ui->btnOk->setFixedWidth(width);
    }
}

void CMessageBox::setCancelText(const QString& text)
{
    ui->btnCancel->setText(text);

    QFont font_title = ui->btnCancel->font();
    QFontMetrics titleMet(font_title);
    int width = titleMet.width(text) + 20;
    if (width > ui->btnCancel->width()) {
        ui->btnCancel->setFixedWidth(width);
    }   
}