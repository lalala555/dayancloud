#include "stdafx.h"
#include "surveyinfowindow.h"
#include "ui_surveyinfowindow.h"

SurveyInfoWindow::SurveyInfoWindow(t_taskid taskId,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SurveyInfoWindow),
	m_SurveyAtId(taskId)
{
	QPalette pal(this->palette());
	pal.setColor(QPalette::Background, QColor("#1f1f27"));
	this->setAutoFillBackground(true);
	this->setPalette(pal);
    ui->setupUi(this);
//	this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);

#ifndef FOXRENDERFARM
	ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("确定"));
	ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("取消"));

#endif // !FOXRENDERFARM


}

SurveyInfoWindow::~SurveyInfoWindow()
{
    delete ui;
}

void SurveyInfoWindow::initUi(bool tptype)
{
	if (tptype) //连接点
	{
		QString ptname = QString("tiePoint_%1").arg(m_tpMaxID +1);
		ui->lineEdit_ptname->setText(ptname);
	}
}

QString SurveyInfoWindow::getPointName()
{
	return ui->lineEdit_ptname->text();
}
int SurveyInfoWindow::getCurrentIndex()
{
	return ui->comboBox_type->currentIndex();
}
QString SurveyInfoWindow::getCurrentText()
{
	return ui->comboBox_type->currentText();
}
void SurveyInfoWindow::setTiePointNum(const short& num)
{
	m_tpSize = num;
}

void SurveyInfoWindow::setTiePointMaxID(const short& MaxID)
{
	m_tpMaxID = MaxID;
}
