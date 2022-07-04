#include "stdafx.h"
#include "MessageBoxATBlock.h"
#include "ui_MessageBoxATBlock.h"


CMessageBoxATBlock::CMessageBoxATBlock(QWidget *parent)
    : Dialog(parent)
    , ui(new Ui::CMessageBoxATBlock)
	,m_isMerage(true)
{
    ui->setupUi(this);
    ui->label_logo->setPixmap(QPixmap(LOGO_ICON));

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setStyleSheet("QLabel{color:#eeeeee;}");
    this->installEventFilter(this);
	this->setModal(true);

	ui->rBtnMerge->setChecked(true);

    connect(ui->labInfo, SIGNAL(linkActivated(const QString&)), this, SIGNAL(linkClicked(const QString&)));
}

CMessageBoxATBlock:: ~CMessageBoxATBlock()
{
    delete ui;
}

bool CMessageBoxATBlock::eventFilter(QObject* obj, QEvent* event)
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



void CMessageBoxATBlock::on_btnOk_clicked()
{
	if (ui->rBtnMerge->isChecked())  //合并
	{
		m_isMerage = true;
	}
	if (ui->rBtnNoMerge->isChecked())
	{
		m_isMerage = false;
	}

    done(enIDOK);
//	QDialog::accept();
}

void CMessageBoxATBlock::on_btnCancel_clicked()
{
    done(enIDCANCEL);
}

void CMessageBoxATBlock::setTitle(const QString &title)
{
    ui->lab_title->setText(title);
    if (!title.isEmpty())ui->label_logo->hide(); // 有title的时候，隐藏logo
}

void CMessageBoxATBlock::setOkBtnText(const QString& text)
{
    ui->btnOk->setText(text);

    QFont font_title = ui->btnOk->font();
    QFontMetrics titleMet(font_title);
    int width = titleMet.width(text) + 20;
    if (width > ui->btnOk->width()) {
        ui->btnOk->setFixedWidth(width);
    }
}

void CMessageBoxATBlock::setCancelText(const QString& text)
{
    ui->btnCancel->setText(text);

    QFont font_title = ui->btnCancel->font();
    QFontMetrics titleMet(font_title);
    int width = titleMet.width(text) + 20;
    if (width > ui->btnCancel->width()) {
        ui->btnCancel->setFixedWidth(width);
    }   
}

bool CMessageBoxATBlock::getMerageOption()
{
	return m_isMerage;
}